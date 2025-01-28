#include "pch.h"

#include "SLogger.h"

#include <cstdio>
#include <ctime>
#include <memory.h>

#include <mutex>
#include <string>

#include "pchar.h"
#include "ArrSize.h"


namespace LogTools
{
    void WriteOsInfo(FILE* file);
    void WriteDefs(FILE* file);
    void WriteCppVers(FILE* file);

    bool BackupFiles(const pchar_t* file, int maxBackupCount);
    bool CheckMaxBackupCount(int lev);
    FILE* OpenFile(const pchar_t* path);

    bool GetCurrTime(std::tm& loct, int& millisec);
    uint32_t GetThreadId();
}

namespace
{
    template<typename T>
    T Max_(T a, T b) { return a < b ? b : a; }

// ---------------------------------------------------------------------
// file path helper

    class PathHolder
    {
        static const pchar_t null;

        std::basic_string<pchar_t> m_FileName;

    public:
        PathHolder() = default;

        bool SetupPath(const pchar_t* path) { m_FileName = path ? path : &null; return !m_FileName.empty(); }
        FILE* OpenFile() { return LogTools::OpenFile(m_FileName.c_str()); }
        void ClearPath() { m_FileName.clear(); } 
        bool BackupFiles(int backupCount) { return LogTools::BackupFiles(m_FileName.c_str(), backupCount); }

        PathHolder(const PathHolder&) = delete;
        PathHolder& operator=(const PathHolder&) = delete;
    };

    const pchar_t PathHolder::null = pchar_t(0);

// --------------------------------------------------------------------
// logger class declaration

    class SLogger
    {
        using Mutex    = std::mutex;
        using AutoLock = std::lock_guard<Mutex>;

        Mutex       m_Mutex;

        SLog::Level m_LogLevel;
        int         m_MaxFileSize;
        int         m_MaxBackupCount;
        
        PathHolder  m_PathHolder;
        FILE*       m_pLogFile;
        int         m_Options;

        int         m_LineCounter;
        int         m_TotalSize;
        int         m_OverflowCount;

    public:

        SLogger();
        ~SLogger();

        bool OpenLog(SLog::Level level, const pchar_t* path, SLog::Options options);
        void CloseLog(const char* txt);
        bool CanLog(SLog::Level level)const { return level <= m_LogLevel; }

        void SetOptions(SLog::Options optionst);
        void SetMaxFileSize(int maxFileSize);
        void SetMaxBackupCount(int backupCount);

        bool IsReady() const { return m_pLogFile ? true : false; }
        void SLog(int lev, const char* format, va_list argptr);
        void SLog(int lev, const char* format, const char* data, va_list argptr);

        void Flush();

    private:
        bool OpenLogFile(bool open, SLog::Level level, SLog::Options options);
        void CloseLogFile();
        bool Reopen();
        void LogFileHeader();
        void LogFileBottom(const char* txt);
        void OpenLine(int lev);
        void WriteSelf(void(*writer)(FILE* flle));
        void WriteTimestamp();
        inline void WriteStr(const char* str) { std::fprintf(m_pLogFile, "%s", str); }
        void CloseLine();
        void CloseLineEx();
    };

// --------------------------------------------------------------------
// logger constants

    const int LevSelf  = 0;

    const char FileHead[]     = "<<<<<<<\n";
    const char FileBottom[]   = ">>>>>>>\n";
    const char LoggerParams[] = "Log Begin: Level=%d, Options=0x%x, MaxSize=%d, MaxBackupCount=%d, Overflow=%d";
    const char LogFileInfo[]  = "Log End: lines=%d, total size=%d";
    const char LineTail[]     = "\n";
    const char LevLab[]       = "[%d] ";
    const char LinePrefix[]   = "%02d %s %02d:%02d:%02d.%03d | ";
    const char BadTimestamp[] = "<bad timestamp>";
    const char AlreadOpened[] = "------------------------- Already opened -----------------------------\n";
    const char Dtor[]         = "Destructor";
    const char Overflow[]     = "Log file overflow";
    const char CloseMan[]     = "Close log";
    const char ThreadLabel[]  = "%04X> ";
    const char EmptyStr[] = "";

    const char* const Month[] =
    {
        EmptyStr,
        "jan", "feb",
        "mar", "apr", "may",
        "jun", "jul", "aug",
        "sep", "oct", "nov",
        "dec"
    };

    const char* const LevPrefix[] =
    {
        EmptyStr, // none
        "ERROR !! ", // error
        "WARNG !! ", // warning
        EmptyStr, // info
        EmptyStr, // verbose
        EmptyStr, // debug
        EmptyStr  // trace
    };

    const int MB = 1024 * 1024;

    const int DefMaxFileSize = MB; // 1 MB
    const int DefMaxBackupCount = 20;

    const int FlushLineFlag = (int)SLog::Options::FlushLine;
    const int ThreadLabFlag = (int)SLog::Options::ThreadLab;

    // maximums, minimums
    const int MaxFileSize = 512 * MB; // 0.5 GB  ?
    const int MinFileSize = 1024;
    const int MinBackupLevel = 1;

// --------------------------------------------------------------------
// logger class defenition

    SLogger::SLogger()
        : m_LogLevel(SLog::Level::None)
        , m_MaxFileSize(DefMaxFileSize)
        , m_MaxBackupCount(DefMaxBackupCount)
        , m_pLogFile(nullptr)
        , m_Options(0)
        , m_LineCounter(0)
        , m_TotalSize(0)
        , m_OverflowCount(0)
    {}

    SLogger::~SLogger()
    {
        CloseLog(Dtor);
    }

    void SLogger::SetOptions(SLog::Options options)
    {
        AutoLock al(m_Mutex);
        m_Options = (int)Max_(SLog::Options::None, options);
    }

    void SLogger::SetMaxFileSize(int maxFileSize)
    {
        AutoLock al(m_Mutex);
        m_MaxFileSize = (MinFileSize <= maxFileSize && maxFileSize <= MaxFileSize) ? maxFileSize : DefMaxFileSize;
    }

    void SLogger::SetMaxBackupCount(int maxBackupCount)
    {
        AutoLock al(m_Mutex);
        m_MaxBackupCount = LogTools::CheckMaxBackupCount(maxBackupCount) ? maxBackupCount : DefMaxBackupCount;
    }

    bool SLogger::OpenLog(SLog::Level level, const pchar_t* path, SLog::Options options)
    {
        bool ret = false;
        AutoLock al(m_Mutex);
        if (m_pLogFile == nullptr)
        {
            if (m_PathHolder.SetupPath(path))
            {
                m_PathHolder.BackupFiles(m_MaxBackupCount);
                if (OpenLogFile(true, level, Max_(SLog::Options::None, options)))
                {
                    ret = true;
                }
                else
                {
                    m_PathHolder.ClearPath();
                }
            }
        }
        else
        {
            WriteStr(AlreadOpened);
        }
        return ret;
    }

    bool SLogger::OpenLogFile(bool open, SLog::Level level, SLog::Options options) // under mutex
    {
        bool ret = false;
        if (FILE* p = m_PathHolder.OpenFile())
        {
            m_pLogFile = p;
            m_LineCounter = 0;
            m_TotalSize = 0;
            if (open) // OpenLog()
            {
                m_LogLevel = level;
                m_Options = (int)options;
                m_OverflowCount = 0;
            }
            else // Reopen()
            {
                ++m_OverflowCount;
            }
            LogFileHeader();

            ret = true;
        }
        return ret;
    }

    void SLogger::CloseLogFile() // under mutex
    {
        WriteStr(FileBottom);
        std::fclose(m_pLogFile);
        m_pLogFile = nullptr;
        m_LineCounter = 0;
        m_TotalSize = 0;
    }

    bool SLogger::Reopen() // under mutex
    {
        LogFileBottom(Overflow);
        CloseLogFile();
        m_PathHolder.BackupFiles(m_MaxBackupCount);
        return OpenLogFile(false, SLog::Level::None, SLog::Options::None);
    }

    void SLogger::WriteSelf(void(*writer)(FILE* flle))
    {
        OpenLine(LevSelf);
        writer(m_pLogFile);
        CloseLine();
    }

    void SLogger::LogFileHeader() // under mutex
    {
        WriteStr(FileHead);

        OpenLine(LevSelf);
        std::fprintf(m_pLogFile, LoggerParams,
            m_LogLevel, m_Options, m_MaxFileSize, m_MaxBackupCount, m_OverflowCount);
        CloseLine();

        WriteSelf(LogTools::WriteOsInfo);
        WriteSelf(LogTools::WriteDefs);
        WriteSelf(LogTools::WriteCppVers);
    }
    
    void SLogger::CloseLog(const char* txt)
    {
        AutoLock al(m_Mutex);
        if (m_pLogFile)
        {
            LogFileBottom(txt);
            CloseLogFile();
            m_LogLevel = SLog::Level::None;
            m_Options = (int)SLog::Options::None;
            m_PathHolder.ClearPath();
            m_OverflowCount = 0;
        }
    }

    void SLogger::LogFileBottom(const char* txt) // under mutex
    {
        if (txt && *txt)
        {
            OpenLine(LevSelf);
            WriteStr(txt);
            CloseLine();
        }
        OpenLine(LevSelf);
        std::fprintf(m_pLogFile, LogFileInfo, m_LineCounter, m_TotalSize);
        CloseLine();
    }

    void SLogger::OpenLine(int lev) // under mutex
    {
        std::fprintf(m_pLogFile, LevLab, lev);
        WriteTimestamp();
        if (m_Options & ThreadLabFlag)
        {
            std::fprintf(m_pLogFile, ThreadLabel, LogTools::GetThreadId());
        }
        const char* lp = (0 <= lev && lev < Ftool::Size(LevPrefix)) ? LevPrefix[lev] : EmptyStr;
        if (*lp)
        {
            WriteStr(lp);
        }
    }

    void SLogger::Flush()
    {
        AutoLock al(m_Mutex);
        if (m_pLogFile && !(m_Options & FlushLineFlag))
        {
            std::fflush(m_pLogFile);
        }
    }

    void SLogger::CloseLine() // under mutex
    {
        WriteStr(LineTail);

        if (m_Options & FlushLineFlag)
        {
            std::fflush(m_pLogFile);
        }

        m_TotalSize = std::ftell(m_pLogFile);
        ++m_LineCounter;
    }

    void SLogger::CloseLineEx() // under mutex
    {
        CloseLine();

        if (m_TotalSize >= m_MaxFileSize)
        {
            Reopen();
        }
    }

    void SLogger::WriteTimestamp() // under mutex
    {
        std::tm loct;
        int millisec;
        if (LogTools::GetCurrTime(loct, millisec))
        {
            std::fprintf(m_pLogFile, LinePrefix,
                loct.tm_mday, Month[loct.tm_mon + 1],
                loct.tm_hour, loct.tm_min, loct.tm_sec, 
                millisec);
        }
        else
        {
            std::fprintf(m_pLogFile, BadTimestamp);
        }
    }

    void SLogger::SLog(int lev, const char* format, va_list argptr)
    {
        AutoLock al(m_Mutex);
        if (m_pLogFile)
        {
            OpenLine(lev);
            std::vfprintf(m_pLogFile, format, argptr);
            CloseLineEx();
        }
    }

    void SLogger::SLog(int lev, const char* format, const char* data, va_list argptr)
    {
        AutoLock al(m_Mutex);
        if (m_pLogFile)
        {
            OpenLine(lev);
            WriteStr(data);
            std::vfprintf(m_pLogFile, format, argptr);
            CloseLineEx();
        }
    }

// --------------------------------------------------------------------
// logger singleton

    SLogger TheLogger;

} // namespace anon

// ---------------------------------------------------------------------
// logger defenition

namespace SLog
{
    bool CanLog(Level level)
    {
        return TheLogger.CanLog(level);
    }

    void SetMaxFileSize(int maxFileSize)
    {
        TheLogger.SetMaxFileSize(maxFileSize);
    }

    void SetMaxBackupCount(int maxBackupCount)
    {
        TheLogger.SetMaxBackupCount(maxBackupCount);
    }

#ifdef WINDOWS_ // windows

    bool Open(Level level, const wchar_t* path, Options options)
    {
        return TheLogger.OpenLog(level, path, options);
    }

    bool Open(Level, const char*, Options) { return false; }

#else // unix zoo

    bool Open(Level level, const char* path, Options options)
    {
        return TheLogger.OpenLog(level, path, options);
    }

    bool Open(Level, const wchar_t*, Options) { return false; }

#endif

    void Close()
    {
        TheLogger.CloseLog(CloseMan);
    }

    void Flush()
    {
        TheLogger.Flush();
    }

    void Log(Level lev, const char* format, ...)
    {
        va_list marker;
        va_start(marker, format);
        TheLogger.SLog((int)lev, format, marker);
        va_end(marker);
    }

    void Log2(Level lev, const char* format, const char* data, va_list argptr)
    {
        TheLogger.SLog((int)lev, format, data, argptr);
    }

} // namespace SLog


