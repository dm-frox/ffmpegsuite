#include "pch.h"

#include "OscBuilderImpl.h"


namespace Fwrap
{

    OscBuilderImpl::OscBuilderImpl() = default;

    OscBuilderImpl::~OscBuilderImpl() = default;

    int OscBuilderImpl::Build(const char* url, const char* fmt, const char* ops, int oscLen)
    {
        return m_OscBuilder.Build(url, fmt, ops, oscLen);
    }

    const void* OscBuilderImpl::Data() const
    {
        return m_OscBuilder.Data();
    }

    int OscBuilderImpl::Length() const
    {
        return m_OscBuilder.Length();
    }

    double OscBuilderImpl::Progress() const
    {
        return m_OscBuilder.Progress();
    }

    bool OscBuilderImpl::EndOfData() const
    {
        return m_OscBuilder.EndOfData();
    }

    double OscBuilderImpl::SourceDuration() const
    {
        return m_OscBuilder.SourceDuration();
    }

// --------------------------------------------------------------------
// factory 

    IOscBuilder* IOscBuilder::CreateInstance()
    {
        return static_cast<IOscBuilder*>(new OscBuilderImpl());
    }

} // namespace Fwrap
