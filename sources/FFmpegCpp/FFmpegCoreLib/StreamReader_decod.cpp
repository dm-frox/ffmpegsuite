#include "pch.h"

#include "StreamReader.h"

#include <exception>

#include "Demuxer.h"
#include "Logger.h"
#include "Misc.h"
#include "DictTools.h"
#include "PacketTools.h"
#include "FrameHub.h"
#include "CommonConsts.h"
#include "FrameProcIface.h"
#include "FrameTools.h"


//#define LOG_FRAME_PTS_VIDEO_
//#define LOG_FRAME_PTS_AUDIO_

//#define LOG_PACKET_PTS_VIDEO_
//#define LOG_PACKET_PTS_AUDIO_


namespace Fcore
{

    void StreamReader::PushPacketToQueue(AVPacket* pack)
    { 
        int qlen = m_PacketQueue.Push(pack);
        if (qlen >= 0)
        {
            if (pack == nullptr)
            {
                LOG_INFO("%s, %s, null packet", __FUNCTION__, MediaTypeStr());
            }
        }
        else
        {
            LOG_ERROR("%s, %s, packet queue is full", __FUNCTION__, MediaTypeStr());
            throw std::runtime_error("StreamReader::PushToPacketQueue, packet queue is full");
        }
    }

    void StreamReader::InitFrameTime()
    {
        if (IsVideo() || IsAudio())
        {
            if (IsVideo())
            {
                m_FrameTime.InitVideo(m_Framerate, Timebase());
            }
            else if (IsAudio())
            {
                m_FrameTime.InitAudio(AudioSampleRate(), Timebase());
            }
            if (!m_FrameTime.IsReady())
            {
                LOG_WARNING("%s, %s, failed to init frame time", __FUNCTION__, MediaTypeStr());
            }
        }
    }

    void StreamReader::SetFramePts(AVFrame* frame)
    {
        int64_t befPts = frame->best_effort_timestamp;
        int64_t iniPts = frame->pts;
        int64_t cntPts = 0;
        int64_t pts    = 0;
        if (befPts != AV_NOPTS_VALUE)
        {
            pts = befPts - StartTime();
        }
        else if (iniPts != AV_NOPTS_VALUE)
        {
            pts = iniPts - StartTime();
        }
        else if (m_FrameTime.IsReady())
        {
            pts = cntPts = m_FrameTime.GetFrameTimestamp(frame);
        }
        frame->pts = pts;

        LogFrame(iniPts, befPts, cntPts, pts,
            FrameTools::HasFlag(frame, AV_FRAME_FLAG_KEY));
    }

    int StreamReader::OpenDecoder(const char* decoderParams)
    {
        int ret = -1;

        if (!AssertFirst(m_Decoder.IsOpened(), __FUNCTION__))
            return ErrorAlreadyInitialized;

        int rr = m_Decoder.OpenDecoder(decoderParams);
        if (rr >= 0)
        {
            if (m_PacketQueue.AllocBuffer())
            {
                ret = rr;
            }
            else
            {
                ret = AVERROR(ENOMEM);
                LOG_ERROR("%s, %s, failed allocate queue", __FUNCTION__, MediaTypeStr());
            }
        }

        return ret;
    }

    int StreamReader::OpenDecoderNoHub()
    {
        int ret = OpenDecoder(nullptr);
        if (ret >= 0)
        {
            SetActivated();
        }
        return ret;
    }

    int StreamReader::SendFrame(AVFrame* frame)
    {
        return m_FrameHub ? m_FrameHub->AddFrame(frame, m_FilterInputIndex) : -1;
    }

    int StreamReader::DecodePacket(AVPacket* pkt)
    {
        return m_Decoder.DecodePacket(pkt, static_cast<ISinkFrameDec*>(this));
    }

    int StreamReader::DecodeAndFreePacket(AVPacket* pkt)
    {
        LogPacket(pkt);
        int ret = DecodePacket(pkt);
        if ((ret >= 0) && (pkt == nullptr)) // end of data
        {
            LOG_INFO("%s, %s, end of stream detected", __FUNCTION__, MediaTypeStr());

            SendFrame(nullptr);
            ret = EndOfDataMarker;
        }
        PacketTools::Free(pkt);
        return ret;
    }

    int StreamReader::ForwardFrame(AVFrame* frame) // ISinkFrameDec impl
    {
        SetFramePts(frame);
        int ret = -1;
        if (!m_FrameProc)
        {
            ret = SendFrame(frame);
        }
        else
        {
            m_FrameProc->ProcFrame(frame);
            FrameTools::FreeFrameBuffer(frame);
            ret = 0;
        }
        return ret;
    }

    AVPacket* StreamReader::PopPacket()
    {
        AVPacket* ret = nullptr;
        if (auto rr = m_PacketQueue.Pop())
        {
            ret = rr.Ptr;
        }
        return ret;
    }

    int StreamReader::DecodeNextPacket()
    {
        int ret = -1;
        if (auto rr = m_PacketQueue.Pop())
        {
            ret = DecodeAndFreePacket(rr.Ptr); // , false);
        }
        else // no packets in queue
        {
            ret = NoPacketsInterval;
        }
        return ret;
    }

    int StreamReader::Proceed()
    {
        int ret = -1;
        if (!m_Decoder.IsOpened())
        {
            LOG_ERROR("%s, %s, no codec", __FUNCTION__, MediaTypeStr());
            return ret;
        }

        return DecodeNextPacket();
    }

    void StreamReader::SetReadPacketPump(Ftool::IPumpSwitchR* readPump) // for transcoder only
    {
        m_PacketQueue.EnableReadPumpCtrl(readPump);
    }

// ---------------------------------------------------------------------
// logging

    void StreamReader::LogPacket_(const AVPacket* pkt)
    {
        if (pkt)
        {
            LOG_TEMP("%s ~~~~~, %5d, packet: pts/dts=%5d/%5d, size=%d",
                MediaTypeTag(), m_CntrPkt, TsToInt(pkt->pts), TsToInt(pkt->dts), pkt->size);
        }
        else
        {
            LOG_TEMP("%s ~~~~~,, %5d, packet null", MediaTypeTag(), m_CntrPkt);
        }
        ++m_CntrPkt;
    }

#pragma warning(disable:4100)
    void StreamReader::LogPacket(const AVPacket* pkt)
    {
#ifdef LOG_PACKET_PTS_VIDEO_
        if (IsVideo())
        {
            LogPacket_(pkt);
        }
#endif
#ifdef LOG_PACKET_PTS_AUDIO_
        if (IsAudio())
        {
            LogPacket_(pkt);
        }
#endif
#pragma warning(default:4100)
    }

    void StreamReader::LogFrame_(int64_t iniPts, int64_t befPts, int64_t cntPts, int64_t pts, bool key)
    {
        LOG_TEMP("%s +++++, %5d, frame: ini/bef/cnt/res=%5d/%5d/%5d/%5d%s",
            MediaTypeTag(), m_CntrFrm++, TsToInt(iniPts), TsToInt(befPts), TsToInt(cntPts), TsToInt(pts), key ? ", key frame" : "");
    }

#pragma warning(disable:4100)
    void StreamReader::LogFrame(int64_t iniPts, int64_t befPts, int64_t cntPts, int64_t pts, int key)
    {
#ifdef LOG_FRAME_PTS_VIDEO_
        if (IsVideo())
        {
            LogFrame_(iniPts, befPts, cntPts, pts, key);
        }
#endif
#ifdef LOG_FRAME_PTS_AUDIO_
        if (IsAudio())
        {
            LogFrame_(iniPts, befPts, cntPts, pts, false);
        }
#endif
#pragma warning(default:4100)
    }

} // namespace Fcore
