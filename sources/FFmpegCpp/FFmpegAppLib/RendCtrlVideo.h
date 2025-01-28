#pragma once

// -------------------------------------------------------------------- -
// File: RendCtrlVideo.h
// Classes: RendCtrlVideo
// Purpose: class to support video rendering
// Module: FFmpegAppLib - aplications based on clases from FFmpegCoreLib
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "RendCtrlBase.h"

#include <atomic>
#include <string>

#include "ffmpeg.h"
#include "FrameTools.h"
#include "FrameBackup.h"
#include "SeekCtrl.h"
#include "Rewinder.h"

namespace Fwrap
{
    class IFrameSinkVideo;
}

namespace Fcore
{
    class IFrameConv;
}

namespace Fapp
{
    class VideoWatch;
    class RendCtrlAudio;

    class RendCtrlVideo : public RendCtrlBase, RendCtrlBase::IProceed
    {
// types
    private:
        using Base       = RendCtrlBase;
        using IFrameConv = Fcore::IFrameConv;
        using IFrameSink = Fwrap::IFrameSinkVideo;
        using FrameTools = Fcore::FrameTools;

// data
    private:
        static const bool UseCondVar;
        static const char ThreadTag[];
        static const int  FrameDelayThresholdMs;
        static const int  MaxFrameDelayMs;
        static const int  NoFrameWaitIntervalMs;
        static const int  WaitFrameWrittenIntervalMs;
        static const int  WaitFrameWrittenCount;

        VideoWatch*        m_VideoWatch { nullptr };
        RendCtrlAudio*     m_RendAudio { nullptr };
        IFrameSink*        m_FrameSink { nullptr };
        IFrameConv*        m_FrameConv{ nullptr };
        AVFrame*           m_Frame{ nullptr };
        int                m_RendPlanes{ 1 };
        Fcore::FrameBackup m_FrameBackup{};
        std::atomic<bool>  m_Paused{ false };
        std::atomic<bool>  m_FrameWritten{ false };
        Fcore::SeekCtrl    m_SeekCtrl;
        Rewinder           m_Rewinder;

        Fcore::LineSwap    m_LineSwap;
        bool               m_SwapFields{ false };
// ctor, dtor, copying
    public:
        RendCtrlVideo();
        ~RendCtrlVideo();
// properties
    public:
        bool SyncByAudioEnabled() const { return m_RendAudio ? true : false; }
        double Position() const;
        double RendPosition() const { return Base::RendPos(); }
        int Width() const;
        int Height() const;
        bool FrameWritten() const { return m_FrameWritten; }
        void SetSwapFields() { m_SwapFields = true; }
// operations
    public:
        void AttachFrameHub(IFrameData* frameHub, IFrameConv* frameConv);
        void SetSyncMode(bool holdPresentationTime, RendCtrlAudio* rendererAudio);
        bool SetFrameSink(IFrameSink* sink, AVPixelFormat pixFmt);

        void Run();
        void Pause();
        void Stop(bool skipPause);
        void Stop(bool skipPause, double pos);

        void StartRewinder(int fps) { m_Rewinder.Start(fps); }
        void StopRewinder() { m_Rewinder.Stop(); }

        int RenderFrame();
        int UpdatePictupeFromFrameBackup();
        void SetPosition(double pos, bool paused) { m_SeekCtrl.SetPosition(pos, paused); }
        bool SeekingInProgress() const { return m_SeekCtrl.InProgress(); }
        bool EndOfVideoByQueue() const;
    private:
        int CheckVideoFrame(bool& drop);
        int RenderFrame(const AVFrame* frame);
        int ConvertAndRenderFrame(AVFrame* frame);
        bool FrameFilter() const;
        int WriteFrame(const AVFrame* frame);
        double CurrentTime();

        static bool WriteFrame(IFrameSink* sink, const AVFrame* frame, int rendPlanes);
        static int ToMilliseconds(double tsec);
        static bool AssertFrame(const AVFrame* frame);
// IProceed impl
    private:
        int Proceed() override final;
//
    }; // class RendCtrlVideo

} // namespace Fapp
