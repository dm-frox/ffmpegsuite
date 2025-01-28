#pragma once

// -------------------------------------------------------------------- -
// File: TimestampCalc.h
// Classes: TimestampCalc
// Purpose: helper to recalc timestamps
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "ffmpeg.h"
#include "NonCopyable.h"

namespace Fcore
{
    class TimestampCalc : NonCopyable
    {
// data
    protected:
        AVRational m_SrcTb{ 0, 0 };
        AVRational m_DstTb{ 0, 0 };

    private:
        int m_Factor{ 0 };
        int m_Denom{ 0 };

        bool m_AreEqual{ false };
// ctor, dtor, copying
    protected:
        TimestampCalc();
        TimestampCalc(AVRational srcTb);
        ~TimestampCalc();
// properties
    public:
        bool SrcIsSet() const { return m_SrcTb.den > 0; }
        int SrcTb_N() const { return m_SrcTb.num; }
        int SrcTb_D() const { return m_SrcTb.den; }
        int DstTb_N() const { return m_DstTb.num; }
        int DstTb_D() const { return m_DstTb.den; }
        bool IsReady() const { return m_Denom > 0 && m_Factor > 0; }
// operations
    protected:
        void Init(AVRational srcTb, AVRational dstTb);
        void InitSrc(AVRational srcTb);
        void InitDst(AVRational dstTb);

        inline int64_t SrcToDst(int64_t ts) const { return m_AreEqual ? ts : ts * m_Factor / m_Denom; }
        inline double GetDstTimeSec(int64_t ts) const { return (m_DstTb.den > 0) ? (double)(ts * m_DstTb.num) / m_DstTb.den : 0.0; }

    private:
        void CalcFactors();

    }; // class TimestampCalc

} // namespace Fcore
