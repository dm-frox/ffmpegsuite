#pragma once

// ---------------------------------------------------------------------
// File: LogLevelEnum.h
// Classes: enum class LogLevel, enum class LogOptions
// Purpose: levels and options for logging
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

namespace Fwrap
{
    enum class LogLevel // copy of SLog::Level, for .NET only
    {
        None = 0,
        Error,
        Warning,
        Info,
        Verbose,
        Debug,
        Trace,

    }; // enum class LogLevel

    enum class LogOptions // copy of SLog::Options, for .NET only
    {
        None        = 0,
        FlushLine   = 1,
        ThreadLabel = (1 << 1),

    }; // enum class LogOptions

} // namespace Fwrap