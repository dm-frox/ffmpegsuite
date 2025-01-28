#include "pch.h"

#include "Probe.h"

#include "Core.h"
#include "Marshal.h"
#include "Exceptions.h"


namespace FFmpegInterop
{
    Probe::Probe()
        : ProbeBase(Core::Ptr()->CreateProbe())
    {
        ASSERT_IMPL();
    }

    StrType^ Probe::Version()
    {
        return NativeString::GetString(Ptr()->Version(), 0);
    }

    int Probe::Do(int param1, StrType^ param2)
    {
        return Ptr()->Do(param1, TO_CHARS(param2));
    }

    void Probe::TestMultiString(StrType^ ms)
    {
        NativeString s(ms);

        Ptr()->TestMultiString(s.CStr());
    }

    int Probe::LogDevList()
    {
        return Ptr()->LogDevList();
    }

    void Probe::TestSubtit(StrType^ path, StrType^ lang, int count)
    {
        Ptr()->TestSubtit(TO_CHARS(path), TO_CHARS(lang), count);
    }

    array<int>^ Probe::SortInt(array<int>^)
    {
         return nullptr;
    }

    void Probe::SortInt2(array<int>^, array<int>^)
    {
    }

} // namespace FFmpegInterop
