#pragma once

// ---------------------------------------------------------------------
// File: ffmpeg_fwd.h
// Purpose: forward declarations for ffmpeg types
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

extern "C"
{
    struct AVFormatContext;
    struct AVInputFormat;
    struct AVOutputFormat;
    struct AVStream;

    struct AVCodec;
    struct AVCodecContext;
    struct AVCodecParameters;
    struct AVBitStreamFilter;

    struct AVChannelLayout;

    struct AVPacket;
    struct AVFrame;

    struct AVFilterGraph;
    struct AVFilterContext;
    struct AVFilterInOut;
    struct AVClass;
    struct AVOption;

    struct AVDictionary;
    struct AVDictionaryEntry;
    struct AVRational;

    struct SwrContext;
    struct SwsContext;

// GCC does not support forward enums
    //enum AVMediaType;
    //enum AVPixelFormat;
    //enum AVSampleFormat;
    //enum AVCodecID;

} // extern "C"