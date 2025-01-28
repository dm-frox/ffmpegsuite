#include "pch.h"

#include "RendCtrlAudio.h"

#include "Logger.h"
#include "RendCtrlVideo.h"

namespace Fapp
{
    const char RendCtrlAudio::ThreadTag[] = "a-rnd";
    const bool RendCtrlAudio::UseCondVar  = true;

    const int RendCtrlAudio::RendBuffDurMs          = 200;
    const int RendCtrlAudio::WaitVideoFrameInterval = 100;

    RendCtrlAudio::RendCtrlAudio()
        : RendCtrlBase(AVMEDIA_TYPE_AUDIO)
    {
        Base::InitPump(static_cast<Base::IProceed*>(this), UseCondVar, ThreadTag);
    }

    RendCtrlAudio::~RendCtrlAudio()
    {
        if (m_FrameSink)
        {
            m_FrameSink->Detach();
        }
    }

    void RendCtrlAudio::Log(const char* str, int r) const
    {
        LOG_INFO("%s, %s, val=%d", __FUNCTION__, str, r);
    }

    void RendCtrlAudio::AttachFrameHub(IFrameData* frameHub)
    {
        Base::AttachFrameHub(frameHub);
        LOG_INFO(__FUNCTION__);
    }

    void RendCtrlAudio::EnablePresentationTime(bool enable)
    {
        m_HoldPresentationTime = enable;
    }

    void RendCtrlAudio::NotifyFreeBuffer(bool free)
    {
        PumpSwitchW()->EnablePumpWrite(free);
    }

    bool RendCtrlAudio::SetFrameSink(IFrameSink* sink, int bytesPerSample, int devId)
    {
        bool ret = false;
        if (sink)
        {
            if (sink->SetFrameSrc(static_cast<Fwrap::IFrameSrcAudio*>(this)))
            {
                m_FrameSink = sink;
                m_BytesPerSample = bytesPerSample;
                m_DevId = devId;
                m_EndOfData = false;
                m_IsReady = false;
                m_FrameHub->SetReadFramePump(PumpSwitchR());

                LOG_INFO("%s, bps=%d, r.buff.dur=%d, devId=%d",
                    __FUNCTION__,
                    m_BytesPerSample, RendBuffDurMs, m_DevId);

                ret = true;
            }
            else
            {
                sink->Detach();
                LOG_ERROR("%s, SetFrameSrc failed", __FUNCTION__);
            }
        }
        else // null rendering
        {
            LOG_INFO("%s, null rendering", __FUNCTION__);
            m_EndOfData = false;
            ret = true;
        }
 
        return ret;
    }

    double RendCtrlAudio::Position() const
    {
        return m_FrameSink
            ? m_BasePosition + m_FrameSink->Position()
            : RendPos();
    }

    void RendCtrlAudio::SetVolume(double vol)
    {
        if (m_FrameSink)
        {
            m_FrameSink->SetVolume(vol);
        }
    }

    bool RendCtrlAudio::FirstVideoFrameWritten()
    {
        return m_RendCtrlVideo ? m_RendCtrlVideo->FrameWritten() : true;
    }

    int RendCtrlAudio::Proceed()
    {
        if (!FirstVideoFrameWritten())
        {
            return WaitVideoFrameInterval;
        }
        return !m_EndOfData 
            ? WriteFrame() 
            : SweepOutBuffers();
    }

    void RendCtrlAudio::CloseIfFrameParamsChanged(int chann, int sampRate)
    {
        if (chann != m_FrameSink->Chann() || sampRate != m_FrameSink->SampleRate())
        {
            m_FrameSink->Close();
            LOG_INFO("%s, new params: chann=%d, sampRate=%d", __FUNCTION__, chann, sampRate);
            m_IsReady = false;
        }
    }

    void RendCtrlAudio::Open(int chann, int sampRate, int frameSize)
    {
        if (m_FrameSink->Open(chann, sampRate, m_BytesPerSample, RendBuffDurMs, m_DevId))
        {
            LOG_INFO("%s, chann=%d, sampRate=%d, bps=%d, r.buff.size=%d, fr.size=%d",
                __FUNCTION__,
                m_FrameSink->Chann(),
                m_FrameSink->SampleRate(),
                m_FrameSink->BytesPerSample(),
                m_FrameSink->RendBufferSize(),
                frameSize);
            m_IsReady = true;
        }
    }

    int RendCtrlAudio::WriteFrame()
    {
        int ret = 0;

        int frameSize = 0, chann = 0, sampRate = 0;
        double pos = 0.0;
        if (const void* buff = m_FrameHub->PeekAudioFrame(frameSize, chann, sampRate, pos)) // frame
        {
            if (m_FrameSink)
            {
                if (m_IsReady)
                {
                    CloseIfFrameParamsChanged(chann, sampRate);
                }
                
                if (!m_IsReady)
                {
                    Open(chann, sampRate, frameSize);
                }

                if (m_IsReady)
                {
                    bool done;
                    if (m_FrameSink->WriteFrame(buff, frameSize, done))
                    {
                        if (done)
                        {
                            m_FrameHub->DropFrame();
                        }
                    }
                    else
                    {
                        LOG_ERROR("%s, rend error", __FUNCTION__);
                        ret = -1;
                    }
                }
                else
                {
                    LOG_ERROR("%s, failed to open renderer", __FUNCTION__);
                    ret = -1;
                }
            }
            else // null rendering
            {
                int bytesPerSec = chann * sampRate * m_BytesPerSample;
                int frameDurMs = frameSize * 1000 / bytesPerSec;
                if (m_HoldPresentationTime && bytesPerSec > 0)
                {
                    ret = frameDurMs;
                }
                IncRendPos(frameSize / (double)bytesPerSec);
                m_FrameHub->DropFrame();
            }
        }
        else // no frame
        {
            if (frameSize == Fcore::IFrameData::EndOfAudioMarker())
            {
                LOG_INFO("%s, no frames, eof", __FUNCTION__);
                m_EndOfData = true;
                ret = SweepOutBuffers();
                if (ret != EndOfDataMarker)
                {
                    PumpSwitchR()->EnablePumpRead(true);
                }
            }
        }

        return ret;
    }

    int RendCtrlAudio::SweepOutBuffers()
    {
        int ret = 0;

        if (m_FrameSink)
        {
            bool done;
            if (m_FrameSink->SweepOutBuffers(done))
            {
                if (done)
                {
                    if (m_RendCtrlVideo && m_RendCtrlVideo->SyncByAudioEnabled())
                    {
                        m_FrameSink->EnableAudioWatch();
                    }
                    ret = EndOfDataMarker;
                    LOG_INFO("%s, end of rendering", __FUNCTION__);
                }
            }
            else
            {
                LOG_ERROR("%s, rend error", __FUNCTION__);
                ret = -1;
            }
        }
        else
        {
            ret = EndOfDataMarker;
            LOG_INFO("%s, end of rendering (null)", __FUNCTION__);
        }

        return ret;
    }

    void RendCtrlAudio::Run()
    {
        LOG_INFO(__FUNCTION__);
        m_Pump.Run();
        if (m_FrameSink)
        {
            m_FrameSink->Run();
        }
    }

    void RendCtrlAudio::Pause()
    {
        LOG_INFO(__FUNCTION__);
        m_Pump.Pause();
        if (m_FrameSink)
        {
            m_FrameSink->Pause();
        }
    }

    void RendCtrlAudio::Stop(bool skipPause)
    {
        Stop(skipPause, 0.0);
    }

    void RendCtrlAudio::Stop(bool skipPause, double pos)
    {
        LOG_INFO("%s, pos=%5.3lf", __FUNCTION__, pos);
        m_Pump.Stop(skipPause);
        m_EndOfData = false;
        if (m_FrameSink)
        {
            m_FrameSink->Stop();
            m_BasePosition = pos;
        }
        SetRendPos(0.0);
    }

} // namespace Fapp


