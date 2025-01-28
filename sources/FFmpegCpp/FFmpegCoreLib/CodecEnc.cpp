#include "pch.h"

#include "CodecEnc.h"

#include "Logger.h"
#include "PacketTools.h"
#include "CodecTools.h"

namespace Fcore
{
    Encoder::Encoder(AVCodecContext* codecCtx)
        : CodecBase(codecCtx)
    {}

    Encoder::~Encoder() = default;

    int Encoder::PrepareEncoding()
    {
        int ret = 0;
        if (m_CodecCtx)
        {
            if (m_Packet.Alloc())
            {
                CodecBase::SetMediaParams();

                LOG_INFO("%s, %s, name=%s", __FUNCTION__, MediaTypeStr(), Name());
            }
            else
            {
                ret = AVERROR(ENOMEM);
                LOG_ERROR("%s, %s, name=%s, failed to alloc packet", __FUNCTION__, MediaTypeStr(), Name());
            }
        }
        return ret;
    }

    int Encoder::EncodeFrame(const AVFrame* frame, ISinkPacketEnc* sink)
    {
        int ret = -1;

        if (!m_CodecCtx || !m_Packet)
            return ret;

        int rr = avcodec_send_frame(m_CodecCtx, frame);
        if (rr >= 0)
        {
            int ct = 0;
            bool end = (frame == nullptr);
            if (end)
            {
                LOG_INFO("%s, %s, %s, null frame sent", __FUNCTION__, MediaTypeStr(), Name());
            }

            while (rr >= 0)
            {
                rr = avcodec_receive_packet(m_CodecCtx, m_Packet);
                if (rr >= 0) // packet received
                {
                    rr = sink->ForwardPacket(m_Packet);
                    if (end)
                        ++ct;
                    if (rr < 0)
                    {
                        ret = rr;
                        LOG_ERROR("%s, %s, forward packet, %s", __FUNCTION__, MediaTypeStr(), FmtErr(ret));
                    }
                }
                else
                {
                    if (rr == EndVal(end)) // OK, no encoded packets
                    {
                        ret = 0;
                        if (end)
                        {
                            LOG_INFO("%s, %s, last packet received, codec buff=%d", __FUNCTION__, MediaTypeStr(), ct);
                        }
                    }
                    else    // error
                    {
                        ret = rr;
                        LOG_ERROR("%s, %s, avcodec_receive_packet, %s", __FUNCTION__, MediaTypeStr(), FmtErr(ret));
                    }
                }
            }
        }
        else
        {
            ret = rr;
            LOG_ERROR("%s, %s, avcodec_send_frame, %s", __FUNCTION__, MediaTypeStr(), FmtErr(ret));
        }

        return ret;
    }

} // namespace Fcore


// int avcodec_receive_packet(AVCodecContext* avctx, AVPacket* avpkt)
// avpkt
// This will be set to a reference - counted packet allocated by the encoder.
// Note that the function will always call av_frame_unref(frame) before doing anything else.