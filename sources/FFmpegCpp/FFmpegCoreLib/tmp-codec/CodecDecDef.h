#pragma once

// ---------------------------------------------------------------------
// File: CodecDecDef.h
// Classes: DecoderDef
// Purpose: IDecoder implementation for FFmpeg decoders
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "CodecDecIface.h"

namespace Fcore
{

    class DecoderDef : IDecoder
    {
    // data
    private:
        AVCodecContext* & m_CodecCtx;

    // ctor, dtor, copying
    public:
        explicit DecoderDef(AVCodecContext* & codecCtx);

    protected:
        ~DecoderDef();

    // IDecoder impl
    private:

        void Delete() override final;

        AVMediaType MediaType() const override final;

        int Width() const override final;
        int Height() const override final;
        AVPixelFormat PixelFormat() const override final;

        int Chann() const override final;
        int SampleRate() const override final;
        AVSampleFormat SampleFormat() const override final;
        const AVChannelLayout* ChannLauoyt() const override final;

        const char* Name() const override final;
        const char* Description() const override final;

        int SendPacket(const AVPacket* pkt) override final;
        int ReceiveFrame(AVFrame* frm) override final;
        void FlushBuffers() override final;

    // factory
        friend IDecoder* IDecoder::CreateDefaultDecoder(AVCodecContext* & codecCtx);

    }; // class CodecDecDef

} // namespace Fcore

