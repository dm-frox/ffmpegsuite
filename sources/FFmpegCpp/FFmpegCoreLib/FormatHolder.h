#pragma once

// ---------------------------------------------------------------------
// File: FormatHolder.h
// Classes: FormatHolder
// Purpose: AVFormatContext wrapper, base class for the other Demuxers and Muxers
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "ffmpeg_fwd.h"
#include "NonCopyable.h"

namespace Fcore
{
    class FormatHolder : NonCopyable
    {
// data
    protected:
        AVFormatContext* m_FmtCtx{ nullptr };
// ctor, dtor, copying
    protected:
        FormatHolder();
        ~FormatHolder();
// operations
    protected:
        bool AssertFormat(const char* msg) const;
        bool AssertStreams(const char* msg) const;
        bool AssertFirst(const char* msg) const;

        void DumpFormat(bool output) const;

    }; // class FormatHolder

} // namespace Fcore