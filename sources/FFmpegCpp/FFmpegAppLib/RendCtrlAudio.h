#pragma once

// ---------------------------------------------------------------------
// File: RendCtrlAudio.h
// Classes: RendCtrlAudio
// Purpose: class to support audio rendering
// Module: FFmpegAppLib - aplications based on clases from FFmpegCoreLib
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "include/FrameSinkAudioIface.h"

#include "RendCtrlBase.h"

namespace Fapp
{
    class RendCtrlVideo;

    class RendCtrlAudio : public RendCtrlBase, RendCtrlBase::IProceed, Fwrap::IFrameSrcAudio
    {
// types
    private:
        using Base       = RendCtrlBase;
        using IFrameSink = Fwrap::IFrameSinkAudio;

// data
    private:
        static const bool UseCondVar;
        static const char ThreadTag[];
        static const int  RendBuffDurMs;
        static const int  WaitVideoFrameInterval;

        IFrameSink*          m_FrameSink { nullptr };
        int                  m_BytesPerSample { 0 };
        int                  m_DevId{ -1 };

        double               m_BasePosition{ 0.0 };
        const RendCtrlVideo* m_RendCtrlVideo{ nullptr };
        bool                 m_IsReady{ false };
        bool                 m_EndOfData{ false };
// ctor, dtor, copying
    public:
        RendCtrlAudio();
        ~RendCtrlAudio();
// properties
    public:
        double Position() const;
        void SetVolume(double vol);
        int Chann() const { return m_FrameSink ? m_FrameSink->Chann() : 0; }
        int SampleRate() const { return m_FrameSink ? m_FrameSink->SampleRate() : 0; }
// operations
    private:
        bool FirstVideoFrameWritten();
        int WriteFrame();
        int SweepOutBuffers();
        void Open(int chann, int sampRate, int frameSize);
        void CloseIfFrameParamsChanged(int chann, int sampRate);
    public:
        void AttachFrameHub(IFrameData* frameHub);
        void EnablePresentationTime(bool enable);
        bool SetFrameSink(IFrameSink* sink, int bytesPerSample, int devId);
        void Run();
        void Pause();
        void Stop(bool skipPause);
        void Stop(bool skipPause, double pos);
        void SetRendCtrlVideo(const RendCtrlVideo* rendCtrlVideo) { m_RendCtrlVideo = rendCtrlVideo; }
// IProceed impl
    private:
        int Proceed() override final;
// Fwrap::IFrameSrcAudio impl
    private:
        void Log(const char* str, int r) const override final;
        void NotifyFreeBuffer(bool free) override final;

    }; // class RendCtrlAudio

} // namespace Fapp
