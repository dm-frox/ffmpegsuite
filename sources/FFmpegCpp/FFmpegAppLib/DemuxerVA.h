#pragma once

// ---------------------------------------------------------------------
// File: DemuxerVA.h
// Classes: DemuxerVA
// Purpose: uses Demuxer to support more comfortabile interface for video/audio streams
// Module: FFmpegAppLib - aplications based on clases from FFmpegCoreLib
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <string>

#include "include/MediaSourceInfoIface.h"

#include "ffmpeg.h"
#include "Demuxer.h"

namespace Fwrap
{
    enum class PixelFormat;
    enum class SampleFormat;
}

namespace Fcore
{
    class FilterGraph;
    class Muxer;
    class StreamReader;
    class ChannLayout;
}

namespace Ftool
{
    class ProceedPump;
}

namespace Fapp
{

    class DemuxerVA : Fwrap::IMediaSourceInfo
    {
// types
        using IndPair = std::pair<const int, const int>;
// data
    private:
        static const unsigned int WidthMask;

        static const char* VideoExts[];
        static const char* AudioExts[];

        Fcore::Demuxer m_Demuxer;

        int    m_StrmIndexVideo{ -1 };
        int    m_Width{ 0 };
        int    m_Height{ 0 };
        AVRational m_Sar{0, 1};
        int    m_Fps_N{ 0 };
        int    m_Fps_D{ 0 };
        double m_Fps{ 0.0 };
        AVPixelFormat m_PixelFormat{ AV_PIX_FMT_NONE };
        std::string m_DecoderVideo;
        int64_t m_BitrateVideo{ 0 };

        int     m_StrmIndexAudio{ -1 };
        int     m_Chann{ 0 };
        int     m_SampleRate{ 0 };
        int64_t m_ChannMask{ 0 };
        AVSampleFormat m_SampleFormat{ AV_SAMPLE_FMT_NONE };
        std::string m_DecoderAudio;
        int64_t m_BitrateAudio{ 0 };

        double m_Duration{ 0.0 };
        double m_DurationAudio{ 0.0 };

// ctor, dtor, copying
    public:
        explicit DemuxerVA(bool player);
        ~DemuxerVA();
// properties
    public:
        int GetWidthSar(int width) const;
        bool Proceeding() const { return m_Demuxer.Proceeding(); }
// 
// operations
    public:
        int Open(const char* url, const char* fmt, const char* fmtOps, bool useVideo, bool useAudio);

        inline int OpenDecoderVideo(const char* decoderParams)
        {
            int ret = m_Demuxer.OpenDecoder(m_StrmIndexVideo, decoderParams);
            if (ret >= 0)
            {
                UpdateVideoCodecParams();
            }
            return ret;
        }

        inline int OpenDecoderAudio(const char* decoderParams)
        {
            int ret = m_Demuxer.OpenDecoder(m_StrmIndexAudio, decoderParams);
            if (ret >= 0)
            {
                UpdateAudioCodecParams();
            }
            return ret;
        }

        const Fcore::Decoder& DecoderVideo() const
        {
            return m_Demuxer.GetDecoder(m_StrmIndexVideo);
        }

        const Fcore::Decoder& DecoderAudio() const
        {
            return m_Demuxer.GetDecoder(m_StrmIndexAudio);
        }

        inline int  ConnectToFrameHubVideo(Fcore::FrameHub* frameHub, int outWidth, int outHeight, AVPixelFormat outPixFmt)
        {
            return m_Demuxer.ConnectToFrameHubVideo(m_StrmIndexVideo, frameHub, outWidth, outHeight, outPixFmt);
        }

        inline int  ConnectToFrameHubAudio(Fcore::FrameHub* frameHub, const Fcore::ChannLayout& outLayout, int outSampleRate, AVSampleFormat outSampFmt)
        {
            return m_Demuxer.ConnectToFrameHubAudio(m_StrmIndexAudio, frameHub, outLayout, outSampleRate, outSampFmt);
        }

        inline int  ConnectToFilterGraphVideo(Fcore::FrameHub* frameHub)
        {
            return m_Demuxer.ConnectToFilterGraphVideo(m_StrmIndexVideo, frameHub);
        }

        inline int  ConnectToFilterGraphAudio(Fcore::FrameHub* frameHub)
        {
            return m_Demuxer.ConnectToFilterGraphAudio(m_StrmIndexAudio, frameHub);
        }

        inline int  ConnectToMuxerVideo(Fcore::Muxer* muxer)
        {
            return m_Demuxer.ConnectToMuxerVideo(m_StrmIndexVideo, muxer);
        }

        inline int  ConnectToMuxerAudio(Fcore::Muxer* muxer)
        {
            return m_Demuxer.ConnectToMuxerAudio(m_StrmIndexAudio, muxer);
        }

        AVPixelFormat PixelFormat() const { return m_PixelFormat; }
        AVSampleFormat SampleFormat() const { return m_SampleFormat; }
        double DurationAudio() const { return m_DurationAudio; }

        void Reset() { m_Demuxer.Reset(); }
        void CancelInput() { m_Demuxer.CancelInput(); }
        int SetPosition(double pos) { return m_Demuxer.SetPosition(pos); }

        bool CreatePump() { return m_Demuxer.CreatePump(); }
        void ClosePump() { m_Demuxer.ClosePump(); }
        void Run() { m_Demuxer.Run(); }
        void Pause() { m_Demuxer.Pause(); }
        void Stop(bool skipPause) { m_Demuxer.Stop(skipPause); }
        bool PumpError() const { return m_Demuxer.PumpError(); }

        const Fcore::Demuxer& Demuxer() const { return m_Demuxer; }

        Ftool::ProceedPump* GetAudioPump();

        AVRational TimebaseVideo() const;
        AVRational TimebaseAudio() const;
    private:
        double SetupVideo(int ind);
        double SetupAudio(int ind);
        void SetupDuration(double durV, double durA);
        int SetStreamToSeek(const char* file);
        void UpdateVideoCodecParams();
        void UpdateAudioCodecParams();
        IndPair GetActiveStreams(bool useVideo, bool useAudio) const;

        static bool CheckExtention(const char* file, const char* exts[]);
        static bool IsVideoFile(const char* file) { return CheckExtention(file, VideoExts); }
        static bool IsAudioFile(const char* file) { return CheckExtention(file, AudioExts); }
// IMediaSourceInfo impl
    private:
        bool HasVideo() const override final;
        int VideoIndex() const override final;
        int Width() const override final;
        int Height() const override final;
        int Fps_N() const override final;
        int Fps_D() const override final;
        double Fps() const override final;
        const char* PixelFormatStr() const override final;
        const char* VideoCodec() const override final;
        int64_t VideoBitrate() const override final;

        bool HasAudio() const override final;
        int AudioIndex() const override final;
        int Chann() const override final;
        int SampleRate() const override final;
        const char* SampleFormatStr() const override final;
        int64_t ChannMask() const override final;
        const char* AudioCodec() const override final;
        int64_t AudioBitrate() const override final;

        bool CanSeek() const override final;
        double Duration() const override final;

public:
        const Fwrap::IMediaSourceInfo* MediaSourceInfo() const
        {
            return static_cast<const Fwrap::IMediaSourceInfo*>(this);
        }

    }; // class DemuxerVA

} // namespace Fapp

