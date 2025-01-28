#include "pch.h"

#include "Rewinder.h"

#include "Logger.h"

namespace Fapp
{

    Rewinder::~Rewinder() = default;

    void Rewinder::Start(int fps)
    {
        AutoLock al(m_Mutex);
        SetStartTime();
        m_FrameTimeMs = 1000 / fps;
        m_Pts = 0;
        m_Running = true;
        LOG_INFO("%s, fps=%d, frame time=%d", __FUNCTION__, fps, m_FrameTimeMs);
    }

    void Rewinder::Stop()
    {
        AutoLock al(m_Mutex);
        m_Running = false;
        ResetStartTime();
        m_FrameTimeMs = 0;
        m_Pts = 0;
        LOG_INFO(__FUNCTION__);
    }

    int Rewinder::CheckTime()
    {
        AutoLock al(m_Mutex);
        auto dd = CurrTime<DurationType>();
        int delt = static_cast<int>(m_Pts - dd);
        int ret = delt > m_Threshold ? delt : 0;
        if (ret == 0)
        {
            m_Pts += m_FrameTimeMs;
        }
        return ret;
    }

} // namespace Fapp
