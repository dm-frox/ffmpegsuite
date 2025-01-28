#include "pch.h"

#include "MediaPlayerStem.h"

#include "include/FrameSinkAudioIface.h"
#include "include/MediaSourceInfoIface.h"

#include "Misc.h"
#include "Logger.h"


namespace Fapp
{

    MediaPlayerStem::MediaPlayerStem(bool useFilterGraphAlways, bool convFrameSequentially)
        : m_FrameSource(useFilterGraphAlways, convFrameSequentially, true)
    {}

    MediaPlayerStem::~MediaPlayerStem()
    {
        ClosePump();
    }

    const char* MediaPlayerStem::PixelFormatStr() const
    {
        return Fcore::PixFmtToStr(m_FrameSource.PixelFormat());
    }

    const char* MediaPlayerStem::SampleFormatStr() const
    {
        return Fcore::SampFmtToStr(m_FrameSource.SampleFormat());
    }

    bool MediaPlayerStem::Build(const char* url, const char* fmt, const char* fmtOps,
        bool useVideo, const char* decPrmsVideo, bool useAudio, const char* decPrmsAudio)
    {
        bool ret = false;
        if (m_State == State::OutOfSource)
        {
            int rr = m_FrameSource.Build(url, fmt, fmtOps,
                useVideo, decPrmsVideo, useAudio, decPrmsAudio);
            if (rr >= 0)
            {
                ret = true;
                if (m_FrameSource.HasAudio())
                {
                    m_FrameSource.SetSeekPumpAudio();
                }
                if (!m_FrameSource.HasMediaSource())
                {
                    LOG_WARNING("%s, no required streams", __FUNCTION__);
                }
            }
            else
            {
                LOG_ERROR(__FUNCTION__);
            }
        }
        else
        {
            LOG_ERROR("%s, bad state=%d", __FUNCTION__, m_State);
        }
        return ret;
    }

    bool MediaPlayerStem::SetupRendering(
        Fwrap::IFrameSinkVideo* rendVideo, bool holdPresentationTime, bool synkByAudio, bool swapFields, 
        Fwrap::IFrameSinkAudio* rendAudio, int audioDevId)
    {
        bool vid = true, aud = true;
        if (HasVideo())
        {
            m_RendCtrlVideo.AttachFrameHub(m_FrameSource.FrameDataVideo(), m_FrameSource.FrameConvVideo());
            if (m_RendCtrlVideo.SetFrameSink(rendVideo, m_FrameSource.PixelFormat()))
            {
                m_RendCtrlVideo.SetSyncMode(
                    holdPresentationTime,
                    (HasAudio() && synkByAudio) ? &m_RendCtrlAudio : nullptr);
                if (swapFields)
                {
                    m_RendCtrlVideo.SetSwapFields();
                }
            }
            else
            {
                vid = false;
            }
        }
        if (HasAudio())
        {
            m_RendCtrlAudio.AttachFrameHub(m_FrameSource.FrameDataAudio());
            m_RendCtrlAudio.EnablePresentationTime(holdPresentationTime);
            int bps = Fcore::BytesPerSample(m_FrameSource.SampleFormat());
            if (m_RendCtrlAudio.SetFrameSink(rendAudio, bps, audioDevId))
            {
                if (HasVideo())
                {
                    m_RendCtrlAudio.SetRendCtrlVideo(&m_RendCtrlVideo);
                }
            }
            else
            {
                aud = false;
            }
        }
        bool ret = false;
        if (vid && aud)
        {
            if (CreatePump())
            {
                LOG_INFO("%s, holdPresentationTime=%d, syncVideoByAudio=%d, swapFields=%d, audioDevId=%d", __FUNCTION__,
                    holdPresentationTime, m_RendCtrlVideo.SyncByAudioEnabled(), swapFields, audioDevId);
                ret = true;
            }
        }
        return ret;
    }

    bool MediaPlayerStem::CreatePump()
    {
        bool ret = false;
        if (m_FrameSource.HasMediaSource())
        {
            if (m_State == State::OutOfSource)
            {
                if (m_FrameSource.CreatePump())
                {
                    bool vid = m_RendCtrlVideo.HasFrameHub() ? m_RendCtrlVideo.CreatePump() : true;
                    bool aud = m_RendCtrlAudio.HasFrameHub() ? m_RendCtrlAudio.CreatePump() : true;

                    if (vid && aud)
                    {
                        m_State = State::Stopped;
                        ret = true;
                    }
                    else
                    {
                        m_FrameSource.ClosePump();
                    }
                }
            }
        }
        else
        {
            ret = true;
        }
        return ret;
    }

    void MediaPlayerStem::ClosePump()
    {
        if (m_State != State::OutOfSource)
        {
            Stop();
            LOG_INFO(__FUNCTION__);
            m_RendCtrlVideo.ClosePump();
            m_RendCtrlAudio.ClosePump();
            m_FrameSource.ClosePump();
            m_State = State::OutOfSource;
        }
    }

    void MediaPlayerStem::Run()
    {
        if ((m_State == State::Stopped || m_State == State::Paused))
        {
            LOG_INFO(__FUNCTION__);
            m_RendCtrlVideo.Run();
            m_RendCtrlAudio.Run();
            m_FrameSource.Run();
            m_State = State::Running;
        }
    }

    void MediaPlayerStem::Pause()
    {
        if (m_State == State::Running)
        {
            LOG_INFO(__FUNCTION__);
            m_RendCtrlVideo.Pause();
            m_RendCtrlAudio.Pause();
            m_FrameSource.Pause();
            m_State = State::Paused;
        }
    }

    void MediaPlayerStem::Stop()
    {
        if (m_State > State::Stopped)
        {
            Pause();
            LOG_INFO(__FUNCTION__);
            m_RendCtrlVideo.Stop(true);
            m_RendCtrlAudio.Stop(true);
            m_FrameSource.Stop(true);
            m_State = State::Stopped;
        }
    }

    bool MediaPlayerStem::SeekingInProgress() const
    {
        bool v = HasVideo() ? m_RendCtrlVideo.SeekingInProgress() : false;
        bool a = HasAudio() ? m_FrameSource.SeekingInProgressAudio() : false;
        return v || a;
    }

    int MediaPlayerStem::SetPosition(double pos, bool paused)
    {
        int ret = -1;
        if (m_State > State::Stopped)
        {
            Pause();
            LOG_INFO("%s, pos=%5.3lf", __FUNCTION__, pos);
            m_RendCtrlVideo.Stop(true, pos);
            m_RendCtrlAudio.Stop(true, pos);
            ret = m_FrameSource.SetPosition(true, pos);
            m_RendCtrlVideo.SetPosition(pos, paused);
            m_FrameSource.SetPositionAudio(pos, paused);
        }
        return ret;
    }

    bool MediaPlayerStem::PumpError() const
    {
        bool ret = false;
        if (m_State > State::Stopped)
        {
            ret = m_RendCtrlVideo.PumpError() ||
                  m_RendCtrlAudio.PumpError() ||
                  m_FrameSource.PumpError();
        }
        return ret;
    }

    bool MediaPlayerStem::EndOfData() const
    {
        bool ret = false;
        if (m_State > State::Stopped)
        {
            ret = (HasVideo() ? m_RendCtrlVideo.EndOfData() : true) &&
                  (HasAudio() ? m_RendCtrlAudio.EndOfData() : true);
        }
        return ret;
    }

    void MediaPlayerStem::StartRewinder(int fps) 
    { 
        m_RendCtrlVideo.StartRewinder(fps);
        m_FrameSource.SetPositionAudio(m_FrameSource.Duration() * 2.0, false);
    }

    void MediaPlayerStem::StopRewinder() 
    { 
        m_RendCtrlVideo.StopRewinder(); 
    }

    double MediaPlayerStem::VideoPosition() const
    {
        return ((m_State == State::Running) || (m_State == State::Paused)) ? m_RendCtrlVideo.RendPosition() : 0.0;
    }

    double MediaPlayerStem::Position() const
    {
        return m_RendCtrlVideo.HasFrameHub()
            ? VideoPosition()
            : (m_RendCtrlAudio.HasFrameHub() ? m_RendCtrlAudio.Position() : 0.0);
    }

    int MediaPlayerStem::SendFilterCommandVideo(const char* tar, const char* cmd, const char* arg)
    {
        int ret = m_FrameSource.SendFilterCommandVideo(tar, cmd, arg);
        if (ret >= 0)
        {
            if (m_State == State::Paused)
            {
                m_RendCtrlVideo.UpdatePictupeFromFrameBackup();
            }
        }
        return ret;
    }

    double MediaPlayerStem::GetShiftedVideoPosition(int frameCount) const
    {
        double newPos = -1.0;
        const Fwrap::IMediaSourceInfo* info = m_FrameSource.MediaSourceInfo();
        if (info->Fps_N() > 0)
        {
            double shift = (static_cast<double>(frameCount) * info->Fps_D()) / info->Fps_N();
            double curPos = m_RendCtrlVideo.RendPos();
            newPos = curPos + shift;
            if (newPos < 0.0)
            {
                newPos = 0.0;
            }
            LOG_INFO("%s, pos(cur/new)=%5.3lf/%5.3lf, shift=%5.3lf", __FUNCTION__, curPos, newPos, shift);
        }
        else
        {
            LOG_ERROR("%s, bad fps=%d/%d", __FUNCTION__, info->Fps_N(), info->Fps_D());
        }
        return newPos;
    }

    bool MediaPlayerStem::ShiftVideoPositionFwd(int frameCount)
    {
        bool ret = false;
        if (HasVideo() && m_State == State::Paused && frameCount > 0)
        {
            double newPos = GetShiftedVideoPosition(frameCount);
            if (newPos >= 0.0)
            {
                if (!m_RendCtrlVideo.EndOfVideoByQueue())
                {
                    m_RendCtrlVideo.SetPosition(newPos, true);
                    ret = true;
                }
                else
                {
                    LOG_INFO("%s, end of data, pos=%5.3lf", __FUNCTION__, newPos);
                }
            }
        }
        return ret;
    }

    bool MediaPlayerStem::EndOfVideo() const
    {
        return HasVideo() ? m_RendCtrlVideo.EndOfVideoByQueue() : true;
    }

    bool MediaPlayerStem::Proceeding() const
    {
        return m_FrameSource.Proceeding() || m_RendCtrlVideo.Proceeding() || m_RendCtrlAudio.Proceeding();
    }

} // namespace Fapp
