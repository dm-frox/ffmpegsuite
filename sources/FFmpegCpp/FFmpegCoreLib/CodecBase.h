#pragma once

// ---------------------------------------------------------------------
// File: CodecBase.h
// Classes: CodecBase
// Purpose: base class for Decoder and Encoder, wrapper for AVCodecContext
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <string>

#include "MediaTypeHolder.h"
#include "ChannLayout.h"

namespace Fcore
{
    class IDecoder;

    class CodecBase : public MediaTypeHolder
    {
// data
    private:
        static const char HWSepr;

        int             m_Width{ 0 };
        int             m_Height{ 0 };
        AVPixelFormat   m_PixelFormat{ AV_PIX_FMT_NONE };

        int             m_SampleRate{ 0 };
        AVSampleFormat  m_SampleFormat{ AV_SAMPLE_FMT_NONE };
        ChannLayout     m_ChannLayout{};

        std::string     m_Name{};
        std::string     m_Description{};

    protected:
        AVCodecContext* m_CodecCtx;
// ctor, dtor, copying
    protected:
        explicit CodecBase(AVCodecContext* codecCtx);
        explicit CodecBase(AVStream& strm);
        ~CodecBase();
// properties
    public:
        AVRational Timebase() const;
        // video
        int Width() const { return m_Width; }
        int Height() const { return m_Height; }
        AVPixelFormat PixelFormat() const { return m_PixelFormat; }
        // audio
        int SampleRate() const { return m_SampleRate; }
        AVSampleFormat SampleFormat() const { return m_SampleFormat; }
        const ChannLayout& ChannLayout() const { return m_ChannLayout; }

        const char* Name() const { return m_Name.c_str(); }
        const char* Description() const { return m_Description.c_str(); }
// operations
    protected:
        void Close();
        void SetMediaParams();
        void SetMediaParams(const IDecoder* decoder, const char* hwaccel);

        inline static int EndVal(bool end) { return !end ? AVERROR(EAGAIN) : AVERROR_EOF; }
    private:
        void CorrectChannLayout();

    }; // class CodecBase

} //namespace Fcore
