#include "pch.h"

#include "VideoWatch.h"

#include "Logger.h"

namespace Fapp
{
    VideoWatch::VideoWatch()
    {
        LOG_INFO("%s, ticks per second=%lld", __FUNCTION__, TicksPerSecond());
    }

    VideoWatch::~VideoWatch() = default;

// gui (control) thread

    void VideoWatch::Run()
    {
        AutoLock al(m_Mutex);
        m_Running = true;
    }

    void VideoWatch::Pause()
    {
        AutoLock al(m_Mutex);
        m_Running = false;
        m_Stopped = true;
    }

    void VideoWatch::Stop(double pos)
    {
        AutoLock al(m_Mutex);
        m_Running = false;
        m_Stopped = true;
        m_RendPos = pos;
        m_StartFramePos = 0.0;
        ResetStartTime();
    }

    double VideoWatch::Position()
    {
        AutoLock al(m_Mutex);
        double ret = m_RendPos;
        return ret;
    }

// proceed thread

    void VideoWatch::SetRendPos(double pos) 
    {
        AutoLock al(m_Mutex);
        m_RendPos = pos;
        if (m_Running && m_Stopped) // catch first running frame after stop/pause
        {
            m_StartFramePos = m_RendPos;
            m_Stopped = false;
            SetStartTime();
        }
    }

    double VideoWatch::RunningTime() const
    {
        return !m_Stopped
            ? CurrTime<DurationType>()
            : StopTime<DurationType>();
    }

    double VideoWatch::CurrentTime()
    {
        AutoLock al(m_Mutex);
        double ret = m_Running
            ? m_StartFramePos + RunningTime()
            : m_RendPos;
        return ret;
    }

} // namespace Fapp
