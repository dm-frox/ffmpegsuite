#include "pch.h"

#include "HdrReader.h"

#include "Core.h"
#include "Exceptions.h"
#include "Marshal.h"


namespace FFmpegInterop
{
    HdrReader::HdrReader()
        : HdrReaderBase(Core::Ptr()->CreateHdrReader())
    {
        ASSERT_IMPL();
    }

    void HdrReader::Open(StrType^ url, StrType^ fmt, StrType^ ops)
    {
        int res = Ptr()->Open(TO_CHARS(url), TO_CHARS(fmt), TO_CHARS(ops));

        ASSERT_RES(res);
    }

    int HdrReader::StreamCount::get()
    { 
        return Ptr()->StreamCount();
    }

    StrType^ HdrReader::GetInfo()
    {
        return NativeString::GetStringEx(Ptr()->GetInfo());
    }

    StrType^ HdrReader::GetMetadata()
    {
        return NativeString::GetStringEx(Ptr()->GetMetadata());
    }

    StrType^ HdrReader::GetChapters()
    {
        return NativeString::GetStringEx(Ptr()->GetChapters());
    }

    StrType^ HdrReader::GetStreamInfo(int strmIdx)
    {
        return NativeString::GetStringEx(Ptr()->GetStreamInfo(strmIdx));
    }

    StrType^ HdrReader::GetStreamMetadata(int strmIdx)
    {
        return NativeString::GetStringEx(Ptr()->GetStreamMetadata(strmIdx));
    }

} // namespace FFmpegInterop

