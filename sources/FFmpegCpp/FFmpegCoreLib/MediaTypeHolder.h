#pragma once

// -------------------------------------------------------------------- -
// File: MediaTypeHolder.h
// Classes: MediaTypeHolder
// Purpose: the class incapsulates media type, base class for many other classes
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------


#include "ffmpeg.h"
#include "NonCopyable.h"

namespace Fcore
{
    class MediaTypeHolder : NonCopyable
    {
// data
    private:
        AVMediaType const m_MediaType;
        const char* const m_MediaTypeStr;
// ctor, dtor, copying
    public:
        explicit MediaTypeHolder(AVMediaType mediaType);
        explicit MediaTypeHolder(const AVCodecParameters* codecpar);
        explicit MediaTypeHolder(const AVStream* strm);
        explicit MediaTypeHolder(const AVStream& strm);
        explicit MediaTypeHolder(const AVCodec* codec);
        explicit MediaTypeHolder(const AVCodecContext* codecCtx);

        ~MediaTypeHolder();
// properties
    public:
        AVMediaType MediaType() const { return m_MediaType; }
        const char* MediaTypeStr() const { return m_MediaTypeStr; }

        bool IsVideo() const { return m_MediaType == AVMEDIA_TYPE_VIDEO; }
        bool IsAudio() const { return m_MediaType == AVMEDIA_TYPE_AUDIO; }
        bool IsSubtit() const { return m_MediaType == AVMEDIA_TYPE_SUBTITLE; }
// operations
    protected:
        bool AssertVideo(const char* msg) const;
        bool AssertAudio(const char* msg) const;
        bool AssertConnect(const MediaTypeHolder* other, const char* msg) const;

        const char* MediaTypeTag() const 
        {
            return IsVideo() ? "vvv" : (IsAudio() ? "aaa" : (IsSubtit() ? "sss" : "unk"));
        }

    }; // class MediaTypeHolder

} // namespace Fcore
