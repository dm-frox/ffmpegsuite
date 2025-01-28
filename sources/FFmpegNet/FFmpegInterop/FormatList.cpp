#include "pch.h"

#include "FormatList.h"

#include "Core.h"
#include "Exceptions.h"


namespace FFmpegInterop
{
    FormatInfo::FormatInfo(InfoPrt p)
        : m_Info(p)
    {
        ASSERT_PTR(m_Info);
    }

    StrType^ FormatInfo::ToString()
    {
        return Demuxer ? Name : StrType::Concat(MuxPrefix, Name);
    }

    bool FormatInfo::Demuxer::get()
    {
        return m_Info->IsDemuxer();
    }

    StrType^ FormatInfo::Name::get()
    {
        return gcnew StrType(m_Info->Name());
    }

    StrType^ FormatInfo::LongName::get()
    {
        return gcnew StrType(m_Info->LongName());
    }

    StrType^ FormatInfo::Mime::get()
    {
        return gcnew StrType(m_Info->Mime());
    }

    StrType^ FormatInfo::Extentions::get()
    {
        return gcnew StrType(m_Info->Extentions());
    }

    StrType^ FormatInfo::PrivOptions::get()
    {
        return gcnew StrType(m_Info->PrivOptions());
    }

    StrType^ FormatInfo::PrivOptionsEx::get()
    {
        return gcnew StrType(m_Info->PrivOptionsEx());
    }

// -------------------------------------------------------------------

    FormatList::FormatList()
        : FormatListBase(Core::Ptr()->CreateFormatList())
    {
        ASSERT_IMPL();

        Ptr()->Setup();
    }

    int FormatList::Count::get()
    {
        return Ptr()->Count();
    }

    FormatInfo^ FormatList::default::get(int i)
    {
        return gcnew FormatInfo(Ptr()->operator[](i));
    }

    StrType^ FormatList::ContextOptionsEx::get()
    {
        return gcnew StrType(Ptr()->ContextOptionsEx());
    }

} // namespace FFmpegInterop