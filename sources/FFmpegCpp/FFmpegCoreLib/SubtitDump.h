#pragma once

// ---------------------------------------------------------------------
// File: SubtitDump.h
// Classes: SubtitDump
// Purpose: test class, subtitle decoding with writing results to the log
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2022
// ---------------------------------------------------------------------

#include "Demuxer.h"


namespace Fcore
{
    class SubtitDump
    {

        Demuxer    m_Muxer;
    public:
        SubtitDump();
        ~SubtitDump();

        void Build(const char* path, const char* lang, int count);
    private:
        void Dump(StreamReader* strm, int count);
        void Dump(const AVSubtitle& sub);

    }; // class SubtitDump

} // namespace Fcore


