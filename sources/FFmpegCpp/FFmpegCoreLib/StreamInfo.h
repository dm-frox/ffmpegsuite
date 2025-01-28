#pragma once

// ---------------------------------------------------------------------
// File: StreamInfo.h
// Classes: StreamInfo
// Purpose: base information about stream
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <string>

#include "ffmpeg.h"

namespace Fcore
{

    struct StreamInfo
    {
        AVMediaType    Type;
        double         DurationS;
        int64_t        Bitrate;

        int            Width;
        int            Height;
        int            Fps_N;
        int            Fps_D;
        AVPixelFormat  PixFormat;
        AVRational     Sar;

        int            Chann;
        int            SampRate;
        AVSampleFormat SampFormat;
        int64_t        ChannMask;
        int            FrameSize;

        std::string    MediaType;
        std::string    CodecId;

        bool           IsDefault;

        StreamInfo()
        {
            Reset();
        }

        ~StreamInfo() = default;

        void Reset()
        {
            Type = AVMEDIA_TYPE_UNKNOWN;

            DurationS = 0;
            Bitrate = 0;

            Width = 0;
            Height = 0;
            Fps_N = 0;
            Fps_D = 0;
            PixFormat = AV_PIX_FMT_NONE;
            Sar = AVRational{ 0, 1 };


            Chann = 0;
            SampRate = 0;
            SampFormat = AV_SAMPLE_FMT_NONE;
            ChannMask = 0;
            FrameSize = 0;

            MediaType.clear();
            CodecId.clear();

            IsDefault = false;
        }

        bool IsVideo() const { return Type == AVMEDIA_TYPE_VIDEO; }

        bool IsAudio() const { return Type == AVMEDIA_TYPE_AUDIO; }

        double Fps() const { return Fps_D > 0 ? Fps_N / static_cast<double>(Fps_D) : 0.0; }

    }; // struct StreamInfo

} // namespace Fcore
