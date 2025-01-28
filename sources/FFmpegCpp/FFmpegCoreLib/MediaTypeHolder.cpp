#include "pch.h"

#include "MediaTypeHolder.h"

#include "Logger.h"


namespace Fcore
{
    MediaTypeHolder::MediaTypeHolder(AVMediaType mediaType)
        : m_MediaType(mediaType)
        , m_MediaTypeStr(av_get_media_type_string(m_MediaType))
    {}

    MediaTypeHolder::MediaTypeHolder(const AVCodecParameters* codecpar)
        : MediaTypeHolder(codecpar ? codecpar->codec_type : AVMEDIA_TYPE_UNKNOWN)
    {}

    MediaTypeHolder::MediaTypeHolder(const AVStream* strm)
        : MediaTypeHolder(strm ? strm->codecpar : nullptr)
    {}

    MediaTypeHolder::MediaTypeHolder(const AVStream& strm)
        : MediaTypeHolder(strm.codecpar)
    {}

    MediaTypeHolder::MediaTypeHolder(const AVCodec* codec)
        : MediaTypeHolder(codec ? codec->type : AVMEDIA_TYPE_UNKNOWN)
    {}

    MediaTypeHolder::MediaTypeHolder(const AVCodecContext* codecCtx)
        : MediaTypeHolder(codecCtx  ? codecCtx->codec : nullptr)
    {}


    MediaTypeHolder::~MediaTypeHolder() = default;

    bool MediaTypeHolder::AssertVideo(const char* msg) const
    {
        bool ret = IsVideo();
        if (!ret)
        {
            LOG_ERROR("%s: must be video media type", msg);
        }
        return ret;
    }

    bool MediaTypeHolder::AssertAudio(const char* msg) const
    {
        bool ret = IsAudio();
        if (!ret)
        {
            LOG_ERROR("%s: must be audio media type", msg);
        }
        return ret;
    }

    bool MediaTypeHolder::AssertConnect(const MediaTypeHolder* other, const char* msg) const
    {
        bool ret = false;
        if (other)
        {
            ret = m_MediaType == other->m_MediaType;
            if (!ret)
            {
                LOG_ERROR("%s: connect, different media types %s, %s", msg, MediaTypeStr(), other->MediaTypeStr());
            }
        }
        return ret;
    }

} // namespace Fcore

//enum AVMediaType {
//    AVMEDIA_TYPE_UNKNOWN = -1,  ///< Usually treated as AVMEDIA_TYPE_DATA
//    AVMEDIA_TYPE_VIDEO,
//    AVMEDIA_TYPE_AUDIO,
//    AVMEDIA_TYPE_DATA,          ///< Opaque data information usually continuous
//    AVMEDIA_TYPE_SUBTITLE,
//    AVMEDIA_TYPE_ATTACHMENT,    ///< Opaque data information usually sparse
//    AVMEDIA_TYPE_NB
//};


