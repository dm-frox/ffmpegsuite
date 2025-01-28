#pragma once

// ---------------------------------------------------------------------
// File: Demuxer.h
// Classes: Demuxer
// Purpose: class for demuxer
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <mutex>

#include "ProceedPump.h"
#include "PumpSwitchIface.h"

#include "StreamListR.h"
#include "FormatHolder.h"
#include "MetadataHolder.h"
#include "PacketTools.h"
#include "DemuxBase.h"

namespace Fcore
{
    class FrameHub;
    class FilterGraph;
    class Muxer;
    struct StreamInfo;
    class ChannLayout;

    class Demuxer : public DemuxBase, Ftool::IProceed, Ftool::IPumpSwitchW
    {
// types
    private:
        using Base = DemuxBase;
// data
    private:
        static const int    PacketQueueMaxLenVA;
        static const int    PacketQueueMaxLen;
        static const int    PacketQueueTail;
        static const int    PacketQueueMinLengthDegree;

        static const bool   UseCondVar;
        static const char   ThreadTag[];
        static const char   LangEng[];


        const int           m_PacketQueueMaxLength;
        const int           m_PacketQueueOffThreshold;
        const int           m_PacketQueueOnThreshold;

        StreamListR         m_Streams;
        Ftool::ProceedPump  m_Pump;
        std::mutex          m_Mutex;
        Packet              m_Packet;

        int                 m_StrmToSeek{ -1 };
        int                 m_SubtitStrmCount{ 0 };

        int                 m_Cntr{ 0 };
        
        std::atomic<bool>   m_InputCanceled{ false };

// ctor, dtor, copying
    public:
        explicit Demuxer(bool player);
        ~Demuxer();
// properties
    public:
        int StreamCount() const { return m_Streams.Count(); }
        int SubtitStreamCount() const { return m_SubtitStrmCount; }
        int PacketQueueMaxLength() const { return m_PacketQueueMaxLength; }
        int PacketQueueOffThreshold() const { return m_PacketQueueOffThreshold; }

        int GetStreamInfo(int strmIdx, StreamInfo& strmInfo);
        const char* GetStreamInfo(int strmIdx, int& len) const;
        const char* GetStreamMetadata(int strmIdx, int& len) const;
        StreamReader* GetStreamReader(int strmIdx);
        const StreamReader* GetStreamReader(int strmIdx) const;

        std::mutex& Mutex() { return m_Mutex; }
        Ftool::IPumpSwitchW* PumpSwitchW() { return static_cast<Ftool::IPumpSwitchW*>(this); }

        bool PacketQueuesNotFull();

        bool PumpError() const;

        bool Proceeding() const;

// operations
    public:
        int Open(const char* url, const char* format, const char* fmtOptions);

        int OpenDecoder(int strmIdx, const char* decoderParams);

        int  ConnectToFrameHubVideo(int strmIdx, FrameHub* frameHub, int outWidth, int outHeight, AVPixelFormat outPixFmt);
        int  ConnectToFrameHubAudio(int strmIdx, FrameHub* frameHub, const ChannLayout& outLayout, int outSampleRate, AVSampleFormat outSampFmt);
        int  ConnectToFilterGraphVideo(int strmIdx, FrameHub* frameHub);
        int  ConnectToFilterGraphAudio(int strmIdx, FrameHub* frameHub);
        int  ConnectToMuxerVideo(int strmIdx, Muxer* muxer);
        int  ConnectToMuxerAudio(int strmIdx, Muxer* muxer);

        const Decoder& GetDecoder(int strmIdx) const;

        void Reset();
        int  SetPosition(double pos);
        int  SetStreamToSeek(int strmIdx);

        void CancelInput() { m_InputCanceled = true; }

        const char* StrmIndexToStr(int strmInd) const;

        bool CreatePump();
        void Run();
        void Pause();
        void Stop(bool skipPause);
        void ClosePump();

        int FindSubtitStreamByLang(const char* lang) const;

        int ProceedEx() { return Proceed(); }

    private:
        bool AssertStreamIndex(int strmIdx, const char* msg) const;
        int  SetupStreams();
        void ResetPosition();
        void PushTerminator();
        bool TryEnableProceedingWrite() const;
        void LogPacket(const AVPacket* pkt, int ind = -1);
        void InitPump(bool useCondVar, const char* tag, bool logEx = false);

 // IProceed impl
    private:
        int Proceed() override final;
 // IPumpSwitchW impl
    private:
        void EnablePumpWrite(bool enable) override final;

    }; // class Demuxer

} // namespace Fcore

