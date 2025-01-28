#include "pch.h"

#include "ProtocolListImpl.h"


namespace Fwrap
{

    ProtocolListImpl::ProtocolListImpl() = default;

    ProtocolListImpl::~ProtocolListImpl() = default;


    void ProtocolListImpl::Setup()
    {
        m_ProtocolList.Setup();
    }

    int ProtocolListImpl::Count() const
    {
        return m_ProtocolList.Count();
    }

    const IProtocolInfo* ProtocolListImpl::operator[](int i) const
    {
        return m_ProtocolList[i];
    }

// --------------------------------------------------------------------
// factory 

    IProtocolList* IProtocolList::CreateInstance()
    {
        return static_cast<IProtocolList*>(new ProtocolListImpl());
    }

} // namespace Fwrap
