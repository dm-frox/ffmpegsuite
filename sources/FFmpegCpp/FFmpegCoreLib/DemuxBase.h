#pragma once

// ---------------------------------------------------------------------
// File: DemuxBase.h
// Classes: DemuxBase
// Purpose: base class for Demuxer
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "FormatHolder.h"
#include "MultiString.h"

namespace Fcore
{
    class DemuxBase  : public FormatHolder
    {
// data
    private:
        static const double AvTimeBase;

        double         m_DurationFmt{ 0.0 };
        MultiString    m_Info;
        MultiString    m_Metadata;
        MultiString    m_Chapters;

// ctor, dtor, copying
    protected:
        DemuxBase();
        ~DemuxBase();
// operations
    protected:
        int OpenInput(const char* url, const char* format, const char* options);
        int FindStreamInfo();
        AVStream* GetStreamCtx(int strmIdx);

        int ReadPacket(AVPacket* pack);
        void SetupFormatContextData(const char* url);

        int SeekFrame(int strmIdx, long long tpos, int flags);

        bool CanSeekFmt() const;

        int Flush();

        void Close();

    public:
        const char* GetInfo(int& len) const;
        const char* GetMetadata(int& len) const;
        const char* GetChapters(int& len) const;

        double DurationFmt() const { return m_DurationFmt; }

        AVRational GuessFrameRate(AVStream& strm);

    private:
        void SetupMetadata();
        void SetupInfo(const char* url);
        void SetupChapters();
        static int FindInputFormat(const char* format, const AVInputFormat*& inputFmt);
        void LogFormat(const char* msg) const;
        double GetDurationFmt() const;

    }; // class DemuxBase

} // namespace Fcore
