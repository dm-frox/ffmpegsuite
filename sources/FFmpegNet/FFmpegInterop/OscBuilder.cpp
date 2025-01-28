#include "pch.h"

#include "OscBuilder.h"

#include "Core.h"
#include "Exceptions.h"
#include "Marshal.h"


namespace FFmpegInterop
{
    OscBuilder::OscBuilder()
        : OscBuilderBase(Core::Ptr()->CreateOscBuilder())
    {
        ASSERT_IMPL();
    }

    void OscBuilder::Build(StrType^ url, StrType^ fmt, StrType^ ops, int oscLen)
    {
        int res = Ptr()->Build(TO_CHARS(url), TO_CHARS(fmt), TO_CHARS(ops), oscLen);
        ASSERT_RES(res);
    }

    PtrType OscBuilder::Data::get()
    {
        const void* p = Ptr()->Data();
        return PtrType(const_cast<void*>(p));
    }

    int OscBuilder::Length::get()
    {
        return Ptr()->Length();
    }

    double OscBuilder::Progress::get()
    {
        return Ptr()->Progress();
    }

    bool OscBuilder::EndOfData::get()
    { 
        return Ptr()->EndOfData();
    }

    double OscBuilder::SourceDuration::get()
    { 
        return Ptr()->SourceDuration();
    }

} // namespace FFmpegInterop

