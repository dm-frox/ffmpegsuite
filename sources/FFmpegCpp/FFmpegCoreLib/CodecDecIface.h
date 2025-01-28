#pragma once

// ---------------------------------------------------------------------
// File: CodecDecIface.h
// Classes: IDecoderExt
// Purpose: interface and factory for decoders
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "BaseIface0.h"

#include "ffmpeg.h"

namespace Fcore
{
    class IDecoder : Ftool::IBase0
    {
    protected:
        IDecoder() = default;
        ~IDecoder() = default;

    public:
        virtual void Delete() = 0;

        virtual AVMediaType MediaType() const = 0;

        virtual int Width() const = 0;
        virtual int Height() const = 0;
        virtual AVPixelFormat PixelFormat() const = 0;

        virtual int Chann() const = 0;
        virtual int SampleRate() const = 0;
        virtual AVSampleFormat SampleFormat() const = 0;
        virtual const AVChannelLayout* ChannLauoyt() const = 0;

        virtual const char* Name() const = 0;
        virtual const char* Description() const = 0;

        virtual int SendPacket(const AVPacket* pkt) = 0;
        virtual int ReceiveFrame(AVFrame* frm) = 0;
        virtual void FlushBuffers() = 0;

        static IDecoder* FindExternalDecoder(const AVStream* strm);
        static IDecoder* CreateDefaultDecoder(AVCodecContext* & codecCtx);

    }; // class IDecoderExt

} // namespace Fcore
