#pragma once

// ---------------------------------------------------------------------
// File: AudioArrMuxerImpl.h
// Classes: AudioArrMuxerImpl
// Purpose: IAudioArrMuxer implementation
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------


#include "AudioArrMuxer.h"

#include "include/AudioArrMuxerIface.h"


namespace Fwrap
{
    class AudioArrMuxerImpl : IAudioArrMuxer
    {
        Fapp::AudioArrMuxer m_Muxer;

        AudioArrMuxerImpl();
        ~AudioArrMuxerImpl();

// IAudioArrMuxer impl
        int Build(const char* urlVideo, const char* urlAudio, const char* urlDst) override final;
        void Close() override final;

        bool EndOfData() const override final;
        bool Error() const override final;
// factory
        friend IAudioArrMuxer* IAudioArrMuxer::CreateInstance();

    }; //class AudioArrMuxerImpl

} // namespace Fwrap

