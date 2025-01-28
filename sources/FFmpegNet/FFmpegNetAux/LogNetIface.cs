// ---------------------------------------------------------------------
// File: LogNetIface.cs
// Classes: interface ILogNet
// Purpose: interface to plug in .NET logger
// Module: FFmpegNetCore - .NET assembly to provide more comfortable interface to FFmpegInterop.dll
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

using System;

namespace FFmpegNetAux
{
    public interface ILogNet
    {
        bool Open(string logFile, string cfgFile);
        void Close();
        void PrintError(string msg);
        void PrintWarning(string msg);
        void PrintInfo(string msg);
        void PrintDebug(string msg);
        void PrintTrace(string msg);
        void PrintExc(string msg, Exception exc);
        void Flush();
        void SkipLine();

    }; // interface ILoggerNet

} // namespace FFmpegNetAux
