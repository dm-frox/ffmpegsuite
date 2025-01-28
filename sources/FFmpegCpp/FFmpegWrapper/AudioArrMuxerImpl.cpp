#include "pch.h"

#include "AudioArrMuxerImpl.h"


namespace Fwrap
{
    AudioArrMuxerImpl::AudioArrMuxerImpl() = default;

    AudioArrMuxerImpl::~AudioArrMuxerImpl() = default;

    int AudioArrMuxerImpl::Build(const char* urlVideo, const char* urlAudio, const char* urlDst)
    {
        return m_Muxer.Build(urlVideo, urlAudio, urlDst);
    }

    void AudioArrMuxerImpl::Close()
    {
        m_Muxer.Close();
    }

    bool AudioArrMuxerImpl::EndOfData() const
    {
        return m_Muxer.EndOfData();
    }

    bool AudioArrMuxerImpl::Error() const
    {
        return m_Muxer.Error();
    }

    IAudioArrMuxer* IAudioArrMuxer::CreateInstance()
    {
        return static_cast<IAudioArrMuxer*>(new AudioArrMuxerImpl());
    }

} // namespace Fwrap