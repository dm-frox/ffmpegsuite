// ---------------------------------------------------------------------
// File: LogNet.cs
// Classes: LogNet
// Purpose: .NET logger which plugs in external logger via ILogNet interface
// Module: FFmpegNetCore - .NET assembly to provide more comfortable interface to FFmpegInterop.dll
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

using System;

using FFmpegInterop;

namespace FFmpegNetAux
{
    public static class LogNet
    {
        static ILogNet  Logger;
        static LogLevel Level;

        static LogNet()
        {
            Logger = null;
            Level = LogLevel.None;
        }

        public static bool Open(ILogNet logger, string logFile, string cfgFile = null, LogLevel level = LogLevel.Info)
        {
            bool ret = false;
            if (logger != null && level >= LogLevel.None)
            {
                if (logger.Open(logFile, cfgFile))
                {
                    Logger = logger;
                    Level = level;
                    ret = true;
                }
            }
            return ret;
        }

        public static void Close() => Logger?.Close();

        public static void PrintInfo(string format, params object[] prms)
        {
            if (Level >= LogLevel.Info)
            {
                Logger?.PrintInfo(string.Format(format, prms));
            }
        }

        public static void PrintDebug(string format, params object[] prms)
        {
            if (Level >= LogLevel.Debug)
            {
                Logger?.PrintDebug(string.Format(format, prms));
            }
        }

        public static void PrintError(string format, params object[] prms)
        {
            if (Level >= LogLevel.Error)
            {
                Logger?.PrintError(string.Format(format, prms));
            }
        }

        public static void PrintExc(Exception exc, string msg = null)
        {
            Logger?.PrintError(string.Format("Exception: {0}{1}       {2}",
                (msg != null) ? msg : string.Empty,
                Environment.NewLine,
                exc.Message));
        }

        public static void Flush() => Logger?.Flush();

        public static void SkipLine() => Logger?.SkipLine();

    } // static class LogNet

} // namespace FFmpegNetAux

