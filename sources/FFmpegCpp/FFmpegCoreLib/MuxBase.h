#pragma once

// -------------------------------------------------------------------- -
// File: MuxBase.h
// Classes: MuxBase
// Purpose: base class for Muxer
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <mutex>

#include "FormatHolder.h"
#include "ffmpeg.h"

namespace Fcore
{
    class MuxBase : public FormatHolder
    {
// types
    private:
        using Mutex    = std::mutex;
        using AutoLock = std::lock_guard<Mutex>;

// data
    private:
        Mutex m_Mutex;
        bool  m_File{ false };
        bool  m_GlobalHeader{ false };
// ctor, dtor, copying
    protected:
        MuxBase();
        ~MuxBase();
// properties
    protected:
        bool IsFile() const { return m_File; }
// operations
    public:
        int Open(const char* url, const char* format);
        int WritePacket(AVPacket* pkt);
    protected:
        static const AVCodec* GetEncoderByName(const char* encoderName);
        int OpenEncoder(AVCodecContext* codecCtx, const AVCodec* encoder, const char* options) const;
        int OpenWriting(const char* options);
        int AddOutputStream(const AVCodecContext* codecCtx, AVStream*& strm);
        int AddOutputStream(const AVStream* srcStream, AVStream*& strm);
        int AddOutputStreamAudio(AVCodecID codecId, int chann, int sampleRate, AVSampleFormat sampFmt, int frameSize);
        int Finalize();
        void SetMetadata(const char* metadata, const char* msg);
    private:
        static bool CheckOutputFlag(const AVOutputFormat* outFmt, int flag);
        int OpenOutputFile();
        int CloseOutputFile();
        int WriteHeader(const char* options);
        int WriteTrailer();

    }; // class MuxBase

} // namespace Fcore
