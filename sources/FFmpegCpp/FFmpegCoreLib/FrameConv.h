#pragma once

// ---------------------------------------------------------------------
// File: FrameConv.h
// Classes: FrameConv
// Purpose: class for video and audio frame convertion
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "FrameConvVideo.h"
#include "FrameConvAudio.h"
#include "NonCopyable.h"

namespace Fcore
{
    class ChannLayout;

    class FrameConv : FrameConvVideo, FrameConvAudio, NonCopyable
    {
// data
    private:
        AVMediaType m_MediaType{ AVMEDIA_TYPE_UNKNOWN };

// ctor, dtor, copying
    public:
        FrameConv();
        ~FrameConv();

// properties
    public:
        bool IsReady() const { return m_MediaType != AVMEDIA_TYPE_UNKNOWN; }

// operations
    public:
        int InitVideo(
            int inWidth, int inHeight, AVPixelFormat inPixFmt,
            int outWidth, int outHeight, AVPixelFormat outPixFmt);

        int InitAudio(
            int inSampleRate, AVSampleFormat inSampFmt, const ChannLayout& inLayout,
            int outSampleRate, AVSampleFormat outSampleFmt, const ChannLayout& outLayout);

        int  Convert(AVFrame* inFrame, AVFrame* outFrame);

    }; // class FrameConv

} // namespace Fcore

