#pragma once

// ---------------------------------------------------------------------
// File: AudioArrMuxer.h
// Classes: AudioArrMuxer, AudioArray
// Purpose: test class to write audio stream with a large number of channels
// Module: FFmpegAppLib - aplications based on clases from FFmpegCoreLib
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2022
// ---------------------------------------------------------------------

#include "Muxer.h"
#include "StreamTime.h"
#include "Misc.h"

#include "FrameSource.h"


namespace Fapp
{

    class AudioArrMuxer
    {
        struct AudioArray
        {
            const AVCodecID CodecId;
            const int Chann;
            const int SampleRate;
            const AVSampleFormat SampFmt;
            const int FrameSize;

            AudioArray(AVCodecID codecId, int chann, int sampleRate, AVSampleFormat sampFmt, int frameSize)
                : CodecId(codecId)
                , Chann(chann)
                , SampleRate(sampleRate)
                , SampFmt(sampFmt)
                , FrameSize(frameSize)
            {}

            int BuffSize() const { return Chann * Fcore::BytesPerSample(SampFmt) * FrameSize; }

            AVRational Timebase() const { return { FrameSize, SampleRate }; }

            void Log(const char* msg) const
            {
                LOG_INFO("%s, AudioArray: cod.id=%s, chann=%d, samp.rate=%d, samp.fmt=%s, frm.size=%d, buff.size=%d", msg,
                    Fcore::CodecIdToStr(CodecId), Chann, SampleRate, Fcore::SampFmtToStr(SampFmt), FrameSize, BuffSize());
            }
        };

        static const double WriteDataThreshold;
        static const int    WaitWriteDataIntervalMs;

        FrameSource  m_FrameSourceV{};
        FrameSource  m_FrameSourceA{};
        Fcore::Muxer m_Muxer{};

        bool m_Ready{ false };
        bool m_Closed{ false };

        int m_DataStrmIndex{ -1 };

        AudioArray        m_AudioArray;
        Fcore::PacketTime m_PacketTime;
        AVPacket*         m_DataPacket;

        std::thread m_DataThread{};
        std::atomic<bool> m_DataEnd{ false };
        std::atomic<bool> m_DataErr{ false };

    public:
        AudioArrMuxer();
        ~AudioArrMuxer();

        int Build(const char* urlVideo, const char* urlAudio, const char* urlDst);
        void Close();
        bool EndOfData() const;
        bool Error() const;

    private:
        void SetupPump();
        void Run();
        void Pause();
        double Position() const;

        bool SetData(int i);
        double SetTimestamps(int i);
        void WriteData(int n);
        const void* GetData(int i);

    }; // class AudioArrMuxer

} // namespace Fapp

