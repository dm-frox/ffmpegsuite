#pragma once

// ---------------------------------------------------------------------
// File: MediaPlayerStem.h
// Classes: MediaPlayerStem
// Purpose: complex class to provide reading and decoding packets, frame transformation
// Module: FFmpegAppLib - aplications based on clases from FFmpegCoreLib
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "FrameSourceEx.h"
#include "RendCtrlVideo.h"
#include "RendCtrlAudio.h"

namespace Fwrap
{
    class IMediaSourceInfo;
    class IFrameSinkVideo;
    class IFrameSinkAudio;
}

namespace Fapp
{
    class MediaPlayerStem
    {
 // types
    private:
        enum class State { OutOfSource, Stopped, Running, Paused };
// data
    private:

        FrameSourceEx m_FrameSource;
        RendCtrlVideo m_RendCtrlVideo;
        RendCtrlAudio m_RendCtrlAudio;

        State         m_State{ State::OutOfSource };
// ctor, dtor, copying
    public:
        MediaPlayerStem(bool useFilterGraphAlways, bool convFrameSequentially);
        ~MediaPlayerStem();
// properties
    public:
        bool HasVideo() const { return m_FrameSource.HasVideo(); }
        int Width() const { return m_FrameSource.Width(); }
        int Height() const { return m_FrameSource.Height(); }
        const char* PixelFormatStr() const;

        bool HasAudio() const { return m_FrameSource.HasAudio(); }
        int Chann() const { return m_FrameSource.Chann(); }
        int SampleRate() const { return m_FrameSource.SampleRate(); }

        int RendWidth() const { return m_RendCtrlVideo.Width(); }
        int RendHeight() const { return m_RendCtrlVideo.Height(); }

        int RendChann() const { return m_RendCtrlAudio.Chann(); }
        int RendSampleRate() const { return m_RendCtrlAudio.SampleRate(); }
        const char* SampleFormatStr() const;

        bool EndOfData() const;
        bool EndOfVideo() const;

        double Position() const;
        double VideoRendPosition() const { return m_RendCtrlVideo.RendPosition(); }
        int SetPosition(double pos, bool onPause);
        bool SeekingInProgress() const; 
        void SetAudioVolume(double vol) { m_RendCtrlAudio.SetVolume(vol); }

        void SetFrameParamsVideo(int width, int height, const char* pixFmt, const char* vidFilter, int fpsFactor)
        {
            m_FrameSource.SetFrameParamsVideo(width, height, pixFmt, vidFilter, fpsFactor);
        }

        void SetFrameParamsAudio(int chann, int sampleRate, const char* sampFmt, const char* channLayout, const char* audFilter)
        {
            m_FrameSource.SetFrameParamsAudio(chann, sampleRate, sampFmt, channLayout, audFilter);
        }

        void SetRendModeAudio(int maxChann, int reduceSampleFormat)
        {
            m_FrameSource.SetRendModeAudio(maxChann, reduceSampleFormat);
        }

        const Fwrap::IMediaSourceInfo* MediaSourceInfo() const { return m_FrameSource.MediaSourceInfo(); }
        const Fcore::Demuxer& Demuxer() const { return m_FrameSource.Demuxer(); }

        bool Proceeding() const;

// operations
    public:
        bool Build(const char* url, const char* fmt, const char* fmtOps,
            bool useVideo, const char* decPrmsVideo, bool useAudio, const char* decPrmsAudio);

        bool SetupRendering(
            Fwrap::IFrameSinkVideo* rendVideo, bool holdPresentationTime, bool syncByAudio, bool swapFields, 
            Fwrap::IFrameSinkAudio* rendAudio, int audioDevId);

        void Run();
        void Pause();
        void Stop();
        bool PumpError() const;

        void StartRewinder(int fps);
        void StopRewinder();

        int SendFilterCommandVideo(const char* tar, const char* cmd, const char* arg);

        int SendFilterCommandAudio(const char* tar, const char* cmd, const char* arg)
        {
            return m_FrameSource.SendFilterCommandAudio(tar, cmd, arg);
        }

        int ReconfigureFilterGraphVideo(const char* filterString)
        {
            return m_FrameSource.ReconfigureFilterGraphVideo(filterString);
        }

        int ReconfigureFilterGraphAudio(const char* filterString)
        {
            return m_FrameSource.ReconfigureFilterGraphAudio(filterString);
        }

        double GetShiftedVideoPosition(int frameCount) const;
        bool ShiftVideoPositionFwd(int frameCount);

    private:
        bool CreatePump();
        void ClosePump();
        double VideoPosition() const;

    }; // class MediaPlayerStem

} // namespace Fapp

