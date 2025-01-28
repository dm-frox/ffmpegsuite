#pragma once

// ---------------------------------------------------------------------
// File: SLogger.h
// Classes: 
// Purpose: enums, function and macros for printf-style logging
// Module: FFmpegToolsLib - auxiliary classes, functions
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <cstdarg>

namespace SLog
{
    enum class Level
    {
        None = 0, 
        Error, 
        Warning, 
        Info, 
        Verbose, 
        Debug, 
        Trace,

    }; // enum class Level

    enum class Options // flags
    {
        None = 0, 
        FlushLine = 0x01, 
        ThreadLab = 0x02

    }; // enum class Options

    bool Open(Level level, const wchar_t* path, Options options = Options::None);
    bool Open(Level level, const char* path, Options options = Options::None);

    void SetMaxFileSize(int maxFileSize);
    void SetMaxBackupCount(int backupCount);

    void Close();
    void Flush();

    void Log(Level lev, const char* form, ...);
    void Log2(Level lev, const char* format, const char* data, va_list argptr);

    bool CanLog(Level lev);

} // namespace SLog

#define CAN_LOG_ERROR() (SLog::LOG_MaxLogLevel >= SLog::Level::Error)

#define LOG_ERROR(...)\
if (SLog::CanLog(SLog::Level::Error))   SLog::Log(SLog::Level::Error,__VA_ARGS__);

#define LOG_WARNING(...)\
if (SLog::CanLog(SLog::Level::Warning)) SLog::Log(SLog::Level::Warning,__VA_ARGS__);

#define LOG_INFO(...)\
if (SLog::CanLog(SLog::Level::Info))    SLog::Log(SLog::Level::Info,__VA_ARGS__);

#define LOG_VERBOSE(...)\
if (SLog::CanLog(SLog::Level::Verbose)) SLog::Log(SLog::Level::Verbose,__VA_ARGS__);

#define LOG_DEBUG(...)\
if (SLog::CanLog(Log::Level::Debug))    SLog::Log(SLog::Level::Debug,__VA_ARGS__);

#define LOG_TRACE(...)\
if (SLog::CanLog(SLog::Level::Trace))   SLog::Log(SLog::Level::Trace,__VA_ARGS__);

#define LOG_TEMP(...)\
if (SLog::CanLog(SLog::Level::Info))    SLog::Log(SLog::Level::Info,__VA_ARGS__);

#define LOG_TEMP_F(...)\
if (SLog::CanLog(SLog::Level::Info))    SLog::Log(SLog::Level::Info,__VA_ARGS__); SLog::Flush();

#define LOG_FLUSH()\
if (SLog::CanLog(SLog::Level::None))    SLog::Flush();



