#pragma once

// ---------------------------------------------------------------------
// File: FrameSinkVideoIface.h
// Classes: IFrameSinkVideo (abstract)
// Purpose: callback interface which must implement a consumer to process decoded video frames
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "BaseIface.h"

namespace Fwrap
{
    class IFrameSinkVideo : IBase0
    {
    protected:
        IFrameSinkVideo() = default;
        ~IFrameSinkVideo() = default;

    public:
        virtual bool CheckPixelFormat(const char* pixFmt) = 0;
        virtual bool WriteFrame(int width, int height, const void* data, int stride) = 0;
        virtual bool WriteFrameYUV(int width, int height, 
            const void* ydata, int ystride,
            const void* udata, int ustride, 
            const void* vdata, int vstride) = 0;
        virtual bool WriteFrameNV(int width, int height,
            const void* ydata, int ystride,
            const void* uvdata, int uvstride) = 0;
        virtual int FrameWidth() const = 0;
        virtual int FrameHeight() const = 0;
        virtual void Detach() = 0;

    }; // class IFrameSinkVideo

} // namespace Fwrap
