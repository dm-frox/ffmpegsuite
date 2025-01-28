#include "pch.h"

#include "RendCtrlVideo.h"

#include "include/FrameSinkVideoIface.h"

#include "Logger.h"
#include "CommonConsts.h"
#include "FrameTools.h"
#include "ThreadTools.h"
#include "Misc.h"
#include "FrameDataIface.h"

#include "VideoWatch.h"
#include "RendCtrlAudio.h"


namespace Fapp
{
    const char RendCtrlVideo::ThreadTag[] = "v-rnd";
    const bool RendCtrlVideo::UseCondVar  = false;

    const int RendCtrlVideo::FrameDelayThresholdMs      = 4;
    const int RendCtrlVideo::MaxFrameDelayMs            = 10 * 1000;
    const int RendCtrlVideo::NoFrameWaitIntervalMs      = 20;
    const int RendCtrlVideo::WaitFrameWrittenIntervalMs = 20;
    const int RendCtrlVideo::WaitFrameWrittenCount      = 15;


    RendCtrlVideo::RendCtrlVideo()
        : RendCtrlBase(AVMEDIA_TYPE_VIDEO)
        , m_SeekCtrl(AVMEDIA_TYPE_VIDEO, m_Pump)
        , m_Rewinder(FrameDelayThresholdMs)
    {
        Base::InitPump(static_cast<Base::IProceed*>(this), UseCondVar, ThreadTag);
    }

    RendCtrlVideo::~RendCtrlVideo()
    {
        delete m_VideoWatch;
        FrameTools::FreeFrame(m_Frame);
        if (m_FrameSink)
        {
            m_FrameSink->Detach();
        }
    }

    //static 
    bool RendCtrlVideo::AssertFrame(const AVFrame* frame)
    {
        return frame && frame->data[0] && frame->width > 0 && frame->height > 0;
    }

    // static
    int RendCtrlVideo::ToMilliseconds(double tsec)
    {
        return (tsec >= 0.0) ? static_cast<int>(tsec * 1000.0) : -1;
    }

    bool RendCtrlVideo::FrameFilter() const 
    { 
        return m_FrameConv ? m_FrameConv->FilterIsUsed() : false; 
    }

    int RendCtrlVideo::Width() const
    { 
        return m_FrameSink ? m_FrameSink->FrameWidth() : 0; 
    }

    int RendCtrlVideo::Height() const
    { 
        return m_FrameSink ? m_FrameSink->FrameHeight() : 0; 
    }

    void RendCtrlVideo::AttachFrameHub(IFrameData* frameHub, IFrameConv* frameConv)
    {
        Base::AttachFrameHub(frameHub);
        m_FrameConv = frameConv;
        LOG_INFO("%s, frame conv=%d, filter=%d", __FUNCTION__, m_FrameConv ? 1 : 0, FrameFilter());
    }

    void RendCtrlVideo::SetSyncMode(bool holdPresentationTime, RendCtrlAudio* rendererAudio)
    {
        if (holdPresentationTime)
        {
            m_HoldPresentationTime = true;
            if (rendererAudio) // by audio rend clock
            {
                m_RendAudio = rendererAudio;
            }
            else // by system clock
            {
                m_VideoWatch = new VideoWatch();
            }
        }
        else
        {
            m_HoldPresentationTime = false;
            m_Pump.EnableUseCondVar();
            m_FrameHub->SetReadFramePump(PumpSwitchR());
        }
    }

    bool RendCtrlVideo::SetFrameSink(IFrameSink* sink, AVPixelFormat pixFmt)
    { 
        bool ret = false;

        if (sink)
        {
            const char* pfs = av_get_pix_fmt_name(pixFmt);
            int planeCount = av_pix_fmt_count_planes(pixFmt);
            if (pfs && sink->CheckPixelFormat(pfs) && planeCount >= 1)
            {
                m_FrameSink = sink;
                m_RendPlanes = planeCount;
                ret = true;
                LOG_INFO("%s, pix.fmt=%s, planes=%d", __FUNCTION__, pfs, m_RendPlanes);
            } 
            else
            {
                sink->Detach();
                LOG_ERROR("%s, pix.fmt=%s, planes=%d", __FUNCTION__, Fcore::StrToLog(pfs), planeCount);
            }
        }
        else
        {
            LOG_INFO("%s, null rendering", __FUNCTION__);
            ret = true;
        }

        return ret;
    }

    double RendCtrlVideo::Position() const
    {
        return m_RendAudio
            ? m_RendAudio->Position()
            : (m_VideoWatch ? m_VideoWatch->Position() : Base::RendPos());
    }

    double RendCtrlVideo::CurrentTime()
    {
        return m_RendAudio
            ? m_RendAudio->Position()
            : (m_VideoWatch ? m_VideoWatch->CurrentTime() : Base::RendPos());
    }

    bool RendCtrlVideo::EndOfVideoByQueue() const
    {
        return m_FrameHub->VideoFramePos() == IFrameData::EndOfVideoMarker();
    }

    // returns 0 if the frame needs to be rendered immediately and the timeout in milliseconds, otherwise 
    int RendCtrlVideo::CheckVideoFrame(bool& drop)
    {
        if (m_Rewinder.IsRunning())
        {
            return m_Rewinder.CheckTime();
        }

        int ret = 0;
        double framePos = m_FrameHub->VideoFramePos();
        if (framePos >= 0.0) // frame
        {
            if (m_SeekCtrl.InProgress())
            {
                if (!m_SeekCtrl.CheckSeekingEnd(framePos))
                {
                    drop = true;
                }
                return 0;
            }
            if (m_HoldPresentationTime && m_FrameWritten)
            {
                double currTime = CurrentTime();
                int deltMs = ToMilliseconds(framePos - currTime);
                if (deltMs > FrameDelayThresholdMs)
                {
                    if (deltMs < MaxFrameDelayMs)
                    {
                        ret = (deltMs < MaxSleepInterval) ? deltMs : MaxSleepInterval;
                    }
                    else
                    {
                        LOG_WARNING("%s, deltMs=%d", __FUNCTION__, deltMs);
                    }
                }
                //LOG_TEMP("++++++++, pos=%5.3lf, cur=%5.3lf, deltMs=%d, ret=%d", framePos, currTime, deltMs, ret);
            }
        }
        else // no frame
        {
            if (framePos != IFrameData::EndOfVideoMarker()) // it's not end, we have to wait 
            {
                ret = NoFrameWaitIntervalMs;
            }
            else // end
            {
                m_SeekCtrl.Cancel();
                ret = EndOfDataMarker;
            }
        }
        return ret;
    }

    int RendCtrlVideo::Proceed()
    {
        int ret = -1;
        if (m_FrameHub)
        {
            bool drop = false;
            ret = CheckVideoFrame(drop);
            if (ret == 0)
            {
                if (!drop)
                {
                    int rr = RenderFrame();
                    if (rr < 0)
                    {
                        ret = rr;
                    }
                }
            }
            else if (ret == EndOfDataMarker)
            {
                drop = true;
            }
            if (drop)
            {
                m_FrameHub->DropFrame();
            }
        }
        else
        {
            LOG_ERROR("%s, no frame hub", __FUNCTION__);
        }
        return ret;
    }

    // static
    bool RendCtrlVideo::WriteFrame(IFrameSink* sink, const AVFrame* frame, int rendPlanes)
    {
        bool ret = false;
        if (sink && frame)
        {
            int w = frame->width, 
                h = frame->height;
            switch (rendPlanes)
            {
            case 1:
                ret = sink->WriteFrame(w, h,
                    frame->data[0], frame->linesize[0]);
                break;
            case 2:
                ret = sink->WriteFrameNV(w, h,
                    frame->data[0], frame->linesize[0],
                    frame->data[1], frame->linesize[1]);
                break;
            case 3:
                ret = sink->WriteFrameYUV(w, h,
                    frame->data[0], frame->linesize[0],
                    frame->data[1], frame->linesize[1],
                    frame->data[2], frame->linesize[2]);
                break;
            }
        }
        return ret;
    }

    int RendCtrlVideo::WriteFrame(const AVFrame* frame)
    {
        bool res = false;
        if (AssertFrame(frame))
        {
            if (m_SwapFields)
            {
                m_LineSwap.SwapEvenOddLines(const_cast<AVFrame*>(frame), m_RendPlanes);
            }
            res = WriteFrame(m_FrameSink, frame, m_RendPlanes);
        }
        else
        {
            LOG_ERROR("%s, bad frame", __FUNCTION__);
            FrameTools::LogVideoFrame3(frame, __FUNCTION__);
        }
        if (res)
        {
            return 0;
        }
        else
        {
            LOG_ERROR(__FUNCTION__);
            return Fcore::ErrorRendFrameSink;
        }
    }

    int RendCtrlVideo::RenderFrame(const AVFrame* frame)
    {
        int ret = 0;
        if (m_FrameSink)
        {
            ret = WriteFrame(frame);
        }
        return ret;    
    }

    int RendCtrlVideo::ConvertAndRenderFrame(AVFrame* srcFrame)
    {
        if (!m_Frame)
        {
            m_Frame = FrameTools::AllocFrame();
        }
        if (!m_Frame)
        {
            return AVERROR(ENOMEM);
        }

        int ret = 0;
        if (m_FrameSink)
        {
            if (AssertFrame(srcFrame))
            {
                ret = m_FrameConv->Convert(srcFrame, m_Frame);
                if (ret >= 0)
                {
                    ret = WriteFrame(m_Frame);
                }
            }
            else
            {
                LOG_ERROR("%s, bad source frame", __FUNCTION__);
                FrameTools::LogVideoFrame3(srcFrame, __FUNCTION__);
                ret = Fcore::ErrorRendFrameSink;
            }
        }
        FrameTools::FreeFrameBuffer(m_Frame);
        return ret;
    }

    int RendCtrlVideo::RenderFrame()
    {
        int ret = 0;
        double pos = 0.0;
        if (AVFrame* srcFrame = m_FrameHub->PeekVideoFrame(pos))
        {
            if (FrameFilter() && m_Paused)
            {
                m_FrameBackup.Set(srcFrame);
            }

            Base::SetRendPos(pos);

            ret = m_FrameConv
                ? ConvertAndRenderFrame(srcFrame) 
                : RenderFrame(srcFrame);

            if (ret >= 0)
            {
                if (!m_FrameWritten)
                {
                    m_FrameWritten = true;
                    LOG_INFO("%s, first frame, pos=%5.3lf", __FUNCTION__, pos);
                }
                if (m_VideoWatch)
                {
                    m_VideoWatch->SetRendPos(pos);
                }
            }
            else 
            {
                Base::SetRendPos(0.0);
                if (FrameFilter())
                {
                    m_FrameBackup.Clear();
                }
            }

            m_FrameHub->DropFrame();
        }
        else // can't be so
        {
            LOG_WARNING("%s, no frame to render", __FUNCTION__);
        }
        return ret;
    }

    int RendCtrlVideo::UpdatePictupeFromFrameBackup()
    {
        int ret = -1;
        if (FrameFilter())
        {
            if (AVFrame* srcFrame = m_FrameBackup.Get())
            {
                ret = ConvertAndRenderFrame(srcFrame);
            }
            else
            {
                ret = RenderFrame();
            }
        }
        return ret;
    }

    void RendCtrlVideo::Run()
    {

        if (FrameFilter())
        {
            m_Paused = false;
            m_FrameBackup.Clear();
        }
        if (m_VideoWatch)
        {
            m_VideoWatch->Run();
        }
        LOG_INFO(__FUNCTION__);
        m_Pump.Run();
        m_Pump.LogState();
    }

    void RendCtrlVideo::Pause()
    {
        if (FrameFilter())
        {
            m_Paused = true;
        }
        if (m_VideoWatch)
        {
            m_VideoWatch->Pause();
        }
        LOG_INFO(__FUNCTION__);
        m_Pump.Pause();
        m_FrameWritten = false;
    }

    void RendCtrlVideo::Stop(bool skipPause)
    {
        Stop(skipPause, 0.0);
    }

    void RendCtrlVideo::Stop(bool skipPause, double pos)
    {
        LOG_INFO("%s, pos=%5.3lf", __FUNCTION__, pos);
        if (FrameFilter())
        {
            m_FrameBackup.Clear();
        }
        if (m_VideoWatch)
        {
            m_VideoWatch->Stop(pos);
        }
        m_Pump.Stop(skipPause);
        m_FrameWritten = false;
        Base::SetRendPos(0.0);
    }

} // namespace Fapp

