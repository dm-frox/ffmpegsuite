#include "pch.h"

#include "FilterListImpl.h"

namespace Fwrap
{
    FilterListImpl::FilterListImpl() = default;

    FilterListImpl::~FilterListImpl() = default;

    void FilterListImpl::Setup()
    {
        m_FilterList.Setup();
    }

    int FilterListImpl::Count() const
    {
        return m_FilterList.Count();
    }

    const IFilterInfo* FilterListImpl::operator[](int i) const
    {
        return m_FilterList[i];
    }

    const char* FilterListImpl::ContextOptionsEx() const
    {
        return m_FilterList.CtxOptsEx();
    }

// --------------------------------------------------------------------
// factory 

    IFilterList* IFilterList::CreateInstance()
    {
        return static_cast<IFilterList*>(new FilterListImpl());
    }

} // namespace Fwrap