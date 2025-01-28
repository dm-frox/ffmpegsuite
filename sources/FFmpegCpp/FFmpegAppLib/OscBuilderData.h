#pragma once

// ---------------------------------------------------------------------
// File: OscBuilderData.h
// Classes: OscBuilderData, MaxMin
// Purpose: helpers for the oscillogram builder
// Module: FFmpegAppLib - aplications based on clases from FFmpegCoreLib
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <atomic>

#include "FrameProcIface.h"

namespace Fapp
{

    struct MaxMin;

    class OscBuilderData : Fcore::IFrameProcessor
    {
// types
    private:
        using AtomicBool = std::atomic<bool>;
// data
    private:
        static const float MaxValue;
        static const float MinValue;

        MaxMin* m_MaxMins{ nullptr };

        int m_OscLen{ 0 };
        double m_ProgressFactor{ 0.0 };
        double m_SampPerPix{ 0.0 };

        int64_t m_SampCntr{ 0 };
        int64_t m_PixCntr{ 0 };
        int64_t m_CurrPixEnd{ 0 };
        float m_CurrMax{ 0.0f };
        float m_CurrMin{ 0.0f };

        double m_Progress{ 0.0 };

        bool m_Started{ false };
        bool m_Completed{ false };
        AtomicBool m_EndOfData{ false };
// ctor, dtor, copying
    public:
        OscBuilderData();
        ~OscBuilderData();
// properties
    public:
        const MaxMin* MaxMins() const { return m_MaxMins; }
        int Length() const { return m_OscLen; }
        double Progress() const { return m_Progress; }
        bool EndOfData() const { return (bool)m_EndOfData; }
// operations
    public:
        bool Init(int sampleRate, double duration, int oscLen);
    private:
        void PutData(const float* data, int len);
        void StartNextPixel();
        void SetEndOfData();
        void ProcSample(float f);
        void SetPixel();

        static MaxMin* Alloc(int len);
        static void Free(MaxMin* p);
// IFrameProc impl
    private:
        const char* Tag() const override final;
        void ProcFrame(const AVFrame* frame) override final;

    public:
        IFrameProcessor* FrameProcessor();

    }; // class OscBuilderData

} // namespace Fapp
