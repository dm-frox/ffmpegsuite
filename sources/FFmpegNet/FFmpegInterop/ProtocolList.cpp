#include "pch.h"

#include "ProtocolList.h"

#include "Core.h"
#include "Exceptions.h"

namespace FFmpegInterop
{
    ProtocolInfo::ProtocolInfo(InfoPrt p)
        : m_Info(p)
    {
        ASSERT_PTR(m_Info);
    }

    StrType^ ProtocolInfo::ToString()
    {
        return Name;
    }

    StrType^ ProtocolInfo::Name::get()
    {
        return gcnew StrType(m_Info->Name());
    }

    bool ProtocolInfo::IsInput::get()
    {
        return m_Info->IsInput();
    }

    bool ProtocolInfo::IsOutput::get()
    {
        return m_Info->IsOutput();
    }

    StrType^ ProtocolInfo::PrivOptions::get()
    {
        return gcnew StrType(m_Info->PrivOptions());
    }

    StrType^ ProtocolInfo::PrivOptionsEx::get()
    {
        return gcnew StrType(m_Info->PrivOptionsEx());
    }

// -------------------------------------------------------------------

    ProtocolList::ProtocolList()
        : ProtocolListBase(Core::Ptr()->CreateProtocolList())
    {
        ASSERT_IMPL();

        Ptr()->Setup();
    }

    int ProtocolList::Count::get()
    {
        return Ptr()->Count();
    }

    ProtocolInfo^ ProtocolList::default::get(int i)
    {
        return gcnew ProtocolInfo(Ptr()->operator[](i));
    }

} // namespace FFmpegInterop
