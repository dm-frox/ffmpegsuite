#pragma once

// ---------------------------------------------------------------------
// File: MediaSourceInfo.h
// Classes: MediaSourceInfo
// Purpose: managed wrapper for native IMediaSourceInfo implementation
// Module: FFmpegInterop - managed wrappers for native object from FFmpegWrapper
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "include/MediaSourceInfoIface.h"

#include "CommTypes.h"

namespace FFmpegInterop
{

    public ref class MediaSourceInfo sealed
    {
        using InfoPrt = const Fwrap::IMediaSourceInfo* const;

        InfoPrt m_Info;

    internal:
        MediaSourceInfo(InfoPrt p);

    public:
        property bool HasVideo { bool get(); }
        property int VideoIndex { int get(); }
        property int Width { int get(); }
        property int Height{ int get(); }
        property int Fps_N { int get(); }
        property int Fps_D { int get(); }
        property double Fps { double get(); }
        property StrType^ PixelFormat { StrType^ get(); }
        property StrType^ VideoCodec { StrType^ get(); }
        property int64_t VideoBitrate { int64_t get(); }

        property bool HasAudio { bool get(); }
        property int AudioIndex { int get(); }
        property int Chann { int get(); }
        property int SampleRate { int get(); }
        property StrType^ SampleFormat { StrType^ get(); }
        property StrType^ AudioCodec { StrType^ get(); }
        property int64_t AudioBitrate { int64_t get(); }

        property bool CanSeek { bool get(); }
        property double Duration { double get(); }

    }; // ref class MediaSourceInfo

} // namespace FFmpegInterop

