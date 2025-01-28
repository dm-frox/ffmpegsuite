#pragma once

// -------------------------------------------------------------------- -
// File: Muxer.h
// Classes: Muxer
// Purpose: class for muxer
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <string>

#include "StreamWriter.h"
#include "StreamListW.h"
#include "MuxBase.h"


namespace Fcore
{
    class FrameHub;
    class PacketQueue;
    class StreamReader;
    class ChannLayout;

    class Muxer : public MuxBase 
    {
// types
    private:
        using Base = MuxBase;
// data
    private:
        StreamListW         m_Streams;

        int                 m_IndexVideo{ -1 };
        int                 m_IndexAudio{ -1 };

        int64_t             m_BitrateVideo{ 0 };
        std::string         m_PresetVideo;
        int                 m_CrfVideo{ -1 };

        int64_t             m_BitrateAudio{ 0 };

// ctor, dtor, copying
    public:
        Muxer();
        ~Muxer();
// properties
    public:
        int StreamCount() const { return m_Streams.Count(); }
        AVRational GetTimebase(int ind) const;

        void SetBitrateVideo(int64_t bitrate) { m_BitrateVideo = bitrate; }
        void SetPresetVideo(const char* preset){ m_PresetVideo = preset; }
        void SetCrfVideo(int crf) { m_CrfVideo = crf; }

        void SetBitrateAudio(int64_t bitrate) { m_BitrateAudio = bitrate; }

        bool EndOfData() const;

        double Position() const;

        const Encoder& GetEncoder(int idx) const;

// operations
    public:
        int OpenWriting(const char* options, const char* metadata, const char* metadataVideo, const char* metadataAudio);

        int AddStreamVideo(const char* encoderName, int width, int height,
            AVPixelFormat pixFmt, AVRational frameRate, AVRational tb, const char* options,
            AVPixelFormat& actPixFmt);
        int AddStreamAudio(const char* encoderName, 
            const ChannLayout& channLayout, int sampRate, AVSampleFormat sampFmt, const char* options,
            ChannLayout& channLayoutAct, AVSampleFormat& actSampFmt, int& frameSize);
        int AddOutputStream(StreamReader* inputStream);

        int CloseWriting();

        int AttachFrameHub(FrameHub* frameHub);

        bool CreatePump();
        void Run();
        void Pause();
        void Stop(bool skipPause);
        void ClosePump();
        bool PumpError() const;

        int SetupOutputStreamAudioEx(AVCodecID codecId, int chann, int sampRate, AVSampleFormat sampFmt, int framesize);

        bool AddDispositionFlagToLastStream(int flag);
        bool SetLastStreamDefault();

    private:
        void SetMetadata(const char* metadata, const char* metadataVideo, const char* metadataAudio);
        double Position(int strmIdx) const;

        bool AssertStreamIndex(int strmIdx, const char* msg) const;
        bool AssertStreams(const char* msg) const;

        int SetupOutputStreamVideo(const AVCodec* encoder, int width, int height,
            AVPixelFormat pixFmt, AVRational frameRate, AVRational tb, const char* options);

        int  SetupOutputStreamAudio(const AVCodec* encoder, const ChannLayout& channLayout, int sampleRate,
            AVSampleFormat samFmt, const char* options, int& frameSize);

        int SetupOutputStream(AVCodecContext* codecCtx);
        int SetupOutputStream(const AVStream* srcStream);

        static void FindFrameParamsAudio(const AVCodec* encoder, const ChannLayout& channLayout, ChannLayout& channLayoutEnc);
        static AVSampleFormat FindFrameParamsAudio(const AVCodec* encoder, AVSampleFormat sampFmt);
        static AVPixelFormat FindFrameParamsVideo(const AVCodec* encoder, AVPixelFormat pixFmt);

    }; // class Muxer

} // namespace Fcore

