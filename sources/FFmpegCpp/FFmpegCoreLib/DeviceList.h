#pragma once

// ---------------------------------------------------------------------
// File: DeviceList.h
// Classes: DeviceList
// Purpose: experimental class to work with input devices (doesnot work in Windows)
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

namespace Fcore
{
    // static
    class DeviceList
    {
// ctor, dtor, copying
    public:
        DeviceList() = delete;
// operations
    public:
        static int LogListInput();

    }; // class DeviceList

} // namespace Fcore