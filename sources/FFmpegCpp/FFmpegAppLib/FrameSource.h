#pragma once

// ---------------------------------------------------------------------
// File: FrameSource.h
// Classes: FrameSource
// Purpose: complex class to provide reading and decoding packets, frame convertion and filtering, 
// base class for FrameSourceEx
// Module: FFmpegAppLib - aplications based on classes from FFmpegCoreLib
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <string>
#include <vector>

#include "ffmpeg.h"
#include "FrameHub.h"
#include "ChannLayout.h"

#include "DemuxerVA.h"

namespace Fwrap
{
    class IMediaSourceInfo;
}

namespace Fcore
{
    class FilterGraph;
    class Muxer;
}

namespace Fapp
{

    class Transcoder;

    class FrameSource
    {
// types
        using PixFmtList = std::vector<AVPixelFormat>;
// data
    private:
        static const char SubtitFltPrefix[];

        bool const    m_UseFilterGraphAlways;
        bool const    m_ConvFrameSeq;

        DemuxerVA     m_Demuxer;

        int           m_MaxChannAudio{ 0 };
        int           m_ReduceSampleFormat{ 0 };

        int           m_Width{ 0 };
        int           m_Height{ 0 };
        AVPixelFormat m_PixFmt{ AV_PIX_FMT_NONE };
        PixFmtList    m_PixFmts{};
        std::string   m_VidFilter{};
        AVRational    m_FrameRate{0, 1};

        Fcore::ChannLayout m_ChannLayout{};
        int                m_SampleRate{ 0 };
        AVSampleFormat     m_SampFmt{ AV_SAMPLE_FMT_NONE };
        int                m_FrameSize{ 0 };
        std::string        m_AudFilter{};

    protected:
        Fcore::FrameHub m_FrameHubVideo;
        Fcore::FrameHub m_FrameHubAudio;

// ctor, dtor, copying
    public:
        FrameSource(bool useFilterGraphAlways, bool convFrameSequentially, bool player);
        FrameSource();
        ~FrameSource();
// properties
    public:
        bool HasVideo() const { return MediaSourceInfo()->HasVideo(); }
        int Width() const { return m_Width; }
        int Height() const { return m_Height; }
        AVPixelFormat PixelFormat() const { return m_PixFmt; }
        AVRational FrameRate() const { return m_FrameRate; }
        AVRational TimebaseVideo() const { return m_FrameHubVideo.Timebase(); }
        AVRational TimebaseAudio() const { return m_FrameHubAudio.Timebase(); }
        bool Proceeding() const { return m_Demuxer.Proceeding(); }

        bool HasAudio() const { return MediaSourceInfo()->HasAudio(); }
        int Chann() const { return m_ChannLayout.Chann(); }
        int SampleRate() const { return m_SampleRate; }
        AVSampleFormat SampleFormat() const { return m_SampFmt; };
        const Fcore::ChannLayout& ChannLayout() const { return m_ChannLayout; }
        int FrameSize() const { return m_FrameSize; }

        bool HasMediaSource() const { return HasVideo() || HasAudio(); }

        double Duration() const { return MediaSourceInfo()->Duration(); }
        double DurationAudio() const { return m_Demuxer.DurationAudio(); }

        const Fwrap::IMediaSourceInfo* MediaSourceInfo() const { return m_Demuxer.MediaSourceInfo(); }
        const Fcore::Demuxer& Demuxer() const { return m_Demuxer.Demuxer(); }

        Fcore::FrameHub* FrameHubVideo() { return &m_FrameHubVideo; }
        Fcore::FrameHub* FrameHubAudio() { return &m_FrameHubAudio; }

        void SetRendPumpAudio_();

// operations    	
    public:
        int OpenDemuxer(const char* url, const char* fmt, const char* fmtOps, bool useVideo, bool useAudio);

        void SetFrameParamsVideo(int width, int height, const char* pixFmts, const char* vidFilter, int fpsFactor);
        void SetFrameParamsAudio(int chann, int sampleRate, const char* sampFmt, const char* channLayout, const char* audFilter);
        void SetRendModeAudio(int maxChann, int reduceSampleFormat);

        int ConnectToFrameHubVideo(const char* decoderParams, Transcoder* trans = nullptr);
        int ConnectToFrameHubAudio(const char* decoderParams, Transcoder* trans = nullptr);

        int ConnectToMuxerVideo(Fcore::Muxer* muxer) { return m_Demuxer.ConnectToMuxerVideo(muxer); }
        int ConnectToMuxerAudio(Fcore::Muxer* muxer) { return m_Demuxer.ConnectToMuxerAudio(muxer); }

        int Build(const char* url, const char* fmt, const char* fmtOps,
            bool useVideo, const char* decPrmsVideo, bool useAudio, const char* decPrmsAudio);

        bool CreatePump() { return m_Demuxer.CreatePump(); }
        void ClosePump() { m_Demuxer.ClosePump(); }
        void Run() { m_Demuxer.Run(); }
        void Pause() { m_Demuxer.Pause(); }
        void Stop(bool skipPause);
        bool PumpError() const { return m_Demuxer.PumpError(); }
        int SetPosition(bool skipPause, double pos);
        void CancelInput() { m_Demuxer.CancelInput(); }

    private:
        void SetupFrameParamsVideo();
        void SetupFrameParamsAudio();
        void SetPixFmts(const char* pixFmts);
        void SetPixFmt(AVPixelFormat pixFmt);

        int SetupFrameParamsVideoEx(Transcoder* trans);
        int SetupFrameParamsAudioEx(Transcoder* trans);

        int SetupFilterVideo();
        int SetupFilterAudio();

        bool ReduceChann(int chann, int& channRed) const;
        AVSampleFormat ReduceSampFmt(AVSampleFormat srcSampFmt) const;

        void Reset();

        std::string PixFmtsStr() const;

    }; // class FrameSource

} // namespace Fapp

