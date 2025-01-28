#pragma once

// -------------------------------------------------------------------- -
// File: Transcoder.h
// Classes: Transcoder
// Purpose: transcoder
// Module: FFmpegAppLib - aplications based on clases from FFmpegCoreLib
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "Muxer.h"
#include "FrameSource.h"

namespace Fwrap
{
    class IMediaSourceInfo;
}

namespace Fcore
{
    class ChannLayout;
}

namespace Fapp
{
    class Transcoder
    {
// data
    private:
        FrameSource  m_FrameSource;
        Fcore::Muxer m_Muxer{};

        std::string m_EncVideo;
        int64_t     m_BitrateVideo{ 0 };
        std::string m_Preset;
        int         m_Crf{ 0 };
        std::string m_OptsVideo;

        std::string m_EncAudio;
        int64_t     m_BitrateAudio{ 0 };
        std::string m_OptsAudio;

        std::string m_Metadata;
        std::string m_MetadataVideo;
        std::string m_MetadataAudio;

        bool        m_Ready{ false };
        bool        m_Closed{ false };

// ctor, dtor, copying
    public:
        explicit Transcoder(bool useFilterGraph);
        ~Transcoder();
// properties
    public:
        double Duration() const;
        bool EndOfData() const;
        bool Error() const;
        double Position() const;
        const Fwrap::IMediaSourceInfo* MediaSourceInfo() const { return m_FrameSource.MediaSourceInfo(); }
// operations       
    public:
        void SetFrameParamsVideo(int width, int height, const char* pixFmt, const char* vidFilter, int fps);
        void SetFrameParamsAudio(int chann, int sampleRate, const char* sampFmt, const char* channLayout, const char* audFilter);
        void SetEncoderParamsVideo(const char* vidEnc, int64_t vidBitrate, const char* preset, int crf, const char* vidOpts);
        void SetEncoderParamsAudio(const char* audEnc, int64_t audBitrate, const char* audOpts);
        void SetMetadata(const char* metadata, const char* metadataVideo, const char* metadataAudio);

        int SetupOutputStreamVideo(AVPixelFormat& pixFmt);
        int SetupOutputStreamAudio(Fcore::ChannLayout& channLayout, AVSampleFormat& sampFmt, int& frameSize);

        int Build(
            const char* srcUrl, const char* srcFormat, const char* srcOptions,
            const char* dstUrl, const char* dstFormat, const char* dstOptions,
            bool useVideo, bool useAudio);

        void CancelInput() { m_FrameSource.CancelInput(); }

        void Run();
        void Pause();
    private:
        int SetupTranscoderVideo();
        int SetupTranscoderAudio();
        void SetupPump();
        void Close();

    }; // class Transcoder

} // namespace Fapp
