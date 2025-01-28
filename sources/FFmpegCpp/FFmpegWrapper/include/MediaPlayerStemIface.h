#pragma once

// ---------------------------------------------------------------------
// File: MediaPlayerBoxIface.h
// Classes: IMediaPlayerStem (abstract)
// Purpose: interface for the complex object which reads, decodes media streams 
// and pass decoded frames to a consumer via callback interfaces IFrameSinkVideo, IFrameSinkAudio
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "HdrReaderBaseIface.h"

namespace Fwrap
{
    class IMediaSourceInfo;

    class IMediaPlayerStem : public IHdrReaderBase
    {
    protected:
        IMediaPlayerStem() = default;
        ~IMediaPlayerStem() = default;

    public:
        virtual void SetFrameParamsVideo(int width, int height, const char* pixFmt, const char* vidFilter, int fpsFactor) = 0;
        virtual void SetRendModeAudio(int maxChann, int reduceSampleFormat) = 0;
        virtual void SetFrameParamsAudio(int chann, int sampleRate, const char* sampFmt, const char* channLayout, const char* audFilter) = 0;

        virtual bool Build(const char* url, const char* fmt, const char* fmtOps, 
            bool useVideo, const char* decPrmsVideo, bool useAudio, const char* decPrmsAudio) = 0;

        virtual bool SetupRendering(
            void* rendVideo, bool holdPresentationTime, bool synkByAudio, bool swapFields, 
            void* rendAudio, int audioDevId) = 0;

        virtual const IMediaSourceInfo* MediaSourceInfo() const = 0;

        virtual bool HasVideo() const = 0;
        virtual int Width() const = 0;
        virtual int Height() const = 0;
        virtual const char* PixelFormatStr() const = 0;

        virtual bool HasAudio() const = 0;
        virtual int Chann() const = 0;
        virtual int SampleRate() const = 0;
        virtual const char* SampleFormatStr() const = 0;

        virtual int RendWidth() const = 0;
        virtual int RendHeight() const = 0;

        virtual int RendChann() const = 0;
        virtual int RendSampleRate() const = 0;

        virtual bool EndOfData() const = 0;
        virtual bool EndOfVideo() const = 0;
        virtual double Position() const = 0;
        virtual double VideoRendPosition() const = 0;
        virtual bool Proceeding() const = 0;

        virtual int SetPosition(double pos, bool onPause) = 0;
        virtual bool SeekingInProgress() const = 0;
        virtual double GetShiftedVideoPosition(int frameCount) const = 0;
        virtual bool ShiftVideoPositionFwd(int frameCount) = 0;
        virtual void SetAudioVolume(double vol) = 0;

        virtual void Run() = 0;
        virtual void Pause() = 0;
        virtual void Stop() = 0;
        virtual bool PumpError() const = 0;

        virtual void StartRewinder(int fps) = 0;
        virtual void StopRewinder() = 0;

        virtual int SendFilterCommandVideo(const char* tar, const char* cmd, const char* arg) = 0;
        virtual int ReconfigureFilterGraphVideo(const char* filterString) = 0;

        virtual int SendFilterCommandAudio(const char* tar, const char* cmd, const char* arg) = 0;
        virtual int ReconfigureFilterGraphAudio(const char* filterString) = 0;

        static IMediaPlayerStem* CreateInstance(bool useFilterGraphAlways, bool convFrameSequentially);

    }; // class IMediaPlayerStem

} // namespace Fwrap
