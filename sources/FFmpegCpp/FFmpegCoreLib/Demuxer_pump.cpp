#include "pch.h"

#include "Demuxer.h"

#include "Logger.h"

namespace Fcore
{
    void Demuxer::InitPump(bool useCondVar, const char* tag, bool logEx)
    {
        m_Pump.Init(static_cast<IProceed*>(this), useCondVar, tag, logEx);
    }

    bool Demuxer::Proceeding() const
    {
        bool ret = m_Pump.Proceeding();
        for (int i = 0, n = m_Streams.Count(); i < n && !ret; ++i)
        {
            const StreamReader& strm = m_Streams[i];
            if (strm.IsActivated())
            {
                if (strm.Proceeding())
                {
                    ret = true;
                }
            }
        }
        return ret;
    }

    bool Demuxer::CreatePump()
    {
        bool ret = false;
        if (m_Pump.CreatePump())
        {
            ret = m_Streams.CreatePump();
        }

        if (!ret)
        {
            LOG_ERROR("%s, failed create pump", __FUNCTION__);
        }
        return ret;
    }

    void Demuxer::ClosePump()
    {
        m_Streams.ClosePump();
        m_Pump.ClosePump();
    }

    bool Demuxer::PumpError() const
    {
        return m_Streams.PumpError() || m_Pump.PumpError();
    }

    void Demuxer::Run()
    {
        m_Pump.Run();
        m_Streams.Run();
    }

    void Demuxer::Pause()
    {
        m_Streams.Pause();
        m_Pump.Pause();
    }

    void Demuxer::Stop(bool skipPause)
    {
         m_Streams.Stop(skipPause);
         m_Pump.Stop(skipPause);
    }

    void Demuxer::PushTerminator()
    {
        for (int i = 0, n = m_Streams.Count(); i < n; ++i)
        {
            StreamReader& strm = m_Streams[i];
            if (strm.IsActivated())
            {
                strm.PushPacketToQueue(nullptr);
            }
        }
    }

    bool Demuxer::PacketQueuesNotFull()
    {
        std::lock_guard<std::mutex> mg(m_Mutex);

        bool notFull = true;
        for (int i = 0, n = m_Streams.Count(); i < n && notFull; ++i)
        {
            const StreamReader& strm = m_Streams[i];
            if (strm.IsActivated() && strm.PacketQueueIsFull())
            {
                notFull = false;
            }
        }
        return notFull;
    }

// IPumpSwitchW impl

    bool Demuxer::TryEnableProceedingWrite() const
    {
        bool needWrite = false;
        for (int i = 0, n = m_Streams.Count(); i < n; ++i)
        {
            const StreamReader& strm = m_Streams[i];
            if (strm.IsActivated())
            {
                int len = strm.PacketQueueDataLenghtNoSync();
                if (len >= m_PacketQueueOffThreshold)
                {
                    return false;
                }
                if (!needWrite && (len <= m_PacketQueueOnThreshold))
                {
                    needWrite = true;
                }
            }
        }
        return needWrite;
    }

    void Demuxer::EnablePumpWrite(bool enable)
    {
        IPumpSwitchW* p = m_Pump.PumpSwitchW();
        if (enable)
        {
            if (TryEnableProceedingWrite())
            {
                p->EnablePumpWrite(true);
            }
        }
        else
        {
            p->EnablePumpWrite(false);
        }
    }

} // namespace Fcore


