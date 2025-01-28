#pragma once

// ---------------------------------------------------------------------
// File: CodecDecGpu.h
// Classes: CodecGpu
// Purpose: helper to support decoders which use GPU
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "ffmpeg.h"
#include "NonCopyable.h"

namespace Fcore
{
    // static
    class CodecGpu : NonCopyable
    {
// types
    private:

        struct GpuDecoderItem
        {
            AVCodecID   Id;
            const char* Name;
        };
// data
    private:
        static const GpuDecoderItem GpuDecoderItemsQsv[];
        static const GpuDecoderItem GpuDecoderItemsCuvid[];

        static const char QsvSuff[];
        static const char CuvidSuff[];

// ctor, dtor, copying
    public:
        CodecGpu() = delete;
// operations
    public:
        static const char* GetName(const AVMediaType mt, AVCodecID cid, const char* gpuSuff);
        static bool IsGpuDecoder(const char* decName);

    private:
        template <size_t N>
        static const char* FindName(const GpuDecoderItem(&a)[N], AVCodecID id);

    }; // class CodecGpu

} // namespace Fcore