#pragma once

// ---------------------------------------------------------------------
// File: FrameConvAudio.h
// Classes: FrameConvAudio
// Purpose: class for audio frame convertion, SwrContext wrapper
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "ffmpeg.h"
#include "ChannLayout.h"

namespace Fcore
{
    class FrameConvAudio
    {
// data
    private:
        SwrContext* m_SwrCtx{ nullptr };

        //int            m_OutChann{ 0 };
        int            m_OutSampleRate{ 0 };
        AVSampleFormat m_OutSampleFmt{ AV_SAMPLE_FMT_NONE };
        ChannLayout    m_OutChannLayout{};
        int            m_Counter{ -1 };

// ctor, dtor, copying
    protected:
        FrameConvAudio();
        ~FrameConvAudio();
// operations
    protected:
        int Init(
            int inSampleRate, AVSampleFormat inSampFmt, const ChannLayout& inLayout,
            int outSampleRate, AVSampleFormat outSampleFmt, const ChannLayout& outLayout);

        int  Convert(AVFrame* inFrame, AVFrame* outFrame); 

    }; // class FrameConvAudio

}  // namespace Fcore
