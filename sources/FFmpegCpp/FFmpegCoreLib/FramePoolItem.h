#pragma once

// ---------------------------------------------------------------------
// File: FramePoolItem.h
// Classes: FramePoolItem
// Purpose: element of frame queue
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <atomic>

#include "ffmpeg_fwd.h"
#include "NonCopyable.h"

namespace Fcore
{
    class FramePoolItem : NonCopyable
    {
// types
        using AtomicBool = std::atomic<bool>;
// data
    private:
        AVFrame*      m_Frame{ nullptr };
        double        m_RendPosition{ -1.0 };
        AtomicBool    m_Free{ true };
// ctor, dtor, copying
    public:
        FramePoolItem();
        ~FramePoolItem();
// properties
    public:
        AVFrame* Frame() { return m_Frame; }
        const AVFrame* Frame() const { return m_Frame; }
        int64_t FramePts() const;
        double RendPosition() const { return m_RendPosition; }
        void SetRendPosition(double pos) { m_RendPosition = pos; }
// operations
    public:
        void ReturnToPool();
        void ForceFree() { m_Free = true; }
        static void ReturnToPool(FramePoolItem* item);
        bool TakeFreeItemForQueue();

    }; // class FramePoolItem

} // namespace Fcore

