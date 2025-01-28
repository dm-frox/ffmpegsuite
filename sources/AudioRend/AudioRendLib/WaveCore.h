#pragma once

// ---------------------------------------------------------------------
// File: WaveCore.h
// Classes: WaveCore, IAudioData
// Purpose: wrapper for windows waveform audio API
// Module: AudioRendLib - audio renderer based on windows waveform audio API (winmm.lib)
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <mutex>

#include "HWrap.h"
#include "WaveBuffer.h"

namespace Fwrap
{
    class IFrameSrcAudio;
}

class WaveCore
{
// types
private:
    using Mutex     = std::mutex;
    using AutoLock  = std::lock_guard<Mutex>;
    using IFrameSrc = Fwrap::IFrameSrcAudio;


    struct WaveDeleter
    {
        void operator()(HWAVEOUT h) const { ::waveOutClose(h); }
    };

    using HWaveOutWrap = HWrap<HWAVEOUT, WaveDeleter>;

    enum class PlayerState
    {
        Error = -1, OutOfSource = 0, Stopped, Playing, Paused
    };

// data
private:

    static const DWORD  InvalidPos;

    static const int    MaxVolume;
    static const double MaxVolumeD;

    static const int    DefBuffDurationMs;
    static const int    BuffAlignSamp;
    static const int    WaitMarker;
    
    IFrameSrc*    m_FrameSrc{ nullptr };

    mutable Mutex m_MtxWave;
    mutable Mutex m_MtxCallback;

    HWaveOutWrap  m_WaveOut{};
    MMTIME        m_MMTIME{};
    WaveBuffers   m_WaveBuffers{};

    int           m_Chann{ 0 };
    int           m_SampleRate{ 0 };
    int           m_Bps{ 0 };

    PlayerState   m_State{ PlayerState::OutOfSource };
    double        m_PositionFactor{ 0.0 };

    DWORD         m_Volume;
    bool          m_RendStarted{ false };

// ctor, dtor, copying
public:
    WaveCore();
    ~WaveCore();
// properties
public:
    bool IsReady() const { return m_WaveOut.IsValid(); }
    double PositionSec();
    bool SetVolumeSq(double vol);
    bool BuffersAreFree() const;
    int BufferSize() const;
    int Chann() const { return m_Chann; }
    int SampleRate() const { return m_SampleRate; }
    int BytesPerSample() const { return m_Bps; }
private:
    DWORD Position();
    int Volume();
    bool SetVolume(int volume);
    bool SetVolume();
// operations
public:
    bool SetFrameSrc(IFrameSrc* src);
    bool Open(int chann, int sampleRate, int bytesPerSample, int buffDurMs, int devId);
    void Close();

    bool BeginPause();
    bool EndPause();
    bool Reset();
    bool Stop();

    WaveBuffer* GetFreeBuffer();
    bool WriteBuffer(WaveBuffer* buff);
private:
    bool WaveOutOpen(const WAVEFORMATEX& fmt, int devId);
    bool InitBuffers(const WAVEFORMATEX& fmt, int buffDurMs);
    void SetPositionType(const WAVEFORMATEX& fmt);
    bool WriteBuffer(WAVEHDR* hdr);
    bool MMR2Bool(MMRESULT res, const char* msg) const;
    void Log(const char* str, int r) const;
//
    void WomDone(DWORD dwParam);

    static void CALLBACK WaveOutProc(
        HWAVEOUT hwo, 
        UINT uMsg,         
        DWORD_PTR dwInstance,  
        DWORD dwParam1,    
        DWORD dwParam2);
//
    static DWORD MakeVolume(int vol);
    static void WaveFormat_Fill(int chann, int sampleRate, int bytesPerSample, WAVEFORMATEX& fmt);

}; // class WaveCore

