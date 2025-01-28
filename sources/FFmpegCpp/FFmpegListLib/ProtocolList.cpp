#include "pch.h"

#include "ProtocolList.h"

#include <set>

#include "ffmpeg.h"
#include "Logger.h"
#include "OptionTools.h"


namespace Flist
{
    ProtocolInfo::ProtocolInfo(const char* name)
        : m_Name(name)
        , m_IsInput(false)
        , m_IsOutput(false)
    {}

    ProtocolInfo::ProtocolInfo(const ProtocolInfo& itm) = default;

    ProtocolInfo::~ProtocolInfo() = default;

    const char* ProtocolInfo::Name() const { return m_Name; }
    bool ProtocolInfo::IsInput() const { return m_IsInput; }
    bool ProtocolInfo::IsOutput() const { return m_IsOutput; }

    const char* ProtocolInfo::PrivOptions() const 
    { 
        return GetPrivOpts(false, m_PrivOpts);
    }

    const char* ProtocolInfo::PrivOptionsEx() const 
    { 
        return GetPrivOpts(true, m_PrivOptsEx);
    }

    const char* ProtocolInfo::GetPrivOpts(bool ex, std::string& optStr) const
    {
        if (optStr.empty())
        {
            if (const AVClass* avc = avio_protocol_get_class(m_Name))
            {
                optStr = ex
                    ? Fcore::OptionTools::OptionsToStrEx(avc, m_Name, "protocol")
                    : Fcore::OptionTools::OptionsToStr(avc);
            }

            SetEmptyEx(optStr);
        }

        return optStr.c_str();
    }

// ---------------------------------------------------------------------

    ProtocolList::ProtocolList() = default;

    ProtocolList::~ProtocolList() = default;

    void ProtocolList::Setup()
    {
        if (IsEmpty())
        {
            std::set<ProtocolInfo> protos;
            int inc = 0, outc = 0;
            void* ctx = nullptr;
            int output = 0;
            while (const char* name = avio_enum_protocols(&ctx, output))
            {
                protos.emplace(name).first->SetInput();
                ++inc;
            }
            ctx = nullptr;
            output = 1;
            while (const char* name = avio_enum_protocols(&ctx, output))
            {
                protos.emplace(name).first->SetOutput();
                ++outc;
            }

            Reserve(protos.size());
            for (const auto& proto : protos)
            {
                List().emplace_back(proto);
            }

            LOG_INFO("%s, proto count=%d  (%d/%d)", __FUNCTION__, Count(), inc, outc);
        }
    }

// test

    void LogProtocolList()
    {
        ProtocolList plst;
        plst.Setup();
        for (int i = 0, n = plst.Count(); i < n; ++i)
        {
            const Fwrap::IProtocolInfo* itm = plst[i];
            LOG_INFO(" -- %-12s %d/%d", itm->Name(), itm->IsInput(), itm->IsOutput());
        }
    }

} // namespace Flist

