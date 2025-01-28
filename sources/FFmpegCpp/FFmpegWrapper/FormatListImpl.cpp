#include "pch.h"

#include "FormatListImpl.h"

namespace Fwrap
{
    FormatListImpl::FormatListImpl() = default;

    FormatListImpl::~FormatListImpl() = default;

    void FormatListImpl::Setup()
    {
        m_FormatList.Setup();
    }

    int FormatListImpl::Count() const
    {
        return m_FormatList.Count();
    }

    const IFormatInfo* FormatListImpl::operator[](int i) const
    {
        return m_FormatList[i];
    }

    const char* FormatListImpl::ContextOptionsEx() const
    {
        return m_FormatList.CtxOptsEx();
    }

// --------------------------------------------------------------------
// factory 

    IFormatList* IFormatList::CreateInstance()
    {
        return static_cast<IFormatList*>(new FormatListImpl());
    }

} // namespace Fwrap