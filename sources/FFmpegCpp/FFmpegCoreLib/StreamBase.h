#pragma once

// ---------------------------------------------------------------------
// File: StreamBase.h
// Classes: StreamBase
// Purpose: AVStream wrapper, base class for other stream classes
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "ProceedPump.h"

#include "MediaTypeHolder.h"

namespace Ftool
{
    class IProceed;
}

namespace Fcore
{
    class StreamBase : public MediaTypeHolder
    {
// data
    private:
        static const char LangKey[];
        static const char NoLang[];
        static char ThreadTagBuff[];


        AVStream&       m_Stream;
        int64_t const   m_StartTime;

        Ftool::ProceedPump  m_Pump;

        std::string          m_Lang;
// ctor, dtor, copying
    protected:
        StreamBase(AVStream& strm);
        ~StreamBase();
// properties
    public:
        int Index() const { return m_Stream.index; }
        int Id() const { return m_Stream.id; }
        AVRational Timebase() const { return m_Stream.time_base; }
        int  AudioChann() const;

        const AVDictionary* Metadata() const;
        const char* GetMetadataItem(const char* key) const;
        static const char* GetMetadataItem(const AVStream& strm, const char* key);

        Ftool::IPumpSwitchR* PumpSwitchR() { return m_Pump.PumpSwitchR(); }
        Ftool::IPumpSwitchW* PumpSwitchW() { return m_Pump.PumpSwitchW(); }
        const AVStream& Stream() const { return m_Stream; }
        AVStream& Stream() { return m_Stream; }
        const char* Language() const { return m_Lang.c_str(); }
        bool CheckDispositionFlag(int flag) const;
        bool IsDefault() const;
        void AddDispositionFlag(int flag);
        double Duration() const;

        Ftool::ProceedPump* GetPump() { return &m_Pump; }
        bool Proceeding() const { return m_Pump.Proceeding(); }

        const char* CodecIdStr() const;
    protected:
        const AVCodecParameters& CodecParams() const { return *m_Stream.codecpar; }
        bool PumpIsSet() const { return m_Pump.IsActive(); }
        int64_t StartTime() const { return m_StartTime; }
        int  AudioSampleRate() const;
        int64_t Bitrate() const;
        AVRational Sar() const;
// operations
    public:
        int64_t PosSecToPosTs(double posSec) const;
        bool CreatePump();
        void Run();
        void Pause();
        void Stop(bool skipPause);
        void ClosePump();
        bool PumpError() const;

    protected:
        void InitPump(Ftool::IProceed* pr, bool useCondVar, const char* tag, bool logEx = false);
        double PosTsToPosSec(int64_t ts) const;
        const char* ThreadTag(const char* suff) const;
    private:
        void SetLanguage();

    }; // class StreamBase

} // namespace Fcore


