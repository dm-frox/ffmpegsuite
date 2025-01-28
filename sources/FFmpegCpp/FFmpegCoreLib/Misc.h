#pragma once

// ---------------------------------------------------------------------
// File: Misc.h
// Classes: free functions
// Purpose: helpers
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <cstring>

#include "ffmpeg.h"

namespace Fcore
{

// -------------------------------------------------------------------
// assertions

    bool AssertPtr(const void* ptr, const char* msg);

    bool AssertArgs(bool a, const char* msg);
    bool AssertArgs(const void* ptr, const char* msg);

    bool AssertFirst(const void* p, const char* msg);
    bool AssertFirst(bool initialized, const char* msg);

    bool AssertAudioParams(int chann, int sampleRate, const char* msg);
    bool AssertAudioFormat(AVSampleFormat sampFmt, const char* msg);

    bool AssertVideoParams(int w, int h, AVRational frameRate, const char* msg);
    bool AssertVideoFormat(AVPixelFormat pixFmt, const char* msg);

// -------------------------------------------------------------------
// string helpers

    template<typename T> // T - char, unsigned char
    inline const char* StrToLog(const T* str)
    {
        return str ? (*str ? reinterpret_cast<const char*>(str) : "<empty>") : "<null>";
    }

    inline const char* PtrToLog(const void* ptr)
    {
        return ptr ? "<yes>" : "<null>";
    }

    inline const char* FormatToLog(const char* str)
    {
        return str ? str : "Unknown";
    }

    inline const char* StrArg(const char* str)
    {
        return str ? str : "";
    }

    inline const char* EmptyToNull(const char* str)
    {
        return (str && *str) ? str : nullptr;
    }

    inline bool StrAreEq(const char* s1, const char* s2)
    {
        return (s1 && s2) ? (std::strcmp(s1, s2) == 0) : false;
    }

    inline AVPixelFormat PixFmtFromStr(const char* fmt)
    {
        return av_get_pix_fmt(fmt);
    }

    inline AVSampleFormat SampFmtFromStr(const char* fmt)
    {
        return av_get_sample_fmt(fmt);
    }

    inline const char* PixFmtToStr(AVPixelFormat pixFmt)
    {
        return FormatToLog(av_get_pix_fmt_name(pixFmt));
    }

    inline const char* PixFmtToStr(int pixFmt)
    {
        return PixFmtToStr(static_cast<AVPixelFormat>(pixFmt));
    }

    inline const char* PixFmtToStr(int64_t pixFmt)
    {
        return PixFmtToStr(static_cast<AVPixelFormat>(pixFmt));
    }

    inline const char* SampFmtToStr(AVSampleFormat sampFmt)
    {
        return FormatToLog(av_get_sample_fmt_name(sampFmt));
    }
   
    inline const char* SampFmtToStr(int sampFmt)
    {
        return SampFmtToStr(static_cast<AVSampleFormat>(sampFmt));
    }

    inline const char* SampFmtToStr(int64_t sampFmt)
    {
        return SampFmtToStr(static_cast<AVSampleFormat>(sampFmt));
    }

    inline const char* CodecIdToStr(AVCodecID id)
    {
        return StrToLog(avcodec_get_name(id));
    }

    inline const char* MediaTypeToStr(AVMediaType mediaType)
    {
        return FormatToLog(av_get_media_type_string(mediaType));
    }

    inline const char* MediaTypeToStr2(AVMediaType mediaType)
    {
        const char* ret = av_get_media_type_string(mediaType);
        return ret ? ret : "-----";
    }

    const char* FourccToStr(uint32_t val);

    const char* SubtitTypeToStr(AVSubtitleType stype);

    const char* FieldOrderToStr(AVFieldOrder fieldOrder);

    bool StrAreEqIgnoreCase(const char* x, const char* y);

    inline bool IsNullOrEmpty(const char* s) { return s == nullptr || *s == '\0'; }

// -------------------------------------------------------------------
// time stamp helpers

    const int NoPtsVal_ = -1;

    const int64_t NoPtsVal64_ = NoPtsVal_;

    inline int TsToInt(int64_t ts)
    {
        return (ts == AV_NOPTS_VALUE) ? NoPtsVal_ : (int)(ts);
    }

    inline int64_t TsToInt64(int64_t ts)
    {
        return (ts == AV_NOPTS_VALUE) ? NoPtsVal64_ : ts;
    }

    inline double PosTsToPosSec(int64_t ts, AVRational tb)
    {
        return (tb.den > 0) ? static_cast<double>(ts * tb.num) / tb.den : 0.0;
    }

    inline double PosTsToPosSecEx(int64_t ts, AVRational tb)
    {
        return (ts != AV_NOPTS_VALUE) ? PosTsToPosSec(ts, tb) : 0.0;
    }

    inline int64_t PosSecToPosTs(double posSec, AVRational tb)
    {
        return  (tb.num > 0) ? static_cast<int64_t>(posSec * tb.den / tb.num) : 0;
    }

// -------------------------------------------------------------------
// audio helpers

    inline int BytesPerSample(AVSampleFormat sampFmt)
    {
        return av_get_bytes_per_sample(sampFmt);
    }

// -------------------------------------------------------------------

    template<typename T>
    T Max_(T x, T y) { return (x > y) ? x : y; }


    template<typename T, typename S>
    T HasFlag(T flags, S flag) { return (flags & flag) ? true : false; }

} // namespace Fcore

