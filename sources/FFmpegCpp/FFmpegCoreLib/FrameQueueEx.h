#pragma once

// ---------------------------------------------------------------------
// File: FrameQueueEx.h
// Classes: FrameQueueEx
// Purpose: FIFO frame container with frame pool
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2021
// ---------------------------------------------------------------------

#include "Queue.h"

#include "MediaTypeHolder.h"
#include "FramePool.h"
#include "FramePoolItem.h"
#include "FrameDataIface.h"


namespace Ftool
{
    class IPumpSwitchW;
}

namespace Fcore
{
    class FrameQueueEx : public MediaTypeHolder, IFrameData
    {
// types
    public:
        using FrameQueue = Ftool::QueueT<FramePoolItem, FramePoolItem::ReturnToPool>;
// data
    private:
        std::mutex           m_Mutex;
        FrameQueue           m_FrameQueue;
        FramePool            m_FramePool;
// ctor, dtor, copying
    public:
        FrameQueueEx(AVMediaType mediaType, int maxLen);
        ~FrameQueueEx();
 // properties
    public:
        FrameQueue& Frames() { return m_FrameQueue; }
        int MaxLength() const { return m_FrameQueue.BufferLength(); }
// operations
    public:
        void EnableWritePumpCtrl(Ftool::IPumpSwitchW* pumpSwitch, int thresholdOff) { m_FrameQueue.EnableWritePumpCtrl(pumpSwitch, thresholdOff); }
        FramePoolItem* FindFreeFramePoolItem() { return m_FramePool.FindFreeItem(); }

        int Push(FramePoolItem* fpi) { return m_FrameQueue.Push(fpi); }
        int Shrink(int len) { return m_FrameQueue.Shrink(len); }
        int Clear() { return m_FrameQueue.Clear(); }

// IFrameData impl
    private:
        void SetReadFramePump(Ftool::IPumpSwitchR* readPump) override final;
        double VideoFramePos() const override final;
        const void* PeekVideoFrame(int& width, int& height, int& stride, double& pos) const override final;
        AVFrame* PeekVideoFrame(double& pos) override final;
        const void* PeekAudioFrame(int& size, int& chann, int& sampleRate, double& pos) const override final;
        void  DropFrame() override final;
        int FrameCount() const override final;
    public:
        IFrameData* FrameData() { return static_cast<IFrameData*>(this); }

    }; // class FrameQueueEx

} // namespace Fcore

