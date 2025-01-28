#pragma once

// ---------------------------------------------------------------------
// File: Core.h
// Classes: FFmpegCore
// Purpose: managed wrapper for native ICore implementation
// Module: FFmpegInterop - managed wrappers for native object from FFmpegWrapper
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "include/CoreIface.h"

#include "CommTypes.h"
#include "Enums.h"

namespace FFmpegInterop
{
    class DllLoader;

    public ref class Core sealed
    {
        static initonly Core^ TheInstance;

        DllLoader*    m_DllLoader;
        Fwrap::ICore* m_Core;

        Core();

        static Core();

    public:
        static void Initialize(StrType^ wrapFolderPath, StrType^ ffmpegFolderPath);
        static void EnableFFmpegLogging(LogLevel lev);
        static void Close();
        static void GetVersions(array<StrType^>^% libs, array<StrType^>^% vers);
        static bool OpenLog(LogLevel lev, StrType^ path, LogOptions options, int maxFileSize, int backupCount);
        static void LogString(LogLevel lev, StrType^ str);
        static void FlushLog();
        static StrType^ PlatformTag();
        static StrType^ GetHWAccels();
        static array<StrType^>^ GetDShowDevList(int devType);

    internal:
        static Fwrap::ICore* Ptr();

    private:
        void Initialize_(StrType^ wrapFolderPath, StrType^ ffmpegFolderPath);
        void Close_();
        StrType^ GetVersion_(Fwrap::LibTag libTag);
        void GetVersions_(array<StrType^>^% libs, array<StrType^>^% vers);
        array<StrType^>^ GetDShowDevList_(int devType);

    }; // ref class Core

} //namespace FFmpegInterop


