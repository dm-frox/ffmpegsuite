#pragma once

// ---------------------------------------------------------------------
// File: CodecEnc.h
// Classes: ISinkPacketEnc (abstract), Encoder
// Purpose: encoder for avcodec_send_frame/avcodec_receive_packet encoding scheme
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "BaseIface0.h"

#include "CodecBase.h"
#include "PacketTools.h"

namespace Fcore
{
    class ISinkPacketEnc : Ftool::IBase0
    {
    protected:
        ISinkPacketEnc() = default;
        ~ISinkPacketEnc() = default;

    public:
        virtual int ForwardPacket(AVPacket* pkt) = 0;

    }; // class ISinkPacketEnc

    class Encoder : public CodecBase
    {
// data
    private:
        Packet             m_Packet{};
 // ctor, dtor, copying
    public:
        explicit Encoder(AVCodecContext* codecCtx);
        ~Encoder();
// operations
        int PrepareEncoding();
        int EncodeFrame(const AVFrame* frame, ISinkPacketEnc* sink);

    }; // class Encoder

} // namespace Fcore
