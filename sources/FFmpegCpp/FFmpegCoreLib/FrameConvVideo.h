#pragma once

// ---------------------------------------------------------------------
// File: FrameConvVideo.h
// Classes: FrameConvVideo
// Purpose: class for video frame convertion, SwsContext wrapper
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "ffmpeg.h"

namespace Fcore
{

    class FrameConvVideo
    {
// data
    private:
        static const int SwsFlagsDef;

        SwsContext*  m_SwsCtx{ nullptr };

        int           m_InHeight{0};
        int           m_OutWidth{ 0 };
        int           m_OutHeight{ 0 };
        AVPixelFormat m_OutPixFmt{ AV_PIX_FMT_NONE };
        int           m_SwsFlags{ SwsFlagsDef };
        int           m_Counter{ 0 };
// ctor, dtor, copying
    protected:
        FrameConvVideo();
        ~FrameConvVideo();
// operations
    protected:
        int Init(
            int inWidth, int inHeight, AVPixelFormat inPixFmt, 
            int outWidth, int outHeight, AVPixelFormat outPixFmt);

        int  Convert(AVFrame* inFrame, AVFrame* outFrame);

    }; // class FrameConvVideo

}  // namespace Fcore
