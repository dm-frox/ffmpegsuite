#include "pch.h"

#include "MediaPlayerStemImpl.h"

#include "SLogger.h"


namespace Fwrap
{
    class IFrameSinkVideo;
    class IFrameSinkAudio;

    MediaPlayerStemImpl::MediaPlayerStemImpl(bool useFilterGraphAlways, bool convFrameSequentially)
        : m_MediaPlayerStem(useFilterGraphAlways, convFrameSequentially)
    {
        LOG_INFO("%s, flags=%d/%d", __FUNCTION__, useFilterGraphAlways, convFrameSequentially);
    }

    MediaPlayerStemImpl::~MediaPlayerStemImpl() = default;

    void MediaPlayerStemImpl::Delete()
    {
        IBase::Delete();
        LOG_INFO(__FUNCTION__);
        LOG_FLUSH();
    }

    int MediaPlayerStemImpl::StreamCount() const
    {
        return m_MediaPlayerStem.Demuxer().StreamCount();
    }

    CStrEx MediaPlayerStemImpl::GetInfo() const
    {
        int len = 0;
        const char* str = m_MediaPlayerStem.Demuxer().GetInfo(len);
        return { str, len };
    }

    CStrEx MediaPlayerStemImpl::GetMetadata() const
    {
        int len = 0;
        const char* str = m_MediaPlayerStem.Demuxer().GetMetadata(len);
        return { str, len };
    }

    CStrEx MediaPlayerStemImpl::GetChapters() const
    {
        int len = 0;
        const char* str = m_MediaPlayerStem.Demuxer().GetChapters(len);
        return { str, len };
    }

    CStrEx MediaPlayerStemImpl::GetStreamInfo(int strmIdx) const
    {
        int len = 0;
        const char* str = m_MediaPlayerStem.Demuxer().GetStreamInfo(strmIdx, len);
        return { str, len };
    }

    CStrEx MediaPlayerStemImpl::GetStreamMetadata(int strmIdx) const
    {
        int len = 0;
        const char* str = m_MediaPlayerStem.Demuxer().GetStreamMetadata(strmIdx, len);
        return { str, len };
    }

    void MediaPlayerStemImpl::SetFrameParamsVideo(int width, int height, const char* pixFmt, const char* vidFilter, int fpsFactor)
    {
        m_MediaPlayerStem.SetFrameParamsVideo(width, height, pixFmt, vidFilter, fpsFactor);
    }

    void MediaPlayerStemImpl::SetRendModeAudio(int maxChann, int reduceSampleFormat)
    {
        m_MediaPlayerStem.SetRendModeAudio(maxChann, reduceSampleFormat);
    }

    void MediaPlayerStemImpl::SetFrameParamsAudio(int chann, int sampleRate, const char* sampFmt, const char* channLayout, const char* audFilter)
    {
        m_MediaPlayerStem.SetFrameParamsAudio(chann, sampleRate, sampFmt, channLayout, audFilter);
    }

    bool MediaPlayerStemImpl::Build(const char* url, const char* fmt, const char* fmtOps,
        bool useVideo, const char* decPrmsVideo, bool useAudio, const char* decPrmsAudio)
    {
        return m_MediaPlayerStem.Build(url, fmt, fmtOps,
            useVideo, decPrmsVideo, useAudio, decPrmsAudio);
    }

    bool MediaPlayerStemImpl::SetupRendering(
        void* rendVideo, bool holdPresentationTime, bool synkByAudio, bool swapFields,
        void* rendAudio, int audioDevId)
    {
        return m_MediaPlayerStem.SetupRendering(
            static_cast<IFrameSinkVideo*>(rendVideo), holdPresentationTime, synkByAudio, swapFields, 
            static_cast<IFrameSinkAudio*>(rendAudio), audioDevId);
    }

    const IMediaSourceInfo* MediaPlayerStemImpl::MediaSourceInfo() const
    {
        return m_MediaPlayerStem.MediaSourceInfo();
    }

    bool MediaPlayerStemImpl::HasVideo() const
    {
        return m_MediaPlayerStem.HasVideo();
    }

    int MediaPlayerStemImpl::Width() const
    {
        return m_MediaPlayerStem.Width();
    }

    int MediaPlayerStemImpl::Height() const
    {
        return m_MediaPlayerStem.Height();
    }

    const char* MediaPlayerStemImpl::PixelFormatStr() const
    {
        return m_MediaPlayerStem.PixelFormatStr();
    }

    bool MediaPlayerStemImpl::HasAudio() const
    {
        return m_MediaPlayerStem.HasAudio();
    }

    int MediaPlayerStemImpl::Chann() const
    {
        return m_MediaPlayerStem.Chann();
    }

    int MediaPlayerStemImpl::SampleRate() const
    {
        return m_MediaPlayerStem.SampleRate();
    }

    const char* MediaPlayerStemImpl::SampleFormatStr() const
    {
        return m_MediaPlayerStem.SampleFormatStr();
    }

    int MediaPlayerStemImpl::RendWidth() const
    {
        return m_MediaPlayerStem.RendWidth();
    }

    int MediaPlayerStemImpl::RendHeight() const
    {
        return m_MediaPlayerStem.RendHeight();
    }

    int MediaPlayerStemImpl::RendChann() const
    {
        return m_MediaPlayerStem.RendChann();
    }

    int MediaPlayerStemImpl::RendSampleRate() const
    {
        return m_MediaPlayerStem.RendSampleRate();
    }

    bool MediaPlayerStemImpl::EndOfData() const
    {
        return m_MediaPlayerStem.EndOfData();
    }

    bool MediaPlayerStemImpl::EndOfVideo() const
    {
        return m_MediaPlayerStem.EndOfVideo();
    }

    double MediaPlayerStemImpl::Position() const
    {
        return m_MediaPlayerStem.Position();
    }

    double MediaPlayerStemImpl::VideoRendPosition() const
    {
        return m_MediaPlayerStem.VideoRendPosition();
    }

    bool MediaPlayerStemImpl::Proceeding() const
    {
        return m_MediaPlayerStem.Proceeding();
    }

    int MediaPlayerStemImpl::SetPosition(double pos, bool onPause)
    {
        return m_MediaPlayerStem.SetPosition(pos, onPause);
    }

    bool MediaPlayerStemImpl::SeekingInProgress() const
    {
        return m_MediaPlayerStem.SeekingInProgress();
    }

    double MediaPlayerStemImpl::GetShiftedVideoPosition(int frameCount) const
    {
        return m_MediaPlayerStem.GetShiftedVideoPosition(frameCount);
    }

    bool MediaPlayerStemImpl::ShiftVideoPositionFwd(int k)
    {
        return m_MediaPlayerStem.ShiftVideoPositionFwd(k);
    }

    void MediaPlayerStemImpl::SetAudioVolume(double vol)
    {
        m_MediaPlayerStem.SetAudioVolume(vol);
    }

    void MediaPlayerStemImpl::Run()
    {
        m_MediaPlayerStem.Run();
    }

    void MediaPlayerStemImpl::Pause()
    {
        m_MediaPlayerStem.Pause();
    }

    void MediaPlayerStemImpl::Stop()
    {
        m_MediaPlayerStem.Stop();
    }

    bool MediaPlayerStemImpl::PumpError() const
    {
        return m_MediaPlayerStem.PumpError();
    }

    void MediaPlayerStemImpl::StartRewinder(int fps)
    {
        m_MediaPlayerStem.StartRewinder(fps);
    }

    void MediaPlayerStemImpl::StopRewinder()
    {
        m_MediaPlayerStem.StopRewinder();
    }

    int MediaPlayerStemImpl::SendFilterCommandVideo(const char* tar, const char* cmd, const char* arg)
    {
        return m_MediaPlayerStem.SendFilterCommandVideo(tar, cmd, arg);
    }

    int MediaPlayerStemImpl::ReconfigureFilterGraphVideo(const char* filterString)
    {
        return m_MediaPlayerStem.ReconfigureFilterGraphVideo(filterString);
    }

    int MediaPlayerStemImpl::SendFilterCommandAudio(const char* tar, const char* cmd, const char* arg)
    {
        return m_MediaPlayerStem.SendFilterCommandAudio(tar, cmd, arg);
    }

    int MediaPlayerStemImpl::ReconfigureFilterGraphAudio(const char* filterString)
    {
        return m_MediaPlayerStem.ReconfigureFilterGraphAudio(filterString);
    }

// ---------------------------------------------------------------------------------------------------------
// factory 

    IMediaPlayerStem* IMediaPlayerStem::CreateInstance(bool useFilterGraphAlways, bool convFrameSequentially)
    {
        return static_cast<IMediaPlayerStem*>(new MediaPlayerStemImpl(useFilterGraphAlways, convFrameSequentially));
    }

} //namespace Fwrap
