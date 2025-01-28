#include "pch.h"

#include "CodecDec.h"

#include <cstring>
#include <cstdlib>

#include "Logger.h"
#include "CommonConsts.h"
#include "Misc.h"
#include "FrameTools.h"
#include "PacketTools.h"
#include "CodecTools.h"
#include "StreamBase.h"
#include "CodecFrameSinkIface.h"
#include "CodecDecIface.h"
#include "CodecDecParams.h"


namespace Fcore
{
// ReopenMode, nested

    Decoder::ReopenMode::ReopenMode() = default;

    Decoder::ReopenMode::~ReopenMode() = default;

    const char* Decoder::ReopenMode::MediaType() const { return m_Codec ? MediaTypeToStr(m_Codec->type) : ""; }

    void Decoder::ReopenMode::Enable(AVCodecContext* codecCtx, const char* options)
    {
        m_CodecCtx = codecCtx;
        m_Codec    = m_CodecCtx->codec;
        m_Options  = StrArg(options);
        m_Closed   = false;
        LOG_INFO("%s, %s", __FUNCTION__, MediaType());
    }

    void Decoder::CloseDecoder()
    {
        CodecBase::Close();
        //LOG_INFO("%s, %s", __FUNCTION__, MediaType());
    }

    void Decoder::ReopenMode::CloseDecoder()
    {
        //avcodec_close(m_CodecCtx); // V7
        m_Closed = true;
        LOG_INFO("%s, %s", __FUNCTION__, MediaType());
    }

    int Decoder::ReopenDecoder()
    {
        int ret = 0;
        if (!m_CodecCtx)
        {
            ret = AllocAndInitContext();
            if (ret >= 0)
            {
                ret = CodecTools::OpenCodec(m_CodecCtx, m_Codec, m_Options.c_str(), __FUNCTION__);
                if (ret >= 0)
                {
                    LOG_INFO("%s, %s", __FUNCTION__, MediaType());
                }
            }
        }
        return ret;
    }

    int Decoder::ReopenMode::ReopenDecoder()
    {
        int ret = 0;
        if (m_Closed)
        {
            ret = CodecTools::OpenCodec(m_CodecCtx, m_Codec, m_Options.c_str(), __FUNCTION__);
            if (ret >= 0)
            {
                m_Closed = false;
                LOG_INFO("%s, %s", __FUNCTION__, MediaType());
            }
        }
        return ret;
    }

// -----------------------------------------------------------------------

    const bool Decoder::SkipBadPackets = true;
    const char Decoder::EncoderMetaKey[] = "encoder";

    const AVPixelFormat Decoder::PrefPixFmtDef = AV_PIX_FMT_YUV420P;
    const AVPixelFormat Decoder::PrefPixFmtAlt = AV_PIX_FMT_NV12;


    Decoder::Decoder(AVStream& strm, IFrameSink* frameSink)
        : CodecBase(strm)
        , m_Stream(strm)
        , m_FrameSink(frameSink)
    {}

    Decoder::~Decoder()
    {
        DeleteDecoder();
    }

    void Decoder::DeleteDecoder()
    {
        if (m_Decoder)
        {
            m_Decoder->Delete();
            m_Decoder = nullptr;
        }
    }

    void Decoder::Reset()
    {
        if (m_Decoder)
        {
            if (!CheckReopenMode()) //!m_ReopenMode.Enabled())
            {
                m_Decoder->FlushBuffers();
                //LOG_INFO("%s, %s, FlushBuffers", __FUNCTION__, MediaType());
            }
            else
            {
                //m_ReopenMode.CloseDecoder();
                CloseDecoder();
                LOG_INFO("%s, %s, CloseDecoder", __FUNCTION__, MediaType());
            }
            LOG_INFO("%s, %s, packs/frames=%d/%d", __FUNCTION__, MediaTypeStr(),
                m_PacktCntr, m_FrameCntr);
            m_Frame.FreeBuffer();
            m_PacktCntr = 0;
            m_FrameCntr = 0;
        }
    }

    const char* Decoder::GetEncoderNameFromMetadata() const
    {
        return StreamBase::GetMetadataItem(m_Stream, EncoderMetaKey);
    }

    const AVCodec* Decoder::FindDecoderByName(const char* decName) const
    {
        const AVCodec* ret = nullptr;
        if (const AVCodec* decoder = avcodec_find_decoder_by_name(decName))
        {
            if (decoder->type == MediaType())
            {
                if (decoder->id == CodecId())
                {
                    ret = decoder;
                    LOG_INFO("%s, %s, found by name=%s", __FUNCTION__, MediaTypeStr(), decName);
                }
                else
                {
                    LOG_WARNING("%s, %s, decoder=%s, wrong cod.id=%s, expected=%s",
                        __FUNCTION__, MediaTypeStr(), decName, CodecIdToStr(decoder->id), CodecIdToStr(CodecId()));
                }
            }
            else
            {
                LOG_WARNING("%s, %s, decoder=%s, wrong media type=%s",
                    __FUNCTION__, MediaTypeStr(), decName, MediaTypeToStr(decoder->type));
            }
        }
        else
        {
            LOG_WARNING("%s, %s, decoder=%s, failed to find decoder",
                __FUNCTION__, MediaTypeStr(), StrToLog(decName));
        }
        return ret;
    }

    const AVCodec* Decoder::FindDecoder(const char* decName, bool& byName) const
    {
        const AVCodec* defDecoder = avcodec_find_decoder(CodecId());
        const AVCodec* decoder = (decName && *decName)
            ? ((defDecoder && StrAreEq(defDecoder->name, decName)) ? defDecoder : FindDecoderByName(decName))
            : defDecoder;

        if (decoder)
        {
            byName = decoder != defDecoder;
        }
        else
        {
            uint32_t tag = CodecParams().codec_tag;
            LOG_WARNING("%s, %s, failed find decoder: cod.id=%s, tag=0x%X/%s, extra size=%d, enc.meta=%s",
                __FUNCTION__,
                MediaTypeStr(),
                CodecIdToStr(CodecId()),
                tag, FourccToStr(tag),
                CodecParams().extradata_size,
                StrToLog(GetEncoderNameFromMetadata()));
        }

        return decoder;
    }

    int Decoder::TryExternalDecoder()
    {
        int ret = DecoderNotFound;
        if (IDecoder* decExt = IDecoder::FindExternalDecoder(&m_Stream))
        {
            m_Decoder = decExt;
            ret = 0;
        }
        else
        {
            LOG_ERROR("%s, %s, decoder not found", __FUNCTION__, MediaTypeStr());
        }
        return ret;
    }

    int Decoder::AllocAndInitContext()
    {
        int ret = -1;

        if (auto ctx = avcodec_alloc_context3(nullptr))
        {
            ret = avcodec_parameters_to_context(ctx, &CodecParams());
            if (ret >= 0)
            {
                m_CodecCtx = ctx;
            }
            else
            {
                LOG_ERROR("%s, %s, avcodec_parameters_to_context, %s", __FUNCTION__, MediaTypeStr(), FmtErr(ret));
            }
        }
        else
        {
            LOG_ERROR("%s, %s, avcodec_alloc_context3", __FUNCTION__, MediaTypeStr());
            ret = AVERROR(ENOMEM);
        }

        return ret;
    }

    void Decoder::UpdatePixFormat(AVPixelFormat prefPixFmt, bool byName, bool hw)
    {
        if (!hw && (byName || prefPixFmt != AV_PIX_FMT_NONE))
        {
            AVPixelFormat pixFmt = AV_PIX_FMT_NONE;
            const AVPixelFormat* fmts = m_CodecCtx->codec->pix_fmts;
            if (fmts == nullptr || *fmts == AV_PIX_FMT_NONE)
            {
                pixFmt = PrefPixFmtDef;
            }
            else
            {
                AVPixelFormat prefs[] = { prefPixFmt, PrefPixFmtDef,  PrefPixFmtAlt };
                for (auto pref : prefs)
                {
                    pixFmt = CodecTools::FindPixelFormat2(fmts, pref);
                    if (pixFmt != AV_PIX_FMT_NONE)
                    {
                        break;
                    }
                }
                if (pixFmt == AV_PIX_FMT_NONE)
                {
                    pixFmt = *fmts;
                }
            }
            m_CodecCtx->pix_fmt = pixFmt;
            LOG_INFO("%s, pix.fmt=%s", __FUNCTION__, PixFmtToStr(m_CodecCtx->pix_fmt));
        }
    }

    void Decoder::UpdateSampFormat(AVSampleFormat prefSampFmt, bool byName)
    {
        if (byName || prefSampFmt != AV_SAMPLE_FMT_NONE)
        {
            AVSampleFormat sampFmt = CodecTools::FindSampleFormat(m_CodecCtx->codec, prefSampFmt);
            if (sampFmt != AV_SAMPLE_FMT_NONE)
            {
                m_CodecCtx->sample_fmt = sampFmt;
                LOG_INFO("%s, samp.fmt=%s", __FUNCTION__, SampFmtToStr(m_CodecCtx->sample_fmt));
            }
            else
            {
                LOG_WARNING("%s, no samp.fmt", __FUNCTION__);
            }
        }
    }

    bool Decoder::InitHWAccel(const char* hwName, bool isGpu)
    {
        bool ret = false;
        if (hwName && *hwName && !isGpu)
        {
            if (m_CodecHW.Enable(m_CodecCtx, hwName))
            {
                m_CodecCtx->pix_fmt = m_CodecHW.PixelFormatDst();
                ret = true;

                LOG_INFO("%s, %s, HW accel=%s, pix.fmt=%s", __FUNCTION__, MediaTypeStr(),
                    m_CodecHW.TypeStr(), PixFmtToStr(m_CodecCtx->pix_fmt));
            }
            else
            {
                LOG_WARNING("%s, %s, codec=%s, failed to enable HW accel, name=%s", __FUNCTION__,
                    MediaTypeStr(), m_CodecCtx->codec->name, hwName);
            }
        }
        return ret;
    }

    void Decoder::InitContextEx(const ParamsEx& prms, bool byName)
    {
        if (IsVideo())
        {
            bool hw = InitHWAccel(prms.HWAccel(), prms.IsGpu());
            UpdatePixFormat(prms.PrefPixFmt(), byName, hw);
        }
        if (IsAudio())
        {
            UpdateSampFormat(prms.PrefSampFmt(), byName);
        }
    }

    bool Decoder::CheckReopenMode() const
    {
        return std::strstr(CodecBase::Name(), "_qsv") ? true : false;
    }

    int Decoder::OpenDecoder(const char* decParamsEx)
    {
        if (!AssertFirst(m_Decoder, __FUNCTION__))
        {
            return ErrorAlreadyInitialized;
        }

        int ret = -1;
        ParamsEx paramsEx(decParamsEx, MediaType(), CodecId());
        bool byName = false;
        if (const AVCodec* decoder = FindDecoder(paramsEx.Name(), byName))
        {
            m_Codec = decoder;
            m_Options = StrArg(paramsEx.Options());
            ret = AllocAndInitContext();
            if (ret >= 0)
            {
                //ret = CodecTools::OpenCodec(m_CodecCtx, decoder, paramsEx.Options(), __FUNCTION__);
                ret = CodecTools::OpenCodec(m_CodecCtx, m_Codec, m_Options.c_str(), __FUNCTION__);
                if (ret >= 0)
                {
                    InitContextEx(paramsEx, byName);
                    m_Decoder = IDecoder::CreateDefaultDecoder(m_CodecCtx);
                }
            }
        }
        else
        {
            ret = TryExternalDecoder();
        }

        if (m_Decoder)
        {
            if (m_Frame.Alloc())
            {
                m_PacktCntr = 0;
                m_FrameCntr = 0;
                CodecBase::SetMediaParams(m_Decoder, m_CodecHW.TypeStr());
                CodecTools::LogCodecContext(m_CodecCtx, true, __FUNCTION__);
                //if (CheckReopenMode())
                //{
                //    m_ReopenMode.Enable(m_CodecCtx, paramsEx.Options());
                //}
            }
            else
            {
                LOG_ERROR("%s, %s, failed to allocate frame", __FUNCTION__, MediaTypeStr());
                ret = AVERROR(ENOMEM);
                DeleteDecoder();
            }
        }
        return ret;
    }

    int Decoder::DecodePacket(const AVPacket* pkt)
    {
        if (!m_Frame)
        {
            return ErrorBadState;
        }

        //if (m_ReopenMode.ReopenDecoder() < 0)

        if (ReopenDecoder() < 0)
        {
            return ErrorBadState;
        };

        int ret = -1;
        int rr = m_Decoder->SendPacket(pkt);
        if (rr >= 0)
        {
            ++m_PacktCntr;
            int ct = 0;
            bool end = (pkt == nullptr);
            if (end)
            {
                LOG_INFO("%s, %s, null packet sent", __FUNCTION__, MediaTypeStr());
            }
            while (rr >= 0)
            {
                rr =  m_Decoder->ReceiveFrame(m_Frame);
                if (rr >= 0) // OK, frame received
                {
                    ++m_FrameCntr;
                    rr = !m_CodecHW.IsEnabled()
                        ? m_FrameSink->ForwardFrame(m_Frame)
                        : m_CodecHW.ForwardFrame(m_Frame, m_FrameSink);
                    if (end)
                    {
                        ++ct;
                    }
                    if (rr < 0)
                    {
                        ret = rr;
                        LOG_ERROR("%s, %s, forward frame", __FUNCTION__, MediaTypeStr());
                    }
                }
                else
                {
                    if (rr == EndVal(end)) // OK, no decoded frames
                    {
                        ret = 0;
                        if (end)
                        {
                            LOG_INFO("%s, %s, last frame received, codec buff=%d", __FUNCTION__, MediaTypeStr(), ct);
                        }
                    }
                    else              // error
                    {
                        ret = rr;
                        LOG_ERROR("%s, %s, avcodec_receive_frame, %s", __FUNCTION__, MediaTypeStr(), FmtErr(ret));
                    }
                }
            }
        }
        else
        {
            if (!SkipBadPackets)
            {
                ret = rr;
                LOG_ERROR("%s, %s, avcodec_send_packet, %s", __FUNCTION__, MediaTypeStr(), FmtErr(ret));
            }
            else
            {
                LOG_WARNING("%s, %s, skip bad packet: avcodec_send_packet, %s", __FUNCTION__, MediaTypeStr(), FmtErr(rr));
                ret = 0;
            }
        }
        return ret;
    }

    int Decoder::DecodeSubtitle(AVPacket* pkt, AVSubtitle* sub, int* got)
    {
        return (IsSubtit() && IsOpened()) ? avcodec_decode_subtitle2(m_CodecCtx, sub, got, pkt) : -1;
    }

    //static 
    void Decoder::FreeSubtitle(AVSubtitle* sub)
    {
        avsubtitle_free(sub);
    }

} // namespace Fcore

//const unsigned char J2K_FrameTitle[] =
//{
//    0x00, 0x00, 0x00, 0x0c, 0x6a, 0x50, 0x20, 0x20, 0x0d, 0x0a, 0x87, 0x0a, 0x00, 0x00, 0x00, 0x14,
//    0x66, 0x74, 0x79, 0x70, 0x6a, 0x70, 0x32, 0x20, 0x00, 0x00, 0x00, 0x00, 0x6a, 0x70, 0x32, 0x20,
//    0x00, 0x00, 0x00, 0x2d, 0x6a, 0x70, 0x32, 0x68, 0x00, 0x00, 0x00, 0x16, 0x69, 0x68, 0x64, 0x72,
//    0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0xd0, 0x00, 0x03, 0x07, 0x07, 0x00, 0x00, 0x00, 0x00,
//    0x00, 0x0f, 0x63, 0x6f, 0x6c, 0x72, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00,
//    0x00
//};


