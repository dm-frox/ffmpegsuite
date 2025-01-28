#pragma once

// ---------------------------------------------------------------------
// File: FrameSinkAudioImpl.h
// Classes: FrameSinkAudioImpl
// Purpose: Fwrap::IFrameSinkAudio implementation
// Module: AudioRendLib - audio renderer based on windows waveform audio API (winmm.lib)
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------


#include "include/FrameSinkAudioIface.h"

#include "WaveCore.h"
#include "AudioWatch.h"
#include "FrameTailBuff.h"

class FrameSinkAudioImpl : Fwrap::IFrameSinkAudio
{
// types
private:
    using IFrameSrc = Fwrap::IFrameSrcAudio;

// data
private:
    static const int MaxFrameSize;

    WaveCore      m_WaveCore;
    AudioWatch    m_AudioWatch;

    FrameTailBuff m_FrameTailBuff{};

    IFrameSrc*    m_FrameSrc{ nullptr };

    WaveBuffer*   m_CurrBuff{ nullptr };

// ctor, dtor, copying
    FrameSinkAudioImpl();
    ~FrameSinkAudioImpl();

// Fwrap::IFrameSinkAudio impl
public:
    bool SetFrameSrc(IFrameSrc* src) override final;
    bool Open(int chann, int sampleRate, int bytesPerSample, int buffDurMs, int devId) override final;

    bool WriteFrame(const void* data, int size, bool& copied) override final;
    bool SweepOutBuffers(bool& end) override final;
    void Close() override final;

    int Chann() const override final;
    int SampleRate() const override final;
    int BytesPerSample() const override final;

    double Position() override final;
    void SetVolume(double vol) override final;
    int  RendBufferSize() const override final;

    void Run() override final;
    void Pause() override final;
    void Stop() override final;

    void EnableAudioWatch();

    void Detach() override final;

private:
    void DisablePumpWrite();
    void AddFrameToCurrBuff(const void* data, int size);
    void AddTailToCurrBuff();

// friends
    friend void* CreateFrameSinkAudio();

}; // class FrameSinkAudioImpl