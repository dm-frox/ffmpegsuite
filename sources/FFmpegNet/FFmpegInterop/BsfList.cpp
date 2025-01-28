#include "pch.h"

#include "BsfList.h"

#include "Core.h"
#include "Exceptions.h"

namespace FFmpegInterop
{
    BsfInfo::BsfInfo(InfoPrt p)
        : m_Info(p)
    {
        ASSERT_PTR(m_Info);
    }

    StrType^ BsfInfo::ToString()
    {
        return Name;
    }

    StrType^ BsfInfo::Name::get()
    {
        return gcnew StrType(m_Info->Name());
    }

    StrType^ BsfInfo::CodecIds::get()
    {
        return gcnew StrType(m_Info->CodecIds());
    }

    StrType^ BsfInfo::PrivOptions::get()
    {
        return gcnew StrType(m_Info->PrivOptions());
    }

    StrType^ BsfInfo::PrivOptionsEx::get()
    {
        return gcnew StrType(m_Info->PrivOptionsEx());
    }

// -------------------------------------------------------------------

    BsfList::BsfList()
        : BsfListBase(Core::Ptr()->CreateBsfList())
    {
        ASSERT_IMPL();

        Ptr()->Setup();
    }

    int BsfList::Count::get()
    {
        return Ptr()->Count();
    }

    BsfInfo^ BsfList::default::get(int i)
    {
        return gcnew BsfInfo(Ptr()->operator[](i));
    }

    StrType^ BsfList::ContextOptionsEx::get()
    {
        return gcnew StrType(Ptr()->ContextOptionsEx());
    }

} // namespace FFmpegInterop
