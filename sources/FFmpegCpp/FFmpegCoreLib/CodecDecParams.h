#pragma once

// ---------------------------------------------------------------------
// File: CodecDecParams.h
// Classes: ParamsEx
// Purpose: additional parameters for decoder
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <string>

#include "ffmpeg.h"
#include "MediaTypeHolder.h"


namespace Fcore
{

    class ParamsEx : MediaTypeHolder
    {
    // data
    private:
        static const char GpuPrefix          = '*';

        std::string       m_Name{};
        bool              m_IsGpu{ false };
        AVPixelFormat     m_PrefPixFmt{ AV_PIX_FMT_NONE };
        AVSampleFormat    m_PrefSampFmt{ AV_SAMPLE_FMT_NONE };
        std::string       m_Options{};
        std::string       m_HWAccel{};

        std::string       m_Threads{};
        std::string       m_OptsIni{};

    // ctor, dtor, copying
    public:
        ParamsEx(const char* decParamsEx, AVMediaType mediaType, AVCodecID codecId);
        ~ParamsEx();

    // properties
    public:
        const char* Name() const { return m_Name.c_str(); }
        bool IsGpu() const { return m_IsGpu; }
        AVPixelFormat  PrefPixFmt() const { return m_PrefPixFmt; }
        AVSampleFormat PrefSampFmt() const { return m_PrefSampFmt; }
        const char* Options() const { return m_Options.c_str(); }
        const char* HWAccel() const { return m_HWAccel.c_str(); }
        const std::string& GetOptions() const { return m_Options; }
 
    // operations
    private:
        void Parse(const char* decParamsEx, AVCodecID codecId);
        void SetName(const char* name, AVCodecID codecId);
        void SetFormat(const char* fmt);
        void SetOptions(const char* opts);
        void SetHWAccel(const char* hw);
        void SetThreads(const char* th);
        void SetupOptions();

        static bool HasGpuPrefix(const char* dn) { return *dn == GpuPrefix; }

    }; // classt ParamsEx

} // namespace Fcore
