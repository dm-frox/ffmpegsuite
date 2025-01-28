#include "pch.h"

#include "TranscoderImpl.h"

#include "SLogger.h"


namespace Fwrap
{
    TranscoderImpl::TranscoderImpl(bool useFilterGrap)
        : m_Transcoder(useFilterGrap)
    {}

    TranscoderImpl::~TranscoderImpl() = default;

    void TranscoderImpl::Delete()
    {
        IBase::Delete();
        LOG_INFO(__FUNCTION__);
    }
    
    void TranscoderImpl::SetFrameParamsVideo(int width, int height, const char* pixFmt, const char* vidFilter, int fps)
    {
        m_Transcoder.SetFrameParamsVideo(width, height, pixFmt, vidFilter, fps);
    }

    void TranscoderImpl::SetEncoderParamsVideo(const char* vidEnc, int64_t vidBitrate, const char* preset, int crf, const char* vidOpts)
    {
        m_Transcoder.SetEncoderParamsVideo(vidEnc, vidBitrate, preset, crf, vidOpts);
    }
        
    void TranscoderImpl::SetFrameParamsAudio(int chann, int sampleRate, const char* sampFmt, const char* channLayout, const char* audFilter)
    {
        m_Transcoder.SetFrameParamsAudio(chann, sampleRate, sampFmt, channLayout, audFilter);
    } 

    void TranscoderImpl::SetEncoderParamsAudio(const char* audEnc, int64_t audBitrate, const char* audOpts)
    {
        m_Transcoder.SetEncoderParamsAudio(audEnc, audBitrate, audOpts);
    }

    void TranscoderImpl::SetMetadata(const char* metadata, const char* metadataVideo, const char* metadataAudio)
    {
        m_Transcoder.SetMetadata(metadata, metadataVideo, metadataAudio);
    }
        
    int TranscoderImpl::Build(
            const char* srcUrl, const char* srcFormat, const char* srcOptions,
            const char* dstUrl, const char* dstFormat, const char* dstOptions,
            bool useVideo, bool useAudio)
    {
        return m_Transcoder.Build(
            srcUrl, srcFormat, srcOptions,
            dstUrl, dstFormat, dstOptions,
            useVideo, useAudio);
    }
    
    bool TranscoderImpl::EndOfData() const
    {
        return m_Transcoder.EndOfData();
    }
    
    bool TranscoderImpl::Error() const
    {
        return m_Transcoder.Error();
    }
    
    double TranscoderImpl::Duration() const
    {
        return m_Transcoder.Duration();
    }
    
    double TranscoderImpl::Position() const
    {
        return m_Transcoder.Position();
    }

    void TranscoderImpl::CancelInput()
    {
        m_Transcoder.CancelInput();
    }

    void TranscoderImpl::Run()
    {
        m_Transcoder.Run();
    }

    void TranscoderImpl::Pause()
    {
        m_Transcoder.Pause();
    }

    const IMediaSourceInfo* TranscoderImpl::MediaSourceInfo() const
    {
        return m_Transcoder.MediaSourceInfo();
    }

// --------------------------------------------------------------------
// factory 
    
    ITranscoder* ITranscoder::CreateInstance(bool useFilterGraph)
    {
        LOG_INFO("%s, useFilterGraph=%d", __FUNCTION__, useFilterGraph);

        return static_cast<ITranscoder*>(new TranscoderImpl(useFilterGraph));
    }
    
} // namespace Fwrap
