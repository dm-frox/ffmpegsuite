#pragma once

// ---------------------------------------------------------------------
// File: Rewinder.h
// Classes: Rewinder
// Purpose: a helper to control presentation time
// Module: FFmpegAppLib - aplications based on clases from FFmpegCoreLib
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <atomic>

#include "VideoWatchBase.h"

namespace Fapp
{
    class Rewinder : VideoWatchBase
    {
        using DurationType = std::chrono::milliseconds;

        const int        m_Threshold;

        std::atomic_bool m_Running{ false };
        int              m_FrameTimeMs{ 0 };
        int64_t          m_Pts{ 0 };
    public:
        Rewinder(int threshold) : m_Threshold(threshold) {}
        ~Rewinder();

        void Start(int fps);
        void Stop();
        bool IsRunning() const { return m_Running; }
        int CheckTime();

    }; // class Rewinder

} // namespace Fapp




