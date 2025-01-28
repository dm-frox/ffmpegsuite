#pragma once

// -------------------------------------------------------------------- -
// File: StreamWriter.h
// Classes: StreamWriter
// Purpose: class for stream data encoding and forwarding to Muxer
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <atomic>

#include "ProceedIface.h"

#include "CodecEnc.h"
#include "StreamTime.h"
#include "StreamBase.h"


namespace Fcore
{
    class MuxBase;
    class FrameHub;
    class StreamReader;
    class FramePoolItem;

    class StreamWriter : public StreamBase, ISinkPacketEnc, Ftool::IProceed
    {
// types
        using AtomicBool = std::atomic<bool>;
// data
    private:
        static const int  WaitFrameInterval;
        static const int  WaitPacketInterval;
        static const bool UseCondVar;
        static const char ThreadTagSuff[];

        MuxBase&         m_Muxer; // packet sink

        FrameHub*        m_FrameHub   { nullptr }; // source
        StreamReader*    m_StreamReader{ nullptr }; // source

        Encoder          m_Encoder;
        FrameTime        m_FrameTime;
        PacketTime       m_PacketTime;
        AtomicBool       m_EndOfData{ false };
        double           m_CurrPos{ 0.0 };
        int              m_PacketCntr{ 0 };

// ctor, dtor, copying
    public:
        StreamWriter(AVStream& strm, AVCodecContext* codecCtx, MuxBase& muxer);
        ~StreamWriter();
// properties
    public:
        bool IsActivated() const { return true; }
        bool NeedsPump() const { return true; }
        double Position() const { return m_CurrPos; }
        bool EndOfData() const { return m_EndOfData; }
        const Encoder& Encoder() const { return m_Encoder; }
// operations
    public:
        int PrepareEncoding() { return m_Encoder.PrepareEncoding(); }
        bool SetFrameHub(FrameHub* frameHub);
        bool SetStreamReader(StreamReader* streamReader);
        void SetMetadata(const char* metadata, const char* msg);

        void SetPacketTimebaseSrc(AVRational tb);
        void SetupTimebases();

        void SetFrameTimebases(AVRational srcTb, AVRational dstTb);
        void SetFrameTimebasesAudio(int sampleRate, AVRational dstTb);
        void LogStatistics() const;

    private:
        int EncodeFrame(AVFrame* frame);
        int EncodeNextFrame();
        int EncodeEndFreeFrame(FramePoolItem* fpi);
        int ProceedPacket();
        void SetFrameTimestamp(AVFrame* frame);
        void SetPacketTimestamps(AVPacket* pkt);
        int WritePacket(AVPacket* pkt, bool encoded);

// IPacketSink impl
    private:
        int ForwardPacket(AVPacket* pkt) override final;
// IProceed impl
    private:
        int Proceed() override final;

    }; // class StreamWriter

} // namespace Fcore
