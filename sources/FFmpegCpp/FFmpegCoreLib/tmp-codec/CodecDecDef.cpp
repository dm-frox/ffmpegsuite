#include "pch.h"

#include "CodecDecDef.h"

namespace Fcore
{

    DecoderDef::DecoderDef(AVCodecContext* & codecCtx)
        : m_CodecCtx(codecCtx)
    {}

    DecoderDef::~DecoderDef() = default;

    void DecoderDef::Delete()
    {
        delete this;
    }

    AVMediaType DecoderDef::MediaType() const
    {
        return m_CodecCtx->codec->type;
    }

    int DecoderDef::Width() const
    {
        return m_CodecCtx->width;
    }

    int DecoderDef::Height() const
    {
        return m_CodecCtx->height;
    }

    AVPixelFormat DecoderDef::PixelFormat() const
    {
        return m_CodecCtx->pix_fmt;
    }

    int DecoderDef::Chann() const
    {
        return m_CodecCtx->ch_layout.nb_channels;
    }

    int DecoderDef::SampleRate() const
    {
        return m_CodecCtx->sample_rate;
    }

    AVSampleFormat DecoderDef::SampleFormat() const
    {
        return m_CodecCtx->sample_fmt;
    }

    const AVChannelLayout* DecoderDef::ChannLauoyt() const
    {
        return &m_CodecCtx->ch_layout;
    }

    const char* DecoderDef::Name() const
    {
        return m_CodecCtx->codec->name;
    }

    const char* DecoderDef::Description() const
    {
        return m_CodecCtx->codec->long_name;
    }

    int DecoderDef::SendPacket(const AVPacket* pkt)
    {
        return avcodec_send_packet(m_CodecCtx, pkt);
    }

    int DecoderDef::ReceiveFrame(AVFrame* frm)
    {
        return avcodec_receive_frame(m_CodecCtx, frm);
    }

    void DecoderDef::FlushBuffers()
    {
        avcodec_flush_buffers(m_CodecCtx);
    }

    IDecoder* IDecoder::CreateDefaultDecoder(AVCodecContext* & codecCtx)
    {
        return static_cast<IDecoder*>(new DecoderDef(codecCtx));
    }

} // namespace Fcore

