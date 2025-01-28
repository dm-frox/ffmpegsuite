#include "pch.h"

#include "ChannList.h"

#include "Core.h"
#include "Exceptions.h"

namespace FFmpegInterop
{
    ChannInfo::ChannInfo(InfoPrt p)
        : m_Info(p)
    {
        ASSERT_PTR(m_Info);
    }

    StrType^ ChannInfo::Name::get()
    {
        return gcnew StrType(m_Info->Name());
    }

    StrType^ ChannInfo::Descr::get()
    {
        return gcnew StrType(m_Info->Descr());
    }

    int ChannInfo::Chann::get()
    {
        return m_Info->Chann();
    }

    long long ChannInfo::Mask::get()
    {
        return m_Info->Mask();
    }

// -------------------------------------------------------------------

    ChannList::ChannList()
        : ChannListBase(Core::Ptr()->CreateChannList())
    {
        ASSERT_IMPL();

        Ptr()->Setup();
    }

    int ChannList::Count::get()
    {
        return Ptr()->Count();
    }

    ChannInfo^ ChannList::default::get(int i)
    {
        return gcnew ChannInfo(Ptr()->operator[](i));
    }

} // namespace FFmpegInterop
