#include "pch.h"

#include "ProceedPump.h"

#include <exception>

#include "ThreadTools.h"
#include "ProceedIface.h"
#include "SLogger.h"


//#define USE_LOG_EX_

namespace Ftool
{

    const int ProceedPump::StopInterval   = 20;
    const int ProceedPump::PauseInterval  = IProceed::EndOfDataMarker + 1;
    const char ProceedPump::DefaultTag[]  = "-anon";

    ProceedPump::ProceedPump() = default;
 
    ProceedPump::~ProceedPump()
    {
        ClosePump();
    }

#pragma warning(disable:4100)
    void ProceedPump::Init(IProceed* pr, bool useCondVar, const char* tag, bool logEx)
    {
        if (!m_Proceeder)
        {
            // assert(pr)
            m_Proceeder = pr;
            m_UseCondVar = useCondVar;
            m_Tag = (tag && *tag) ? tag : DefaultTag;
#ifdef USE_LOG_EX_
            m_LogEx = logEx;
#endif
            LOG_INFO("%s, %s, cond.var=%d, logex=%d", __FUNCTION__, Tag(), m_UseCondVar, m_LogEx);
        }
        else
        {
            LOG_ERROR("%s, %s, already initialized", __FUNCTION__, Tag());
        }
#pragma warning(default:4100)
    }

    void ProceedPump::EnableUseCondVar()
    { 
        m_UseCondVar = true;
        LOG_INFO("%s, %s", __FUNCTION__, Tag());
    }

    bool ProceedPump::CreatePump()
    {
        bool ret = false;

        if (m_Proceeder)
        {
            if (!m_Thread.joinable()) // no thread
            {
                InitFlags();
                try
                {
                    m_Thread = Thread([this] { DoJob(); });
                    ret = true;
                }
                catch (const std::exception& exc)
                {
                    LOG_ERROR("%s, %s, std::exeption: %s", __FUNCTION__, Tag(), exc.what());
                }
                catch (...)
                {
                    LOG_ERROR("%s, %s, exeption !!!!!!!!!!", __FUNCTION__, Tag());
                }
            }
            else
            {
                LOG_ERROR("%s, %s, already active", __FUNCTION__, Tag());
            }
        }
        else
        {
            LOG_ERROR("%s, %s, is not initialized", __FUNCTION__, Tag());
        }

        return ret;
    }

    void ProceedPump::ClosePump()
    {
        if (m_IsActive)
        {
            Stop(false);
            m_IsActive = false;
            Run();
        }
        if (m_Thread.joinable())
        {
            m_Thread.join();
            m_Thread = std::thread();
        }
    }

    void ProceedPump::InitFlags()
    {
        m_EndOfData = false;
        m_IsError = false;
        m_Proceeding = false;

        m_IsRunning = false;
        m_CanProceedR = true;
        m_CanProceedW = true;

        m_IsActive = true;
    }

    bool ProceedPump::IsRunning()
    {
        AutoLock al(m_Mutex);
        return m_IsRunning;
    }

    void ProceedPump::SetRunning(bool running)
    {
        AutoLock al(m_Mutex);
        m_IsRunning = running;
    }

    int ProceedPump::Proceed()
    {
        int res = IProceed::ProceedException;
        try
        {
            res = m_Proceeder->Proceed();
        }
        catch (const std::exception& exc)
        {
            LOG_ERROR("%s, %s, std::exeption: %s", __FUNCTION__, Tag(), exc.what());
        }
        catch (...)
        {
            LOG_ERROR("%s, %s, exeption !!!!!!!!!!", __FUNCTION__, Tag());
        }
        return ProcResult(res);
    }

    int ProceedPump::ProcResult(int res)
    {
        if (res != 0)
        {
            if (res > IProceed::EndOfDataMarker) // need to wait
            {
                ThreadTools::Sleep(res);
                if (m_UseCondVar)
                {
                    LOG_INFO("%s, %s, wait %d msec", __FUNCTION__, Tag(), res);
                }
                res = 0;
            }
            else if (res == IProceed::EndOfDataMarker) // EOF
            {
                m_EndOfData = true;
                SetRunning(false);
                LOG_INFO("%s, %s, end of data ::::::::::", __FUNCTION__, Tag());
            }
            else // res < 0, error
            {
                m_IsError = true;
                SetRunning(false);
                LOG_ERROR("%s, %s, error, retVal=%d", __FUNCTION__, Tag(), res);
            }
        }
        return res;
    }

    bool ProceedPump::CanProceed() const
    {
        return m_IsRunning && m_CanProceedR && m_CanProceedW;
    }

    bool ProceedPump::Proceeding() const
    {
        return m_Proceeding;
    }

    void ProceedPump::DoJob() // thread function
    {
        LOG_INFO("%s, %s, enter thread func {{", __FUNCTION__, Tag());
        
        auto canProceed = [this](){ return CanProceed(); };
        m_Proceeding = true;
        while (m_IsActive)
        {
            int res = PauseInterval;
            if (IsRunning())
            {
                res = Proceed();
            }

            m_Proceeding = false;
            UniqLock ulk(m_Mutex);
            m_CondVar.wait(ulk, canProceed);
            m_Proceeding = true;

        } // while (m_IsActive)

        // exit thread function
        LOG_INFO("%s, %s, exit thread func }}", __FUNCTION__, Tag());
    }

    void ProceedPump::Stop(bool skipPause)
    {
        if (!skipPause)
        {
            SetRunning(false);
        }
        while (m_Proceeding);
        {
            ThreadTools::Sleep(StopInterval);
        }
        m_CanProceedR = true;
        m_CanProceedW = true;

        m_EndOfData = false;
        m_IsError = false;
    }

#ifdef USE_LOG_EX_
    static const char* Lab_(bool r)
    {
        return r ? ">>" : "||";
    }
#endif

    bool ProceedPump::EnableRead(bool enable)
    {
        bool ret = false;
        AutoLock al(m_Mutex);
        if (m_CanProceedR != enable)
        {
            m_CanProceedR = enable;
#ifdef USE_LOG_EX_
            if (m_LogEx)
            {
                //LOG_TEMP("%s, %s, val=%d", __FUNCTION__, Name(), m_CanProceed1);
                LOG_TEMP_F("%s, r, %s", Tag(), Lab_(m_CanProceedR));
            }
#endif
            if (CanProceed())
            {
                ret = true;
            }
        }
        return ret;
    }
    
    bool ProceedPump::EnableWrite(bool enable)
    {
        bool ret = false;
        AutoLock al(m_Mutex);
        if (m_CanProceedW != enable)
        {
            m_CanProceedW = enable;
#ifdef USE_LOG_EX_
            if (m_LogEx)
            {
                //LOG_TEMP("%s, %s, val=%d", __FUNCTION__, Name(), m_CanProceed2);
                LOG_TEMP_F("%s, W, %s", Tag(), Lab_(m_CanProceedW));
            }
#endif
            if (CanProceed())
            {
                ret = true;
            }
        }
        return ret;
    }


    void ProceedPump::EnablePumpRead(bool enable)
    {
        if (EnableRead(enable))
        {
            m_CondVar.notify_one();
        }
    }

    void ProceedPump::EnablePumpWrite(bool enable)
    {
        if (EnableWrite(enable))
        {
            m_CondVar.notify_one();
        }
    }

    void ProceedPump::Run()
    {
        bool notify = false;
        if (!m_EndOfData && !m_IsError)
        {
            AutoLock al(m_Mutex);
            if (!m_IsRunning)
            {
                m_IsRunning = true;
                if (m_CanProceedR && m_CanProceedW)
                {
                    notify = true;
                }
            }
        }
        if (notify)
        {
            m_CondVar.notify_one();
        }
    }

    void ProceedPump::LogState()
    {
        bool rn = m_IsRunning, rd = m_CanProceedR, wr = m_CanProceedW;
        if (!(rn && rd && wr))
        {
            LOG_INFO("========== %s, %d/%d/%d", Tag(), rn, rd, wr);
        }
    }

    void ProceedPump::Pause()
    {
        SetRunning(false);
    }

    bool ProceedPump::PumpError() const
    {
        return (bool)m_IsError;
    }

    bool ProceedPump::EndOfData() const
    {
        return (bool)m_EndOfData;
    }

    bool ProceedPump::IsActive() const
    {
        return (bool)m_IsActive;
    }

    const char* ProceedPump::Tag() const
    {
        return m_Tag.c_str();
    }

} // namespace Ftool
