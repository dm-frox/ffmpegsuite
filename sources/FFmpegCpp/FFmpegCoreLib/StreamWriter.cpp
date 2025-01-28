#include "pch.h"

#include "StreamWriter.h"

#include "Logger.h"
#include "Misc.h"
#include "FrameHub.h"
#include "StreamReader.h"
#include "FramePoolItem.h"
#include "PacketTools.h"
#include "MuxBase.h"
#include "MetadataTools.h"


namespace Fcore
{

    const int  StreamWriter::WaitFrameInterval  = 20;
    const int  StreamWriter::WaitPacketInterval = 20;
    const bool StreamWriter::UseCondVar         = true;
    const char StreamWriter::ThreadTagSuff[]    = "enc";


    StreamWriter::StreamWriter(AVStream& strm, AVCodecContext* codecCtx, MuxBase& muxer)
        : StreamBase(strm) 
        , m_Muxer(muxer)
        , m_Encoder(codecCtx)
    {
        StreamBase::InitPump(static_cast<IProceed*>(this), UseCondVar, StreamBase::ThreadTag(ThreadTagSuff));
    }

    StreamWriter::~StreamWriter() = default;

// connection with source

    bool StreamWriter::SetFrameHub(FrameHub* frameHub)
    {
        bool ret = false;
        if (!PumpIsSet())
        {
            m_FrameHub = frameHub;
            if (UseCondVar)
            {
                m_FrameHub->FrameData()->SetReadFramePump(PumpSwitchR());
            }
            ret = true;
            LOG_INFO("%s, connected with frame hub", __FUNCTION__);
        }
        else
        {
            LOG_ERROR("%s, source already set", __FUNCTION__);
        }
        return ret;
    }

    bool StreamWriter::SetStreamReader(StreamReader* streamReader)
    {
        bool ret = false;
        if (!PumpIsSet())
        {
            m_StreamReader = streamReader;
            if (UseCondVar)
            {
                m_StreamReader->SetReadPacketPump(PumpSwitchR());
            }
            LOG_INFO("%s, %s, ind=%d, connected with stream reader", __FUNCTION__, MediaTypeStr(), Index());
        }
        else
        {
            LOG_ERROR("%s, source already set", __FUNCTION__);
        }
        return ret;
    }

    void StreamWriter::SetMetadata(const char* metadata, const char* msg)
    {
        Fcore::SetMetadata(&Stream(), metadata, msg);
    }

// timestamps

    void StreamWriter::SetFrameTimebases(AVRational srcTb, AVRational dstTb)
    {
        m_FrameTime.InitTimebases(srcTb, dstTb);
    }

    void StreamWriter::SetFrameTimebasesAudio(int sampleRate, AVRational dstTb)
    {
        AVRational srcTb{1, sampleRate };
        m_FrameTime.InitTimebases(srcTb, dstTb);
    }

    void StreamWriter::SetPacketTimebaseSrc(AVRational tb)
    {
        m_PacketTime.InitSrc(tb);
    }

    void StreamWriter::SetFrameTimestamp(AVFrame* frame)
    {
        // assert(frame)
        frame->pts = IsVideo()
            ? m_FrameTime.RecalcFrameTimestamp(frame->pts)
            : m_FrameTime.GetFrameTimestamp(frame);
        frame->duration = 0;
    }

    void StreamWriter::SetupTimebases()
    {
        AVRational strmTb = Stream().time_base;
        m_CurrPos = 0.0;

        if (!m_PacketTime.SrcIsSet()) // transcoding
        {
            m_PacketTime.InitSrc(m_Encoder.Timebase());
            m_PacketTime.InitDst(strmTb);

            LOG_INFO("%s, %s, timebases: src=%d/%d, codec=%d/%d, dst=%d/%d",
                __FUNCTION__,
                MediaTypeStr(),
                m_FrameTime.SrcTb_N(), m_FrameTime.SrcTb_D(),
                m_FrameTime.DstTb_N(), m_FrameTime.DstTb_D(),
                m_PacketTime.DstTb_N(), m_PacketTime.DstTb_D());
        }
        else // packet translocation
        {
            m_PacketTime.InitDst(strmTb);

            LOG_INFO("%s, %s, timebases: src=%d/%d, dst=%d/%d",
                __FUNCTION__,
                MediaTypeStr(),
                m_PacketTime.SrcTb_N(), m_PacketTime.SrcTb_D(),
                m_PacketTime.DstTb_N(), m_PacketTime.DstTb_D());
        }
    }

    void StreamWriter::SetPacketTimestamps(AVPacket* pkt) // codec ts -> stream ts
    {
        pkt->stream_index = Index();
        m_CurrPos = m_PacketTime.RecalcPacketTimestamps(pkt);
    }

// proceed

    int StreamWriter::Proceed()
    {
        int ret = -1;

        if (m_FrameHub)
        {
            ret = EncodeNextFrame();
        }
        else if (m_StreamReader)
        {
            ret = ProceedPacket();
        }
        else
        {
            LOG_ERROR("%s, %s, ind=%d, is not connected", __FUNCTION__, MediaTypeStr(), Index());
        }

        return ret;
    }

// frame encoding

    int StreamWriter::EncodeFrame(AVFrame* frame)
    {
        return m_Encoder.EncodeFrame(frame, static_cast<ISinkPacketEnc*>(this));
    }

    int StreamWriter::EncodeNextFrame()
    {
        int ret = -1;
        if (m_FrameHub)
        {
            if (auto rr = m_FrameHub->Frames().Pop())
            {
                if (rr.Ptr)
                {
                    ret = EncodeEndFreeFrame(rr.Ptr);
                }
                else // eof
                {
                    LOG_INFO("%s, %s: end of data detected", __FUNCTION__, MediaTypeStr());
                    EncodeFrame(nullptr);
                    m_EndOfData = true;
                    ret = EndOfDataMarker;
                }
            }
            else
            {
                ret = WaitFrameInterval;
            }
        }
        return ret;
    }

    int StreamWriter::EncodeEndFreeFrame(FramePoolItem* fpi)
    {
        // assert(fpi)
        int ret = -1;
        if (AVFrame* frame = fpi->Frame())
        {
            SetFrameTimestamp(frame);
            ret = EncodeFrame(frame);
            fpi->ReturnToPool();
        }
        return ret;
    }

    int StreamWriter::ForwardPacket(AVPacket* pkt) // ISinkPacketEnc impl
    {
        return WritePacket(pkt, true);
    }

// packet translocation

    int StreamWriter::ProceedPacket()
    {
        int ret = -1;
        if (m_StreamReader)
        {
            if (auto rr = m_StreamReader->Packets().Pop())
            {
                if (rr.Ptr)
                {
                    ret = WritePacket(rr.Ptr, false);
                }
                else // end of data
                {
                    LOG_INFO("%s, %s: end of data detected", __FUNCTION__, MediaTypeStr());
                    m_EndOfData = true;
                    ret = EndOfDataMarker;
                }
            }
            else // no packets in queue
            {
                ret = WaitPacketInterval;
            }
        }
        return ret;
    }

// packet writing

    int StreamWriter::WritePacket(AVPacket* pkt, bool encoded)
    {
        SetPacketTimestamps(pkt);

        int ret = m_Muxer.WritePacket(pkt);
        if (ret >= 0)
        {
            ++m_PacketCntr;
        }

        if (!encoded) // from input queue
        {
            PacketTools::Free(pkt);
        }
        return ret;
    }

//

    void  StreamWriter::LogStatistics() const
    {
        if (Index() >= 0)
        {
            LOG_INFO("%s: %s, idx=%d, frames=%lld, packet written=%d", 
                __FUNCTION__,
                MediaTypeStr(), Index(), m_FrameTime.FrameCount(), m_PacketCntr);
        }
    }

} // namespace Fcore
