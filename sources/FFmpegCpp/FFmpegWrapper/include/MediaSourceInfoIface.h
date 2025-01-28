#pragma once

// ---------------------------------------------------------------------
// File: MediaSourceInfoIface.h
// Classes: IMediaSourceInfo (abstract)
// Purpose: the class gives information about a media source
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "BaseIface.h"

namespace Fwrap
{

    class IMediaSourceInfo : IBase0
    {
    protected:
        IMediaSourceInfo() = default;
        ~IMediaSourceInfo() = default;

    public:
        virtual bool HasVideo() const = 0;
        virtual int VideoIndex() const = 0;
        virtual int Width() const = 0;
        virtual int Height() const = 0;
        virtual int Fps_N() const = 0;
        virtual int Fps_D() const = 0;
        virtual double Fps() const = 0;
        virtual const char* PixelFormatStr() const = 0;
        virtual const char* VideoCodec() const = 0;
        virtual int64_t VideoBitrate() const = 0;

        virtual bool HasAudio() const = 0;
        virtual int AudioIndex() const = 0;
        virtual int Chann() const = 0;
        virtual int SampleRate() const = 0;
        virtual const char* SampleFormatStr() const = 0;
        virtual int64_t ChannMask() const = 0;
        virtual const char* AudioCodec() const = 0;
        virtual int64_t AudioBitrate() const = 0;

        virtual bool CanSeek() const = 0;
        virtual double Duration() const = 0;

    }; // class IMediaSourceInfo

} // namespace Fwrap