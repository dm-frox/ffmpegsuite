#pragma once

// ---------------------------------------------------------------------
// File: FramePool.h
// Classes: FramePool
// Purpose: container for elements of frame queue
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "NonCopyable.h"

namespace Fcore
{
    class FramePoolItem;

    class FramePool : NonCopyable
    {
// data
        FramePoolItem* const m_Pool;
        const int            m_Length;
// ctor, dtor, copying
    public:
        explicit FramePool(int len);
        ~FramePool();
// operations
    public:
        FramePoolItem* FindFreeItem() const;

    }; // class FramePool

} // namespace Fcore

