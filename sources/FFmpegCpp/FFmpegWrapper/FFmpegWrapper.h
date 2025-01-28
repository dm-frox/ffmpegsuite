#pragma once

// ---------------------------------------------------------------------
// File: FFmpegWrapper.h
// Classes: 
// Purpose: module export
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

extern "C"
{
    void* GetFFmpegWrapper();

} // extern "C"

const char FFmpegWrapperEntryPoint[] = "GetFFmpegWrapper";

#include "osdefs.h"

#if defined(WINDOWS_)

const wchar_t FFmpegWrapperModuleName[] = L"FFmpegWrapper.dll";

#else

// unix zoo

#endif  // WINDOWS_

