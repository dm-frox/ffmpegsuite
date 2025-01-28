#include "pch.h"

#include "FormatList.h"

#include <algorithm>
#include <cstring>

#include "Logger.h"

namespace Flist
{

    FormatInfo::FormatInfo(const AVInputFormat* dem, const AVOutputFormat* mux)
        : m_Demux(dem)
        , m_Muxer(mux)
    {}

    FormatInfo::~FormatInfo() = default;

    bool FormatInfo::IsDemuxer() const {         return m_Demux ? true                : false; }
    const char* FormatInfo::Name() const {       return m_Demux ? m_Demux->name       : m_Muxer->name; }
    const char* FormatInfo::LongName() const {   return m_Demux ? m_Demux->long_name  : m_Muxer->long_name; }
    const char* FormatInfo::Mime() const {       return m_Demux ? m_Demux->mime_type  : m_Muxer->mime_type; }
    const char* FormatInfo::Extentions() const { return m_Demux ? m_Demux->extensions : m_Muxer->extensions; }

    const char* FormatInfo::PrivOptions() const 
    { 
        return GetPrivOpts(false, m_PrivOpts);
    }

    const char* FormatInfo::PrivOptionsEx() const 
    { 
        return GetPrivOpts(true, m_PrivOptsEx);
    }

    const char* FormatInfo::GetPrivOpts(bool ex, std::string& optStr) const
    {
        if (optStr.empty())
        {
            if (const AVClass* avc = m_Demux ? m_Demux->priv_class : m_Muxer->priv_class)
            {
                optStr = ex
                    ? Fcore::OptionTools::OptionsToStrEx(
                        avc, Name(), (m_Demux ? "demuxer" : "muxer"))
                    : Fcore::OptionTools::OptionsToStr(avc);

                SetEmptyEx(optStr);
            }
        }

        return optStr.c_str();
    }

    // static
    const char* FormatInfo::GetCtxOptsEx(std::string& optStr)
    {
        if (optStr.empty())
        {
            if (AVFormatContext* fmtCtx = avformat_alloc_context())
            {
                if (const AVClass* avc = fmtCtx->av_class)
                {
                    optStr = Fcore::OptionTools::OptionsToStrEx(avc, "Format context", nullptr);

                    SetEmptyEx(optStr);
                }
                avformat_free_context(fmtCtx);
            }
            else
            {
                LOG_ERROR("%s, avformat_alloc_context", __FUNCTION__);
            }
        }
 
        return optStr.c_str();
    }

// ---------------------------------------------------------------------

    FormatList::FormatList() = default;

    FormatList::~FormatList() = default;

    size_t FormatList::GetCapacity()
    {
        int cntr = 0;
        void* ctx = nullptr;
        while (av_demuxer_iterate(&ctx))
        {
            ++cntr;
        }
        ctx = nullptr;
        while (av_muxer_iterate(&ctx))
        {
            ++cntr;
        }
        return cntr + 1;
    }

    void FormatList::Setup()
    {
        struct Fmt
        {
            const AVInputFormat*  Demux;
            const AVOutputFormat* Muxer;
            const char*           Name;

            Fmt(const AVInputFormat* dem) : Demux(dem), Muxer(nullptr), Name(Demux->name) {}
            Fmt(const AVOutputFormat* mux) : Demux(nullptr), Muxer(mux), Name(Muxer->name) {}
            bool operator<(const Fmt& rh) const { return std::strcmp(Name, rh.Name) < 0; }
        };

        if (IsEmpty())
        {
            std::vector<Fmt> fmts;
            fmts.reserve(GetCapacity());
            void* ctx = nullptr;
            while (const AVInputFormat* pf = av_demuxer_iterate(&ctx))
            {
                fmts.emplace_back(pf);
            }
            ctx = nullptr;
            while (const AVOutputFormat* pf = av_muxer_iterate(&ctx))
            {
                fmts.emplace_back(pf);
            }

            std::stable_sort(fmts.begin(), fmts.end());

            Reserve(fmts.size());
            for (const auto& fmt : fmts)
            {
                List().emplace_back(fmt.Demux, fmt.Muxer);
            }
        }
    }

    const char* FormatList::CtxOptsEx() const
    {
        return FormatInfo::GetCtxOptsEx(m_CtxOptsEx);
    }

// test

    int LogFormatList()
    {
        FormatList formats;
        int n = formats.Count();
        LOG_INFO("%s, format count=%d", __FUNCTION__, n);
        for (int i = 0; i < n; ++i)
        {
            const Fwrap::IFormatInfo* itm = formats[i];
            LOG_INFO(" -- %20s, %s", itm->Name(), itm->IsDemuxer() ? "Demuxer" : "Muxer");
        }
        return n;
    }

} // namespace Flist