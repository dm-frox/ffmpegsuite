#include "pch.h"

#include "FrameSource.h"

#include "Logger.h"
#include "Misc.h"
#include "FilterGraph.h"
#include "FilterTools.h"
#include "Transcoder.h"
#include "CodecTools.h"
#include "ChannLayout.h"

using namespace Fcore;

namespace Fapp
{

    const char FrameSource::SubtitFltPrefix[] = "subtitles=default";

    FrameSource::FrameSource(bool useFilterGraphAlways, bool convFrameSequentially, bool player)
        : m_UseFilterGraphAlways(useFilterGraphAlways)
        , m_ConvFrameSeq(convFrameSequentially)
        , m_Demuxer(player)
        , m_FrameHubVideo(AVMEDIA_TYPE_VIDEO, m_ConvFrameSeq)
        , m_FrameHubAudio(AVMEDIA_TYPE_AUDIO, true)
    {
        LOG_INFO("%s, useFilterGraph=%d, convFrameSeq=%d",
            __FUNCTION__,
            m_UseFilterGraphAlways, m_ConvFrameSeq);
    }

    FrameSource::FrameSource()
        : FrameSource(false, false, false)
    {}

    FrameSource::~FrameSource() = default;

    int FrameSource::OpenDemuxer(const char* url, const char* fmt, const char* fmtOps, bool useVideo, bool useAudio)
    {
        return m_Demuxer.Open(url, fmt, fmtOps, useVideo, useAudio);
    }

    void FrameSource::SetRendPumpAudio_()
    {
        m_FrameHubAudio.SetRendPump(m_Demuxer.GetAudioPump());
    }

// --------------------------------------------------------------------
// video

    void FrameSource::SetPixFmts(const char* pixFmts)
    {
        m_PixFmts.clear();
        m_PixFmts.reserve(4);

        if (pixFmts && *pixFmts)
        {
            MultiStringIterator itr(pixFmts);
            for (const char* pfs = itr.Curr(); *pfs; pfs = itr.Next())
            {
                m_PixFmts.push_back(PixFmtFromStr(pfs));
            }
        }

        m_PixFmts.push_back(AV_PIX_FMT_NONE);
    }

    void FrameSource::SetPixFmt(AVPixelFormat pixFmtDec)
    {
        AVPixelFormat pf = CodecTools::FindPixelFormat(m_PixFmts.data(), pixFmtDec);
        m_PixFmt = (pf != AV_PIX_FMT_NONE) ? pf : pixFmtDec;
        m_PixFmts.clear();
    }

    std::string FrameSource::PixFmtsStr() const
    {
        std::string ret;
        ret.reserve(64);
        for (auto pf : m_PixFmts)
        {
            if (pf != AV_PIX_FMT_NONE)
            {
                ret += PixFmtToStr(pf);
                ret += ' ';
            }
            else if (!ret.empty())
            {
                ret.erase(ret.length() - 1, 1);
                break;
            }
        }
        return ret;
    }

    void FrameSource::SetFrameParamsVideo(int width, int height, const char* pixFmts, const char* vidFilter, int fpsFactor)
    {
        m_Width  = (width > 0)  ? width : 0;
        m_Height = (height > 0) ? height : 0;
        SetPixFmts(pixFmts);
        m_VidFilter = (vidFilter && *vidFilter) ? vidFilter : "";
        m_FrameHubVideo.SetFpsFactor(fpsFactor);
        m_FrameRate.num = (fpsFactor > 0) ? fpsFactor : 0;

        LOG_INFO("%s, frame=%dx%d, pix.fmts=%s, filt.str=%s, fps fact=%d",
            __FUNCTION__,
            m_Width, m_Height, PixFmtsStr().c_str(), StrToLog(m_VidFilter.c_str()), fpsFactor);
    }

    void FrameSource::SetupFrameParamsVideo()
    {
        const Decoder& dec = m_Demuxer.DecoderVideo();
        if (m_Width == 0)
        {
            int width = dec.Width();
            m_Width = m_Demuxer.GetWidthSar(width);
            if (m_Width != width)
            {
                LOG_INFO("%s, sar width correction: %d->%d", __FUNCTION__, width, m_Width);
            }
        }

        if (m_Height == 0)
        {
            m_Height = dec.Height();
        }
 
        SetPixFmt(dec.PixelFormat());

        if (m_FrameRate.num == 0)
        {
            m_FrameRate = AVRational{ MediaSourceInfo()->Fps_N(), MediaSourceInfo()->Fps_D() };
        }

        m_FrameHubVideo.SetTimebase(m_Demuxer.TimebaseVideo());

    }

    int FrameSource::SetupFrameParamsVideoEx(Transcoder* trans)
    {
        int ret = 0;
        SetupFrameParamsVideo();
        if (trans)
        {
            AVPixelFormat pixFmt = AV_PIX_FMT_NONE;
            ret = trans->SetupOutputStreamVideo(pixFmt);
            if (ret >= 0)
            {
                if (pixFmt != m_PixFmt)
                {
                    m_PixFmt = pixFmt;
                }
            }
        }

        if (ret >= 0)
        {
            LOG_INFO("%s, %sframe=%dx%d, pix.fmt=%s, fr.rate=%d/%d",
                __FUNCTION__,
                trans ? "trans, " : "",
                m_Width, m_Height, PixFmtToStr(m_PixFmt), m_FrameRate.num, m_FrameRate.den);
        }

        return ret;
    }

    int FrameSource::SetupFilterVideo()
    {
        LOG_INFO("%s, begin...", __FUNCTION__);
        int ret = m_FrameHubVideo.CreateFilterGraph();
        if (ret >= 0)
        {
            ret = m_Demuxer.ConnectToFilterGraphVideo(&m_FrameHubVideo);
            if (ret >= 0)
            {
                ret = m_FrameHubVideo.AddFilterGraphOutputVideo(m_PixFmt);
                if (ret >= 0)
                {
                    const Decoder& dec = m_Demuxer.DecoderVideo();
                    if (m_Width != dec.Width() || m_Height != dec.Height())
                    {
                        m_VidFilter = FilterTools::GetFilterStringVideo(m_Width, m_Height, m_VidFilter);
                    }
                    ret = m_FrameHubVideo.ConfigureFilterGraph(m_VidFilter.c_str());
                    if (ret >= 0)
                    {
                        //UpdateTimebaseVideo();
                        LOG_INFO("%s, filt.str=%s", __FUNCTION__, StrToLog(m_VidFilter.c_str()));
                    }
                }
            }
        }
        return ret;
    }

    int FrameSource::ConnectToFrameHubVideo(const char* decoderParams, Transcoder* trans)
    {
        int ret = m_Demuxer.OpenDecoderVideo(decoderParams);
        if (ret >= 0)
        {
            ret = SetupFrameParamsVideoEx(trans);
            if (ret >= 0)
            {
                bool flt = m_UseFilterGraphAlways || !m_VidFilter.empty();
                ret = !flt
                    ? m_Demuxer.ConnectToFrameHubVideo(&m_FrameHubVideo, m_Width, m_Height, m_PixFmt)
                    : SetupFilterVideo();
            }
        }
        return ret;
    }

// --------------------------------------------------------------------
// audio

    void FrameSource::SetRendModeAudio(int maxChann, int reduceSampleFormat)
    {
        m_MaxChannAudio = maxChann;
        m_ReduceSampleFormat = reduceSampleFormat;

        LOG_INFO("%s, max chann=%d, reduce samp.fmt=%d", __FUNCTION__, m_MaxChannAudio, m_ReduceSampleFormat);
    }

    void FrameSource::SetFrameParamsAudio(int chann, int sampleRate, const char* sampFmt, const char* channLayout, const char* audFilter)
    {
        m_SampleRate = (sampleRate > 0) ? sampleRate : 0;
        m_SampFmt    = (sampFmt && *sampFmt) ? SampFmtFromStr(sampFmt) : AV_SAMPLE_FMT_NONE;
        m_ChannLayout.Clear();
        if (channLayout && *channLayout)
        {
            m_ChannLayout.SetFromString(channLayout);
        }
        if (m_ChannLayout.IsNull() && (chann > 0))
        {
            m_ChannLayout.SetFromChann(chann);
        }
        m_FrameSize = 0;
        m_AudFilter = (audFilter && *audFilter) ? audFilter : "";

        LOG_INFO("%s, chann=%d, samp.rate=%d, samp.fmt=%s, ch.mask=%lld, filt.str=%s",
            __FUNCTION__,
            m_ChannLayout.Chann(), m_SampleRate, SampFmtToStr(m_SampFmt), m_ChannLayout.Mask(), StrToLog(m_AudFilter.c_str()));
    }

    void FrameSource::SetupFrameParamsAudio()
    {
        const Decoder& dec = m_Demuxer.DecoderAudio();

        if (m_SampleRate == 0)
        {
            m_SampleRate = dec.SampleRate();
        }

        if (m_SampFmt == AV_SAMPLE_FMT_NONE)
        {
            m_SampFmt = dec.SampleFormat();
        }
        m_SampFmt = ReduceSampFmt(m_SampFmt);

        if (m_ChannLayout.IsNull())
        {
            m_ChannLayout.Set(dec.ChannLayout());
        }
        int chann = m_ChannLayout.Chann(), channRed = 0;
        if (ReduceChann(chann, channRed))
        {
            m_ChannLayout.SetFromChann(channRed);
            LOG_INFO("%s, reduce chann: %d->%d", __FUNCTION__, chann, m_ChannLayout.Chann());
        }

        m_FrameSize = 0;

        m_FrameHubAudio.SetTimebase(m_Demuxer.TimebaseAudio());
    }

    int FrameSource::SetupFrameParamsAudioEx(Transcoder* trans)
    {
        int ret = 0;
        SetupFrameParamsAudio();
        if (trans)
        {
            AVSampleFormat sampFmt = AV_SAMPLE_FMT_NONE;
            Fcore::ChannLayout channLayout;
            int frameSize = 0;
            ret = trans->SetupOutputStreamAudio(channLayout, sampFmt, frameSize);
            if (ret >= 0)
            {
                if (channLayout != m_ChannLayout)
                {
                    m_ChannLayout.Set(channLayout);
                }
                if (sampFmt != m_SampFmt)
                {
                    m_SampFmt = sampFmt;
                }
                if (frameSize > 0)
                {
                    m_FrameSize = frameSize;
                }
            }
        }

        if (ret >= 0)
        {
            LOG_INFO("%s, %schann=%d, samp.rate=%d, samp.fmt=%s, ch.mask=%lld, fr.size=%d",
                __FUNCTION__,
                trans ? "trans, " : "",
                m_ChannLayout.Chann(), m_SampleRate, SampFmtToStr(m_SampFmt), m_ChannLayout.Mask(), m_FrameSize);
        }

        return ret;
    }

    int FrameSource::SetupFilterAudio()
    {
        LOG_INFO("%s, begin...", __FUNCTION__);
        int ret = m_FrameHubAudio.CreateFilterGraph();
        if (ret >= 0)
        {
            ret = m_Demuxer.ConnectToFilterGraphAudio(&m_FrameHubAudio);
            if (ret >= 0)
            {
                ret = m_FrameHubAudio.AddFilterGraphOutputAudio(m_SampFmt);
                if (ret >= 0)
                {
                    const Decoder& dec = m_Demuxer.DecoderAudio();
                    int sampleRate = (m_SampleRate != dec.SampleRate()) ? m_SampleRate : 0;
                    const Fcore::ChannLayout& channLayout = (m_ChannLayout != dec.ChannLayout())
                        ? m_ChannLayout 
                        : ChannLayout::GetNull();
                    if (sampleRate > 0 || !channLayout.IsNull() || m_FrameSize > 0)
                    {
                        m_AudFilter = FilterTools::GetFilterStringAudio(sampleRate, channLayout, m_AudFilter, m_FrameSize);
                    }

                    ret = m_FrameHubAudio.ConfigureFilterGraph(m_AudFilter.c_str());
                    if (ret >= 0)
                    {
                        LOG_INFO("%s, filt.str=%s", __FUNCTION__, StrToLog(m_AudFilter.c_str()));
                    }
                }
            }
        }
        return ret;
    }

    int FrameSource::ConnectToFrameHubAudio(const char* decoderParams, Transcoder* trans)
    {
        int ret = m_Demuxer.OpenDecoderAudio(decoderParams);
        if (ret >= 0)
        {
            ret = SetupFrameParamsAudioEx(trans);
            if (ret >= 0)
            {
                bool flt = m_UseFilterGraphAlways || !m_AudFilter.empty() ||
                           trans || (m_SampleRate != MediaSourceInfo()->SampleRate());

                ret = !flt
                    ? m_Demuxer.ConnectToFrameHubAudio(&m_FrameHubAudio, m_ChannLayout, 0, m_SampFmt)
                    : SetupFilterAudio();
            }
        }
        return ret;
    }

    bool FrameSource::ReduceChann(int chann, int& chanRed) const
    {
        bool ret = false;
        if (m_MaxChannAudio > 0)
        {
            if (chann > m_MaxChannAudio)
            {
                ret = true;
                chanRed = m_MaxChannAudio;
            }
        }
        if (!ret)
        {
            chanRed = chann;
        }
        return ret;
    }

    AVSampleFormat FrameSource::ReduceSampFmt(AVSampleFormat sampFmt) const
    {
        if (m_ReduceSampleFormat)
        {
            switch (sampFmt)
            {
            case AV_SAMPLE_FMT_U8:
            case AV_SAMPLE_FMT_U8P:
            case AV_SAMPLE_FMT_S16:
            case AV_SAMPLE_FMT_S16P:
                return AV_SAMPLE_FMT_S16;
            default:
                return AV_SAMPLE_FMT_FLT;
            }
        }
        else
        {
            return sampFmt;
        }
    }

// ---------------------------------------------------------------------------------------

    int FrameSource::Build(
        const char* url, const char* fmt, const char* fmtOps,
        bool useVideo, const char* decPrmsVideo, bool useAudio, const char* decPrmsAudio)
    {
        int ret = m_Demuxer.Open(url, fmt, fmtOps, useVideo, useAudio);
        if (ret >= 0)
        {
            if (MediaSourceInfo()->HasVideo())
            {
                ret = ConnectToFrameHubVideo(decPrmsVideo);
            }
            if (ret >= 0 && MediaSourceInfo()->HasAudio())
            {
                ret = ConnectToFrameHubAudio(decPrmsAudio);
            }
        }

        return ret;
    }

    void FrameSource::Reset()
    {
        LOG_INFO("%s, begin...", __FUNCTION__);
        m_FrameHubVideo.Reset();
        m_FrameHubAudio.Reset();
        m_Demuxer.Reset();
        LOG_INFO("%s, end", __FUNCTION__);
    }

    void FrameSource::Stop(bool skipPause)
    {
        m_Demuxer.Stop(skipPause);
        Reset();
    }

    int FrameSource::SetPosition(bool skipPause, double pos)
    {
        Stop(skipPause);

        return m_Demuxer.SetPosition(pos);
    }

} // namespace Fapp
