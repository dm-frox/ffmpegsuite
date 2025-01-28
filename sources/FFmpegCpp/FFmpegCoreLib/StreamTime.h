#pragma once

// -------------------------------------------------------------------- -
// File: StreamTime.h
// Classes: FrameTime, PacketTime
// Purpose: helpers to calc timestamps for frames and packets
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "TimestampCalc.h"


namespace Fcore
{
    class FrameTime : public TimestampCalc
    {
        int64_t          m_FrameCntr{ 0 };
        int64_t          m_SampCntr{ -1 };

    public:
        FrameTime();
        ~FrameTime();

        int64_t FrameCount() const { return m_FrameCntr; }
        int64_t GetFrameTimestamp(const AVFrame* frame);
        int64_t RecalcFrameTimestamp(int64_t pts);

    public:
        void InitAudio(int sampleRate, AVRational dstTb);
        void InitVideo(AVRational frameRate, AVRational dstTb);
        void InitTimebases(AVRational srcTb, AVRational dstTb);

    }; // class FrameTime

    class PacketTime : public TimestampCalc
    {
    public:
        PacketTime();
        PacketTime(AVRational srcTb);
        ~PacketTime();

        void InitSrc(AVRational srcTb);
        void InitDst(AVRational dstTb);

        double RecalcPacketTimestamps(AVPacket* pkt) const;

        int GetPacketCount(double srcDur) const;

    }; // class PacketTime

} // namespace Fcore

