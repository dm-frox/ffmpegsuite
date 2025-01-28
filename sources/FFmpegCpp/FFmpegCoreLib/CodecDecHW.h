#pragma once

// ---------------------------------------------------------------------
// File: CodecDecHW.h
// Classes: CodecHW
// Purpose: hardware devices support for decoding
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <string>

#include "ffmpeg.h"
#include "NonCopyable.h"
#include "Misc.h"


namespace Fcore
{
    class ISinkFrameDec;

    class CodecHW : NonCopyable
    {
        AVBufferRef*   m_DevCtx{ nullptr };
        AVHWDeviceType m_HWType{ AV_HWDEVICE_TYPE_NONE };
        AVPixelFormat  m_PixFmtDst{ AV_PIX_FMT_NONE };
        AVFrame*       m_Frame{ nullptr };

    public:
        CodecHW();
        ~CodecHW();

        bool Enable(AVCodecContext* codecCtx, const char* hwType);
        bool IsEnabled() const { return m_DevCtx ? true : false; }
        int ForwardFrame(AVFrame* frame, ISinkFrameDec* frameSink);

        AVPixelFormat  PixelFormatDst() const { return m_PixFmtDst; }
        const char* TypeStr() const;

        static std::string GetHWAccels(const AVCodec* codec);
        static std::string GetHWAccels();
        static int LogDeviceList();
        static void LogCodec(const char* name, bool decod = true);

    private:
        static bool CheckCodec(const AVCodecContext* codecCtx, AVHWDeviceType hwtype, AVPixelFormat& pixFmtSrc);
        int Init(AVCodecContext* codecCtx, AVHWDeviceType hwtype);
        AVPixelFormat PixFmtDst();

    }; // class CodecHW

} // namespace Fcore
