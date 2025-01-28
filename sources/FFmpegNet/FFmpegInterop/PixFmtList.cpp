#include "pch.h"

#include "PixFmtList.h"

#include "Core.h"
#include "Exceptions.h"


namespace FFmpegInterop
{
    PixFmtInfo::PixFmtInfo(InfoPrt p)
        : m_Info(p)
    {
        ASSERT_PTR(m_Info);
    }

    StrType^ PixFmtInfo::Name::get()
    {
        return gcnew StrType(m_Info->Name());
    }

    StrType^ PixFmtInfo::Descr::get()
    {
        return gcnew StrType(m_Info->Descr());
    }

    int PixFmtInfo::CompCount::get()
    {
        return m_Info->CompCount();
    }

    int PixFmtInfo::PlaneCount::get()
    {
        return m_Info->PlaneCount();
    }

    int PixFmtInfo::BitPerPix::get()
    {
        return m_Info->BitPerPix();
    }

    bool PixFmtInfo::HWAccel::get()
    {
        return m_Info->HWAccel();
    }

    bool PixFmtInfo::Palette::get()
    {
        return m_Info->Palette();
    }

    bool PixFmtInfo::Bitstream::get()
    {
        return m_Info->Bitstream();
    }

    uint32_t PixFmtInfo::CompDepth::get()
    {
        return m_Info->CompDepth();
    }

 // -------------------------------------------------------------------

    PixFmtList::PixFmtList()
        : PixFmtListBase(Core::Ptr()->CreatePixFmtList())
    {
        ASSERT_IMPL();

        Ptr()->Setup();
    }

    int PixFmtList::Count::get()
    {
        return Ptr()->Count();
    }

    PixFmtInfo^ PixFmtList::default::get(int i)
    {
        return gcnew PixFmtInfo(Ptr()->operator[](i));
    }

} // namespace FFmpegInterop
