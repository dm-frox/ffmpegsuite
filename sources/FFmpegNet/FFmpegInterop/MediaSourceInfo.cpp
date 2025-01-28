#include "pch.h"

#include "MediaSourceInfo.h"

#include "Exceptions.h"


namespace FFmpegInterop
{

    MediaSourceInfo::MediaSourceInfo(InfoPrt p)
        : m_Info(p)
    { 
        ASSERT_PTR(m_Info);
    }

    bool MediaSourceInfo::HasVideo::get() 
    { 
        return m_Info->HasVideo();
    }

    int MediaSourceInfo::VideoIndex::get()
    { 
        return m_Info->VideoIndex();
    }

    int MediaSourceInfo::Width::get()
    {
        return m_Info->Width();
    }

    int MediaSourceInfo::Height::get()
    {
        return m_Info->Height();
    }

    int MediaSourceInfo::Fps_N::get()
    {
        return m_Info->Fps_N();
    }

    int MediaSourceInfo::Fps_D::get()
    {
        return m_Info->Fps_D();
    }

    double MediaSourceInfo::Fps::get()
    {
        return m_Info->Fps();
    }

    StrType^ MediaSourceInfo::PixelFormat::get()
    {
        return gcnew StrType(m_Info->PixelFormatStr());
    }

    StrType^ MediaSourceInfo::VideoCodec::get()
    {
        return gcnew StrType(m_Info->VideoCodec());
    }

    int64_t MediaSourceInfo::VideoBitrate::get()
    {
        return m_Info->VideoBitrate();
    }

    bool MediaSourceInfo::HasAudio::get()
    {
        return m_Info->HasAudio();
    }

    int MediaSourceInfo::AudioIndex::get()
    {
        return m_Info->AudioIndex();
    }

    int MediaSourceInfo::Chann::get()
    {
        return m_Info->Chann();
    }

    int MediaSourceInfo::SampleRate::get()
    {
        return m_Info->SampleRate();
    }

    StrType^ MediaSourceInfo::SampleFormat::get()
    {
        return gcnew StrType(m_Info->SampleFormatStr());
    }

    StrType^ MediaSourceInfo::AudioCodec::get()
    {
        return gcnew StrType(m_Info->AudioCodec());
    }

    int64_t MediaSourceInfo::AudioBitrate::get()
    {
        return m_Info->AudioBitrate();
    }

    bool MediaSourceInfo::CanSeek::get()
    {
        return m_Info->CanSeek();
    }

    double MediaSourceInfo::Duration::get()
    {
        return m_Info->Duration();
    }

} // namespace FFmpegInterop
