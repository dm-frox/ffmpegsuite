#include "pch.h"

#include "StreamTime.h"

#include "ffmpeg.h"
#include "Misc.h"


namespace Fcore
{
    FrameTime::FrameTime() = default;
    FrameTime::~FrameTime() = default;

    int64_t FrameTime::GetFrameTimestamp(const AVFrame* frame)
    {
        // assert(frame)

        int64_t ts = 0;
        if (m_SampCntr >= 0)
        {
            ts = m_SampCntr;
            m_SampCntr += frame->nb_samples;
        }
        else
        {
            ts = m_FrameCntr;
            ++m_FrameCntr;
        }
        return SrcToDst(ts);
    }

    int64_t FrameTime::RecalcFrameTimestamp(int64_t pts)
    {
        ++m_FrameCntr;
        return SrcToDst(pts);
    }

    void FrameTime::InitTimebases(AVRational srcTb, AVRational dstTb)
    {
        m_SampCntr = 0;
        m_FrameCntr = 0;
        TimestampCalc::Init(srcTb, dstTb);
    }

    void FrameTime::InitAudio(int sampleRate, AVRational dstTb)
    {
        m_SampCntr = 0;
        m_FrameCntr = 0;
        Init({ 1 , sampleRate }, dstTb);
    }

    void FrameTime::InitVideo(AVRational frameRate, AVRational dstTb)
    {
        m_SampCntr = -1;
        m_FrameCntr = 0;
        Init(av_inv_q(frameRate), dstTb);
    }

// ----------------------------------------------------------------------------

    PacketTime::PacketTime() = default;

    PacketTime::PacketTime(AVRational srcTb) 
        : TimestampCalc(srcTb) 
    {}

    PacketTime::~PacketTime() = default;

// The dts for subsequent packets in one stream must be strictly increasing
// (unless the output format is flagged with the AVFMT_TS_NONSTRICT, 
// then they merely have to be nondecreasing).

    void PacketTime::InitSrc(AVRational srcTb)
    { 
        TimestampCalc::InitSrc(srcTb); 
    }

    void PacketTime::InitDst(AVRational dstTb)
    { 
        TimestampCalc::InitDst(dstTb); 
    }

    double PacketTime::RecalcPacketTimestamps(AVPacket* pkt) const
    {
        double ret = 0.0;

        int64_t pts = pkt->pts;
        if (pts != AV_NOPTS_VALUE)
        {
            pts = SrcToDst(pts);
        }

        int64_t dts = pkt->dts;
        if (dts != AV_NOPTS_VALUE)
        {
            dts = SrcToDst(dts);
        }

        int64_t dur = pkt->duration;
        if (dur != 0 && dur != AV_NOPTS_VALUE)
        {
            dur = SrcToDst(dur);
        }

        pkt->pts = pts;
        pkt->dts = dts;
        pkt->duration = dur;
        if (dts != AV_NOPTS_VALUE)
        {
            ret = GetDstTimeSec(dts);
        }

        return ret;
    }

    int PacketTime::GetPacketCount(double srcDur) const
    { 
        int num = SrcTb_N();
        return (num > 0) ? (int)(srcDur / num * SrcTb_D()) : 0;
    }

} // namespace Fcore

// void av_packet_rescale_ts(AVPacket *pkt, AVRational tb_src, AVRational tb_dst);

// static
//AVRational FrameTime::CalcAudioTimebase(int sampleRate, int frameSize)
//{
//    if (frameSize > 0)
//    {
//        int n = 0, d = 0;
//        av_reduce(&n, &d, frameSize, sampleRate, Max_(frameSize, sampleRate));
//        bool rr = n > 0 && d > 0;
//        int num = rr ? n : frameSize;
//        int den = rr ? d : sampleRate;
//        return { num, den };
//    }
//    else
//    {
//        return { 1 , sampleRate };
//    }
//}
