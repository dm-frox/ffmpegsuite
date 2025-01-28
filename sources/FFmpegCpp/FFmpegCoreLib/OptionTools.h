#pragma once

// ---------------------------------------------------------------------
// File: OptionTools.h
// Classes: OptionTools
// Purpose: helpers to work with options
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <string>

#include "ffmpeg_fwd.h"

namespace Fcore
{
    // static
    class OptionTools
    {
        static const char NullClassPtr[];

    public:

        OptionTools() = delete;

        static const AVClass* GetContextClass(const void* ctx);  // dangerous !!
        static const char* GetContextName(const void* ctx);      // dangerous !!

        static int SetOption(void* ctx, const char* name, const char* val);
        static int SetOption(void* ctx, const char* name, int64_t val);
        static int SetOption(void* ctx, const char* name, const void* val, int size);

        static std::string OptionsToStr(const AVClass* avc);
        static std::string OptionsToStrEx(const AVClass* avc, const char* name, const char* nameType);

        static void LogOptions(const char* txt, const AVClass* avc);
        static void LogOptionsEx(const char* txt, const char* name, const char* nameType, const AVClass* avc);

    }; // class OptionTools

} // namespace Fcore