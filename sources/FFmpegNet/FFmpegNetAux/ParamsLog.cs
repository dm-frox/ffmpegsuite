// ---------------------------------------------------------------------
// File: ParamsLog.cs
// Classes: FFmpegLoggingParams
// Purpose: helper to store parametrs for logging
// Module: FFmpegNetCore - .NET assembly to provide more comfortable interface to FFmpegInterop.dll
// Copyright (c) 2017 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2018
// ---------------------------------------------------------------------

using System;

using FFmpegInterop;

namespace FFmpegNetCore
{
    public class FFmpegLoggingParams
    {
        const LogLevel DefLogLevelWrap     = LogLevel.Info;
        const LogLevel DefLogLevelFFmpeg   = LogLevel.Info;
        const LogOptions DefLogOptionsWrap = LogOptions.None;

        readonly string m_LogFileNameWrap;
        LogLevel   m_LogLevelWrap;
        LogLevel   m_LogLevelFFmpeg;
        LogOptions m_LogOptionsWrap;

        public FFmpegLoggingParams(string logFileNameWrap)
        {
            m_LogFileNameWrap = logFileNameWrap;

            LogLevelWrap = DefLogLevelWrap;
            LogLevelFFmpeg = DefLogLevelFFmpeg;
            LogOptionsWrap = DefLogOptionsWrap;
        }

        public string LogFileNameWrap => m_LogFileNameWrap;

        public LogLevel LogLevelWrap { get => m_LogLevelWrap; set => m_LogLevelWrap = value; }

        public LogLevel LogLevelFFmpeg { get => m_LogLevelFFmpeg; set => m_LogLevelFFmpeg = value; }

        public LogOptions LogOptionsWrap { get => m_LogOptionsWrap; set => m_LogOptionsWrap = value; }

    } // class FFmpegLogConfig

} // namespace FFmpegNetCore
