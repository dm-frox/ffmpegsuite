#include "pch.h"

#include "SampFmtList.h"

#include "Core.h"
#include "Exceptions.h"


namespace FFmpegInterop
{
    SampFmtInfo::SampFmtInfo(InfoPrt p)
        : m_Info(p)
    {
        ASSERT_PTR(m_Info);
    }

    StrType^ SampFmtInfo::Name::get()
    {
        return gcnew StrType(m_Info->Name());
    }

    StrType^ SampFmtInfo::Descr::get()
    {
        return gcnew StrType(m_Info->Descr());
    }

    int SampFmtInfo::BytesPerSamp::get()
    {
        return m_Info->BytesPerSamp();
    }

    bool SampFmtInfo::IsPlanar::get()
    {
        return m_Info->IsPlanar();
    }

// -------------------------------------------------------------------

    SampFmtList::SampFmtList()
        : SampFmtListBase(Core::Ptr()->CreateSampFmtList())
    {
        ASSERT_IMPL();

        Ptr()->Setup();
    }

    int SampFmtList::Count::get()
    {
        return Ptr()->Count();
    }

    SampFmtInfo^ SampFmtList::default::get(int i)
    {
        return gcnew SampFmtInfo(Ptr()->operator[](i));
    }

} // namespace FFmpegInterop
