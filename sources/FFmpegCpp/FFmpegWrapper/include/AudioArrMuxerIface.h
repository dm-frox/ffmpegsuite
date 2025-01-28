#pragma once

// ---------------------------------------------------------------------
// File: AudioArrMuxerIface.h
// Classes: IAudioArrMuxer (abstract)
// Purpose: interface for the test object which writes audio stream with a large number of channels
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------


#include "BaseIface.h"

namespace Fwrap
{
    class IAudioArrMuxer : public IBase
    {
    protected:
        IAudioArrMuxer() = default;
        ~IAudioArrMuxer() = default;

    public:
        virtual int Build(const char* urlVideo, const char* urlAudio, const char* urlDst) = 0;
        virtual void Close() = 0;

        virtual bool EndOfData() const = 0;
        virtual bool Error() const = 0;

        static IAudioArrMuxer* CreateInstance();

    }; // class IAudioArrMuxer

} // namespace Fwrap
