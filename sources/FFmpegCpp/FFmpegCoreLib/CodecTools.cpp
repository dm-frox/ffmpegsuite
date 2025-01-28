#include "pch.h"

#include "CodecTools.h"

#include "Logger.h"
#include "Misc.h"
#include "MediaTypeHolder.h"
#include "CodecDecIface.h"
#include "OptionTools.h"
#include "DictTools.h"
#include "MultiString.h"
#include "ChannLayout.h"

namespace
{
    inline const char* ToLog(const char* str)
    {
        return Fcore::StrToLog(str);
    }

    inline const char* ToString(AVCodecID id)
    {
        return Fcore::CodecIdToStr(id);
    }

    inline const char* ToString(AVMediaType mediaType)
    {
        return Fcore::MediaTypeToStr(mediaType);
    }

    inline const char* ToString(AVPixelFormat pixFmt)
    {
        return Fcore::PixFmtToStr(pixFmt);
    }

    inline const char* ToString(AVSampleFormat sampFmt)
    {
        return Fcore::SampFmtToStr(sampFmt);
    }
}

namespace Fcore
{

    // static
    int CodecTools::OpenCodec(AVCodecContext* codecCtx, const AVCodec* codec, const char* options, const char* msg)
    {
        AVDictionary* opts = MultiString::CreateDictionary(options);
        if (opts)
        {
            DictHolder::Log(opts, msg, "options");
        }
        int ret = avcodec_open2(codecCtx, codec, &opts);
        if (ret >= 0)
        {
            DictHolder::LogNotFound(opts, msg);
        }
        else
        {
            LOG_ERROR("%s, avcodec_open2, %s", msg, FmtErr(ret));
        }
        return ret;
    }

    // static
    const char CodecTools::PresetVideoOption[] = "preset";
    const char CodecTools::CrfVideoOption[]    = "crf";

    // static
    void CodecTools::SetupVideoCodecContext(AVCodecContext* codecCtx, int width, int height, AVPixelFormat pixFmt, AVRational frameRate, AVRational tb)
    {
        codecCtx->width = width;
        codecCtx->height = height;
        codecCtx->pix_fmt = pixFmt;
        codecCtx->sample_aspect_ratio = { 0, 1 };
        codecCtx->framerate = frameRate;
        codecCtx->time_base = tb;
    }

    template<typename T>
    int SetPrivOption(AVCodecContext* codecCtx, const char* name, T val)
    {
        int ret = -1;
        if (const AVCodec* codec = codecCtx->codec)
        {
            if (codecCtx->codec->priv_class)
            {
                ret = OptionTools::SetOption(codecCtx->priv_data, name, val);
                if (ret < 0)
                {
                    LOG_ERROR("%s, codec %s, opt.name=%s, %s", __FUNCTION__, codec->name, ToLog(name), FmtErr(ret));
                }
            }
            else
            {
                LOG_ERROR("%s, codec %s does not support options", __FUNCTION__, codec->name);
            }
        }
        else
        {
            LOG_ERROR("%s, no codec", __FUNCTION__);
        }
        return ret;
    }

    // static
    void CodecTools::SetupVideoCodecContext(AVCodecContext* codecCtx, int64_t bitrate, const char* preset, int crf)
    {
        if (bitrate > 0)
        {
            codecCtx->bit_rate = bitrate;
        }
        if (preset && *preset)
        {
            SetPrivOption(codecCtx, PresetVideoOption, preset);
        }
        if (crf >= 0)
        {
            SetPrivOption(codecCtx, CrfVideoOption, crf);
        }

        LOG_INFO("%s, bitrate=%lld, preset=%s, crf=%d", __FUNCTION__, codecCtx->bit_rate, ToLog(preset), crf);

    }

    void CodecTools::SetupAudioCodecContext(AVCodecContext* codecCtx, const ChannLayout& channLayout, int sampleRate, AVSampleFormat sampFmt)
    {
        channLayout.CopyTo(codecCtx->ch_layout);
        codecCtx->sample_rate = sampleRate;
        codecCtx->sample_fmt = sampFmt;
        codecCtx->time_base = AVRational{ 1, sampleRate };
    }

    // static
    void CodecTools::SetupAudioCodecContext(AVCodecContext* codecCtx, int64_t bitrate)
    {
        if (bitrate > 0)
            codecCtx->bit_rate = bitrate;

        LOG_INFO("%s, bitrate=%lld", __FUNCTION__, codecCtx->bit_rate);
    }


    template<typename T>
    T FindValue(const T* vals, T prefVal, T termVal, bool useHeadIfNotFound)
    {
        T ret = termVal;
        if (vals)
        {
            bool found = false;
            if (prefVal != termVal)
            {
                for (const T* pval = vals; (*pval != termVal) && !found; ++pval)
                {
                    if (*pval == prefVal)
                    {
                        ret = prefVal;
                        found = true;
                    }
                }
            }
            if (!found && useHeadIfNotFound)
            {
                ret = *vals;
            }
        }
        return ret;
    }


    //static 
    AVPixelFormat CodecTools::FindPixelFormat(const AVPixelFormat* fmts, AVPixelFormat prefFmt)
    {
        return FindValue<AVPixelFormat>(fmts, prefFmt, AV_PIX_FMT_NONE, true);
    }

    AVPixelFormat CodecTools::FindPixelFormat2(const AVPixelFormat* fmts, AVPixelFormat prefFmt)
    {
        return FindValue<AVPixelFormat>(fmts, prefFmt, AV_PIX_FMT_NONE, false);
    }

    // static
    AVPixelFormat CodecTools::FindPixelFormat(const AVCodec* codec, AVPixelFormat prefFmt)
    {
        return FindPixelFormat(GetCodecConfig<AVPixelFormat>(codec) , prefFmt);
    }

    // static
    AVSampleFormat CodecTools::FindSampleFormat(const AVCodec* codec, AVSampleFormat prefFmt)
    {
        return FindValue<AVSampleFormat>(GetCodecConfig<AVSampleFormat>(codec), prefFmt, AV_SAMPLE_FMT_NONE, true);
    }

    template<typename T>
    void LogFmts(const T* fmts)
    {
        if (fmts)
        {
            T term = static_cast<T>(-1);
            std::string sf;
            sf.reserve(128);
            for (const T* fmt = fmts; *fmt != term; ++fmt)
            {
                sf += ToString(*fmt);
                sf += ' ';
            }
            LOG_INFO(" -- %s", sf.c_str());
        }
    }
 
    // static
    void CodecTools::LogVideoCodec(const AVCodec* codec, const char* msg)
    {
        const AVPixelFormat* fmts = GetCodecConfig<AVPixelFormat>(codec);
        LOG_INFO("%s, videocodec: id=%s, name=%s, l.name=%s%s",
            msg, ToString(codec->id), ToLog(codec->name), ToLog(codec->long_name), 
            fmts ? "; pix.fmts:" : "");
        LogFmts(fmts);
    }

    // static
    void CodecTools::LogAudioCodec(const AVCodec* codec, const char* msg)
    {
        const AVSampleFormat* fmts = GetCodecConfig<AVSampleFormat>(codec);
        LOG_INFO("%s, audiocodec: id=%s, name=%s, l.name=%s%s",
            msg, ToString(codec->id), ToLog(codec->name), ToLog(codec->long_name), 
            fmts ? "; samp.fmts:" : "");
        LogFmts(fmts);
    }

    // static
    void CodecTools::LogDecoder(const IDecoder* decoder, const char* msg)
    {
        MediaTypeHolder mth(decoder->MediaType());
        const char* mt = mth.MediaTypeStr();
        bool ok = false;
        if (mth.IsVideo())
        {
            LOG_INFO("%s, %s: frame=%dx%d, pix.fmt=%s",
                msg, mt,
                decoder->Width(), decoder->Height(), ToString(decoder->PixelFormat()));
            ok = true;
        }
        else if (mth.IsAudio())
        {
            LOG_INFO("%s, %s: chann=%d, samp.rate=%d, samp.fmt=%s",
                msg, mt,
                decoder->Chann(),
                decoder->SampleRate(),
                ToString(decoder->SampleFormat()));
            ok = true;
        }
        else
        {
            LOG_ERROR("%s, %s, unexpected media type, custom decoder",
                msg, mt);
        }
        if (ok)
        {
            LOG_INFO("%s, %scodec: custom, name=%s, l.name=%s",
                msg, mt, ToLog(decoder->Name()), ToLog(decoder->Description()));
        }
    }

    // static
    void CodecTools::LogCodecContext(const AVCodecContext* codecCtx, bool logCodec, const char* msg)
    {
        MediaTypeHolder mth(codecCtx);
        const char* mt = mth.MediaTypeStr();
        bool ok = false;
        if (mth.IsVideo())
        {
            LOG_INFO("%s, %s: frame=%dx%d, pix.fmt=%s, fr.rate=%d/%d, t.base=%d/%d",
                msg, mt,
                codecCtx->width, codecCtx->height, ToString(codecCtx->pix_fmt),
                codecCtx->framerate.num, codecCtx->framerate.den,
                codecCtx->time_base.num, codecCtx->time_base.den)
            ok = true;
            if (logCodec)
            {
                LogVideoCodec(codecCtx->codec, msg);
            }
        }
        else if (mth.IsAudio())
        {
            LOG_INFO("%s, %s: chann=%d, samp.rate=%d, samp.fmt=%s, ch.mask=%lld, fr.size=%d, t.base=%d/%d",
                msg, mt,
                //codecCtx->channels,
                codecCtx->ch_layout.nb_channels,
                codecCtx->sample_rate,
                ToString(codecCtx->sample_fmt), 
                //codecCtx->channel_layout,
                codecCtx->ch_layout.u.mask,
                codecCtx->frame_size,
                codecCtx->time_base.num, codecCtx->time_base.den)
                ok = true;
            if (logCodec)
            {
                LogAudioCodec(codecCtx->codec, msg);
            }
        }
        else
        {
            LOG_INFO("%s, %s, codec: id=%s, name=%s, l.name=%s", msg, mt, 
                ToString(codecCtx->codec_id), ToLog(codecCtx->codec->name), ToLog(codecCtx->codec->long_name));
        }
    }

    // static
    void CodecTools::LogCodecParameters(const AVCodecParameters* codecpar, const char* msg)
    {
        MediaTypeHolder mth(codecpar);
        const char* name = ToString(codecpar->codec_id);
        if (mth.IsVideo())
        {
            LOG_ERROR("%s, %s; codec params: frame=%dx%d, pix.fmt=%s, cod.id=%s",
                msg, mth.MediaTypeStr(),
                codecpar->width, codecpar->height,
                PixFmtToStr(codecpar->format),
                name);
        }
        else if (mth.IsAudio())
        {
            LOG_ERROR("%s, %s; codec params: chann=%d, samp.rate=%d, samp.fmt=%s, fr.size=%d, cod.id=%s",
                msg, mth.MediaTypeStr(),
                //codecpar->channels,
                codecpar->ch_layout.nb_channels,
                codecpar->sample_rate,
                SampFmtToStr(codecpar->format),
                codecpar->frame_size,
                name);
        }
        else
        {
            LOG_ERROR("%s, %s, unexpected media type, name=%s", msg, mth.MediaTypeStr(), name);
        }
    }

    // static
    void CodecTools::LogCodecParameters2(const AVCodecParameters* codecpar, const char* msg)
    {
        MediaTypeHolder mth(codecpar);
        auto cid = codecpar->codec_id;
        const char* name = ToString(cid);
        if (mth.IsVideo())
        {
            LOG_INFO("%s, codec params: %s, frame=%dx%d, pix.fmt=%s, cod.id=%s/0x%X",
                msg, mth.MediaTypeStr(),
                codecpar->width, codecpar->height,
                PixFmtToStr(codecpar->format),
                ToString(cid), cid);
        }
        else if (mth.IsAudio())
        {
            LOG_INFO("%s, codec params: %s, chann=%d, samp.rate=%d, samp.fmt=%s, fr.size=%d, cod.id=%s/0x%X",
                msg, mth.MediaTypeStr(),
                codecpar->ch_layout.nb_channels,
                codecpar->sample_rate,
                SampFmtToStr(codecpar->format),
                codecpar->frame_size,
                ToString(cid), cid);
        }
        else
        {
            LOG_ERROR("%s, %s, unexpected media type, name=%s", msg, mth.MediaTypeStr(), name);
        }
    }

} // namespace Fcore


