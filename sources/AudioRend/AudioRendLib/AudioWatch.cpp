//#include "pch.h"  // to exclude windows.h

#include "AudioWatch.h"


AudioWatch::AudioWatch() = default;

AudioWatch::~AudioWatch() = default;

void AudioWatch::Reset()
{ 
    if (m_Enabled)
    {
        AutoLock al(m_Mutex);
        m_Enabled = false;
        m_BasePosition = 0.0;
        m_PausePosition = 0.0;
        m_StartTime = TimePointType::min();
        m_Paused = false;
    }
}

void AudioWatch::Enable(double basePosition)
{ 
    AutoLock al(m_Mutex);
    m_BasePosition = basePosition;
    m_PausePosition = 0.0;
    m_StartTime = ClockType::now();
    m_Paused = false;
    m_Enabled = true;
}

double AudioWatch::RunningTime() const
{
    auto rt = ClockType::now() - m_StartTime;
    return std::chrono::duration_cast<DurationType>(rt).count();
}

double AudioWatch::Pos() const
{
    double pos = (m_PausePosition > 0.0) ? m_PausePosition : m_BasePosition;
    return m_Paused ? pos : pos + RunningTime();
}

double AudioWatch::Position() const
{
    AutoLock al(m_Mutex);
    return Pos();
}

void AudioWatch::Pause()
{
    if (m_Enabled)
    {
        AutoLock al(m_Mutex);
        m_PausePosition = Pos();
        m_Paused = true;
    }
}

void AudioWatch::Run()
{
    if (m_Enabled)
    {
        AutoLock al(m_Mutex);
        m_StartTime = ClockType::now();
        m_Paused = false;
    }
}
