#pragma once

// ---------------------------------------------------------------------
// File: MediaPlayerStemImpl.h
// Classes: MediaPlayerStemImpl
// Purpose: IMediaPlayerStem implementation
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "MediaPlayerStem.h"

#include "include/MediaPlayerStemIface.h"

namespace Fwrap
{

    class MediaPlayerStemImpl : IMediaPlayerStem
    {
        Fapp::MediaPlayerStem m_MediaPlayerStem;

        MediaPlayerStemImpl(bool useFilterGraphAlways, bool convFrameSequentially);
        ~MediaPlayerStemImpl();

// IBase  impl
        void Delete() override final;
// IHdrReaderBase  impl
        int StreamCount() const override final;
        CStrEx GetInfo() const override final;
        CStrEx GetMetadata() const override final;
        CStrEx GetChapters() const override final;
        CStrEx GetStreamInfo(int strmIdx) const override final;
        CStrEx GetStreamMetadata(int strmIdx) const override final;
// IMediaPlayerStem  impl
        void SetFrameParamsVideo(int width, int height, const char* pixFmt, const char* vidFilter, int fpsFactor) override final;
        void SetRendModeAudio(int maxChann, int reduceSampleFormat) override final;
        void SetFrameParamsAudio(int chann, int sampleRate, const char* sampFmt, const char* channLayout, const char* audFilter) override final;

        bool Build(const char* url, const char* fmt, const char* fmtOps,
            bool useVideo, const char* decPrmsVideo, bool useAudio, const char* decPrmsAudio)  override final;

        bool SetupRendering(
            void* rendVideo, bool holdPresentationTime, bool synkByAudio, bool swapFields,
            void* rendAudio, int audioDevId) override final;

        const IMediaSourceInfo* MediaSourceInfo() const override final;

        bool HasVideo() const override final;
        int Width() const override final;
        int Height() const override final;
        const char* PixelFormatStr() const override final;

        bool HasAudio() const override final;
        int Chann() const override final;
        int SampleRate() const override final;
        const char* SampleFormatStr() const override final;

        int RendWidth() const override final;
        int RendHeight() const override final;

        int RendChann() const override final;
        int RendSampleRate() const override final;

        bool EndOfData() const  override final;
        bool EndOfVideo() const  override final;
        double Position() const override final;
        double VideoRendPosition() const override final;
        bool Proceeding() const override final;

        int SetPosition(double pos, bool onPause) override final;
        bool SeekingInProgress() const override final;
        double GetShiftedVideoPosition(int frameCount) const override final;
        bool ShiftVideoPositionFwd(int k) override final;
        void SetAudioVolume(double vol) override final;

        void Run() override final;
        void Pause() override final;
        void Stop() override final;
        bool PumpError() const override final;

        void StartRewinder(int fps) override final;
        void StopRewinder() override final;

        int SendFilterCommandVideo(const char* tar, const char* cmd, const char* arg) override final;
        int ReconfigureFilterGraphVideo(const char* filterString) override final;

        int SendFilterCommandAudio(const char* tar, const char* cmd, const char* arg) override final;
        int ReconfigureFilterGraphAudio(const char* filterString) override final;
// factory
        friend IMediaPlayerStem* IMediaPlayerStem::CreateInstance(bool useFilterGraphAlways, bool convFrameSequentially);

    }; // class MediaPlayerStemImpl

} // namespace Fwrap
