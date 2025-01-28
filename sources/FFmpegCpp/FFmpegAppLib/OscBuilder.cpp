#include "pch.h"

#include "OscBuilder.h"

#include "Logger.h"
#include "FrameHub.h"

namespace Fapp
{
    const int OscBuilder::Chann           = 1;

    const char OscBuilder::SampleFormat[] = "flt";

    OscBuilder::OscBuilder()
        : m_FrameSource()
    {}

    OscBuilder::~OscBuilder()
    {
        m_FrameSource.Stop(false);
    }

    int OscBuilder::Build(const char* url, const char* fmt, const char* fmtOps, int oscLen)
    {
        m_FrameSource.SetFrameParamsAudio(Chann, 0, SampleFormat, nullptr, nullptr);

        int ret = m_FrameSource.Build(url, fmt, fmtOps,
            false, nullptr, true, nullptr);

        if (ret >= 0)
        {
            if (m_OscBuilderData.Init(
                m_FrameSource.SampleRate(), 
                SourceDuration(),
                oscLen))
            {
                m_FrameSource.SetFrameProcAudio(m_OscBuilderData.FrameProcessor());
                m_FrameSource.CreatePump();
                m_FrameSource.Run();
            }
            else
            {
                ret = -1;
            }
        }
        return ret;
    }

    double OscBuilder::SourceDuration() const
    {
        double ret = m_FrameSource.DurationAudio();
        if (ret == 0.0)
        {
            ret = m_FrameSource.Duration();
        }
        return ret;
    }

    const void* OscBuilder::Data() const
    {
        return m_OscBuilderData.MaxMins();
    }

    int OscBuilder::Length() const
    {
        return m_OscBuilderData.Length();
    }

    double OscBuilder::Progress() const
    {
        return m_OscBuilderData.Progress();
    }

    bool OscBuilder::EndOfData() const
    {
        return m_OscBuilderData.EndOfData();
    }

} // namespace Fapp
