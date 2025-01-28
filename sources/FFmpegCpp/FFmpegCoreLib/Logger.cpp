#include "pch.h"

#include "Logger.h"

#include <cstring>

#include "ffmpeg.h"
#include "OptionTools.h"
#include "ArrSize.h"

namespace
{
    const size_t LogBuffLen = 256;
    const size_t FmtBuffLen = 512;
    const size_t ErrStrBuffLen   = AV_ERROR_MAX_STRING_SIZE;
    const size_t ErrStrBuffLenEx = AV_ERROR_MAX_STRING_SIZE + 32;

    const char NullCtx[] = "0";

    template<size_t N>
    int WriteContextName(char(&buff)[N], const void* logCtx)
    {
        const char* name = Fcore::OptionTools::GetContextName(logCtx);
        buff[0] = '\0';
        return (int)av_strlcatf(buff, N, "[~%s] ", name ? name : NullCtx);
    }

    template <size_t N>
    bool CheckFormat(char(&format)[N], const char* fmt)
    {
        av_strlcpy(&format[0], fmt, N);
        int n = (int)std::strlen(format);
        if (n > 0)
        {
            char& tt = format[n - 1];
            if (tt == '\n')
            {
                tt = '\0';
                --n;
            }
        }
        return n > 0;
    }

    SLog::Level FromAVLogLevel(int levelAV)
    {
        SLog::Level ret = SLog::Level::None;
        switch (levelAV)
        {
        //   AV_LOG_QUIET;   // -8
        case AV_LOG_PANIC:   // 0
        case AV_LOG_FATAL:   // 8
        case AV_LOG_ERROR:   // 16
            ret = SLog::Level::Error;
            break;
        case AV_LOG_WARNING: // 24
            ret = SLog::Level::Warning;
            break;
        case AV_LOG_INFO:    // 32
            ret = SLog::Level::Info;
            break;
        case AV_LOG_VERBOSE: // 40
            ret = SLog::Level::Verbose;
            break;
        case AV_LOG_DEBUG:   // 48
            ret = SLog::Level::Debug;
            break;
        case AV_LOG_TRACE:   // 56 (a lot of messages !!)
            ret = SLog::Level::Trace;
            break;
        }
        return ret;
    }

    const int AVLogLevels[] =
    {
        AV_LOG_QUIET,
        AV_LOG_ERROR,
        AV_LOG_WARNING,
        AV_LOG_INFO,
        AV_LOG_VERBOSE,
        AV_LOG_DEBUG,
        AV_LOG_TRACE,
    };

    int ToAVLogLevel(int level)
    {
        return (0 <= level && level < Ftool::Size(AVLogLevels))
            ? AVLogLevels[level]
            : AV_LOG_QUIET;
    }

    SLog::Level SLogLev(int level)
    {
        return static_cast<SLog::Level>(level);
    }

    SLog::Options SLogOptions(int opts)
    {
        return static_cast<SLog::Options>(opts);
    }


    static int FFmpegLogLevel = AV_LOG_QUIET;

    const bool PrintVaListArgs = true;

} // namespace anon

extern "C"
{
    static void FFmpegLogCallback(void* logCtx, int levelAV, const char* fmt, va_list args)
    {
        if (levelAV <= FFmpegLogLevel)
        {
            char buff[LogBuffLen];
            memset(buff, 0, sizeof(buff));
            int buffOffset = WriteContextName(buff, logCtx);

            SLog::Level level = FromAVLogLevel(levelAV);
            bool written = false;
            if (PrintVaListArgs)
            {
                try
                {
                    char format[FmtBuffLen];
                    if (CheckFormat(format, fmt))
                    {
                        SLog::Log2(level, format, buff, args);
                        written = true;
                    }
                }
                catch (...)
                {
                    SLog::Log(SLog::Level::Error, "ffmpeg_log_callback, exception !!!!!");
                }
            }
            if (!written && buffOffset > 0)
            {
                SLog::Log(level, buff);
            }
        }
    }
} // extern "C"


namespace Fcore
{
    void Logger::EnableFFmpegLogging(int level)
    {
        FFmpegLogLevel = ToAVLogLevel(level);
        av_log_set_level(FFmpegLogLevel);

        if (FFmpegLogLevel > AV_LOG_QUIET)
        {
            av_log_set_callback(FFmpegLogCallback);
        }
        SLog::Log(SLogLev(0), "Enable FFmpeg logging, level=%d (%d)", level, av_log_get_level());
    }

    void Logger::SetMaxFileSize(int maxFileSize)
    {
        SLog::SetMaxFileSize(maxFileSize);
    }

    void Logger::SetMaxBackupCount(int maxBackupCount)
    {
        SLog::SetMaxBackupCount(maxBackupCount);
    }

    bool Logger::Open(int level, const wchar_t* path, int options)
    {
        return SLog::Open(SLogLev(level), path, SLogOptions(options));
    }

    bool Logger::Open(int level, const char* path, int options)
    {
        return SLog::Open(SLogLev(level), path, SLogOptions(options));
    }

    void Logger::Close() 
    { 
        SLog::Close(); 
    }

    void Logger::LogString(int level, const char* str)
    { 
        SLog::Log(SLogLev(level), str);
    }

    void Logger::LogFFmpegVersionInfo()
    {
        SLog::Log(SLogLev(0), "FFmpeg version info: %s", av_version_info());
    }

    void Logger::Flush()
    {
        SLog::Flush();
    }

    thread_local static char ThreadBuff[ErrStrBuffLenEx];

    const char* FmtErr(int errCode)
    {
        char errStr[ErrStrBuffLen];
        av_make_error_string(errStr, sizeof(errStr), errCode);

        ThreadBuff[0] = '\0';
        av_strlcatf(ThreadBuff, sizeof(ThreadBuff), "code=%d, descr=%s", errCode, errStr);
        return ThreadBuff;
    }

} // namespace Fcore



