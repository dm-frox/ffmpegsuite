#pragma once

// ---------------------------------------------------------------------
// File: VideoWatch.h
// Classes: VideoWatch
// Purpose: a helper to control presentation time
// Module: FFmpegAppLib - aplications based on clases from FFmpegCoreLib
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "VideoWatchBase.h"

namespace Fapp
{
    class VideoWatch : public VideoWatchBase
    {
// types
    private:
        using DurationType  = std::chrono::duration<double>;
// data
    private:
//        MutexType     m_Mutex;
        bool          m_Running { false };
        bool          m_Stopped { true };
        double        m_RendPos { 0.0 };
        double        m_StartFramePos { 0.0 };

 // ctor, dtor, copying
    public:
        VideoWatch();
        ~VideoWatch();
 // properties
    public:
        double CurrentTime();
        double Position();
        void SetRendPos(double pos);
    private:
        double RunningTime() const;
// operations
    public:
        void Run();
        void Pause();
        void Stop(double pos = 0.0);

    }; // class VideoWatch

} // namespace Fapp
