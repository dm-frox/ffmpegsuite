#include "pch.h"

#include "CodecDecHW.h"

#include "FrameTools.h"
#include "Logger.h"
#include "CodecDec.h"
#include "StrBuilder.h"


extern "C"
{
    static AVPixelFormat hw_pix_fmt = AV_PIX_FMT_NONE;

    static AVPixelFormat get_hw_format(AVCodecContext *ctx, const enum AVPixelFormat *pix_fmts)
    {
        for (const enum AVPixelFormat *p = pix_fmts; *p != AV_PIX_FMT_NONE; ++p) 
        {
            if (*p == hw_pix_fmt)
            {
                return *p;
            }
        }

        av_log(ctx, AV_LOG_ERROR, "get_hw_format\n");

        return AV_PIX_FMT_NONE;
    }

} // extern "C"

namespace
{
    const AVPixelFormat  PixFmtDstDef = AV_PIX_FMT_NV12;

    const char* ToStr(AVHWDeviceType hwType)
    {
        const char* ret = av_hwdevice_get_type_name(hwType);
        return ret ? ret : "unknown";
    }

    bool MethodCtx(const AVCodecHWConfig* config)
    {
        return Fcore::HasFlag(config->methods, AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX);
    }

    template<typename F>
    static bool IterateConfigs(const AVCodec* codec, F f)
    {
        bool ret = false;
        for (int i = 0; !ret; ++i)
        {
            if (const AVCodecHWConfig* config = avcodec_get_hw_config(codec, i))
            {
                if (MethodCtx(config))
                {
                    ret = f(config);
                }
            }
            else
            {
                break;
            }
        }
        return ret;
    }

} // namespace anon

namespace Fcore
{

    CodecHW::CodecHW() = default;

    CodecHW::~CodecHW()
    {
        if (IsEnabled())
        {
            av_buffer_unref(&m_DevCtx);
            FrameTools::FreeFrame(m_Frame);
        }
    }

    const char* CodecHW::TypeStr() const
    {
        return IsEnabled() ? ToStr(m_HWType) : nullptr;
    }

    bool CodecHW::Enable(AVCodecContext* codecCtx, const char* hwName)
    {
        bool ret = false;
        if (!IsEnabled())
        {
            AVHWDeviceType hwType = av_hwdevice_find_type_by_name(hwName);
            if (hwType != AV_HWDEVICE_TYPE_NONE)
            {
                if (Init(codecCtx, hwType) >= 0)
                {
                    ret = true;
                }
            }
            else
            {
                LOG_ERROR("%s, unknown hw name=%s", __FUNCTION__, hwName);
            }
        }
        else
        {
            LOG_ERROR("%s, already enabled, hw name=%s", __FUNCTION__, hwName);
        }
        return ret;
    }

    AVPixelFormat CodecHW::PixFmtDst()
    {
        return PixFmtDstDef;
    }

    // static
    bool CodecHW::CheckCodec(const AVCodecContext* codecCtx, AVHWDeviceType hwType, AVPixelFormat& pixFmtSrc)
    {
        bool ret = IterateConfigs(codecCtx->codec, 
            [hwType, &pixFmtSrc](const AVCodecHWConfig* config)
            {
                bool ret = false;
                if (config->device_type == hwType)
                {
                    pixFmtSrc = config->pix_fmt;
                    ret = true;
                }
                return ret;
            });

        if (!ret)
        {
            LOG_INFO("%s, not supported by codec, hw type=%s", __FUNCTION__, ToStr(hwType));
        }
        return ret;
    }

    int CodecHW::Init(AVCodecContext* codecCtx, AVHWDeviceType hwType)
    {
        int ret = -1;
        AVPixelFormat  pixFmtSrc = AV_PIX_FMT_NONE;
        if (CheckCodec(codecCtx, hwType, pixFmtSrc))
        {
            AVBufferRef* devCtx = nullptr;
            ret = av_hwdevice_ctx_create(&devCtx, hwType, nullptr, nullptr, 0);
            if (ret >= 0)
            {
                if (AVFrame* frm = FrameTools::AllocFrame())
                {
                    m_DevCtx = devCtx;
                    m_HWType = hwType;
                    m_PixFmtDst = PixFmtDst();
                    m_Frame = frm;

                    hw_pix_fmt = pixFmtSrc;
                    codecCtx->get_format = get_hw_format;
                    codecCtx->hw_device_ctx = av_buffer_ref(m_DevCtx);

                    LOG_INFO("%s, type=%s, pix.fmts(src/dst)=%s / %s", __FUNCTION__,
                        TypeStr(), PixFmtToStr(pixFmtSrc), PixFmtToStr(m_PixFmtDst));
                }
                else
                {
                    ret = AVERROR(ENOMEM);
                    LOG_ERROR("%s, failed allocate frame", __FUNCTION__);
                }
            }
            else
            {
                LOG_ERROR("%s, av_hwdevice_ctx_create, %s", __FUNCTION__, FmtErr(ret));
            }
        }
        return ret;
    }

    int CodecHW::ForwardFrame(AVFrame* frame, ISinkFrameDec* sink)
    {
        if (!m_Frame)
        {
            return ErrorBadState;
        }

        int ret = av_hwframe_transfer_data(m_Frame, frame, 0);
        if (ret >= 0)
        {
            auto pixFmt = static_cast<AVPixelFormat>(m_Frame->format);
            if (m_PixFmtDst == pixFmt)
            {
                m_Frame->pts = frame->pts;
                m_Frame->best_effort_timestamp = frame->best_effort_timestamp;
                ret = sink->ForwardFrame(m_Frame);
            }
            else
            {
                FrameTools::FreeFrameBuffer(m_Frame);
                LOG_ERROR("%s, wrong pix.fmt=%s", __FUNCTION__, PixFmtToStr(pixFmt));
            }
        }
        else
        {
            LOG_ERROR("%s, av_hwframe_transfer_data, %s", __FUNCTION__, FmtErr(ret));
        }

        FrameTools::FreeFrameBuffer(frame);

        return ret;
    }

    // static
    std::string CodecHW::GetHWAccels(const AVCodec* codec)
    {
        StrBuilder sb(64);
        IterateConfigs(codec,
            [&sb](const AVCodecHWConfig* config)
            {
                sb.Append(av_hwdevice_get_type_name(config->device_type));
                return false;
            });
        return sb.Str();
    }

    // static
    std::string CodecHW::GetHWAccels()
    {
        StrBuilder sb(64, " ");
        AVHWDeviceType hwtype = AV_HWDEVICE_TYPE_NONE;
        while ((hwtype = av_hwdevice_iterate_types(hwtype)) != AV_HWDEVICE_TYPE_NONE)
        {
            sb.Append(av_hwdevice_get_type_name(hwtype));
        }
        return sb.Str();
    }

    // static
    int CodecHW::LogDeviceList()
    {
        int ret = 0;
        LOG_INFO("%s", __FUNCTION__);
        AVHWDeviceType hwtype = AV_HWDEVICE_TYPE_NONE;
        while (true)
        {
            hwtype = av_hwdevice_iterate_types(hwtype);
            if (hwtype != AV_HWDEVICE_TYPE_NONE)
            {
                const char* name = ToStr(hwtype);
                LOG_INFO(" -- %d  %s", ret++, name);
            }
            else
            {
                break;
            }
        }
        return ret;
    }

    // static
    void CodecHW::LogCodec(const char* codecName, bool decod)
    {
        LOG_INFO("%s, try %s", __FUNCTION__, codecName);
        if (const AVCodec* decoder = decod
            ? avcodec_find_decoder_by_name(codecName)
            : avcodec_find_encoder_by_name(codecName))
        {
            for (int i = 0; ; ++i)
            {
                const AVCodecHWConfig *config = avcodec_get_hw_config(decoder, i);
                if (config)
                {
                    bool flag = MethodCtx(config);
                    AVHWDeviceType hwtype = config->device_type;
                    const char* typeName = ToStr(hwtype);
                    AVPixelFormat pixFmt = config->pix_fmt;
                    LOG_INFO(" -- config: flag=%d, name=%s, pix.fmt=%s/%d", flag, typeName, PixFmtToStr(pixFmt), (int)pixFmt);
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            LOG_ERROR("%s, no codec", __FUNCTION__);
        }
    }

// --------------------------------------------------------------------

    static void LogCodecs()
    {
        CodecHW::LogCodec("h264");
        CodecHW::LogCodec("h264_qsv");
        CodecHW::LogCodec("hevc");
        CodecHW::LogCodec("vp9");
        CodecHW::LogCodec("vc1");
        CodecHW::LogCodec("theora");
        CodecHW::LogCodec("aac");
        CodecHW::LogCodec("libx264", false);
        CodecHW::LogCodec("libx265", false);
    }

    int LogHWDeviceList()
    {
        int ret = CodecHW::LogDeviceList();
        LogCodecs();
        return ret;
    }

 //--config, flag = 1, name = dxva2, pix.fmt = dxva2_vld / 53
 //--config, flag = 0, name = (null), pix.fmt = d3d11va_vld / 118
 //--config, flag = 1, name = d3d11va, pix.fmt = d3d11 / 174
 //--config, flag = 1, name = cuda, pix.fmt = cuda / 119

} // namespace Fcore


//enum AVHWDeviceType {
//    AV_HWDEVICE_TYPE_NONE,
//    AV_HWDEVICE_TYPE_VDPAU,
//    AV_HWDEVICE_TYPE_CUDA,
//    AV_HWDEVICE_TYPE_VAAPI,
//    AV_HWDEVICE_TYPE_DXVA2,
//    AV_HWDEVICE_TYPE_QSV,
//    AV_HWDEVICE_TYPE_VIDEOTOOLBOX,
//    AV_HWDEVICE_TYPE_D3D11VA,
//    AV_HWDEVICE_TYPE_DRM,
//    AV_HWDEVICE_TYPE_OPENCL,
//    AV_HWDEVICE_TYPE_MEDIACODEC,
//    AV_HWDEVICE_TYPE_VULKAN,
//    AV_HWDEVICE_TYPE_D3D12VA,
//};

//Fcore::CodecHW::LogDeviceList
//-- 0  cuda
//-- 1  dxva2
//-- 2  qsv
//-- 3  d3d11va

//Fcore::CodecHW::LogCodec, try h264
//--config, flag = 1, name = dxva2, pix.fmt = dxva2_vld / 53
//--config, flag = 0, name = (null), pix.fmt = d3d11va_vld / 118
//--config, flag = 1, name = d3d11va, pix.fmt = d3d11 / 174
//--config, flag = 1, name = cuda, pix.fmt = cuda / 119


