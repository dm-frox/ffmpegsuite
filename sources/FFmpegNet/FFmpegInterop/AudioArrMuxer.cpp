#include "pch.h"

#include "AudioArrMuxer.h"

#include "Core.h"
#include "Exceptions.h"
#include "Marshal.h"


namespace FFmpegInterop
{
    AudioArrMuxer::AudioArrMuxer()
        : DataMuxerBase(Core::Ptr()->CreateDataMuxer())
    {}

    void AudioArrMuxer::Build(StrType^ urlVideo, StrType^ urlAudio, StrType^ urlDst)
    {
        int res = Ptr()->Build(TO_CHARS(urlVideo), TO_CHARS(urlAudio), TO_CHARS(urlDst));

        ASSERT_RES(res);
    }

    void AudioArrMuxer::Close()
    {
        Ptr()->Close();
    }

    bool AudioArrMuxer::EndOfData::get()
    {
        return Ptr()->EndOfData();
    }

    bool AudioArrMuxer::Error::get()
    {
        return Ptr()->Error();
    }

} // namespace FFmpegInterop
