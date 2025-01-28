#include "pch.h"

#include "Transcoder.h"

#include "Core.h"
#include "Exceptions.h"
#include "Marshal.h"


namespace FFmpegInterop
{
    Transcoder::Transcoder(bool useFilterGraph)
        : TranscoderBase(Core::Ptr()->CreateTranscoder(useFilterGraph))
    {
        ASSERT_IMPL();
    }

    void Transcoder::SetFrameParamsVideo(int width, int height, StrType^ pixFmt, StrType^ vidFilter, int fps)
    {
        Ptr()->SetFrameParamsVideo(width, height, TO_CHARS(pixFmt), TO_CHARS(vidFilter), fps);
    }

    void Transcoder::SetEncoderParamsVideo(StrType^ vidEnc, int64_t vidBitrate, StrType^ preset, int crf, StrType^ vidOpts)
    {
        Ptr()->SetEncoderParamsVideo(TO_CHARS(vidEnc), vidBitrate, TO_CHARS(preset), crf, TO_CHARS(vidOpts));
    }

    void Transcoder::SetFrameParamsAudio(int chann, int sampleRate, StrType^ sampFmt, StrType^ channLayout, StrType^ audFilter)
    {
        Ptr()->SetFrameParamsAudio(chann, sampleRate, TO_CHARS(sampFmt), TO_CHARS(channLayout), TO_CHARS(audFilter));
    }

    void Transcoder::SetEncoderParamsAudio(StrType^ audEnc, int64_t audBitrate, StrType^ audOpts)
    {
        Ptr()->SetEncoderParamsAudio(TO_CHARS(audEnc), audBitrate, TO_CHARS(audOpts));
    }

    void Transcoder::SetMetadata(StrType^ metadata, StrType^ metadataVideo, StrType^ metadataAudio)
    {
        Ptr()->SetMetadata(TO_CHARS(metadata), TO_CHARS(metadataVideo), TO_CHARS(metadataAudio));
    }

    void Transcoder::Build(
        StrType^ srcUrl, StrType^ srcFormat, StrType^ srcOptions,
        StrType^ dstUrl, StrType^ dstFormat, StrType^ dstOptions,
        bool useVideo, bool useAudio)
    {
        int res = Ptr()->Build(
            TO_CHARS(srcUrl), TO_CHARS(srcFormat), TO_CHARS(srcOptions),
            TO_CHARS(dstUrl), TO_CHARS(dstFormat), TO_CHARS(dstOptions),
            useVideo, useAudio);

        ASSERT_RES(res);
    }

    double Transcoder::Duration::get()
    {
        return Ptr()->Duration();
    }

    double Transcoder::Position::get()
    {
        return Ptr()->Position();
    }

    bool Transcoder::EndOfData::get()
    {
        return Ptr()->EndOfData();
    }

    bool Transcoder::Error::get()
    {
        return Ptr()->Error();
    }

    void Transcoder::CancelInput()
    {
        Ptr()->CancelInput();
    }

    void Transcoder::Run()
    {
        Ptr()->Run();
    }

    void Transcoder::Pause()
    {
        Ptr()->Pause();
    }

    MediaSourceInfo^ Transcoder::SourceInfo::get() 
    { 
        if (m_MediaSourceInfo == nullptr)
        {
            m_MediaSourceInfo = gcnew MediaSourceInfo(Ptr()->MediaSourceInfo());
        }

        return m_MediaSourceInfo;
    }

} // namespace FFmpegInterop

