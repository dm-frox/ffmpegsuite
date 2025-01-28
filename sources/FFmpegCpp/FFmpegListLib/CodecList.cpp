#include "pch.h"

#include "CodecList.h"

#include <algorithm>

#include "CodecDecHW.h"
#include "Logger.h"
#include "OptionTools.h"
#include "CodecTools.h"
#include "Misc.h"
#include "StrBuilder.h"
#include "ArrSize.h"


namespace Flist
{
    const char* const CodecInfo::GpuSuffixes[] = { "_qsv", "_cuvid", "_nvenc" };


    CodecInfo::CodecInfo(const AVCodec* codec) 
        : m_Codec(codec) 
    {}

    CodecInfo::~CodecInfo() = default;

    const char* CodecInfo::Name() const 
    { 
        return m_Codec->name; 
    }

    const char* CodecInfo::LongName() const 
    { 
        return m_Codec->long_name; 
    }

    bool CodecInfo::IsDecoder() const
    {
        return av_codec_is_decoder(m_Codec);
    }

    bool CodecInfo::IsExperimental() const 
    { 
        return Fcore::HasFlag(m_Codec->capabilities, AV_CODEC_CAP_EXPERIMENTAL);
    }

    const char* CodecInfo::Formats() const
    {
        if (m_Formats.empty())
        {
            m_Formats = SetFormats(m_Codec);
        }
        return m_Formats.c_str();
    }

    const char* CodecInfo::HWAccels() const
    {
        if (m_HWAccels.empty())
        {
            m_HWAccels = SetHWAccel(m_Codec);
        }
        return m_HWAccels.c_str();
    }

    const char* CodecInfo::PrivOptions() const
    {
        return GetPrivOpts(false, m_PrivOpts);
    }

    const char* CodecInfo::PrivOptionsEx() const
    {
        return GetPrivOpts(true, m_PrivOptsEx);
    }

    // static
    std::string CodecInfo::SetFormats(const AVCodec* codec)
    {
        Fcore::StrBuilder sb(64);
        if (codec->type == AVMEDIA_TYPE_VIDEO)
        {
            if (const AVPixelFormat* fmts = Fcore::GetCodecConfig<AVPixelFormat>(codec))
            {
                for (const AVPixelFormat* fmt = fmts; *fmt != AV_PIX_FMT_NONE; ++fmt)
                {
                    sb.Append(Fcore::PixFmtToStr(*fmt));
                }
            }
        }
        else if (codec->type == AVMEDIA_TYPE_AUDIO)
        {
            if (const AVSampleFormat* fmts = Fcore::GetCodecConfig<AVSampleFormat>(codec))
            {
                for (const AVSampleFormat* fmt = fmts; *fmt != AV_SAMPLE_FMT_NONE; ++fmt)
                {
                    sb.Append(Fcore::SampFmtToStr(*fmt));
                }
            }
        }
        return sb.Str();
    }

    //static 
    bool CodecInfo::IsGpuCodec(const char* name)
    {
        bool ret = false;
        for (int i = 0, n = Ftool::Size(GpuSuffixes); i < n && !ret; ++i)
        {
            ret = std::strstr(name, GpuSuffixes[i]) != nullptr;
        }
        return ret;
    }

    // static
    std::string CodecInfo::SetHWAccel(const AVCodec* codec)
    {
        return (codec->type == AVMEDIA_TYPE_VIDEO && !IsGpuCodec(codec->name))
            ? Fcore::CodecHW::GetHWAccels(codec)
            : std::string();
    }

    const char* CodecInfo::GetPrivOpts(bool ex, std::string& optStr) const
    {
        if (optStr.empty())
        {
            if (const AVClass* avc = m_Codec->priv_class)
            {
                optStr = ex
                    ? Fcore::OptionTools::OptionsToStrEx(
                        avc, Name(), (IsDecoder() ? "deñoder" : "encoder"))
                    : Fcore::OptionTools::OptionsToStr(avc);

                SetEmptyEx(optStr);
            }
        }
        return optStr.c_str();
    }

    // statc
    const char* CodecInfo::GetCtxOptsEx(std::string& optStr)
    {
        if (optStr.empty())
        {
            if (AVCodecContext* codecCtx = avcodec_alloc_context3(nullptr))
            {
                if (const AVClass* avc = codecCtx->av_class)
                {
                    optStr = Fcore::OptionTools::OptionsToStrEx(
                        avc, "Codec context", nullptr);

                    SetEmptyEx(optStr);
                }
                avcodec_free_context(&codecCtx);
            }
            else
            {
                LOG_ERROR("%s, avcodec_alloc_context3", __FUNCTION__);
            }
        }
        return optStr.c_str();
    }

// ---------------------------------------------------------------------

    CodecNode::CodecNode(const AVCodecDescriptor* codecDescr)
        : m_CodecDescr(codecDescr)
    {}

    CodecNode::~CodecNode() = default;

    void CodecNode::AddCodec(const AVCodec* codec)
    {
        av_codec_is_decoder(codec) 
            ? m_Decoders.emplace_back(codec) 
            : av_codec_is_encoder(codec) 
                ? m_Encoders.emplace_back(codec) 
                : void();
    }

    const char* CodecNode::Name() const 
    { 
        return m_CodecDescr->name; 
    }

    const char* CodecNode::LongName() const 
    { 
        return m_CodecDescr->long_name; 
    }

    const char* CodecNode::MediaTypeStr() const 
    { 
        return av_get_media_type_string(m_CodecDescr->type);
    }

    int CodecNode::CodecCount() const 
    { 
        return DecoderCount() + EncoderCount();
    }

    int CodecNode::DecoderCount() const 
    { 
        return (int)m_Decoders.size(); 
    }

    int CodecNode::EncoderCount() const 
    { 
        return (int)m_Encoders.size(); 
    }

    int CodecNode::MediaType() const
    {
        return m_CodecDescr->type;
    }

    bool CodecNode::CheckProp(int prop) const
    {
        return Fcore::HasFlag(m_CodecDescr->props, prop);
    }

    bool CodecNode::Lossless() const
    {
        return CheckProp(AV_CODEC_PROP_LOSSLESS);
    }

    bool CodecNode::Lossy() const
    {
        return CheckProp(AV_CODEC_PROP_LOSSY);
    }

    bool CodecNode::IntraOnly() const
    {
        return CheckProp(AV_CODEC_PROP_INTRA_ONLY);
    }

    const Fwrap::ICodecInfo* CodecNode::Decoder(int i) const
    { 
        return m_Decoders[i].GetIface();
    }

    const Fwrap::ICodecInfo* CodecNode::Encoder(int i) const
    { 
        return m_Encoders[i].GetIface();
    }

// --------------------------------------------------------------------

    CodecList::CodecList() = default;

    CodecList::~CodecList() = default;

    //static 
    size_t CodecList::GetCapacityCodecs()
    {
        int cntr = 0;
        void* ctx = nullptr;
        while (av_codec_iterate(&ctx))
        {
            ++cntr;
        }
        return cntr + 1;
    }

    void CodecList::Setup()
    {
        if (IsEmpty())
        {
            CodecsType codecs;
            codecs.reserve(GetCapacityCodecs());
            void* ctx = nullptr;
            while (const AVCodec* codec = av_codec_iterate(&ctx))
            {
                codecs.push_back(codec);
            }
            m_CodecCount = (int)codecs.size();
            std::stable_sort(codecs.begin(), codecs.end(),
                [](const AVCodec* x, const AVCodec* y) { return x->id < y->id; });

            SetupNodes(codecs);

            LOG_INFO("%s, codec count=%d, node count=%d", __FUNCTION__, CodecCount(), Count());
        }
    }

    //static 
    size_t CodecList::GetCapacityNodes()
    {
        int cntr = 0;
        const AVCodecDescriptor* codecDescr = nullptr;
        while (codecDescr = avcodec_descriptor_next(codecDescr))
        {
            ++cntr;
        }
        return cntr + 1;
    }

    void CodecList::SetupNodes(const CodecsType& codecs)
    {
        Reserve(GetCapacityNodes());
        auto citer = codecs.cbegin(), end = codecs.cend();
        const AVCodecDescriptor* codecDescr = nullptr;
        while (codecDescr = avcodec_descriptor_next(codecDescr))
        {
            List().emplace_back(codecDescr);
            CodecNode& node = List().back();
            while (citer != end && (*citer)->id == node.Id())
            {
                node.AddCodec(*citer);
                ++citer;
            }
        }
    }

    const char* CodecList::CtxOptsEx() const
    {
        return CodecInfo::GetCtxOptsEx(m_CtxOptsEx);
    }

    int LogCodecList()
    {
        CodecList cf;
        cf.Setup();
        LOG_INFO("%s, node count=%d", __FUNCTION__, cf.Count());
        for (int i = 0; i < cf.Count(); ++i)
        {
            const Fwrap::ICodecNode* node = cf[i];
            LOG_INFO(" -- %3d %18s, %8s, %d/%d", i++, node->Name(), node->MediaTypeStr(), node->DecoderCount(), node->EncoderCount());
        }
        return cf.CodecCount();
    }

} // namespace Flist

//typedef struct AVCodecDescriptor {
//    enum AVCodecID     id;
//    enum AVMediaType type;
//    /**
//     * Name of the codec described by this descriptor. It is non-empty and
//     * unique for each codec descriptor. It should contain alphanumeric
//     * characters and '_' only.
//     */
//    const char* name;
//    /**
//     * A more descriptive name for this codec. May be NULL.
//     */
//    const char* long_name;
//    /**
//     * Codec properties, a combination of AV_CODEC_PROP_* flags.
//     */
//    int             props;
//    /**
//     * MIME type(s) associated with the codec.
//     * May be NULL; if not, a NULL-terminated array of MIME types.
//     * The first item is always non-NULL and is the preferred MIME type.
//     */
//    const char* const* mime_types;
//    /**
//     * If non-NULL, an array of profiles recognized for this codec.
//     * Terminated with AV_PROFILE_UNKNOWN.
//     */
//    const struct AVProfile* profiles;
//} AVCodecDescriptor;


