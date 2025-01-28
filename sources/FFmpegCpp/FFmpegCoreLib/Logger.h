#pragma once

// ---------------------------------------------------------------------
// File: Logger.h
// Classes: Logger
// Purpose: class to support logging
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "SLogger.h"

namespace Fcore
{
    // static
    class Logger
    {
// ctor, dtor, copying
    public:
        Logger() = delete;
// properties
    public:
        static void SetMaxFileSize(int maxFileSize);
        static void SetMaxBackupCount(int maxBackupCount);
// operations
    public:
        static bool Open(int level, const wchar_t* path, int options);
        static bool Open(int level, const char* path, int options);
        static void Close();

        static void EnableFFmpegLogging(int level);

        static void LogString(int level, const char* str);
        static void LogFFmpegVersionInfo();
        static void Flush();

    }; // class Logger

    const char* FmtErr(int errCode);

} // namespace Fcore

