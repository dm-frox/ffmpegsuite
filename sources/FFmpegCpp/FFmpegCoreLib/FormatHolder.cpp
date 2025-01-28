#include "pch.h"

#include "FormatHolder.h"

#include "ffmpeg.h"
#include "Logger.h"
#include "Misc.h"


namespace Fcore
{

    FormatHolder::FormatHolder() = default;
 
    FormatHolder::~FormatHolder()
    {
        avformat_free_context(m_FmtCtx);
    }

    bool FormatHolder::AssertFormat(const char* msg) const
    {
        bool ret = m_FmtCtx ? true : false;
        if (!ret)
        {
            LOG_ERROR("%s, %s", msg, "no format context");
        }
        return ret;
    }

    bool FormatHolder::AssertStreams(const char* msg) const
    {
        bool ret = m_FmtCtx && m_FmtCtx->nb_streams > 0;
        if (!ret)
        {
            LOG_ERROR("%s, %s", msg, "no opened streams");
        }
        return ret;
    }

    bool FormatHolder::AssertFirst(const char* msg) const
    {
        return Fcore::AssertFirst(m_FmtCtx, msg);
    }

    void FormatHolder::DumpFormat(bool output) const
    {
        if (m_FmtCtx)
        {
            LOG_INFO("%s, begin, streams count=%d", __FUNCTION__, m_FmtCtx->nb_streams);
            LOG_INFO("%s", "av_dump_format <<<<<");
            av_dump_format(m_FmtCtx, 0, m_FmtCtx->url, output ? 1 : 0);
            LOG_INFO(">>>>>");
        }
    }

} // namespace Fcore