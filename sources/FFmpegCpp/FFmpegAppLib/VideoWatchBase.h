#pragma once

// ---------------------------------------------------------------------
// File: VideoWatchBase.h
// Classes: VideoWatchBase
// Purpose: base class for VideoWatch, Rewinder
// Module: FFmpegAppLib - aplications based on clases from FFmpegCoreLib
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <chrono>
#include <mutex>

#include "NonCopyable.h"

namespace Fapp
{
    class VideoWatchBase : Fcore::NonCopyable
    {
// types
    private:
        using ClockType     = std::chrono::steady_clock;
        using TimePointType = ClockType::time_point;
        using MutexType     = std::mutex;

    protected:
        using AutoLock      = std::lock_guard<MutexType>;

// data
    private:
        TimePointType m_StartTime{ TimePointType::min() };

    protected:
        MutexType     m_Mutex;

        template<typename D>
        static D DurationCast(typename ClockType::duration dur)
        {
            return std::chrono::duration_cast<D>(dur);
        }

    protected:

        VideoWatchBase() = default;
        ~VideoWatchBase() = default;

        void SetStartTime() { m_StartTime = ClockType::now(); }
        void ResetStartTime() { m_StartTime = TimePointType::min(); }


        template<typename D>
        auto CurrTime() const -> typename D::rep
        {
            return (DurationCast<D>(ClockType::now() - m_StartTime)).count();
        }

        template<typename D>
        static auto StopTime() -> typename D::rep
        {
            return (DurationCast<D>(TimePointType::max() - ClockType::now())).count();
        }

    public:
        static long long TicksPerSecond()
        {
            return ClockType::period::den / ClockType::period::num;
        }

    }; // class VideoWatchBase

} // namespace Fapp


