#include "pch.h"

#include "ProbeImpl.h"


namespace Fwrap
{
    ProbeImpl::ProbeImpl() = default;

    ProbeImpl::~ProbeImpl() = default;

    const char* ProbeImpl::Version() const
    {
        return m_Probe.Version();
    }

    int ProbeImpl::Do(int par1, const char* par2)
    {
        return m_Probe.Do(par1, par2);
    }

    void ProbeImpl::TestMultiString(const char* par)
    {
        m_Probe.TestMultiString(par);
    }

    void ProbeImpl::SortInt(int* arr, int n)
    {
        m_Probe.SortInt(arr, n);
    }

    void ProbeImpl::SortInt2(const int* inArr, int len, int* outArr)
    {
        m_Probe.SortInt2(inArr, len, outArr);
    }

    int ProbeImpl::LogDevList()
    {
        return m_Probe.LogDevList();
    }

    void ProbeImpl::TestSubtit(const char* path, const char* lang, int count)
    {
        m_Probe.TestSubtit(path, lang, count);
    }

// --------------------------------------------------------------------
// factory 

    IProbe* IProbe::CreateInstance()
    {
        return static_cast<IProbe*>(new ProbeImpl());
    }

} // namespace Fwrap
