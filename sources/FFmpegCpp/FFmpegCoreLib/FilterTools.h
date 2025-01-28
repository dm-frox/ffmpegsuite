#pragma once

// ---------------------------------------------------------------------
// File: FilterTools.h
// Classes: FilterTools
// Purpose: filter helpers
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <string>

#include "ffmpeg.h"

namespace Fcore
{
    class ChannLayout;

    // static
    class FilterTools
    {
// data
    private:
        static const char FiltSepr;
        static const char ParmSepr;
// ctor, dtor, copying
    public:
        FilterTools() = delete;
// operations
    public:
        static std::string GetFilterStringVideo(int width, int height, const std::string& flt);
        static std::string GetFilterStringAudio(int sampleRate, const ChannLayout& channLayout, const std::string& flt, int frameSize);

        static void FillInFilterArgsVideo(int width, int height, AVPixelFormat pixFmt, AVRational timeBase, AVRational frameRate, char* buff, int buffSize);
        static void FillInFilterArgsAudio(const ChannLayout& channLayout, int sampleRate, AVSampleFormat sampFmt, char* buff, int buffSize);

        static void AppendFps(std::string& str, int fps);
    private:
        static void AppendFiltSepr(std::string& str);
        static void AppendFilter(std::string& str, const std::string& flt);
        static void AppendScale(std::string& str, int width, int height);
        static void AppendAformat(std::string& str, int sampleRate, const ChannLayout& channLayout);
        static void AppendFrameSize(std::string& str, int frameSize);

    }; // class FilterTools

} // namespace Fcore
