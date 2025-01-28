#include "pch.h"

#include "MediaPlayerStem.h"

#include "Core.h"
#include "Exceptions.h"
#include "Marshal.h"


namespace FFmpegInterop
{
    MediaPlayerStem::MediaPlayerStem(bool useFilterGraphAlways, bool convFrameSequentially)
        : MediaPlayerBoxBase(Core::Ptr()->CreateMediaPlayerStem(useFilterGraphAlways, convFrameSequentially))
    {
        ASSERT_IMPL();
    }

    void MediaPlayerStem::SetFrameParamsVideo(int width, int height, StrType^ pixFmt, StrType^ vidFilter, int fpsFactor)
    {
        Ptr()->SetFrameParamsVideo(width, height, TO_CHARS(pixFmt), TO_CHARS(vidFilter), fpsFactor);
    }

    void MediaPlayerStem::SetRendModeAudio(int maxChann, int reduceSampleFormat)
    {
        Ptr()->SetRendModeAudio(maxChann, reduceSampleFormat);
    }

    void MediaPlayerStem::SetFrameParamsAudio(int chann, int sampleRate, StrType^ sampFmt, StrType^ channLayout, StrType^ audFilter)
    {
        Ptr()->SetFrameParamsAudio(chann, sampleRate, TO_CHARS(sampFmt), TO_CHARS(channLayout), TO_CHARS(audFilter));
    }

    void MediaPlayerStem::Build(StrType^ url, StrType^ fmt, StrType^ fmtOps,
        bool useVideo, StrType^ decPrmsVideo, bool useAudio, StrType^ decPrmsAudio)
    {
        bool res = Ptr()->Build(TO_CHARS(url), TO_CHARS(fmt), TO_CHARS(fmtOps),
            useVideo, TO_CHARS(decPrmsVideo), useAudio, TO_CHARS(decPrmsAudio));

        ASSERT_GEN(res, "error");
    }

    void MediaPlayerStem::SetupRendering(
        PtrType rendVideo, bool holdPresentationTime, bool synkByAudio, bool swapFields,
        PtrType rendAudio, int audioDevId)
    {
        bool res = Ptr()->SetupRendering(
            rendVideo.ToPointer(), holdPresentationTime, synkByAudio, swapFields, 
            rendAudio.ToPointer(), audioDevId);
        ASSERT_GEN(res, "error");
    }

    void MediaPlayerStem::SetPosition(double pos, bool onPause)
    {
        Ptr()->SetPosition(pos, onPause);
    }

    bool MediaPlayerStem::SeekingInProgress::get() 
    { 
        return Ptr()->SeekingInProgress();
    }

    double MediaPlayerStem::GetShiftedVideoPosition(int frameCount)
    {
        return Ptr()->GetShiftedVideoPosition(frameCount);
    }

    bool MediaPlayerStem::ShiftVideoPositionFwd(int frameCount)
    {
        return Ptr()->ShiftVideoPositionFwd(frameCount);
    }

    void MediaPlayerStem::SetAudioVolume(double vol)
    {
        Ptr()->SetAudioVolume(vol);
    }

    double MediaPlayerStem::Position::get()
    {
        return Ptr()->Position();
    }

    double MediaPlayerStem::VideoRendPosition::get()
    {
        return Ptr()->VideoRendPosition();
    }

    bool MediaPlayerStem::Proceeding::get()
    {
        return Ptr()->Proceeding();
    }

    bool MediaPlayerStem::EndOfData::get()
    {
        return Ptr()->EndOfData();
    }

    bool MediaPlayerStem::EndOfVideo::get()
    {
        return Ptr()->EndOfVideo();
    }

    MediaSourceInfo^ MediaPlayerStem::SourceInfo::get()
    {
        if (m_MediaSourceInfo == nullptr)
        {
            m_MediaSourceInfo = gcnew MediaSourceInfo(Ptr()->MediaSourceInfo());
        }
        return m_MediaSourceInfo;
    }

    bool MediaPlayerStem::HasVideo::get()
    {
        return Ptr()->HasVideo();
    }

    int MediaPlayerStem::Width::get()
    {
        return Ptr()->Width();
    }

    int MediaPlayerStem::Height::get()
    {
        return Ptr()->Height();
    }

    StrType^ MediaPlayerStem::PixFmtStr::get()
    {
        return gcnew StrType(Ptr()->PixelFormatStr());
    }

    bool MediaPlayerStem::HasAudio::get()
    {
        return Ptr()->HasAudio();
    }

    int MediaPlayerStem::Chann::get()
    {
        return Ptr()->Chann();
    }

    int MediaPlayerStem::SampleRate::get()
    {
        return Ptr()->SampleRate();
    }

    StrType^ MediaPlayerStem::SampFmtStr::get()
    {
        return gcnew StrType(Ptr()->SampleFormatStr());
    }

    int MediaPlayerStem::RendWidth::get()
    {
        return Ptr()->RendWidth();
    }

    int MediaPlayerStem::RendHeight::get()
    {
        return Ptr()->RendHeight();
    }

    int MediaPlayerStem::RendChann::get()
    {
        return Ptr()->RendChann();
    }

    int MediaPlayerStem::RendSampleRate::get()
    {
        return Ptr()->RendSampleRate();
    }

    int MediaPlayerStem::StreamCount::get()
    {
        return Ptr()->StreamCount();
    }

    StrType^ MediaPlayerStem::GetInfo()
    {
        return NativeString::GetStringEx(Ptr()->GetInfo());
    }

    StrType^ MediaPlayerStem::GetMetadata()
    {
        return NativeString::GetStringEx(Ptr()->GetMetadata());
    }

    StrType^ MediaPlayerStem::GetChapters()
    {
        return NativeString::GetStringEx(Ptr()->GetChapters());
    }

    StrType^ MediaPlayerStem::GetStreamInfo(int strmIdx)
    {
        return NativeString::GetStringEx(Ptr()->GetStreamInfo(strmIdx));
    }

    StrType^ MediaPlayerStem::GetStreamMetadata(int strmIdx)
    {
        return NativeString::GetStringEx(Ptr()->GetStreamMetadata(strmIdx));
    }

    void MediaPlayerStem::Run()
    {
        Ptr()->Run();
    }

    void MediaPlayerStem::Pause()
    {
        Ptr()->Pause();
    }

    void MediaPlayerStem::Stop()
    {
        Ptr()->Stop();
    }

    bool MediaPlayerStem::PumpError::get()
    {
        return Ptr()->PumpError();
    }

    void MediaPlayerStem::StartRewinder(int fps)
    {
        Ptr()->StartRewinder(fps);
    }

    void MediaPlayerStem::StopRewinder()
    {
        Ptr()->StopRewinder();
    }

    bool MediaPlayerStem::SendFilterCommandVideo(StrType^ tar, StrType^ cmd, StrType^ arg)
    {
        int res = Ptr()->SendFilterCommandVideo(TO_CHARS(tar), TO_CHARS(cmd), TO_CHARS(arg));
        return res >= 0;
    }

    void MediaPlayerStem::ReconfigureFilterGraphVideo(StrType^ filterString)
    {
        int res = Ptr()->ReconfigureFilterGraphVideo(TO_CHARS(filterString));
        ASSERT_RES(res);
    }

    bool MediaPlayerStem::SendFilterCommandAudio(StrType^ tar, StrType^ cmd, StrType^ arg)
    {
        int res = Ptr()->SendFilterCommandAudio(TO_CHARS(tar), TO_CHARS(cmd), TO_CHARS(arg));
        return res >= 0;
    }

    void MediaPlayerStem::ReconfigureFilterGraphAudio(StrType^ filterString)
    {
        int res = Ptr()->ReconfigureFilterGraphAudio(TO_CHARS(filterString));
        ASSERT_RES(res);
    }

} // namespace FFmpegInterop