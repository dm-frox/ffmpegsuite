#include "pch.h"

#include "OscBuilderData.h"

#include <cmath>

#include "ffmpeg.h"
#include "Logger.h"


namespace Fapp
{
    struct MaxMin
    {
        float Max;
        float Min;

        MaxMin(float max = 0.0f, float min = 0.0f)
            : Max(max)
            , Min(min)
        {}
    };

    const float OscBuilderData::MaxValue = 100.0f;
    const float OscBuilderData::MinValue = -MaxValue;

    //static 
    MaxMin* OscBuilderData::Alloc(int len)
    {
        return static_cast<MaxMin*>(av_mallocz(len * sizeof(MaxMin)));
    }

    //static 
    void OscBuilderData::Free(MaxMin* p)
    {
        av_free(p);
    }

    OscBuilderData::OscBuilderData() = default;

    OscBuilderData::~OscBuilderData()
    {
        Free(m_MaxMins);
    }

    Fcore::IFrameProcessor* OscBuilderData::FrameProcessor()
    {
        return static_cast<Fcore::IFrameProcessor*>(this);
    }

    bool OscBuilderData::Init(int sampleRate, double duration, int oscLen)
    {
        bool ret = false;
        if (!m_Started && !m_Completed && !m_EndOfData)
        {
            if (sampleRate > 0 && duration > 0.0 && oscLen > 0)
            {
                double sampPerPix = (duration / oscLen) * sampleRate;
                if (sampPerPix >= 1.0)
                {
                    if (MaxMin* p = Alloc(oscLen))
                    {
                        m_MaxMins = p;
                        m_OscLen = oscLen;
                        m_ProgressFactor = 1.0 / m_OscLen;
                        m_SampPerPix = sampPerPix;
                        ret = true;
                        LOG_INFO("%s, samp.rate=%d, dur=%3.1f sec, osc.len=%d, samp.per pix=%3.1f",
                            __FUNCTION__, sampleRate, duration, m_OscLen, m_SampPerPix);
                    }
                    else
                    {
                        LOG_ERROR("%s, av_mallocz(), oscLen=%d", __FUNCTION__, oscLen);
                    }
                }
            }
        }
        else
        {
            LOG_ERROR("%s, already started", __FUNCTION__);
        }
        return ret;
    }

    const char* OscBuilderData::Tag() const
    {
        return "OscBuilder";
    }

    void OscBuilderData::ProcFrame(const AVFrame* frame)
    {
        //assert(frame->channels == 1 && (AVSampleFormat)frame->format == AV_SAMPLE_FMT_FLT);

        if (frame)
        {
            if (!m_Completed)
            {
                const void* p = frame->data[0];
                PutData(static_cast<const float*>(p), frame->nb_samples);
            }
        }
        else
        {
            SetEndOfData();
        }
    }

    void OscBuilderData::ProcSample(float f)
    {
        if (f > m_CurrMax)
        {
            m_CurrMax = f;
        }
        else if (f < m_CurrMin)
        {
            m_CurrMin = f;
        }
        ++m_SampCntr;
    }

    void OscBuilderData::SetPixel()
    {
        m_MaxMins[m_PixCntr] = MaxMin(m_CurrMax, m_CurrMin);
        ++m_PixCntr;
    }

    void OscBuilderData::StartNextPixel()
    {
        m_CurrMax = MinValue;
        m_CurrMin = MaxValue;
        m_CurrPixEnd = static_cast<int64_t>(std::ceil(m_SampPerPix * (m_PixCntr + 1)));
        m_Progress = m_PixCntr * m_ProgressFactor;
    }

    void OscBuilderData::PutData(const float* data, int len)
    {

        if (!m_Started)
        {
            StartNextPixel();
            m_Started = true;
        }

        for (const float *pf = data, *end = data + len; pf < end; ++pf) // pf += m_Step)
        {
            ProcSample(*pf);

            if (m_SampCntr == m_CurrPixEnd)
            {
                SetPixel();
                if (m_PixCntr < m_OscLen)
                {
                    StartNextPixel();
                }
                else
                {
                    LOG_INFO("%s, Completed", __FUNCTION__);
                    m_Completed = true;
                    m_Progress = 1.0;
                    break;
                }
            }
        }
    }

    void OscBuilderData::SetEndOfData()
    {
        LOG_INFO("%s, pixCntr=%d, oscLen=%d", __FUNCTION__, m_PixCntr, m_OscLen);

        if ((m_PixCntr < m_OscLen) && (m_CurrMax > MinValue && m_CurrMin < MaxValue))
        {
            m_MaxMins[m_PixCntr] = MaxMin(m_CurrMax, m_CurrMin);
        }
        m_Completed = true;
        m_Progress = 1.0;
        m_EndOfData = true;
    }

} // namespace Fapp

