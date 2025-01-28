#pragma once

// ---------------------------------------------------------------------
// File: FrameProcIface.h
// Classes: IFrameProcessor (abstract)
// Purpose: callback interface to process frames in FrameHub immediately
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "BaseIface0.h"

#include "ffmpeg_fwd.h"

namespace Fcore
{
    class IFrameProcessor : Ftool::IBase0
    {
    protected:
        IFrameProcessor() = default;
        ~IFrameProcessor() = default;
    public:
        virtual const char* Tag() const = 0;
        virtual void ProcFrame(const AVFrame* frame) = 0;

    }; // class IFrameProcessor

} // namespace Fcore
