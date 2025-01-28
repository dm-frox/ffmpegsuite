#include "pch.h"

#include "Demuxer.h"

#include "Logger.h"
#include "CommonConsts.h"
#include "Misc.h"
#include "PacketTools.h"
#include "StreamReader.h"
#include "DictTools.h"

//#define LOG_PACKETS_

namespace Fcore
{
    const int Demuxer::PacketQueueMaxLenVA     = 256;
    const int Demuxer::PacketQueueMaxLen       = 8;
    const int Demuxer::PacketQueueTail            = 4;
    const int Demuxer::PacketQueueMinLengthDegree = 3;

    const bool Demuxer::UseCondVar  = true;
    const char Demuxer::ThreadTag[] = "demux";
    const char Demuxer::LangEng[]   = "eng";


    Demuxer::Demuxer(bool player)
        : m_PacketQueueMaxLength(player ? PacketQueueMaxLenVA : PacketQueueMaxLen)
        , m_PacketQueueOffThreshold(m_PacketQueueMaxLength - PacketQueueTail)
        , m_PacketQueueOnThreshold(m_PacketQueueMaxLength >> PacketQueueMinLengthDegree)
    {
        InitPump(UseCondVar, ThreadTag);

        m_Packet.Alloc();

        LOG_INFO("%s, player=%d, pack queue max len=%d, thresholds=%d/%d", __FUNCTION__, 
            player, m_PacketQueueMaxLength, m_PacketQueueOffThreshold, m_PacketQueueOnThreshold);
    }

    Demuxer::~Demuxer() = default;

    bool Demuxer::AssertStreamIndex(int strmIdx, const char* msg) const
    {
        bool ret = m_Streams.IndexIsValid(strmIdx);
        if (!ret)
        {
            LOG_ERROR("%s, bad strm index=%d", msg, strmIdx);
        }
        return ret;
    }

    int Demuxer::SetupStreams()
    {
        int n = Base::FindStreamInfo();
        if (n > 0) // number of streams
        {
            m_Streams.Reserve(n);
            m_SubtitStrmCount = 0;
            for (int i = 0; i < n && n > 0; ++i)
            {
                if (AVStream* strm = GetStreamCtx(i))
                {
                    if (strm->codecpar)
                    {
                        auto sr = new StreamReader(*strm, *this);
                        if (sr->IsSubtit())
                        {
                            ++m_SubtitStrmCount;
                        }
                        m_Streams.AppendItem(sr);
                    }
                    else
                    {
                        n = ErrorNullCodecpar;
                        LOG_ERROR("%s, null codecpar,  index=%d", __FUNCTION__, i);
                    }
                }
                else
                {
                    n = ErrorNullStream;
                    LOG_ERROR("%s, null stream,  index=%d", __FUNCTION__, i);
                }
            }
        }
        return n;
    }

    const char* Demuxer::StrmIndexToStr(int strmIdx) const
    {
        const char* ret = "";
        if (0 <= strmIdx && strmIdx < m_Streams.Count())
        {
            ret = m_Streams[strmIdx].MediaTypeStr();
        }
        return ret;
    }

    int Demuxer::Open(const char* url, const char* format, const char* options)
    {
        int ret = -1;

        LOG_INFO("%s, try: url=%s", __FUNCTION__, StrToLog(url));

        if (!AssertFirst(__FUNCTION__))
            return ErrorAlreadyInitialized;

        if (!AssertArgs(url && *url, __FUNCTION__))
            return ErrorBadArgs;

        ret = Base::OpenInput(url, format, options);
        if (ret >= 0)
        {
            ret = SetupStreams();
            if (ret >= 0)
            {
                Base::SetupFormatContextData(url);
            }
        }
        else
        {
            Base::Close();
        }

        return ret;
    }

    StreamReader* Demuxer::GetStreamReader(int strmIdx)
    {
        return m_Streams.IndexIsValid(strmIdx) ? &m_Streams[strmIdx] : nullptr;
    }

    const StreamReader* Demuxer::GetStreamReader(int strmIdx) const
    {
        return m_Streams.IndexIsValid(strmIdx) ? &m_Streams[strmIdx] : nullptr;
    }

    const char* Demuxer::GetStreamMetadata(int strmIdx, int& len) const
    {
        const char* ret = nullptr;
        if (AssertStreamIndex(strmIdx, __FUNCTION__))
        {
            ret = m_Streams[strmIdx].GetMetadata(len);
        }
        else
        {
            len = 0;
        }
        return ret;
    }

    const char* Demuxer::GetStreamInfo(int strmIdx, int& len) const
    {
        const char* ret = nullptr;
        if (AssertStreamIndex(strmIdx, __FUNCTION__))
        {
            ret = m_Streams[strmIdx].GetInfo(len);
        }
        else
        {
            len = 0;
        }
        return ret;
    }

    int Demuxer::GetStreamInfo(int strmIdx, StreamInfo& strmInfo)
    {
        int ret = ErrorBadStreamIndex;
        if (AssertStreamIndex(strmIdx, __FUNCTION__))
        {
            m_Streams[strmIdx].GetStreamInfo(strmInfo);
            ret = 0;
        }
        return ret;
    }

// connection

    int Demuxer::OpenDecoder(int strmIdx, const char* decoderParams)
    {
        if (!AssertStreamIndex(strmIdx, __FUNCTION__))
            return ErrorBadStreamIndex;

        return m_Streams[strmIdx].OpenDecoder(decoderParams);;
    }

    int  Demuxer::ConnectToFrameHubVideo(int strmIdx, FrameHub* frameHub, int outWidth, int outHeight, AVPixelFormat outPixFmt)
    {
        if (!AssertStreamIndex(strmIdx, __FUNCTION__))
            return ErrorBadStreamIndex;

        return m_Streams[strmIdx].ConnectToFrameHubVideo(frameHub, outWidth, outHeight, outPixFmt);
    }

    int  Demuxer::ConnectToFrameHubAudio(int strmIdx, FrameHub* frameHub, const ChannLayout& outLayout, int outSampleRate, AVSampleFormat outSampFmt)
    {
        if (!AssertStreamIndex(strmIdx, __FUNCTION__))
            return ErrorBadStreamIndex;

        return m_Streams[strmIdx].ConnectToFrameHubAudio(frameHub, outLayout, outSampleRate, outSampFmt);
    }

    int  Demuxer::ConnectToFilterGraphVideo(int strmIdx, FrameHub* frameHub)
    {
        if (!AssertStreamIndex(strmIdx, __FUNCTION__))
            return ErrorBadStreamIndex;

        return m_Streams[strmIdx].ConnectToFilterGraphVideo(frameHub);;
    }

    int  Demuxer::ConnectToFilterGraphAudio(int strmIdx, FrameHub* frameHub)
    {
        if (!AssertStreamIndex(strmIdx, __FUNCTION__))
            return ErrorBadStreamIndex;

        return m_Streams[strmIdx].ConnectToFilterGraphAudio(frameHub);
    }

    int  Demuxer::ConnectToMuxerVideo(int strmIdx, Muxer* muxer)
    {
        if (!AssertStreamIndex(strmIdx, __FUNCTION__))
            return ErrorBadStreamIndex;

        return m_Streams[strmIdx].ConnectToMuxerVideo(muxer);
    }

    int  Demuxer::ConnectToMuxerAudio(int strmIdx, Muxer* muxer)
    {
        if (!AssertStreamIndex(strmIdx, __FUNCTION__))
            return ErrorBadStreamIndex;

       return m_Streams[strmIdx].ConnectToMuxerAudio(muxer);
    }

    const Decoder& Demuxer::GetDecoder(int strmIdx) const
    {
        return m_Streams[strmIdx].Decoder();
    }

// proceed

    int Demuxer::Proceed()
    {
        int ret = -1;

        if (!m_Packet)
        {
            return ErrorBadState;
        }
 
        if (m_InputCanceled)
        {
            LOG_INFO("%s, input canceled", __FUNCTION__);
            PushTerminator();
            return EndOfDataMarker;
        }
        
        for (bool repeat = true; repeat;)
        {
            repeat = false;
            int res = Base::ReadPacket(m_Packet);
            if (res >= 0)
            {
                int strmIdx = m_Packet.StreamIndex();
                if (m_Streams.IndexIsValid(strmIdx))
                {
                    LogPacket(m_Packet);
                    StreamReader& strm = m_Streams[strmIdx];
                    if (strm.IsActivated())
                    {
                        if (AVPacket* pkt = m_Packet.MoveToNewPacket())
                        {
                            strm.PushPacketToQueue(pkt);
                            ret = 0;
                        }
                        else
                        {
                            ret = AVERROR(ENOMEM);
                            m_Packet.FreeBuffer();
                            LOG_ERROR("%s, failed to allocate packet", __FUNCTION__);
                        }
                    }
                    else // skip inactive stream
                    {
                        m_Packet.FreeBuffer();
                        repeat = true;
                    }
                }
                else // ignore bad stream index
                {
                    LOG_WARNING("%s, bad stream index=%d", __FUNCTION__, strmIdx);
                    m_Packet.FreeBuffer();
                    repeat = true;
                }
            }
            else if (res == AVERROR_EOF) // end of data
            {
                LOG_INFO("%s, end of source detected", __FUNCTION__);
                PushTerminator();
                ret = EndOfDataMarker;
            }
            else // error
            {
                ret = res;
            }

        } // for (bool repeat = true; repeat;)

        return ret;
    }

    int Demuxer::FindSubtitStreamByLang(const char* lang) const
    {
        int ret = -1;
        if (m_SubtitStrmCount == 0)
        {
            return ret;
        }

        int ind = -1, indFirst = -1, indDef = -1, indEng = -1;
        for (int i = 0, n = m_Streams.Count(); i < n && ind < 0; ++i)
        {
            const StreamReader& strm = m_Streams[i];
            if (strm.IsSubtit())
            {
                if (indFirst < 0)
                {
                    indFirst = i;
                }
                const char* sl = strm.Language();
                if (StrAreEq(sl, lang)) // OK, found
                {
                    ind = i;
                }
                else if (indDef < 0 && strm.IsDefault())
                {
                    indDef = i;
                }
                else if (indEng < 0 && StrAreEq(sl, LangEng))
                {
                    indEng = i;
                }
            }
        }

        if (ind < 0 && indFirst >= 0)
        {
            if (indDef >= 0)
            {
                ind = indDef;
            }
            else if (indEng >= 0)
            {
                ind = indEng;
            }
            else if (indFirst >= 0)
            {
                ind = indFirst;
            }
        }

        if (ind >= 0)
        {
            ret = ind;
        }

        return ret;
    }

// ---------------------------------------------------------------------
// logging

#pragma warning(disable:4100)
    void Demuxer::LogPacket(const AVPacket* pkt, int ind)
    {
#ifdef LOG_PACKETS_
        if (ind < 0 || pkt->stream_index == ind)
        {
            LOG_INFO("***** %5d, ind=%2d, size=%d", m_Cntr++, pkt->stream_index, pkt->size);
        }
#endif
#pragma warning(default:4100)
    }

} // namespace Fcore

