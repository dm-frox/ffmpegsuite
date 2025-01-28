#include "pch.h"

#include "FilterList.h"

#include "Logger.h"
#include "Misc.h"
#include "OptionTools.h"


namespace Flist
{
    // static
    inline bool FilterInfo::IsVideo(int count, const AVFilterPad* pads)
    {
        return (count > 0 && pads) ? (avfilter_pad_get_type(pads, 0) == AVMEDIA_TYPE_VIDEO) : false;
    }

    FilterInfo::FilterInfo(const AVFilter* filter)
        : m_Name(filter->name)
        , m_Descr(filter->description)
        , m_InCount(avfilter_filter_pad_count(filter, 0))
        , m_OutCount(avfilter_filter_pad_count(filter, 1))
        , m_InVideo(IsVideo(m_InCount, filter->inputs))
        , m_OutVideo(IsVideo(m_OutCount, filter->outputs))
        , m_Cmd(filter->process_command ? true : false)
    {}

    FilterInfo::~FilterInfo() = default;

    const char* FilterInfo::Name() const { return m_Name; }
    const char* FilterInfo::Descr() const { return m_Descr; }
    int FilterInfo::InCount() const { return m_InCount; }
    int FilterInfo::OutCount() const { return m_OutCount; }
    bool FilterInfo::InVideo() const { return m_InVideo; }
    bool FilterInfo::OutVideo() const { return m_OutVideo; }
    bool FilterInfo::Cmd() const { return m_Cmd; }

    const char* FilterInfo::PrivOptions() const
    {
        return GetOpts(true, false, m_PrivOpts);
    }

    const char* FilterInfo::PrivOptionsEx() const
    {
        return GetOpts(true, true, m_PrivOptsEx);
    }

    const char* FilterInfo::GetOpts(bool priv, bool ex, std::string& optStr) const
    {
        if (optStr.empty())
        {
            if (auto fg = avfilter_graph_alloc())
            {
                if (const AVFilter* flt = avfilter_get_by_name(m_Name))
                {
                    AVFilterContext* filterCtx = nullptr;
                    void* opaque = nullptr;
                    std::string instName = m_Name;
                    instName += "_0";
                    int ret = avfilter_graph_create_filter(&filterCtx, flt, instName.c_str(), "", opaque, fg);
                    if (ret >= 0)
                    {
                        const AVClass* avc = priv
                            ? Fcore::OptionTools::GetContextClass(filterCtx->priv)
                            : filterCtx->av_class;

                        optStr = ex
                            ? Fcore::OptionTools::OptionsToStrEx(
                                avc,
                                priv ? m_Name : "Filter context",
                                priv ? "filter" : nullptr)
                            : Fcore::OptionTools::OptionsToStr(avc);
                    }
                    else
                    {
                        LOG_ERROR("%s, name=%s, %s", __FUNCTION__, m_Name, Fcore::FmtErr(ret));
                    }
                    avfilter_graph_free(&fg);
                }
            }
            SetEmptyEx(optStr);
        }
        return optStr.c_str();
    }

// ----------------------------------------------------------------------------------

    FilterList::FilterList() = default;
    FilterList::~FilterList() = default;

    // static
    size_t FilterList::GetCapacity()
    {
        int cntr = 0;
        void* ctx = nullptr;
        while (av_filter_iterate(&ctx))
        {
            ++cntr;
        }
        return cntr + 1;
    }

    void FilterList::Setup()
    {
        if (IsEmpty())
        {
            Reserve(GetCapacity());
            void* ctx = nullptr;
            while (const AVFilter* filter = av_filter_iterate(&ctx))
            {
                List().emplace_back(filter);
            }
        }
    }

    const char* FilterList::CtxOptsEx() const
    {
        return !IsEmpty() ? List()[0].GetOpts(false, true, m_CtxOptsEx) : "";
    }

// test

    void LogFilterList()
    {
        LOG_TEMP("Filter list:");
        int i = 0;
        void* ctx = nullptr;
        while (const AVFilter* filter = av_filter_iterate(&ctx))
        {
            AVMediaType tin = AVMEDIA_TYPE_UNKNOWN;
            int cin = 0;
            if (const AVFilterPad* pads = filter->inputs)
            {
                tin = avfilter_pad_get_type(pads, 0);
                cin = avfilter_filter_pad_count(filter, 0);
            }

            AVMediaType tout = AVMEDIA_TYPE_UNKNOWN;
            int cout = 0;
            if (const AVFilterPad* pads = filter->outputs)
            {
                tout = avfilter_pad_get_type(pads, 0);
                cout = avfilter_filter_pad_count(filter, 1);
            }

            bool comm = filter->process_command ? true : false;

            LOG_INFO(" -- %3d, %20s, %s/%s, %d/%d, %s    %s",
                i++, filter->name, Fcore::MediaTypeToStr2(tin), Fcore::MediaTypeToStr2(tout), cin, cout, comm ? "c" : " ", filter->description);
        }
    }

} // namespace Flist
