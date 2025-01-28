#include "pch.h"

#include <cstdio>

#include "windefs.h"

#ifdef WINDOWS_

#include "windows_std.h"

#else // unix zoo

#include <sys/utsname.h>
#include <cstring>
#include <cerrno>

#endif // WINDOWS_


namespace LogTools
{

#ifdef WINDOWS_
#pragma warning(disable:28159 4996)
    void WriteOsInfo(FILE* file)
    {
        OSVERSIONINFOW vi;
        ::memset(&vi, 0, sizeof vi);
        vi.dwOSVersionInfoSize = sizeof vi;
        if (::GetVersionExW(&vi))
        {
            DWORD_PTR affMaskProc = 0, affMaskComp = 0;
            ::GetProcessAffinityMask(::GetCurrentProcess(), &affMaskProc, &affMaskComp);
            std::fprintf(file, "OS Windows: version=%d.%d, build=%d (%S); affinity(proc/comp)=0x%llX/0x%llX",
                vi.dwMajorVersion, vi.dwMinorVersion, vi.dwBuildNumber, vi.szCSDVersion,
                (uint64_t)affMaskProc, (uint64_t)affMaskComp);
        }
        else
        {
            int code = ::GetLastError();
            std::fprintf(file, "GetVersionEx error, code=%d", code);
        }
#pragma warning(default:28159 4996)
    }

    void WriteDefs(FILE* file)
    {
        bool win32_ = false, w32 = false, w64 = false;
#ifdef WIN32
        win32_ = true;
#endif
#ifdef _WIN32
        w32 = true;
#endif
#ifdef _WIN64
        w64 = true;
#endif

        std::fprintf(file, "Windows defs: WIN32=%d, _WIN32=%d, _WIN64=%d", win32_, w32, w64);
    }

#else // unix zoo

    void WriteOsInfo(FILE* file)
    {
        struct utsname un;
        ::memset(&un, 0, sizeof un);
        if (::uname(&un) == 0)
        {
            std::fprintf(file, "OS: %s, release=%s, version=%s; machine=%s",
                un.sysname, un.release, un.version, un.machine);
        }
        else
        {
            int code = errno;
            std::fprintf(file, "uname error, code=%d", code);
        }
    }

    void WriteDefs(FILE* file)
    {
        std::fprintf(file, "UNIX ZOO");
    }

//  OS: Linux, release = 3.19.0 - 25 - generic, version = #26~14.04.1 - Ubuntu SMP Fri Jul 24 21:16 : 20 UTC 2015; machine = x86_64

#endif // WINDOWS_

    void WriteCppVers(FILE* file)
    {
        std::fprintf(file, "C++ version=%d", __cplusplus);
    }

} // namespace LogTools
