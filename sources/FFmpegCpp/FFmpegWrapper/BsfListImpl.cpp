#include "pch.h"

#include "BsfListImpl.h"

namespace Fwrap
{
    BsfListImpl::BsfListImpl() = default;

    BsfListImpl::~BsfListImpl() = default;

    void BsfListImpl::Setup()
    {
        m_BsfList.Setup();
    }

    int BsfListImpl::Count() const
    {
        return m_BsfList.Count();
    }

    const IBsfInfo* BsfListImpl::operator[](int i) const
    {
        return m_BsfList[i];
    }

    const char* BsfListImpl::ContextOptionsEx() const
    {
        return m_BsfList.CtxOptsEx();
    }

// --------------------------------------------------------------------
// factory 

    IBsfList* IBsfList::CreateInstance()
    {
        return static_cast<IBsfList*>(new BsfListImpl());
    }

} // namespace Fwrap

