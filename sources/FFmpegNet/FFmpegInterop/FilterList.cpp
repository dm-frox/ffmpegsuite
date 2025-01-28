#include "pch.h"

#include "FilterList.h"

#include "Core.h"
#include "Exceptions.h"


namespace FFmpegInterop
{

    FilterInfo::FilterInfo(InfoPrt p)
        : m_Info(p)
    {
        ASSERT_PTR(m_Info);
    }

    StrType^ FilterInfo::Name::get()
    {
        return gcnew StrType(m_Info->Name());
    }

    StrType^ FilterInfo::Descr::get()
    {
        return gcnew StrType(m_Info->Descr());
    }

    int FilterInfo::InCount::get()
    {
        return m_Info->InCount();
    }

    int FilterInfo::OutCount::get()
    {
        return m_Info->OutCount();
    }

    bool FilterInfo::InVideo::get()
    {
        return m_Info->InVideo();
    }

    bool FilterInfo::OutVideo::get()
    {
        return m_Info->OutVideo();
    }

    bool FilterInfo::Cmd::get()
    {
        return m_Info->Cmd();
    }

    StrType^ FilterInfo::PrivOptions::get()
    {
        return gcnew StrType(m_Info->PrivOptions());
    }

    StrType^ FilterInfo::PrivOptionsEx::get()
    {
        return gcnew StrType(m_Info->PrivOptionsEx());
    }

// -------------------------------------------------------------------

    FilterList::FilterList()
        : FilterListBase(Core::Ptr()->CreateFilterList())
    {
        ASSERT_IMPL();

        Ptr()->Setup();
    }

    int FilterList::Count::get()
    {
        return Ptr()->Count();
    }

    FilterInfo^ FilterList::default::get(int i)
    {
        return gcnew FilterInfo(Ptr()->operator[](i));
    }

    StrType^ FilterList::ContextOptionsEx::get()
    {
        return gcnew StrType(Ptr()->ContextOptionsEx());
    }

} // namespace FFmpegInterop