#include "pch.h"

#include "Transcoder.h"

#include "Logger.h"
#include "CommonConsts.h"
#include "Misc.h"
#include "FrameHub.h"
#include "FilterTools.h"

using namespace Fcore;

namespace Fapp
{

    Transcoder::Transcoder(bool useFilterGraph)
        : m_FrameSource(useFilterGraph, true, false) 
    {}

    Transcoder::~Transcoder()
    {
        Close();
    }

    void Transcoder::SetFrameParamsVideo(int width, int height, const char* pixFmt, const char* vidFilter, int fpsFactor)
    {
        std::string vf = vidFilter;
        if (fpsFactor > 0)
        {
            FilterTools::AppendFps(vf, fpsFactor);
        }
        m_FrameSource.SetFrameParamsVideo(width, height, pixFmt, vf.c_str(), fpsFactor);
    }

    void Transcoder::SetFrameParamsAudio(int chann, int sampleRate, const char* sampFmt, const char* channLayout, const char* audFilter)
    {
        m_FrameSource.SetFrameParamsAudio(chann, sampleRate, sampFmt, channLayout, audFilter);
    }

    void Transcoder::SetEncoderParamsVideo(const char* vidEnc, int64_t vidBitrate, const char* preset, int crf, const char* vidOpts)
    {
        if (vidEnc && *vidEnc)
            m_EncVideo = vidEnc;
        m_BitrateVideo = vidBitrate;
        if (preset && *preset)
            m_Preset = preset;
        m_Crf = crf;

        MultiString::Assign(m_OptsVideo, vidOpts);
    }

    void Transcoder::SetEncoderParamsAudio(const char* audEnc, int64_t audBitrate, const char* audOpts)
    {
        if (audEnc && *audEnc)
            m_EncAudio = audEnc;
        m_BitrateAudio = audBitrate;

        MultiString::Assign(m_OptsAudio, audOpts);
    }

    void Transcoder::SetMetadata(const char* metadata, const char* metadataVideo, const char* metadataAudio)
    {
        MultiString::Assign(m_Metadata, metadata);
        MultiString::Assign(m_MetadataVideo, metadataVideo);
        MultiString::Assign(m_MetadataAudio, metadataAudio);
    }

    int Transcoder::SetupTranscoderVideo()
    {
        int ret = -1;
        if (!m_EncVideo.empty())
        {
            int rr = m_FrameSource.ConnectToFrameHubVideo(nullptr, this);
            if (rr >= 0)
            {
                int strmIdx = m_Muxer.AttachFrameHub(m_FrameSource.FrameHubVideo());
                if (strmIdx >= 0)
                {
                    ret = strmIdx;
                    LOG_INFO("%s, strmIdx=%d, encoder=%s, frame=%dx%d, pix.fmt=%s",
                        __FUNCTION__, strmIdx,
                        Fcore::StrToLog(m_EncVideo.c_str()), 
                        m_FrameSource.Width(), m_FrameSource.Height(),
                        Fcore::PixFmtToStr(m_FrameSource.PixelFormat()));
                }
                else
                {
                    LOG_ERROR("%s, AttachFrameHub, code=%d", __FUNCTION__, strmIdx);
                }
            }
            else
            {
                ret = rr;
            }
        }
        else
        {
            LOG_ERROR("%s, no encoder", __FUNCTION__);
        }
        return ret;
    }

    int Transcoder::SetupTranscoderAudio()
    {
        int ret = -1;
        if (!m_EncAudio.empty())
        {
            int rr = m_FrameSource.ConnectToFrameHubAudio(nullptr, this);
            if (rr >= 0)
            {
                int strmIdx = m_Muxer.AttachFrameHub(m_FrameSource.FrameHubAudio());
                if (strmIdx >= 0)
                {
                    ret = strmIdx;
                    LOG_INFO("%s, strmIdx=%d, encoder=%s, chann=%d, samp.rate=%d, samp.fmt=%s, ch.mask=%lld, fr.size=%d",
                        __FUNCTION__, strmIdx,
                        Fcore::StrToLog(m_EncAudio.c_str()), m_FrameSource.ChannLayout().Chann(), m_FrameSource.SampleRate(),
                        Fcore::SampFmtToStr(m_FrameSource.SampleFormat()),
                        m_FrameSource.ChannLayout().Mask(), m_FrameSource.FrameSize());
                }
                else
                {
                    LOG_ERROR("%s, AttachFrameHub, code=%d", __FUNCTION__, strmIdx);
                }
            }
            else
            {
                ret = rr;
            }
        }
        else
        {
            LOG_ERROR("%s, no encoder", __FUNCTION__);
        }
        return ret;
    }

    int Transcoder::SetupOutputStreamVideo(AVPixelFormat& pixFmt)
    {
        if (m_BitrateVideo > 0)
        {
            m_Muxer.SetBitrateVideo(m_BitrateVideo);
        }
        if (!m_Preset.empty())
        {
            m_Muxer.SetPresetVideo(m_Preset.c_str());
        }
        if (m_Crf >= 0)
        {
            m_Muxer.SetCrfVideo(m_Crf);
        }

        return m_Muxer.AddStreamVideo(m_EncVideo.c_str(), 
            m_FrameSource.Width(), m_FrameSource.Height(), 
            m_FrameSource.PixelFormat(), m_FrameSource.FrameRate(), m_FrameSource.TimebaseVideo(), m_OptsVideo.c_str(),
            pixFmt);
    }

    int Transcoder::SetupOutputStreamAudio(Fcore::ChannLayout& channLayout, AVSampleFormat& sampFmt, int& frameSize)
    {
        if (m_BitrateAudio > 0)
        {
            m_Muxer.SetBitrateAudio(m_BitrateAudio);
        }

        return m_Muxer.AddStreamAudio(m_EncAudio.c_str(), 
            m_FrameSource.ChannLayout(),
            m_FrameSource.SampleRate(),
            m_FrameSource.SampleFormat(), 
            m_OptsAudio.c_str(),
            channLayout, sampFmt, frameSize);
    }

    int Transcoder::Build(
        const char* srcUrl, const char* srcFormat, const char* srcOptions,
        const char* dstUrl, const char* dstFormat, const char* dstOptions,
        bool useVideo, bool useAudio
    )
    {
        if (m_Ready)
            return ErrorAlreadyInitialized;

        if (!useVideo && !useAudio)
            return ErrorBadArgs;

        int ret = -1;

        LOG_INFO("%s, begin transcoding, useVideo=%d, useAudio=%d", __FUNCTION__, useVideo, useAudio);
        LOG_INFO("%s, try open source...", __FUNCTION__);
        int rr = m_FrameSource.OpenDemuxer(srcUrl, srcFormat, srcOptions, useVideo, useAudio);
        if (rr >= 0)
        {
            bool cc = true;
            if (useVideo && !m_FrameSource.HasVideo())
                cc = false;
            if (useAudio && !m_FrameSource.HasAudio())
                cc = false;
            if (cc)
            {
                LOG_INFO("%s, try open destination...", __FUNCTION__);
                rr = m_Muxer.Open(dstUrl, dstFormat);
                if (rr >= 0)
                {
                    if (useVideo)
                    {
                        if (!m_EncVideo.empty())
                        {
                            rr = SetupTranscoderVideo();
                        }
                        else // packet translocation
                        {
                            rr = m_FrameSource.ConnectToMuxerVideo(&m_Muxer);
                        }
                    }
                }
                if (rr >= 0)
                {
                    if (useAudio)
                    {
                        if (!m_EncAudio.empty())
                        {
                            rr = SetupTranscoderAudio();
                        }
                        else // packet translocation
                        {
                            rr = m_FrameSource.ConnectToMuxerAudio(&m_Muxer);
                        }
                    }
                }
                if (rr >= 0)
                {
                    rr = m_Muxer.OpenWriting(dstOptions, m_Metadata.c_str(), m_MetadataVideo.c_str(), m_MetadataAudio.c_str());
                    if (rr >= 0)
                    {
                        LOG_INFO("%s, app graph has built successfully, start work threads", __FUNCTION__);
                        SetupPump();
                        Run();
                        m_Ready = true;
                    }
                }
            }
            else
            {
                rr = ErrorNoRequiredStreams; // no required streams
                LOG_ERROR("%s, no required streams", __FUNCTION__);
            }
        }
        ret = rr;
        return ret;
    }

    void Transcoder::Close()
    {
        if (!m_Closed && m_Ready)
        {
            LOG_INFO("%s, begin...", __FUNCTION__);
            LOG_FLUSH();
            m_Muxer.CloseWriting();
            m_FrameSource.ClosePump();
            m_Muxer.ClosePump();
            LOG_INFO("%s, end", __FUNCTION__);
            LOG_FLUSH();
            m_Closed = true;
        }
    }

    void Transcoder::SetupPump()
    {
        m_Muxer.CreatePump();
        m_FrameSource.CreatePump();
    }

    void Transcoder::Run()
    {
        m_Muxer.Run();
        m_FrameSource.Run();
    }

    void Transcoder::Pause()
    {
        m_Muxer.Pause();
        m_FrameSource.Pause();
    }

    bool Transcoder::EndOfData() const
    {
        return m_Muxer.EndOfData();
    }

    bool Transcoder::Error() const
    {
        return m_Muxer.PumpError() || m_FrameSource.PumpError();
    }

    double Transcoder::Position() const
    {
        return m_Ready ? m_Muxer.Position() : 0.0;
    }

    double Transcoder::Duration() const
    { 
        return m_FrameSource.MediaSourceInfo()->Duration(); 
    }

 } // namespace Fapp
