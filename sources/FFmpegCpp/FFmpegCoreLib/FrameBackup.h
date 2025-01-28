#pragma once

// ---------------------------------------------------------------------
// File: FrameBackup.h
// Classes: FrameBackup
// Purpose: helper to store frame copy
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2021
// ---------------------------------------------------------------------

#include "ffmpeg_fwd.h"

namespace Fcore
{
    class FrameBackup
    {
        bool     m_IsSet{ false };
        AVFrame* m_Frame{ nullptr };
        AVFrame* m_FrameOut{ nullptr };
    public:
        FrameBackup();
        ~FrameBackup();

        bool Set(AVFrame* frame);
        AVFrame* Get();
        void Clear();

    }; // FrameBackup

} // namespace Fcore


