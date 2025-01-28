#include "pch.h"

#include "ChannListImpl.h"

namespace Fwrap
{
    ChannListImpl::ChannListImpl() = default;

    ChannListImpl::~ChannListImpl() = default;

    void ChannListImpl::Setup()
    {
        m_ChannList.Setup();
    }

    int ChannListImpl::Count() const
    {
        return m_ChannList.Count();
    }

    const IChannInfo* ChannListImpl::operator[](int i) const
    {
        return m_ChannList[i];
    }

// --------------------------------------------------------------------
// factory 

    IChannList* IChannList::CreateInstance()
    {
        return static_cast<IChannList*>(new ChannListImpl());
    }

} // namespace Fwrap
