#include "pch.h"

#include "DemuxBase.h"

#include "ffmpeg.h"
#include "Logger.h"
#include "Misc.h"
#include "CommonConsts.h"
#include "DictTools.h"
#include "StreamBase.h"


namespace Fcore
{
    const double DemuxBase::AvTimeBase = AV_TIME_BASE; // 1'000'000


    DemuxBase::DemuxBase() = default;

    DemuxBase::~DemuxBase()
    {
        Close();
    }

    void DemuxBase::Close()
    {
        if (m_FmtCtx)
        {
            avformat_close_input(&m_FmtCtx);
            LOG_INFO(__FUNCTION__);
        }
    }

    const char* DemuxBase::GetInfo(int& len) const
    {
        len = m_Info.Length();
        return m_Info.Str();
    }

    const char* DemuxBase::GetMetadata(int& len) const
    { 
        len = m_Metadata.Length();
        return m_Metadata.Str();
    }

    const char* DemuxBase::GetChapters(int& len) const
    {
        len = m_Chapters.Length();
        return m_Chapters.Str();
    }

    AVRational DemuxBase::GuessFrameRate(AVStream& strm)
    {
        return av_guess_frame_rate(m_FmtCtx, &strm, nullptr);
    }

    // static
    int DemuxBase::FindInputFormat(const char* format, const AVInputFormat*& inputFmt)
    {
        int ret = 0;
        if (format && *format)
        {
            if (const AVInputFormat* inFmt = av_find_input_format(format))
            {
                LOG_INFO("%s, format=%s", __FUNCTION__, format);
                inputFmt = inFmt;
            }
            else
            {
                ret = AVERROR_DEMUXER_NOT_FOUND;
                LOG_ERROR("%s, failed to find input format '%s'", __FUNCTION__, format);
            }
        }
        else
        {
            inputFmt = nullptr;
            LOG_INFO("%s, no explicit format", __FUNCTION__);
        }
        return ret;
    }

    //static 
    int DemuxBase::OpenInput(const char* url, const char* format, const char* options)
    {
        int ret = -1;
        if (AVFormatContext* fmtCtx = avformat_alloc_context())
        {
            const AVInputFormat* inputFmt = nullptr;
            ret = FindInputFormat(format, inputFmt);
            if (ret >= 0)
            {
                AVDictionary* opts = MultiString::CreateDictionary(options);
                DictHolder::Log(opts, __FUNCTION__, "options");
                ret = avformat_open_input(&fmtCtx, url, inputFmt, &opts);
                if (ret >= 0)
                {
                    DictHolder::LogNotFound(opts, __FUNCTION__);
                    m_FmtCtx = fmtCtx;
                    LOG_INFO("%s", __FUNCTION__);
                }
                else
                {
                    LOG_ERROR("%s, avformat_open_input, %s", __FUNCTION__, FmtErr(ret));
                }
            }
            if (ret < 0)
            {
                avformat_free_context(fmtCtx);
            }
        }
        else
        {
            ret = AVERROR(ENOMEM);
            LOG_ERROR("%s, failed to allocate format context", __FUNCTION__);
        }
        return ret;
    }

    AVStream* DemuxBase::GetStreamCtx(int strmIdx)
    {
        // assert(m_FmtCtx);
        // assert(0 <= strmIdx && strmIdx < (int)m_FmtCtx->nb_streams);
        return m_FmtCtx->streams[strmIdx];
    }

    double DemuxBase::GetDurationFmt() const
    {
        // assert(m_FmtCtx);
        double ret = 0.0;
        auto dur = m_FmtCtx->duration;
        if (dur != AV_NOPTS_VALUE)
        {
            ret = dur / AvTimeBase;
        }
        return ret;
    }

    int DemuxBase::FindStreamInfo() // return number of streams
    {
        if (!AssertFormat(__FUNCTION__))
            return ErrorNoFormatCtx;

        int ret = 0;
        int rr = avformat_find_stream_info(m_FmtCtx, nullptr);
        if (rr >= 0)
        {
            ret = (int)m_FmtCtx->nb_streams;
            if (ret > 0)
            {
                m_DurationFmt = GetDurationFmt();
                LOG_INFO("%s, stream count=%d, duration=%5.3lf", __FUNCTION__, ret, m_DurationFmt);
            }
            else
            {
                ret = ErrorNoStreams;
                LOG_ERROR("%s, no input streams", __FUNCTION__);
            }
        }
        else
        {
            ret = rr;
            LOG_ERROR("%s, avformat_find_stream_info, %s", __FUNCTION__, FmtErr(ret));
        }

        return ret;
    }

    int DemuxBase::Flush()
    {
        if (!AssertFormat(__FUNCTION__))
            return ErrorNoFormatCtx;

        avio_flush(m_FmtCtx->pb);
        int ret = avformat_flush(m_FmtCtx);

        if (ret < 0)
        {
            LOG_ERROR("%s, avformat_flush, %s", __FUNCTION__, FmtErr(ret));
        }

        return ret;
    }

    int DemuxBase::ReadPacket(AVPacket* pkt)
    {
        if (!AssertFormat(__FUNCTION__))
            return ErrorNoFormatCtx;

        int ret = av_read_frame(m_FmtCtx, pkt);

        if (ret < 0 && ret != AVERROR_EOF)
        {
            LOG_ERROR("%s, av_read_frame, %s", __FUNCTION__, FmtErr(ret));
        }

        return ret;
    }

    void DemuxBase::SetupInfo(const char* url)
    {
        // assetr(m_FmtCtx)
        m_Info.Reset();
        m_Info.Reserve(6);

        m_Info.Add("Url", url);

        if (const AVInputFormat* ifmt = m_FmtCtx->iformat)
        {
            m_Info.Add("Format", ifmt->name);
            m_Info.Add("Format description", ifmt->long_name);
        }

        m_Info.Add("Number of streams", m_FmtCtx->nb_streams);
        m_Info.AddTime("Duration", m_DurationFmt);

        m_Info.Add("Number of chapters", m_FmtCtx->nb_chapters);

        m_Info.SetEnd();
    }

    void DemuxBase::SetupMetadata()
    {
        // assetr(m_FmtCtx)
        m_Metadata.SetDictionary(m_FmtCtx->metadata);
        int k = m_Metadata.Count();
        if (k > 0)
        {
            LOG_INFO("%s, %d items", __FUNCTION__, k);
        }
        else
        {
            LOG_INFO("%s, no metadata", __FUNCTION__);
        }
    }

    void DemuxBase::SetupChapters()
    {
        // assetr(m_FmtCtx)
        m_Chapters.Reset();
        int nchp = m_FmtCtx->nb_chapters;
        if (nchp > 0)
        {
            char chapter[32] = "Chapter ";
            int plen = (int)std::strlen(chapter);
            for (int i = 0; i < nchp; ++i)
            {
                if (AVChapter* chp = m_FmtCtx->chapters[i])
                {
                    chapter[plen] = '\0';
                    av_strlcatf(chapter, sizeof(chapter), "%d", i + 1);
                    double start = PosTsToPosSecEx(chp->start, chp->time_base),
                             end = PosTsToPosSecEx(chp->end, chp->time_base);
                    m_Chapters.AddTime(chapter, start, end);
                    m_Chapters.AddDictionary(chp->metadata);
                }
                else
                {
                    break;
                }
            }
            m_Chapters.SetEnd();
        }
    }

    void DemuxBase::SetupFormatContextData(const char* url)
    {
        if (m_FmtCtx)
        {
            SetupMetadata();
            SetupInfo(url);
            SetupChapters();
            LogFormat(__FUNCTION__);
        }
        else
        {
            LOG_ERROR("%s, null format context", __FUNCTION__);
        }
    }

    bool DemuxBase::CanSeekFmt() const
    {
        return m_DurationFmt > 0.0; // TODO
    }

    void DemuxBase::LogFormat(const char* msg) const
    {
        // assetr(m_FmtCtx)
        if (auto ifmt = m_FmtCtx->iformat)
        {
            LOG_INFO("%s, dur=%5.3lf, score=%d, format: name=%s, l.name=%s",
                msg,
                m_DurationFmt,
                m_FmtCtx->probe_score,
                ifmt->name,
                ifmt->long_name);
        }
        else
        {
            LOG_ERROR("%s, no input format", msg);
        }
    }

    int DemuxBase::SeekFrame(int strmIdx, long long tpos, int flags)
    {
        if (!AssertFormat(__FUNCTION__))
            return ErrorNoFormatCtx;

        int ret = av_seek_frame(m_FmtCtx, strmIdx, tpos, flags);

        if (ret < 0)
        {
            LOG_ERROR("%s, av_seek_frame, ind=%d, pos=%lld, flags%d, %s", __FUNCTION__, strmIdx, tpos, flags, FmtErr(ret));
        }

        return ret;
    }

} // namespace Fcore

// This structure contains the data a format has to probe a file.
//
//typedef struct AVProbeData {
//    const char* filename;
//    unsigned char* buf; /**< Buffer must have AVPROBE_PADDING_SIZE of extra allocated bytes filled with zero. */
//    int buf_size;       /**< Size of buf except extra allocated bytes */
//    const char* mime_type; /**< mime_type, when known. */
//} AVProbeData;

//#define AVPROBE_SCORE_RETRY (AVPROBE_SCORE_MAX/4)
//#define AVPROBE_SCORE_STREAM_RETRY (AVPROBE_SCORE_MAX/4-1)
//
//#define AVPROBE_SCORE_EXTENSION  50 ///< score for file extension
//#define AVPROBE_SCORE_MIME       75 ///< score for file mime type
//#define AVPROBE_SCORE_MAX       100 ///< maximum score
//
//#define AVPROBE_PADDING_SIZE 32   

