#include "pch.h"

#include "Muxer.h"

#include "Logger.h"
#include "CommonConsts.h"
#include "Misc.h"
#include "FrameHub.h"
#include "CodecTools.h"
#include "StreamReader.h"


namespace Fcore
{
    Muxer::Muxer()
    {
        m_Streams.Reserve(8);
    }

    Muxer::~Muxer() = default;

    bool Muxer::AddDispositionFlagToLastStream(int flag)
    {
        bool ret = false;
        if (m_Streams.Count() > 0)
        {
            m_Streams.Back().AddDispositionFlag(flag);
            ret = true;
        }
        return ret;
    }

    bool Muxer::SetLastStreamDefault()
    {
        return AddDispositionFlagToLastStream(AV_DISPOSITION_DEFAULT);
    }

    void Muxer::SetMetadata(const char* metadata, const char* metadataVideo, const char* metadataAudio)
    {
        LOG_INFO(__FUNCTION__);
        if (metadata && *metadata)
        {
            Base::SetMetadata(metadata, "General");
        }
        if (metadataVideo && *metadataVideo && m_IndexVideo >= 0)
        {
            m_Streams[m_IndexVideo].SetMetadata(metadataVideo, "Video");
        }
        if (metadataAudio && *metadataAudio && m_IndexAudio >= 0)
        {
            m_Streams[m_IndexAudio].SetMetadata(metadataAudio, "Audio");
        }
    }

    int Muxer::OpenWriting(const char* options, const char* metadata, const char* metadataVideo, const char* metadataAudio)
    {
        if (!AssertStreams(__FUNCTION__))
            return ErrorNoStreams;

        if (Base::IsFile())
        {
            SetMetadata(metadata, metadataVideo, metadataAudio);
        }

        int ret = Base::OpenWriting(options);

        if (ret >= 0)
        {
            for (int i = 0, n = m_Streams.Count(); i < n; ++i)
            {
                m_Streams[i].SetupTimebases();
            }
        }

        return ret;
    }

    int Muxer::CloseWriting()
    {
        for (int i = 0, n = m_Streams.Count(); i < n; ++i)
        {
            m_Streams[i].LogStatistics();
        }
        return Base::Finalize();
    }

    const Encoder& Muxer::GetEncoder(int idx) const
    {
        return m_Streams[idx].Encoder();
    }

// ---------------------------------------------------------------------
// setup output streams

    int Muxer::SetupOutputStream(AVCodecContext* codecCtx) // real transcoding
    {
        AVStream* strm = nullptr;
        int ret = Base::AddOutputStream(codecCtx, strm);
        if (ret >= 0)
        {
            strm->time_base = codecCtx->time_base;
            auto sw = new StreamWriter(*strm, codecCtx, *this);
            ret = sw->PrepareEncoding();
            if (ret >= 0)
            {
                m_Streams.AppendItem(sw);
                ret = m_Streams.Count() - 1;
            }
        }
        return ret;
    }

    int Muxer::SetupOutputStream(const AVStream* srcStream) // packet translocation
    {
        AVStream* strm = nullptr;
        int ret = Base::AddOutputStream(srcStream, strm);
        if (ret >= 0)
        {
            auto sw = new StreamWriter(*strm, nullptr, *this);
            sw->SetPacketTimebaseSrc(srcStream->time_base);
            m_Streams.AppendItem(sw);
            ret = m_Streams.Count() - 1;
        }
        return ret;
    }

    int Muxer::SetupOutputStreamAudioEx(AVCodecID codecId, int chann, int sampRate, AVSampleFormat sampFmt, int frameSize)
    {
        return Base::AddOutputStreamAudio(codecId, chann, sampRate, sampFmt, frameSize);
    }

// video

    int Muxer::SetupOutputStreamVideo(const AVCodec* encoder, 
        int width, int height, AVPixelFormat pixFmt, AVRational frameRate, AVRational tb, const char* options)
    {
        int ret = -1;
        if (AVCodecContext* codecCtx = avcodec_alloc_context3(encoder))
        {
            CodecTools::SetupVideoCodecContext(codecCtx, width, height, pixFmt, frameRate, tb);
            CodecTools::SetupVideoCodecContext(codecCtx, m_BitrateVideo, m_PresetVideo.c_str(), m_CrfVideo);
            ret = OpenEncoder(codecCtx, encoder, options);
            if (ret >= 0)
            {
                ret = SetupOutputStream(codecCtx);
                if (ret >= 0)
                {
                    m_Streams.Back().SetFrameTimebases(tb, codecCtx->time_base);
                    m_IndexVideo = ret;
                    LOG_INFO("%s, video, idx=%d", __FUNCTION__, m_IndexVideo);
                    CodecTools::LogCodecContext(codecCtx, true, __FUNCTION__);
                }
            }
            if (ret < 0)
            {
                avcodec_free_context(&codecCtx);
            }
        }
        else
        {
            ret = AVERROR(ENOMEM);
            LOG_ERROR("%s, video, avcodec_alloc_context3", __FUNCTION__);
        }
        return ret;
    }

    //static 
    AVPixelFormat Muxer::FindFrameParamsVideo(const AVCodec* encoder, AVPixelFormat pixFmt)
    {
        AVPixelFormat pf = CodecTools::FindPixelFormat(encoder, pixFmt);
        if (pf != AV_PIX_FMT_NONE)
        {
            if (pf != pixFmt)
            {
                LOG_INFO("%s, force new pix.fmt: %s->%s", __FUNCTION__, PixFmtToStr(pixFmt), PixFmtToStr(pf));
            }
            return pf;
        }
        else
        {
            LOG_WARNING("%s, failed to get pix.fmt", __FUNCTION__);
            return pixFmt;
        }
    }

    int Muxer::AddStreamVideo(
        const char* encoderName, 
        int width, int height, AVPixelFormat pixFmt, AVRational frameRate, AVRational tb, const char* options,
        AVPixelFormat& actPixFmt)
    {
        int ret = -1;

        if (!AssertFormat(__FUNCTION__))
            return ErrorNoFormatCtx;
        if (!AssertVideoParams(width, height, frameRate, __FUNCTION__))
            return ErrorBadArgs;

        LOG_INFO("%s, try: enc=%s, frame=%dx%d, pix.fmt=%s, fr.rate=%d/%d, timebase=%d/%d",
            __FUNCTION__, 
            encoderName,
            width, height, PixFmtToStr(pixFmt), frameRate.num, frameRate.den, tb.num, tb.den);

        if (const AVCodec* encoder = GetEncoderByName(encoderName))
        {
            AVPixelFormat pixFmt2 = FindFrameParamsVideo(encoder, pixFmt);
            ret = SetupOutputStreamVideo(encoder, width, height, pixFmt2, frameRate, tb, options);
            if (ret >= 0)
            {
                actPixFmt = pixFmt2;
            }
        }

        return ret;
    }

// audio

    int  Muxer::SetupOutputStreamAudio(const AVCodec* encoder, 
        const ChannLayout& channLayout, int sampleRate, AVSampleFormat sampFmt, 
        const char* options, int& frameSize)
    {
        int ret = -1;
        if (AVCodecContext* codecCtx = avcodec_alloc_context3(encoder))
        {
            CodecTools::SetupAudioCodecContext(codecCtx, channLayout, sampleRate, sampFmt);
            CodecTools::SetupAudioCodecContext(codecCtx, m_BitrateAudio);
            ret = OpenEncoder(codecCtx, encoder, options); 
            if (ret >= 0)
            {
                ret = SetupOutputStream(codecCtx);
                if (ret >= 0)
                {
                    m_Streams.Back().SetFrameTimebasesAudio(codecCtx->sample_rate, codecCtx->time_base);
                    m_IndexAudio = ret;
                    frameSize = codecCtx->frame_size;
                    LOG_INFO("%s, audio, idx=%d", __FUNCTION__, m_IndexAudio);
                    CodecTools::LogCodecContext(codecCtx, true, __FUNCTION__);
                }
            }
            if (ret < 0)
            {
                avcodec_free_context(&codecCtx);
            }
        }
        else
        {
            ret = AVERROR(ENOMEM);
            LOG_ERROR("%s, audio, avcodec_alloc_context3", __FUNCTION__);
        }
        return ret;
    }

    //static 
    AVSampleFormat Muxer::FindFrameParamsAudio(const AVCodec* encoder, AVSampleFormat sampFmt)
    {
        AVSampleFormat sf = CodecTools::FindSampleFormat(encoder, sampFmt);
        if (sf != AV_SAMPLE_FMT_NONE)
        {
            if (sf != sampFmt)
            {
                LOG_INFO("%s, force new samp.fmt: %s->%s", __FUNCTION__, SampFmtToStr(sampFmt), SampFmtToStr(sf));
            }
            return sf;
        }
        else
        {
            LOG_WARNING("%s, failed to get samp.fmt", __FUNCTION__);
            return sampFmt;
        }
    }

    // static
    void Muxer::FindFrameParamsAudio(const AVCodec* encoder, const ChannLayout& channLayout, ChannLayout& channLayoutEnc)
    {
        ChannLayout chl;
        chl.Set(ChannLayout::Find(GetCodecConfig<AVChannelLayout>(encoder), channLayout));
        if (!chl.IsNull())
        {
            if (chl.Chann() == 1 && channLayout.Chann() > 1) // try stereo
            {
                ChannLayout ster;
                ster.Set(ChannLayout::Find(GetCodecConfig<AVChannelLayout>(encoder), ChannLayout(2)));
                if (ster.Chann() == 2) // found
                {
                    chl.Set(ster);
                }
            }
            if (chl != channLayout)
            {
                LOG_INFO("%s, force new chann: %d->%d", __FUNCTION__, channLayout.Chann(), chl.Chann());
            }

            channLayoutEnc.Set(chl);
        }
        else
        {
            LOG_WARNING("%s, failed to get chann.layout", __FUNCTION__);
            channLayoutEnc.Set(channLayout);
        }
    }

    int Muxer::AddStreamAudio(
        const char* encoderName, 
        const ChannLayout& channLayout, int sampleRate, AVSampleFormat sampFmt, const char* options,
        ChannLayout& channLayoutAct, AVSampleFormat& actSampFmt, int& frameSize)
    {
        int ret = -1;

        if (!AssertFormat(__FUNCTION__))
        {
            return ErrorNoFormatCtx;
        }
        if (!AssertAudioParams(channLayout.Chann(), sampleRate, __FUNCTION__))
        {
            return ErrorBadArgs;
        }

        LOG_INFO("%s, try: enc=%s, chann=%d, samp.rate=%d, samp.fmt=%s, ch.mask=%lld",
            __FUNCTION__, 
            encoderName, 
            channLayout.Chann(), sampleRate, SampFmtToStr(sampFmt), channLayout.Mask());

        if (const AVCodec* encoder = GetEncoderByName(encoderName))
        {
            ChannLayout channLayoutEnc;
            FindFrameParamsAudio(encoder, channLayout, channLayoutEnc);
            AVSampleFormat sampFmtEnc = FindFrameParamsAudio(encoder, sampFmt);

            ret = SetupOutputStreamAudio(encoder, channLayoutEnc, sampleRate, sampFmtEnc, options, frameSize);
            if (ret >= 0)
            {
                channLayoutAct.Set(channLayoutEnc);
                actSampFmt = sampFmtEnc;
            }
        }

        return ret;
    }

// packet translocation, without decoding-encoding

    int  Muxer::AddOutputStream(StreamReader* sreamReader)
    {
        if (!AssertFormat(__FUNCTION__))
            return ErrorNoFormatCtx;

        LOG_INFO("%s, try packet translocation: cod.id=%s", __FUNCTION__, sreamReader->CodecIdStr());

        int ret = SetupOutputStream(&sreamReader->Stream());
        if (ret >= 0)
        {
            m_Streams.Back().SetStreamReader(sreamReader);

            auto mediaType = sreamReader->MediaType();
            if (mediaType == AVMEDIA_TYPE_VIDEO)
            {
                m_IndexVideo = ret;
            }
            else if (mediaType == AVMEDIA_TYPE_AUDIO)
            {
                m_IndexAudio = ret;
            }
        }
 
        return ret;
    }

// ---------------------------------------------------------------------

    AVRational Muxer::GetTimebase(int ind) const
    {
        return  m_Streams.IndexIsValid(ind)
            ? m_Streams[ind].Timebase()
            : AVRational{ 0, 1 };
    }

    double Muxer::Position(int strmIdx) const
    {
        return  m_Streams.IndexIsValid(strmIdx)
            ? m_Streams[strmIdx].Position()
            : 0.0;
    }

    double Muxer::Position() const
    {
        return (m_IndexVideo >= 0)
            ? Position(m_IndexVideo)
            : ((m_IndexAudio >= 0) ? Position(m_IndexAudio) : 0.0);
    }

    int Muxer::AttachFrameHub(FrameHub* frameHub)
    {
        if (!AssertStreams(__FUNCTION__))
            return ErrorNoStreams;
        if (!AssertArgs(frameHub, __FUNCTION__))
            return ErrorBadArgs;

        int ret = m_Streams.Count() - 1;
        m_Streams[ret].SetFrameHub(frameHub);
        return ret;
    }

    bool Muxer::EndOfData() const
    {
        return m_Streams.EndOfData();
    }

    bool Muxer::CreatePump()
    {
        bool ret = m_Streams.CreatePump();
        if (!ret)
        {
            LOG_ERROR("%s, failed create pump", __FUNCTION__);
        }
        return ret;
    }

    bool Muxer::PumpError() const
    {
        return m_Streams.PumpError();
    }

    void Muxer::Run()
    {
        m_Streams.Run(); 
        LOG_INFO(__FUNCTION__);
    }

    void Muxer::Pause()
    {
        m_Streams.Pause(); 
    }

    void Muxer::Stop(bool skipPause)
    {
        m_Streams.Stop(skipPause);
        LOG_INFO("%s, stopped", __FUNCTION__);
    }

    void Muxer::ClosePump()
    {
        m_Streams.ClosePump();
    }

    bool Muxer::AssertStreamIndex(int strmIdx, const char* msg) const
    {
        bool ret = m_Streams.IndexIsValid(strmIdx);
        if (!ret)
        {
            LOG_ERROR("%s, bad stream index=%d", msg, strmIdx);
        }
        return ret;
    }

    bool Muxer::AssertStreams(const char* msg) const
    {
        bool ret = m_Streams.Count() > 0;
        if (!ret)
        {
            LOG_ERROR("%s, no streams", msg);
        }
        return ret;
    }

} // namespace Fcore
