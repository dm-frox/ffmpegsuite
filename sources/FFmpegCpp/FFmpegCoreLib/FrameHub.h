#pragma once

// ---------------------------------------------------------------------
// File: FrameHub.h
// Classes: FrameHub
// Purpose: receives frames from decoder, supports frame convertion or filtering, 
//     stores frames in a frame queue and serves as a source for rendering or transcoding
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "Queue.h"

#include "FrameDataIface.h"
#include "FrameHubTools.h"
#include "FrameQueueEx.h"
#include "FrameConv.h"
#include "FilterTools.h"
#include "FilterGraph.h"
#include "FrameTools.h"
#include "SeekCtrl.h"


namespace Ftool
{
    class IPumpSwitchR;
    class ProceedPump;
}

namespace Fcore
{
    class FramePoolItem;
    class IFrameProcessor;
    class StreamReader;

    class FrameHub : public MediaTypeHolder, IFrameConv, ISinkFrameFlt
    {
// data
    private:
        static const int    MaxFramesInPacketVideo;
        static const int    MaxFramesInPacketAudio;
        static const int    FrameQueueThresholdVideo;
        static const int    FrameQueueThresholdAudio;

        int const            m_FrameQueueThreshold;
        bool const           m_SkipConvFlt;

        FrameQueueEx         m_FrameQueue;

        FrameConv            m_FrameConv{};
        FrameProc            m_FrameProc{};

        FilterGraph          m_FilterGraph{};
        Frame                m_FltFrame{};
        MultiInput           m_MultiInput;

        bool                 m_KeepAtThreshold{ false };

        int                  m_FpsFactor{ 0 };
        int                  m_Timebase_N{ 0 };
        int                  m_Timebase_D{ 1 };
        double               m_RendPosFactor{ 0.0 };

        int                  m_FrameCntr{ 0 };

        SeekCtrl             m_SeekCtrl;

// ctor, dtor, copying
    public:
        FrameHub(AVMediaType mt, bool frameConvSeq);
        ~FrameHub();
// properties
    public:
        bool IsReady() const { return m_FrameConv.IsReady() || m_FilterGraph.IsReady(); }
        void SetKeepAtThreshold() { m_KeepAtThreshold = true; }
        IFrameData* FrameData() { return m_FrameQueue.FrameData(); }
        FrameQueueEx::FrameQueue& Frames() { return m_FrameQueue.Frames(); }
        AVRational Timebase() const { return { m_Timebase_N, m_Timebase_D}; }
    private:
        static double EndOfVideoMarker() { return IFrameData::EndOfVideoMarker(); }
        static int EndOfAudioMarker() { return IFrameData::EndOfAudioMarker(); }
        static int FrameQueueThreshold(bool isVideo);
        static int FrameQueueMaxLength(bool isVideo);
// operations
    public:
        int CreateFilterGraph();

        int AddFilterGraphInputVideo(StreamReader* strm);
        int AddFilterGraphInputAudio(StreamReader* strm);

        int AddFilterGraphOutputVideo(AVPixelFormat pixFmt);
        int AddFilterGraphOutputAudio(AVSampleFormat sampleFmt);

        int ConfigureFilterGraph(const char* filterString);
        int ReconfigureFilterGraph(const char* filterString);
        int SendFilterGraphCommand(const char* tar, const char* cmd, const char* arg);

        int SetupConverterVideo(StreamReader* strm, int outWidth, int outHeight, AVPixelFormat outPixFmt);
        int SetupConverterAudio(StreamReader* strm, int outSampleRate, AVSampleFormat outSampleFmt, const ChannLayout& outLayout);

        bool SetFrameProc(IFrameProcessor* frameProc);

        int AddFrame(AVFrame* frame, int inputIdx);
        void Reset();
        void SetFpsFactor(int fpsFactor) { m_FpsFactor = fpsFactor; }
        void SetTimebase(AVRational tb);

        void SetPosition(double pos, bool paused);
        void SetRendPump(Ftool::ProceedPump* rendPump) { m_SeekCtrl.SetPump(rendPump); }
        bool SeekingInProgress() const { return m_SeekCtrl.InProgress(); }
        void ClearQueue();
    private:
        bool AssertFilterGraph(const char* msg) const;
        bool AssertNoFilterGraph(const char* msg) const;
        bool AssertNoConverter(const char* msg) const;
        bool AssertStreamReader(const StreamReader* strm, const char* msg) const;

        int CreateConverterVideo(
            int inWidth, int inHeight, AVPixelFormat inPixFmt,
            int outWidth, int outHeight, AVPixelFormat outPixFmt);
        int CreateConverterAudio(
            int inSampleRate, AVSampleFormat inSampleFmt, const ChannLayout& inLayout,
            int outSampleRate, AVSampleFormat outSampleFmt, const ChannLayout& outLayout);

        void SetEndOfData();
        int PushFrame(AVFrame* frame, bool conv);
        bool CheckSeekingEnd(AVFrame* frame);
        int AddFrameConv(AVFrame* frame);
        int AddFrameFlt(AVFrame* frame, int inputIdx);
        int FilterFrame(AVFrame* frame, int inputIdx);
        double CalcRendPosition(const AVFrame* frame) const;
        void SetRendPosition(FramePoolItem* fpi);
        void ClearFilterGraph();
        int PushFramePoolItem(FramePoolItem* item);
        void EnableWritePumpCtrl();

        void LogPts(int64_t pts, double pos);
        void LogPts_(int64_t pts, double pos);

// IFrameConv impl
    private:
        int  Convert(AVFrame* inFrame, AVFrame* outFrame) override final;
        bool FilterIsUsed() const override final;
    public:
        IFrameConv* FrameConv();

// ISinkFrameFlt impl
    private:
        int ForwardFrame(AVFrame* frame, int outputIdx) override final;

    }; // class FrameHub

} // namespace Fcore

