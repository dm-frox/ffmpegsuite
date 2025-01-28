#pragma once

// ---------------------------------------------------------------------
// File: FrameSinkAudioIface.h
// Classes: IFrameSrcAudio (abstract), IFrameSinkAudio (abstract)
// Purpose: IFrameSinkAudio is an interface which must implement an audio renderer or another consumer of decoded audio frames
//          IFrameSrcAudio is an interface to control rendering from the audio renderer
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "BaseIface.h"


namespace Fwrap
{
    class IFrameSrcAudio : IBase0
    {
    protected:
        IFrameSrcAudio() = default;
        ~IFrameSrcAudio() = default;
    public:
        virtual void Log(const char* str, int r) const = 0;
        virtual void NotifyFreeBuffer(bool free) = 0;
    };

    class IFrameSinkAudio : IBase0
    {
    protected:
        IFrameSinkAudio() = default;
        ~IFrameSinkAudio() = default;

    public:
        virtual bool SetFrameSrc(IFrameSrcAudio* src) = 0;
        virtual bool Open(int chann, int sampleRate, int bytesPerSample, int buffDurMs, int devId) = 0;
        virtual bool WriteFrame(const void* data, int size, bool& copied) = 0;
        virtual bool SweepOutBuffers(bool& end) = 0;
        virtual void Close() = 0;

        virtual int Chann() const = 0;
        virtual int SampleRate() const = 0;
        virtual int BytesPerSample() const = 0;

        virtual double Position() = 0;
        virtual void SetVolume(double vol) = 0;
        virtual int  RendBufferSize() const = 0;

        virtual void Run() = 0;
        virtual void Pause() = 0;
        virtual void Stop() = 0;

        virtual void EnableAudioWatch() = 0;

        virtual void Detach() = 0;

        static const int ErrorAuduioRend = -1020;

    }; // class IFrameSinkAudio

} // namespace Fwrap
