#include "pch.h"

#include "PixFmtListImpl.h"

namespace Fwrap
{
    PixFmtListImpl::PixFmtListImpl() = default;

    PixFmtListImpl::~PixFmtListImpl() = default;

    void PixFmtListImpl::Setup()
    {
        m_PixFmtList.Setup();
    }

    int PixFmtListImpl::Count() const
    {
        return m_PixFmtList.Count();
    }

    const IPixFmtInfo* PixFmtListImpl::operator[](int i) const
    {
        return m_PixFmtList[i];
    }

// --------------------------------------------------------------------
// factory 

    IPixFmtList* IPixFmtList::CreateInstance()
    {
        return static_cast<IPixFmtList*>(new PixFmtListImpl());
    }

} // namespace Fwrap
