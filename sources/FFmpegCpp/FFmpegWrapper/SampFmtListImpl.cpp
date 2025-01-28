#include "pch.h"

#include "SampFmtListImpl.h"

namespace Fwrap
{
    SampFmtListImpl::SampFmtListImpl() = default;

    SampFmtListImpl::~SampFmtListImpl() = default;

    void SampFmtListImpl::Setup() { m_SampFmtList.Setup(); }

    int SampFmtListImpl::Count() const
    {
        return m_SampFmtList.Count();
    }

    const ISampFmtInfo* SampFmtListImpl::operator[](int i) const
    {
        return m_SampFmtList[i];
    }

// --------------------------------------------------------------------
// factory 

    ISampFmtList* ISampFmtList::CreateInstance()
    {
        return static_cast<ISampFmtList*>(new SampFmtListImpl());
    }

} // namespace Fwrap
