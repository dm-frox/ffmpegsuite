#pragma once

// ---------------------------------------------------------------------
// File: Version.h
// Classes: Version
// Purpose: accessor to ffmpeg versions
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

namespace Fcore
{
    // static
    class Version
    {
// ctor, dtor, copying
        Version() = delete;
// properties
    public:
        static const char* Wrapper();
        static const char* FFmpeg();

        static const char* AvUtil();
        static const char* AvCodec();
        static const char* AvFormat();
        static const char* AvDevice();
        static const char* AvFilter();
        static const char* SwScale();
        static const char* SwResample();
        static const char* PostProc();

    }; // class Version

    void InitializeFFmpeg();

} // namespace Fcore

