#pragma once

// ---------------------------------------------------------------------
// File: FrameTools.h
// Classes: FrameTools, Frame
// Purpose: helpers to work with AVFrame and brga32 frames
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "ffmpeg.h"

#include "Logger.h"
#include "CommonConsts.h"
#include "NonCopyable.h"

namespace Fcore
{
    class ChannLayout;

    // static
    class FrameTools
    {
// data
    private:
        static const int BytesPerPixel32bpp;
// ctor, dtor, copying
    public:
        FrameTools() = delete;
// operations
    public:
        static AVFrame* AllocFrame() { return av_frame_alloc(); }
        static void FreeFrame(AVFrame* frame) { av_frame_free(&frame); }
        static void FreeFrameBuffer(AVFrame* frame) { if (frame) av_frame_unref(frame); }
        static int  CopyFrameRef(AVFrame* dst, const AVFrame* src) { return (dst && src) ? av_frame_ref(dst, src) : -1; }
        static int  MoveFrameRef(AVFrame* dst, AVFrame* src) { return (dst && src) ? (av_frame_move_ref(dst, src), 0) : -1; }

        static int AllocFrameBuffVideo(int width, int height, AVPixelFormat pixFmt, AVFrame* frame);
        static int AllocFrameBuffAudio(int sampleRate, AVSampleFormat sampFmt, const ChannLayout& channLayout, int frameSize, AVFrame* frame);

        static void FillFrameBuffer0(const void* srcData, int srcSize, AVFrame* frame);

        static bool GetVideoData32bpp(const AVFrame* frame, void* buff, int width, int height, int stride);
        
        static void FillBitmapBuffer32bppEmul(int cntr, void* buff, int width, int height, int stride);
        static void FillFrameBuffer32bppEmul(int cntr, AVFrame* frame);

        static void FillFrameBufferInterlaced(const void* srcData, int srcStride, bool even, bool evenOnly, AVFrame* frame);

        static int CalcAudioDataSize(const AVFrame* frame);

        static bool HasFlag(const AVFrame* frame, int flag) { return (frame->flags & flag) ? true : false; }

        static void LogVideoFrame(const AVFrame* frame);
        static void LogVideoFrame2(const AVFrame* frame);
        static void LogVideoFrame3(const AVFrame* frame, const char* msg);

    private:
        static void FillBitmapBufferInterlaced(const void* srcData, int srcStride, bool even, bool evenOnly, void* dstBuff, int dstHeight, int dstStride);
        static void FillBitmapBuffer(const void* srcData, int srcStride, void* dstBuff, int dstWidth, int dstHeight, int dstStride, int bytesPerPixel);
        static void FillBitmapBufferAlt(const void* srcData, int srcStride, void* dstBuff, int dstWidth, int dstHeight, int dstStride, int bytesPerPixel);

    }; // class FrameTools

    class Frame : NonCopyable
    {
// data
    private:
        AVFrame* m_Frame{ nullptr };

// ctor, dtor, copying
    public:
        Frame() = default;
        ~Frame() { av_frame_free(&m_Frame); }

// operations
    public:
        explicit operator bool() const { return m_Frame != nullptr; }

        operator const AVFrame* () const { return m_Frame; }

        operator AVFrame* () { return m_Frame; }

        bool Alloc()
        {
            if (!m_Frame)
            {
                m_Frame = av_frame_alloc();
            }
            return m_Frame != nullptr;
        }

        int MoveTo(AVFrame* dstFrame)
        {
            if (!m_Frame)
                return ErrorBadState;
            if (!dstFrame)
                return ErrorBadArgs;

            av_frame_move_ref(dstFrame, m_Frame);
            return 0;
        }
        
        void FreeBuffer() 
        { 
            if (m_Frame)
            {
                av_frame_unref(m_Frame);
            }
        }

    }; // class Frame

    class LineSwap : NonCopyable
    {
// data
        void*   m_Buff{ nullptr };
        size_t  m_BuffSize{ 0 };
    public:
// ctor, dtor, copying
    public:
        LineSwap() = default;
        ~LineSwap() { ::free(m_Buff); }
// operations
        void SwapEvenOddLines(AVFrame* frame, int planeCount);
    };

} // namespace Fcore

