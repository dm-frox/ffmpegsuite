#pragma once

// ---------------------------------------------------------------------
// File: AudioWatch.h
// Classes: AudioWatch
// Purpose: class is used if video is synchronized over audio and audio track is shorter than video
// Module: AudioRendLib - audio renderer based on windows waveform audio API (winmm.lib)
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <atomic>
#include <mutex>
#include <chrono>


class AudioWatch
{
    using MutexType = std::mutex;
    using AutoLock  = std::lock_guard<MutexType>;

    using ClockType     = std::chrono::steady_clock;
    using TimePointType = typename ClockType::time_point;
    using DurationType  = std::chrono::duration<double>;

// data
private:

    mutable MutexType m_Mutex{};
    std::atomic<bool> m_Enabled { false };
    double            m_BasePosition{ 0.0 };
    double            m_PausePosition{ 0.0 };
    TimePointType     m_StartTime{};
    bool              m_Paused{false};

// ctor, dtor, copying
public:
    AudioWatch();
    ~AudioWatch();

    AudioWatch(const AudioWatch&) = delete;
    AudioWatch& operator=(const AudioWatch&) = delete;
// properties
public:
    bool Enabled() { return m_Enabled; }
// operations
public:
    void Enable(double basePosition);
    void Pause();
    void Run();
    void Reset();
    double Position() const;
private:
    double Pos() const;
    double RunningTime() const;

}; // class AudioWatch
