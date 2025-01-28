#include "pch.h"

#include "CodecDec.h"

#include <cstring>
#include <cstdlib>

#include "Logger.h"
#include "CommonConsts.h"
#include "Misc.h"
#include "PacketTools.h"
#include "CodecTools.h"
#include "StreamBase.h"
#include "CodecDecIface.h"
#include "CodecDecParams.h"


namespace Fcore
{
    const bool Decoder::SkipBadPackets   = true;
    const char Decoder::EncoderMetaKey[] = "encoder";
    const char Decoder::ReopenSuffix[]   = "_qsv";

    const AVPixelFormat Decoder::PrefPixFmtDef = AV_PIX_FMT_YUV420P;
    const AVPixelFormat Decoder::PrefPixFmtAlt = AV_PIX_FMT_NV12;


    Decoder::Decoder(AVStream& strm)
        : CodecBase(strm)
        , m_Stream(strm)
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
            bool flush = ResetDecoder();
            LOG_INFO("%s, %s, %s, packs/frames=%d/%d", 
                __FUNCTION__, MediaTypeStr(),
                flush ? "FlushBuffers" : "CloseDecoder",
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
            ret = AVERROR(ENOMEM);
            LOG_ERROR("%s, %s, avcodec_alloc_context3", __FUNCTION__, MediaTypeStr());
        }

        return ret;
    }

    void Decoder::UpdatePixFormat(AVPixelFormat prefPixFmt, bool isGpu)
    {
        AVPixelFormat pixFmt = AV_PIX_FMT_NONE;

        const AVPixelFormat* fmts = GetCodecConfig<AVPixelFormat>(m_CodecCtx->codec);
        if (fmts == nullptr || *fmts == AV_PIX_FMT_NONE) // empty format list
        {
            pixFmt = (prefPixFmt != AV_PIX_FMT_NONE) 
                ? prefPixFmt 
                : (isGpu ? PrefPixFmtAlt : PrefPixFmtDef);
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

        if (pixFmt != AV_PIX_FMT_NONE)
        {
            m_CodecCtx->pix_fmt = pixFmt;
            LOG_INFO("%s, pix.fmt=%s", __FUNCTION__, PixFmtToStr(m_CodecCtx->pix_fmt));
        }
        else
        {
            LOG_WARNING("%s, no pix.fmt", __FUNCTION__);
        }
    }

    void Decoder::UpdateSampFormat(AVSampleFormat prefSampFmt)
    {
        if (prefSampFmt != AV_SAMPLE_FMT_NONE)
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
            if (!hw && byName)
            {
                UpdatePixFormat(prms.PrefPixFmt(), prms.IsGpu());
            }
        }
        if (IsAudio() && byName)
        {
            UpdateSampFormat(prms.PrefSampFmt());
        }
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
            ret = AllocAndInitContext();
            if (ret >= 0)
            {
                ret = CodecTools::OpenCodec(m_CodecCtx, decoder, paramsEx.Options(), __FUNCTION__);
                if (ret >= 0)
                {
                    InitContextEx(paramsEx, byName);
                    m_Decoder = IDecoder::CreateDefaultDecoder(m_CodecCtx);
                    if (CloseOnReset())
                    {
                        m_ReopenParams.Enable(m_CodecCtx, paramsEx.GetOptions());
                    }
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
            }
            else
            {
                ret = AVERROR(ENOMEM);
                LOG_ERROR("%s, %s, failed to allocate frame", __FUNCTION__, MediaTypeStr());
                DeleteDecoder();
            }
        }
        return ret;
    }

    int Decoder::DecodePacket(const AVPacket* pkt, ISinkFrameDec* sink)
    {
        if (!m_Frame)
        {
            return ErrorBadState;
        }

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
                        ? sink->ForwardFrame(m_Frame)
                        : m_CodecHW.ForwardFrame(m_Frame, sink);
                    if (end)
                    {
                        ++ct;
                    }
                    if (rr < 0)
                    {
                        ret = rr;
                        LOG_ERROR("%s, %s, forward frame, ret=%d", __FUNCTION__, MediaTypeStr(), ret);
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
                LOG_ERROR("%s, %s, SendPacket, %s", __FUNCTION__, MediaTypeStr(), FmtErr(ret));
            }
            else
            {
                LOG_WARNING("%s, %s, skip bad packet: SendPacket, %s", __FUNCTION__, MediaTypeStr(), FmtErr(rr));
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

// -----------------------------------------------------------------------
// reopen feature

    void Decoder::ReopenParams::Enable(const AVCodecContext* codecCtx, const std::string& options)
    {
        m_Codec = codecCtx->codec;
        m_PixFmt = codecCtx->pix_fmt;
        m_Options = options;
        LOG_INFO("%s, %s, codec=%s, pix.fmt=%s", 
            __FUNCTION__, MediaTypeToStr(m_Codec->type), 
            m_Codec->name, PixFmtToStr(m_PixFmt));
    }

    bool Decoder::CloseOnReset() const
    {
        return m_Decoder 
            && m_Decoder->MediaType() == AVMEDIA_TYPE_VIDEO 
            && std::strstr(m_Decoder->Name(), ReopenSuffix);
    }

    bool Decoder::ResetDecoder()
    {
        bool flush = !m_ReopenParams.IsEnabled();
        flush ? m_Decoder->FlushBuffers() : CodecBase::Close();
        return flush;
    }

    int Decoder::ReopenDecoder()
    {
        int ret = 0;
        if (!m_CodecCtx)
        {
            ret = AllocAndInitContext();
            if (ret >= 0)
            {
                ret = CodecTools::OpenCodec(m_CodecCtx, m_ReopenParams.Codec(), m_ReopenParams.Options(), __FUNCTION__);
                if (ret >= 0)
                {
                    m_CodecCtx->pix_fmt = m_ReopenParams.PixFmt();
                    LOG_INFO("%s, %s", __FUNCTION__, MediaTypeStr());
                }
            }
        }
        return ret;
    }

} // namespace Fcore

