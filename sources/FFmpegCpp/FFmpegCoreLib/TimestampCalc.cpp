#include "pch.h"

#include "TimestampCalc.h"

namespace Fcore
{
    TimestampCalc::TimestampCalc() = default;

    TimestampCalc::TimestampCalc(AVRational srcTb)
        : m_SrcTb(srcTb)
    {}

    TimestampCalc::~TimestampCalc() = default;

    void TimestampCalc::Init(AVRational srcTb, AVRational dstTb)
    {
        m_SrcTb = srcTb;
        m_DstTb = dstTb;

        CalcFactors();
    }

    void TimestampCalc::InitSrc(AVRational srcTb)
    {
        m_SrcTb = srcTb;
    }

    void TimestampCalc::InitDst(AVRational dstTb)
    {
        m_DstTb = dstTb;

        CalcFactors();
    }

    void TimestampCalc::CalcFactors()
    {
        m_Factor = m_SrcTb.num * m_DstTb.den;
        m_Denom  = m_SrcTb.den * m_DstTb.num;
        m_AreEqual = (m_SrcTb.num == m_DstTb.num) && (m_SrcTb.den == m_DstTb.den);
    }

} // namespace Fcore
