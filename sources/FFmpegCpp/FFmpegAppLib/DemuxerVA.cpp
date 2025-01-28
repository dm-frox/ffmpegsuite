#include "pch.h"

#include "DemuxerVA.h"

#include "Logger.h"
#include "Misc.h"
#include "StreamInfo.h"

using namespace Fcore;

namespace Fapp
{
    const char* DemuxerVA::VideoExts[] = 
    { ".avi", ".mp4", ".mkv", ".wmv", ".asf", ".mov", ".flv", ".3gp", ".mpg", 
      ".divx", ".webm", ".m4v", ".ogv", ".swf", ".vob", ".m2ts", ".ts", ".rm", nullptr };
    
    const char* DemuxerVA::AudioExts[] = 
    { ".wav", ".mp3", ".wma", ".ogg", ".opus", ".m4a", ".aif", ".aac", ".mp2", ".ac3", ".amr", ".flac", nullptr };


    const unsigned int DemuxerVA::WidthMask = 0xFF'FF'FF'FC; // makes width multiple 4


    DemuxerVA::DemuxerVA(bool player)
        : m_Demuxer(player)
    {}
 
    DemuxerVA::~DemuxerVA() = default;

    void DemuxerVA::UpdateVideoCodecParams()
    {
        if (Fcore::StreamReader* strm = m_Demuxer.GetStreamReader(m_StrmIndexVideo))
        {
            const Fcore::Decoder& dec = strm->Decoder();
            m_PixelFormat = dec.PixelFormat();
            m_DecoderVideo = dec.Name();
            LOG_INFO("%s, pix.fmt=%s, decoder=%s", __FUNCTION__, Fcore::PixFmtToStr(m_PixelFormat), m_DecoderVideo.c_str());
        }
    }

    Ftool::ProceedPump* DemuxerVA::GetAudioPump()
    {
        Ftool::ProceedPump* ret = nullptr;
        if (Fcore::StreamReader* strm = m_Demuxer.GetStreamReader(m_StrmIndexAudio))
        {
            ret = strm->GetPump();
        }
        return ret;
    }

    void DemuxerVA::UpdateAudioCodecParams()
    {
        if (Fcore::StreamReader* strm = m_Demuxer.GetStreamReader(m_StrmIndexAudio))
        {
            const Fcore::Decoder& dec = strm->Decoder();
            m_SampleFormat = dec.SampleFormat();
            m_DecoderAudio = dec.Name();
            LOG_INFO("%s, samp.fmt=%s, decoder=%s", __FUNCTION__, Fcore::SampFmtToStr(m_SampleFormat), m_DecoderAudio.c_str());
        }
    }

    AVRational DemuxerVA::TimebaseVideo() const
    {
        if (auto sr = m_Demuxer.GetStreamReader(m_StrmIndexVideo))
        {
            return sr->Timebase();
        }
        else
        {
            return { 0, 1 };
        }
    }

    AVRational DemuxerVA::TimebaseAudio() const
    {
        if (auto sr = m_Demuxer.GetStreamReader(m_StrmIndexAudio))
        {
            return sr->Timebase();
        }
        else
        {
            return { 0, 1 };
        }
    }

    DemuxerVA::IndPair DemuxerVA::GetActiveStreams(bool useVideo, bool useAudio) const
    {
        int indVideo = -1, indAudio = -1;
        for (int i = 0, n = m_Demuxer.StreamCount(); i < n; ++i)
        {
            if (const StreamReader* sr = m_Demuxer.GetStreamReader(i))
            {
                if (useVideo && sr->IsVideo())
                {
                    if (indVideo < 0 || sr->IsDefault())
                    {
                        indVideo = i;
                    }
                }
                if (useAudio && sr->IsAudio())
                {
                    if (indAudio < 0 || sr->IsDefault())
                    {
                        indAudio = i;
                    }
                }
            }
        }
        return { indVideo, indAudio };
    }

    int DemuxerVA::Open(const char* url, const char* fmt, const char* fmtOps, bool useVideo, bool useAudio)
    {
        int ret = -1;
        int rr = m_Demuxer.Open(url, fmt, fmtOps);
        if (rr >= 0)
        {
            IndPair ip = GetActiveStreams(useVideo && !CheckExtention(url, AudioExts), useAudio);
            double durVideo = SetupVideo(ip.first);
            double durAudio = SetupAudio(ip.second);

            if (HasVideo() || HasAudio())
            {
                SetupDuration(durVideo, durAudio);
                int seekInd = CanSeek() ? SetStreamToSeek(url) : -1;

                ret = 0;

                if (CanSeek())
                {
                    LOG_INFO("%s, video=%d, audio=%d, dur=%5.3lf, seek ind=%d (%s)", __FUNCTION__,
                        HasVideo(), HasAudio(), Duration(), seekInd, m_Demuxer.StrmIndexToStr(seekInd));
                }
                else
                {
                    LOG_INFO("%s, video=%d, audio=%d, cannot seek", __FUNCTION__,
                        HasVideo(), HasAudio());
                }
            }
            else
            {
                LOG_WARNING("%s, no video/audio", __FUNCTION__);
                ret = 0;
            }
        }
        else
        {
            ret = rr;
        }
        return ret;
    }

    int DemuxerVA::GetWidthSar(int width) const
    {
        return ((m_Sar.num > 0 && m_Sar.den > 0) && m_Sar.num != m_Sar.den)
            ? (width * m_Sar.num / m_Sar.den) & WidthMask
            : width;
    }

    double DemuxerVA::SetupVideo(int ind)
    {
        double dur = 0.0;
        if (ind >= 0)
        {
            Fcore::StreamInfo strmInfo;
            m_Demuxer.GetStreamInfo(ind, strmInfo);
            m_StrmIndexVideo = ind;
            m_Width = strmInfo.Width;
            m_Height = strmInfo.Height;
            m_Sar = strmInfo.Sar;
            m_Fps_N = strmInfo.Fps_N;
            m_Fps_D = strmInfo.Fps_D;
            m_Fps = strmInfo.Fps();
            dur = strmInfo.DurationS;
            m_PixelFormat = strmInfo.PixFormat;
            m_DecoderVideo = strmInfo.CodecId;
            m_BitrateVideo = strmInfo.Bitrate;

            LOG_INFO("%s, ind=%d, frame=%dx%d, pix.fmt=%s, sar=%d/%d, fr.rate=%d/%d, dur=%5.3lf, bitrate=%lld, cod.id=%s",
                __FUNCTION__, ind,
                m_Width, m_Height,
                Fcore::PixFmtToStr(m_PixelFormat),
                m_Sar.num, m_Sar.den,
                m_Fps_N, m_Fps_D,
                strmInfo.DurationS,
                m_BitrateVideo,
                m_DecoderVideo.c_str());
        }
        return dur;
    }

    double DemuxerVA::SetupAudio(int ind)
    {
        double dur = 0.0;
        if (ind >= 0)
        {
            Fcore::StreamInfo strmInfo;
            m_Demuxer.GetStreamInfo(ind, strmInfo);
            m_StrmIndexAudio = ind;
            m_Chann = strmInfo.Chann;
            m_SampleRate = strmInfo.SampRate;
            m_ChannMask = strmInfo.ChannMask;
            dur = strmInfo.DurationS;
            m_SampleFormat = strmInfo.SampFormat;
            m_DecoderAudio = strmInfo.CodecId;
            m_BitrateAudio = strmInfo.Bitrate;

            LOG_INFO("%s, ind=%d, chann=%d, samp.rate=%d, samp.fmt=%s, ch.mask=%lld, dur=%5.3lf, bitrate=%lld, cod.id=%s",
                __FUNCTION__, ind,
                m_Chann, m_SampleRate,
                Fcore::SampFmtToStr(m_SampleFormat),
                m_ChannMask,
                strmInfo.DurationS,
                m_BitrateAudio,
                m_DecoderAudio.c_str());
        }
        return dur;
    }

    void DemuxerVA::SetupDuration(double durV, double durA)
    {
        double dur = m_Demuxer.DurationFmt();
        if (dur == 0.0)
        {
            dur = Max_(durV, durA);
        }
        m_Duration = (dur > 0.0) ? dur : 0.0;
        m_DurationAudio = durA;
        LOG_INFO("%s, fmt/vid/aud=%5.3lf/%5.3lf/%5.3lf, demux=%5.3lf", __FUNCTION__, m_Demuxer.DurationFmt(), durV, durA, m_Duration);
    }

    int DemuxerVA::SetStreamToSeek(const char* file)
    {
        int idx = -1;
        if (HasVideo() && !HasAudio())
        {
            idx = m_StrmIndexVideo;
        }
        else if (!HasVideo() && HasAudio())
        {
            idx = m_StrmIndexAudio;
        }
        else
        {
            if (IsAudioFile(file))
            {
                idx = m_StrmIndexAudio;
            }
            else
            {
                idx = m_StrmIndexVideo;
            }
        }

        if (idx >= 0)
        {
            m_Demuxer.SetStreamToSeek(idx);
        }

        return idx;
    }

    // static
    bool DemuxerVA::CheckExtention(const char* file, const char* exts[])
    {
        bool ret = false;
        size_t n = std::strlen(file);
        for (int i = 0; !ret; ++i)
        {
            if (const char* ext = exts[i])
            {
                size_t ne = std::strlen(ext);
                if (n > ne)
                {
                    if (StrAreEqIgnoreCase(file + (n - ne), ext))
                    {
                        ret = true;
                    }
                }
            }
            else
            {
                break;
            }
        }
        return ret;
    }

    bool DemuxerVA::HasVideo() const { return m_StrmIndexVideo >= 0; }
    int DemuxerVA::VideoIndex() const { return m_StrmIndexVideo; }
    int DemuxerVA::Width() const { return m_Width; }
    int DemuxerVA::Height() const { return m_Height; }
    int DemuxerVA::Fps_N() const { return m_Fps_N; }
    int DemuxerVA::Fps_D() const { return m_Fps_D; }
    double DemuxerVA::Fps() const { return  m_Fps; }
    const char* DemuxerVA::PixelFormatStr() const { return Fcore::PixFmtToStr(m_PixelFormat); }
    const char* DemuxerVA::VideoCodec() const { return m_DecoderVideo.c_str(); }
    int64_t DemuxerVA::VideoBitrate() const { return m_BitrateVideo; }

    bool DemuxerVA::HasAudio() const { return m_StrmIndexAudio >= 0; }
    int DemuxerVA::AudioIndex() const { return m_StrmIndexAudio; }
    int DemuxerVA::Chann() const { return m_Chann; }
    int DemuxerVA::SampleRate() const { return m_SampleRate; }
    const char* DemuxerVA::SampleFormatStr() const { return Fcore::SampFmtToStr(m_SampleFormat); }
    int64_t DemuxerVA::ChannMask() const { return m_ChannMask; }
    const char* DemuxerVA::AudioCodec() const { return m_DecoderAudio.c_str(); }
    int64_t DemuxerVA::AudioBitrate() const { return m_BitrateAudio; }

    bool DemuxerVA::CanSeek() const { return m_Duration > 0.0; }
    double DemuxerVA::Duration() const { return m_Duration; }

} // namespace Fapp
