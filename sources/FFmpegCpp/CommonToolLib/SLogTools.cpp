#include "pch.h"

#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <cerrno>
#include <chrono>
#include <thread>

#include "pchar.h"

#ifdef WINDOWS_

#include <io.h>

#else // unix zoo

#include <unistd.h>

#endif // WINDOWS_


namespace
{
    const int MaxBackupCountThreshold = 99;

// platform depended stuff
#ifdef WINDOWS_ 

    const int MaxPathLen = 260;

    int StrLen_(const wchar_t* str)
    {
        return (int)std::wcslen(str);
    }

    int Str2Int_(const wchar_t* src)
    {
        return ::_wtoi(src);
    }

    bool DeleteFile_(const wchar_t* path)
    {
        return (::_wremove(path) == 0);
    }

    bool MoveFile_(const wchar_t* path, const wchar_t* newPath)
    {
        return (::_wrename(path, newPath) == 0);
    }

#ifdef VISUAL_STUDIO_

    void str_n_cpy_(wchar_t* dst, int dstLen, const wchar_t* src)
    {
#pragma warning(disable:4996)
        std::wcsncpy(dst, src, dstLen);
#pragma warning(default:4996)
    }

    int AccessFile_(const wchar_t* path, int mode)
    {
        return ::_waccess_s(path, mode);
    }

    bool GetLocalTime_(std::time_t tt, std::tm& loct)
    {
        return ::localtime_s(&loct, &tt) == 0;
    }

    FILE* OpenFile_(const wchar_t* path)
    {
        FILE* ret = nullptr;
        bool rr = ::_wfopen_s(&ret, path, L"wt") == 0;
        return rr ? ret : nullptr;
    }

#else // mingw

    void str_n_cpy_(wchar_t* dst, int dstLen, const wchar_t* src)
    {
        std::wcsncpy(dst, src, dstLen);
    }

    int AccessFile_(const wchar_t* path, int mode)
    {
        return (::_waccess(path, mode) == 0) ? 0 : errno;
    }

    FILE* OpenFile_(const wchar_t* path)
    {
        return ::_wfopen(path, L"wt");
    }

#endif // VISUAL_STUDIO_

#else // unix zoo

    const int MaxPathLen = 400;
  
    int StrLen_(const char* str)
    {
        return (int)std::strlen(str);
    }

    int Str2Int_(const char* src)
    {
        return std::atoi(src);
    }

    void str_n_cpy_(char* dst, int dstLen, const char* src)
    {
        std::strncpy(dst, src, dstLen);
    }

    int AccessFile_(const char* path, int mode)
    {
        return (::access(path, mode) == 0) ? 0 : errno;
    }

    FILE* OpenFile_(const char* path)
    {
        return std::fopen(path, "wt");
    }

    bool DeleteFile_(const char* path)
    {
        return (::remove(path) == 0);
    }

    bool MoveFile_(const char* path, const char* newPath)
    {
        return (::rename(path, newPath) == 0);
    }

#endif // WINDOWS_

#ifndef VISUAL_STUDIO_ // mingw | unix zoo

    bool GetLocalTime_(std::time_t tt, std::tm& loct)
    {
        if (auto p = std::localtime(&tt))
        {
            loct = *p;
            return true;
        }
        return false;
    }

#endif // VISUAL_STUDIO_

    // truncate src if it length more than dstLen
    void StrCpy(pchar_t* dst, int dstLen, const pchar_t* src)
    {
        if (dstLen > 0)
        {
            str_n_cpy_(dst, dstLen, src);
            dst[dstLen - 1] = pchar_t(0);
        }
    }

    void StrCat(pchar_t* dst, int dstLen, const pchar_t* src)
    {
        if (dstLen > 0)
        {
            int len = StrLen_(dst);
            if (len < dstLen)
            {
                StrCpy(dst + len, dstLen - len, src);
            }
        }
    }

    template <size_t N>
    void StrCat(pchar_t(&dst)[N], const pchar_t* src)
    {
        StrCat(&dst[0], N, src);
    }

    int FileExists(const pchar_t* path)
    {
        int ret = -1;
        int mode = 0; // existence only
        int r = AccessFile_(path, mode);
        if (r == 0) // exists
        {
            ret = 1;
        }
        else if (r == ENOENT) // file name or path not found
        {
            ret = 0;
        }
        return ret;
    }

    bool IsDigit(const pchar_t* p)
    {
        enum : int { Null = '0', Nine = '9' };

        int c = *p;
        return Null <= c && c <= Nine;
    }

    int FindDotIdx(const pchar_t* path)
    {
        static const int DOT = '.';
        static const int BS  = '\\';
        static const int SL  = '/';

        int dotIdx = -1;
        int i = StrLen_(path) - 1;
        for (const pchar_t* b = path + i; i >= 0 && dotIdx < 0; --i, --b)
        {
            int c = *b;
            if (c == DOT)
            {
                dotIdx = i;
            }
            else if (c == BS || c == SL) // file must have an extention
            {
                break;
            }
        }
        return dotIdx;
    }

    bool GenBackupPath(const pchar_t* path, pchar_t* newPath, int buffLen)
    {
        static const int BackupSuffixLen = 3; // '_NN'
        static const int MaxExtentionLen = 60;
        static const pchar_t X0          = pchar_t('_');

        bool ret = false;

        int rest = buffLen - StrLen_(path);

        if (rest <= BackupSuffixLen)
            return ret;

        StrCpy(newPath, buffLen, path);
        int dotIdx = FindDotIdx(newPath);
        if (dotIdx > 0) // file must have an extention
        {
            pchar_t* dotPos = newPath + dotIdx;
            pchar_t* p0 = dotPos - 3;
            pchar_t* p1 = dotPos - 2;
            pchar_t* p2 = dotPos - 1;
            bool backuped = 
                (newPath < p0) && (*p0 == X0) &&
                IsDigit(p1) && IsDigit(p2);
            int bnum = (backuped ? Str2Int_(p1) : 0) + 1;
            if (bnum <= MaxBackupCountThreshold)
            {
                pchar_t x1 = pchar_t('0' + bnum / 10);
                pchar_t x2 = pchar_t('0' + bnum % 10);
                if (backuped)
                {
                    *p1 = x1;
                    *p2 = x2;
                }
                else
                {
                    pchar_t tail[BackupSuffixLen + MaxExtentionLen + 1] = { X0, x1, x2, pchar_t(0) };
                    StrCat(tail, dotPos);
                    StrCpy(dotPos, rest, tail);
                }
                ret = true;
            }
        }

        return ret;
    }

    bool BackupFiles_(const pchar_t* path, int lev, int maxLevel) // lev = 0, ...
    {
        bool ret = false;
        if (path && *path && maxLevel > 0)
        {
            int rr = FileExists(path);
            if (rr > 0) // exists
            {
                if (lev < maxLevel)
                {
                    pchar_t newPath[MaxPathLen];
                    if (GenBackupPath(path, newPath, MaxPathLen))
                    {
                        if (BackupFiles_(newPath, lev + 1, maxLevel)) // recursion
                        {
                            ret = MoveFile_(path, newPath);
                        }
                    }
                }
                else // the last file in the backup chain
                {
                    ret = DeleteFile_(path);
                }
            }
            else if (rr == 0) // does not exist
            {
                ret = true;
            }
        }
        return ret;
    }

} // namespace anon

namespace LogTools
{
    bool BackupFiles(const pchar_t* path, int maxLevel)
    { 
        return BackupFiles_(path, 0, maxLevel); 
    }

    bool CheckMaxBackupCount(int count)
    {
        return 0 < count && count <= MaxBackupCountThreshold;
    }
    FILE* OpenFile(const pchar_t* path)
    {
        return OpenFile_(path);
    }

    bool GetCurrTime(std::tm& loct, int& millisec)
    {
        namespace chr = std::chrono;
        using clck_t = chr::system_clock;

        bool ret = false;
        clck_t::time_point tp = clck_t::now();
        if (GetLocalTime_(clck_t::to_time_t(tp), loct))
        {
            clck_t::duration t = tp.time_since_epoch();
            auto ms = chr::duration_cast<chr::milliseconds>(t).count();
            millisec = static_cast<int>(ms % 1000);
            ret = true;
        }
        return ret;
    }

    uint32_t GetThreadId()
    {
        union
        {
            uint32_t        id;
            std::thread::id tid;
        } u{ 0 };
        u.tid = std::this_thread::get_id();
        return u.id;
    }

} // namespace LogTools

