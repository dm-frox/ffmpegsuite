#include "pch.h"

#include "Filter.h"

#include "Logger.h"
#include "CommonConsts.h"
#include "Misc.h"
#include "FilterGraph.h"
#include "FilterTools.h"
#include "FrameTools.h"
#include "OptionTools.h"


namespace Fcore
{
    const int FilterBase::MaxInstNameLen = 64;

    FilterBase::FilterBase(AVMediaType mediaType, int idx, FilterGraph& graph)
        : MediaTypeHolder(mediaType)
        , m_Index(idx)
        , m_Graph(graph)
    {}

    FilterBase::~FilterBase() = default;

    bool FilterBase::Assert(const char* msg) const
    {
        bool ret = m_FilterCtx ? true : false;
        if (!ret)
        {
            LOG_ERROR("%s, %s, no filter context", msg, MediaTypeStr());
        }
        return ret;
    }

    bool FilterBase::AssertFirst(const char* msg) const
    {
        return Fcore::AssertFirst(m_FilterCtx, msg);
    }

    int FilterBase::CreateFilter(const char* filtName, const char* instName, const char *args)
    {
        int ret = -1;
        if (const AVFilter* flt = avfilter_get_by_name(filtName))
        {
            AVFilterContext* filterCtx = nullptr;
            void* opaque = nullptr;
            ret = avfilter_graph_create_filter(&filterCtx, flt, instName, args, opaque, m_Graph.Get());
            if (ret >= 0)
            {
                m_FilterCtx = filterCtx;
                if (m_FiltName.empty())
                {
                    m_FiltName = filtName;
                }
                if (m_InstName.empty())
                {
                    m_InstName = instName;
                }
                if (m_Args.empty() && (args && *args))
                {
                    m_Args = args;
                }
            }
            else
            {
                LOG_ERROR("%s, %s, avfilter_graph_create_filter, filt.name=%s, %s", __FUNCTION__, MediaTypeStr(), StrToLog(filtName), FmtErr(ret));
            }
        }
        else
        {
            LOG_ERROR("%s, %s, avfilter_get_by_name, filt.name=%s, %s", __FUNCTION__, MediaTypeStr(), StrToLog(filtName), FmtErr(ret));
        }

        if (ret >= 0)
        {
            ret = m_Index;

            LOG_INFO("%s, %s, idx=%d, filt.name=%s, inst.name=%s, args=%s",
                __FUNCTION__, MediaTypeStr(), m_Index, StrToLog(m_FiltName.c_str()), StrToLog(m_InstName.c_str()), StrToLog(args));
        }
        
        return ret;
    }

    void FilterBase::FreeFilter()
    {
        if (m_FilterCtx)
        {
            m_FilterCtx = nullptr;
        }
    }

    int FilterBase::RecreateFilter()
    {
        if (!AssertFirst(__FUNCTION__))
            return ErrorAlreadyInitialized;

        return CreateFilter(m_FiltName.c_str(), m_InstName.c_str(), EmptyToNull(m_Args.c_str()));
    }

    // static
    int FilterBase::LinkDirect(FilterBase& in, FilterBase& out)
    {
        int ret = avfilter_link(in.m_FilterCtx, 0, out.m_FilterCtx, 0);
        if (ret >= 0)
        {
            LOG_INFO("%s, %s/%s, inst.names=%s/%s", __FUNCTION__, in.MediaTypeStr(), out.MediaTypeStr(), StrToLog(in.m_InstName.c_str()), StrToLog(out.m_InstName.c_str()));
        }
        else
        {
            LOG_ERROR("%s, %s/%s, avfilter_link, inst.names=%s/%s, %s", __FUNCTION__, in.MediaTypeStr(), out.MediaTypeStr(), StrToLog(in.m_InstName.c_str()), StrToLog(out.m_InstName.c_str()), FmtErr(ret));
        }
        return ret;
    }

    void FilterBase::InitInOutItem(AVFilterInOut* fltListItem, char* id) const
    {
        fltListItem->name = id;
        fltListItem->filter_ctx = m_FilterCtx;
        fltListItem->pad_idx = 0;
        fltListItem->next = nullptr;
    }

    int FilterBase::SetPrivOptionAsBin(const char* optName, int optVal)
    {
        int ret = OptionTools::SetOption(m_FilterCtx->priv, optName, &optVal, sizeof(optVal));
        if (ret < 0)
        {
            LOG_ERROR("%s, %s, name=%s, val=%d,  %s", __FUNCTION__, MediaTypeStr(), optName, optVal, FmtErr(ret));
        }
        return ret;
    }

    const char* FilterBase::GenInstName(const char* idBase, const char* cname) const
    {
        static char instName[MaxInstNameLen] = { '\0' };

        instName[0] = '\0';
        av_strlcatf(instName, sizeof(instName), "%s_%s_%d", idBase, cname, m_Index);
        return instName;
    }

    char* FilterBase::GenId(const char* idBase) const
    {
        char buff[16];
        av_strlcpy(buff, idBase, sizeof(buff));
        if (m_Index > 0)
        {
            av_strlcatf(buff, sizeof(buff), "%d", m_Index);
        }
        return av_strdup(buff);
    }

// ---------------------------------------------------------------------

    const char FilterInput::FilterNameVideo[] = "buffer";
    const char FilterInput::FilterNameAudio[] = "abuffer";

    const char FilterInput::IdBase[]          = "in";

    FilterInput::FilterInput(AVMediaType mediaType, int idx, FilterGraph& graph)
        : FilterT<FilterInput>(mediaType, idx, graph)
    {}

    FilterInput::~FilterInput() = default;

    void FilterInput::SetTimebase(AVRational timeBase)
    {
        m_Timebase_N = timeBase.num;
        m_Timebase_D = timeBase.den;
    }

    void FilterInput::GetTimebase(AVRational& timeBase) const
    {
        timeBase.num = m_Timebase_N;
        timeBase.den = m_Timebase_D;
    }

    int FilterInput::CreateFilterVideo(int width, int height, AVPixelFormat pixFmt, AVRational timeBase, AVRational frameRate)
    {
        if (!AssertFirst(__FUNCTION__))
            return ErrorAlreadyInitialized;
        if (!AssertVideo(__FUNCTION__))
            return ErrorWrongMediaType;

        char args[256];
        FilterTools::FillInFilterArgsVideo(width, height, pixFmt, timeBase, frameRate, args, sizeof(args));
        int ret = CreateFilter(args);
        if (ret >= 0)
        {
            SetTimebase(timeBase);
        }

        return ret;
    }

    int FilterInput::CreateFilterAudio(const ChannLayout& channLayout, int sampleRate, AVSampleFormat sampFmt, AVRational timeBase)
    {
        if (!AssertFirst(__FUNCTION__))
            return ErrorAlreadyInitialized;
        if (!AssertAudio(__FUNCTION__))
            return ErrorWrongMediaType;

        char args[256];
        FilterTools::FillInFilterArgsAudio(channLayout, sampleRate, sampFmt, args, sizeof(args));
        int ret = CreateFilter(args);
        if (ret >= 0)
        {
            SetTimebase(timeBase);
        }

        return ret;
    }

    int FilterInput::AddFrame(AVFrame* frame)
    {
        if (!Assert(__FUNCTION__))
            return ErrorNoFiltertCtx;

        int ret = av_buffersrc_add_frame_flags(FilterContext(), frame, AV_BUFFERSRC_FLAG_PUSH);
        if (ret < 0)
        {
            LOG_ERROR("%s, %s, av_buffersrc_add_frame_flags, %s", __FUNCTION__, MediaTypeStr(), FmtErr(ret));
        }

        return ret;
    }

    int FilterInput::SendFrame(AVFrame* frame)
    {
        if (!Assert(__FUNCTION__))
            return ErrorNoFiltertCtx;

        int ret = av_buffersrc_add_frame_flags(FilterContext(), frame, AV_BUFFERSRC_FLAG_PUSH);
        if (ret < 0)
        {
            LOG_ERROR("%s, %s, idx=%d, av_buffersrc_add_frame_flags, %s", __FUNCTION__, MediaTypeStr(), Index(), FmtErr(ret));
        }

        return ret;
    }

// --------------------------------------------------------------------

    const char FilterOutput::FilterNameVideo[] = "buffersink";
    const char FilterOutput::FilterNameAudio[] = "abuffersink";

    const char FilterOutput::IdBase[]          = "out";

    const char FilterOutput::OptNameVideo[]    = "pix_fmts";
    const char FilterOutput::OptNameAudio[]    = "sample_fmts";


    FilterOutput::FilterOutput(AVMediaType mediaType, int idx, FilterGraph& graph)
        : FilterT<FilterOutput>(mediaType, idx, graph)
    {
        m_Frame.Alloc();
    }

    FilterOutput::~FilterOutput() = default;

    int FilterOutput::CreateFilterVideo(AVPixelFormat pixFmt)
    {
        if (!AssertFirst(__FUNCTION__))
            return ErrorAlreadyInitialized;
        if (!AssertVideo(__FUNCTION__))
            return ErrorWrongMediaType;

        int ret = CreateFilter(nullptr);

        if (ret >= 0)
        {
            ret = SetPrivOptionAsBin(OptNameVideo, pixFmt);
        }

        if (ret >= 0)
        {
            m_Format = pixFmt;
            ret = Index();
            LOG_INFO("%s, %s, idx=%d, pix.fmt=%s", __FUNCTION__, MediaTypeStr(), ret, PixFmtToStr(pixFmt));
        }

        return ret;
    }

    int FilterOutput::CreateFilterAudio(AVSampleFormat sampFmt)
    {
        if (!AssertFirst(__FUNCTION__))
            return ErrorAlreadyInitialized;
        if (!AssertAudio(__FUNCTION__))
            return ErrorWrongMediaType;

        int ret = CreateFilter(nullptr);

        if (ret >= 0)
        {
            ret = SetPrivOptionAsBin(OptNameAudio, sampFmt);
        }
        if (ret >= 0)
        {
            m_Format = sampFmt;
            ret = Index();
            LOG_INFO("%s, %s, idx=%d, samp.fmt=%s", __FUNCTION__, MediaTypeStr(), ret, SampFmtToStr(sampFmt));
        }

        return ret;
    }

    int FilterOutput::GetFrame(AVFrame* outFrame)
    {
        if (!Assert(__FUNCTION__))
            return ErrorNoFiltertCtx;
        if (!AssertPtr(outFrame, __FUNCTION__))
            return ErrorBadArgs;

        int ret = av_buffersink_get_frame(FilterContext(), outFrame);

        return ret;
    }

    bool FilterOutput::NoOutputFrames()
    {
        bool ret = false;

        m_Frame.FreeBuffer();
        int res = av_buffersink_get_frame(FilterContext(), m_Frame);

        if (res == AVERROR(EAGAIN)) // no frames
        {
            ret = true;
        }
        else // error
        {
            LOG_ERROR("%s, %s, idx=%d, av_buffersink_get_frame, %s", __FUNCTION__, MediaTypeStr(), Index(), FmtErr(ret));
        }
        return ret;
    }

    int FilterOutput::ReceiveFrame(ISinkFrameFlt* sink)
    {
        if (!Assert(__FUNCTION__))
            return ErrorNoFiltertCtx;
        if (!AssertPtr(sink, __FUNCTION__))
            return ErrorBadArgs;
        if (!AssertPtr(m_Frame, __FUNCTION__))
            return ErrorBadArgs;

        int ret = 0;
        for (int res = 0; res >= 0; )
        {
            m_Frame.FreeBuffer();
            res = av_buffersink_get_frame(FilterContext(), m_Frame);
            if (res >= 0) // OK, frame fetched
            {
                res = sink->ForwardFrame(m_Frame, Index());
            }
            else if (res == AVERROR_EOF) // eof
            {
                sink->ForwardFrame(nullptr, Index());
            }
            else if (res == AVERROR(EAGAIN))
            {
                // no frames
            }
            else // error
            {
                ret = res;
                LOG_ERROR("%s, %s, idx=%d, av_buffersink_get_frame, %s", __FUNCTION__, MediaTypeStr(), Index(), FmtErr(ret));
            }
        }
        return ret;
    }

    int FilterOutput::RemoveFrames()
    {
        if (!Assert(__FUNCTION__))
            return ErrorNoFiltertCtx;
        if (!AssertPtr(m_Frame, __FUNCTION__))
            return ErrorBadArgs;

        int ret = 0;
        while (av_buffersink_get_frame(FilterContext(), m_Frame) >= 0)
        {
            m_Frame.FreeBuffer();
            ++ret;
        }

        return ret;
    }

} // namespace Fcore

