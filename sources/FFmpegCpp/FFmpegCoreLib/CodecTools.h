#pragma once

// ---------------------------------------------------------------------
// File: CodecTools.h
// Classes: CodecTools
// Purpose: helpers for codec and codec context
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "ffmpeg.h"

namespace Fcore
{
    class IDecoder;
    class ChannLayout;

    // static
    class CodecTools
    {
// data
    private:
        static const char PresetVideoOption[];
        static const char CrfVideoOption[];
// ctor, dtor, copying
    public:
        CodecTools() = delete;
// operations
    public:
        static int OpenCodec(AVCodecContext* codecCtx, const AVCodec* codec, const char* options, const char* msg);

        static void SetupVideoCodecContext(AVCodecContext* codecCtx, int width, int height, AVPixelFormat pixFmt, AVRational frameRate, AVRational tb);
        static void SetupVideoCodecContext(AVCodecContext* codecCtx, int64_t bitrate, const char* preset, int crf);

        static void SetupAudioCodecContext(AVCodecContext* codecCtx, const ChannLayout& channLayout, int sampleRate, AVSampleFormat sampFmt);
        static void SetupAudioCodecContext(AVCodecContext* codecCtx, int64_t bitrate);

        static AVPixelFormat FindPixelFormat(const AVPixelFormat* fmts, AVPixelFormat prefFmt);
        static AVPixelFormat FindPixelFormat2(const AVPixelFormat* fmts, AVPixelFormat prefFmt);
        static AVPixelFormat FindPixelFormat(const AVCodec* codec, AVPixelFormat prefFmt);
        static AVSampleFormat FindSampleFormat(const AVCodec* codec, AVSampleFormat prefFmt);

        static void LogCodecContext(const AVCodecContext* codecCtx, bool logCodec, const char* msg);
        static void LogCodecParameters(const AVCodecParameters* codecpar, const char* msg);
        static void LogVideoCodec(const AVCodec* codec, const char* msg);
        static void LogAudioCodec(const AVCodec* codec, const char* msg);
        static void LogCodecParameters2(const AVCodecParameters* codecpar, const char* msg);
        static void LogDecoder(const IDecoder* cd, const char* msg);

        static AVCodecConfig CodecCfg(const AVPixelFormat*)
        {
            return AV_CODEC_CONFIG_PIX_FORMAT;
        }

        static AVCodecConfig CodecCfg(const AVSampleFormat*)
        {
            return AV_CODEC_CONFIG_SAMPLE_FORMAT;
        }

        static AVCodecConfig CodecCfg(const AVChannelLayout*)
        {
            return AV_CODEC_CONFIG_CHANNEL_LAYOUT;
        }

    }; // class CodecTools


    template<typename T>
    const T* GetCodecConfig(const AVCodec* codec)
    {
        const T* ret = nullptr;
        const void* lst = nullptr;
        if (avcodec_get_supported_config(nullptr, codec,
            CodecTools::CodecCfg(ret),
            0, &lst, nullptr) >= 0)
        {
            ret = static_cast<const T*>(lst);
        }
        return ret;
    }

} // namespace Fcore

//enum AVCodecConfig {
//    AV_CODEC_CONFIG_PIX_FORMAT,     ///< AVPixelFormat, terminated by AV_PIX_FMT_NONE
//    AV_CODEC_CONFIG_FRAME_RATE,     ///< AVRational, terminated by {0, 0}
//    AV_CODEC_CONFIG_SAMPLE_RATE,    ///< int, terminated by 0
//    AV_CODEC_CONFIG_SAMPLE_FORMAT,  ///< AVSampleFormat, terminated by AV_SAMPLE_FMT_NONE
//    AV_CODEC_CONFIG_CHANNEL_LAYOUT, ///< AVChannelLayout, terminated by {0}
//    AV_CODEC_CONFIG_COLOR_RANGE,    ///< AVColorRange, terminated by AVCOL_RANGE_UNSPECIFIED
//    AV_CODEC_CONFIG_COLOR_SPACE,    ///< AVColorSpace, terminated by AVCOL_SPC_UNSPECIFIED
//};
