#pragma once

// -------------------------------------------------------------------- -
// File: StreamReader.h
// Classes: StreamReader
// Purpose: class for stream packets decoding and forwarding
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "ProceedIface.h"
#include "Queue.h"

#include "StreamBase.h"
#include "CodecDec.h"
#include "PacketTools.h"
#include "StreamTime.h"
#include "MultiString.h"

namespace Fcore
{
    class FilterGraph;
    class FrameHub;
    class Demuxer;
    class Muxer;
    class IFrameProcessor;
    class ChannLayout;
    struct StreamInfo;

    class StreamReader : public StreamBase, ISinkFrameDec, Ftool::IProceed
    {
// types
    private:
        using PacketQueueBase = Ftool::QueueT<AVPacket, PacketTools::Free>;

        class PacketQueue : public PacketQueueBase
        {
            int const m_BufferLength;
        public:
            PacketQueue(std::mutex& mutex, int buffLen) : PacketQueueBase(mutex, 0), m_BufferLength(buffLen) {}
            bool AllocBuffer() { return PacketQueueBase::AllocBuffer(m_BufferLength); }
            int BufferLengthIni() const { return m_BufferLength; }
        };
// data
    private:
        static const int  FramePoolInterval;
        static const int  NoPacketsInterval;
        static const bool UseCondVar;
        static const char ThreadTagSuff[];


        Demuxer&        m_Demuxer;

        PacketQueue     m_PacketQueue;
        Decoder         m_Decoder;
        FrameTime       m_FrameTime;
        AVRational      m_Framerate{ 0, 1 };

        FrameHub*       m_FrameHub{ nullptr };
        int             m_FilterInputIndex{ -1 };

        int             m_KeyFrameStatus{ 0 };
        bool            m_Activated{ false };
        IFrameProcessor* m_FrameProc{nullptr};

        mutable MultiString m_Info;
        mutable MultiString m_Metadata;

        int             m_CntrPkt{ 0 };
        int             m_CntrFrm{ 0 };

// ctor, dtor, copying
    public:
        StreamReader(AVStream& strm, Demuxer& demuxer);
        ~StreamReader();
// properties
    public:
        AVRational Framerate() const { return m_Framerate; }
        void GetStreamInfo(StreamInfo& strmInfo);
        Decoder& GetDecoder() { return m_Decoder; }
        const Decoder& Decoder() const { return m_Decoder; }
        bool IsActivated() const { return m_Activated; }
        bool NeedsPump() const { return m_Activated && m_Decoder.IsOpened(); }
        const char* GetInfo(int& len) const;
        const char* GetMetadata(int& len) const;
        PacketQueue& Packets() { return m_PacketQueue; }
        bool PacketQueueIsFull() const { return m_PacketQueue.DataLengthNoSync() == m_PacketQueue.BufferLength(); }
        int PacketQueueDataLenghtNoSync() const { return m_PacketQueue.DataLengthNoSync(); }
        AVPacket* PopPacket();
    private:
// operations
    public:
        int OpenDecoder(const char* decodParams);
        int OpenDecoderNoHub();
        int ConnectToFrameHubVideo(FrameHub* frameHub, int outWidth, int outHeight, AVPixelFormat outPixFmt);
        int ConnectToFrameHubAudio(FrameHub* frameHub, const ChannLayout& outLayout, int outSampleRate, AVSampleFormat outSampFmt);
        int ConnectToFilterGraphVideo(FrameHub* frameHub);
        int ConnectToFilterGraphAudio(FrameHub* frameHub);
        int ConnectToMuxerVideo(Muxer* muxer);
        int ConnectToMuxerAudio(Muxer* muxer);

        bool AssertDecoder(const char* msg) const;

        void PushPacketToQueue(AVPacket* pkt);

        void Reset(); 

        void SetReadPacketPump(Ftool::IPumpSwitchR* readPump);
        void SetFrameProc(IFrameProcessor* frameProc) { m_FrameProc = frameProc; }

    private:
        int DecodePacket(AVPacket* pkt);
        int DecodeAndFreePacket(AVPacket* pkt);

        void SetActivated() { m_Activated = true; }

        int SendFrame(AVFrame* frame);

        int DecodeNextPacket();
        void SetFramePts(AVFrame* frame);
        void ClearQueue();

        void InitFrameTime();
        void SetupInfo(MultiString& info) const;
        void SetupMetadata(MultiString& metadata) const;

        void LogStreamInfo() const;
        void LogPacket(const AVPacket* pkt);
        void LogPacket_(const AVPacket* pkt);
        void LogFrame(int64_t iniPts, int64_t befPts, int64_t cntPts, int64_t pts, int key);
        void LogFrame_(int64_t iniPts, int64_t befPts, int64_t cntPts, int64_t pts, bool key);

// ISinkFrameDec impl
    private:
        int ForwardFrame(AVFrame* frame) override final;
// IProceed impl
    private:
        int  Proceed() override final;

    }; // class StreamReader

} // namespace Fcore
