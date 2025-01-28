#include "pch.h"

#include "FrameSinkAudioImpl.h"

#include "PumpSwitchIface.h"


const int FrameSinkAudioImpl::MaxFrameSize = 1024 * 256;


FrameSinkAudioImpl::FrameSinkAudioImpl() = default;
FrameSinkAudioImpl::~FrameSinkAudioImpl() = default;

void FrameSinkAudioImpl::Detach()
{
    delete this;
}

bool FrameSinkAudioImpl::SetFrameSrc(IFrameSrc* src)
{
    bool ret = false;

    if (m_WaveCore.SetFrameSrc(src))
    {
        m_FrameSrc = src;
        m_AudioWatch.Reset();
        ret = true;
    }
    return ret;
}

bool FrameSinkAudioImpl::Open(int chann, int sampleRate, int bytesPerSample, int buffDurMs, int devId)
{
    bool ret = m_WaveCore.Open(chann, sampleRate, bytesPerSample, buffDurMs, devId);
    if (ret)
    {
        if (!m_FrameTailBuff.IsAllocated())
        {
            ret = m_FrameTailBuff.Alloc(MaxFrameSize);
        }
    }
    return ret;
}

void FrameSinkAudioImpl::Close()
{
    m_WaveCore.Close();
}

int FrameSinkAudioImpl::Chann() const
{
    return m_WaveCore.Chann();
}

int FrameSinkAudioImpl::SampleRate() const
{
    return m_WaveCore.SampleRate();
}

int FrameSinkAudioImpl::BytesPerSample() const
{
    return m_WaveCore.BytesPerSample();
}

int  FrameSinkAudioImpl::RendBufferSize() const
{
    return m_WaveCore.BufferSize();
}

void FrameSinkAudioImpl::SetVolume(double vol)
{
    m_WaveCore.SetVolumeSq(vol);
}

double FrameSinkAudioImpl::Position()
{
    return !m_AudioWatch.Enabled() ? m_WaveCore.PositionSec() : m_AudioWatch.Position();
}

void FrameSinkAudioImpl::EnableAudioWatch()
{
    if (!m_AudioWatch.Enabled())
    {
        m_AudioWatch.Enable(m_WaveCore.PositionSec());
    }
}

void FrameSinkAudioImpl::Run()
{
    m_WaveCore.EndPause();
    m_AudioWatch.Run();
}

void FrameSinkAudioImpl::Pause()
{
    m_WaveCore.BeginPause();
    m_AudioWatch.Pause();
}

void FrameSinkAudioImpl::Stop()
{
    m_WaveCore.Reset();
    m_AudioWatch.Reset();
    m_FrameTailBuff.Reset();
}

void FrameSinkAudioImpl::DisablePumpWrite()
{
    m_FrameSrc->NotifyFreeBuffer(false);
}

void FrameSinkAudioImpl::AddFrameToCurrBuff(const void* data, int size)
{
    int written = m_CurrBuff->AddData(data, size);
    if (written < size)
    {
        m_FrameTailBuff.Set(data, written, size - written);
    }
}

void FrameSinkAudioImpl::AddTailToCurrBuff()
{
    int written = m_CurrBuff->AddData(m_FrameTailBuff.Data(), m_FrameTailBuff.DataSize());
    m_FrameTailBuff.MoveFront(written);
}

bool FrameSinkAudioImpl::WriteFrame(const void* data, int size, bool& done)
{
    bool ret = true;
    bool cpd = false;

    if (!m_CurrBuff)
    {
        m_CurrBuff = m_WaveCore.GetFreeBuffer();
    }

    if (m_CurrBuff)
    {
        if (!m_FrameTailBuff.IsEmpty())
        {
            AddTailToCurrBuff();
        }

        if (m_FrameTailBuff.IsEmpty() && !m_CurrBuff->IsFull())
        {
            AddFrameToCurrBuff(data, size);
            cpd = true;
        }

        if (m_CurrBuff->IsFull())
        {
            if (m_WaveCore.WriteBuffer(m_CurrBuff))
            {
                m_CurrBuff = nullptr;
            }
            else
            {
                ret = false;
            }
        }
    }
    else
    {
        DisablePumpWrite();
    }

    done = cpd;
    return ret;
}

bool FrameSinkAudioImpl::SweepOutBuffers(bool& done)
{
    bool ret = true;
    bool end = false;
    if (!m_CurrBuff && !m_FrameTailBuff.IsEmpty())
    {
        m_CurrBuff = m_WaveCore.GetFreeBuffer();
        if (m_CurrBuff)
        {
            AddTailToCurrBuff();
        }
    }

    if (m_CurrBuff)
    {
        if (m_WaveCore.WriteBuffer(m_CurrBuff))
        {
            m_CurrBuff = nullptr;
        }
        else
        {
            ret = false;
        }
    }

    if (m_FrameTailBuff.IsEmpty())
    {
        if (m_WaveCore.BuffersAreFree())
        {
            end = true;
        }
        else
        {
            DisablePumpWrite();
        }
    }

    done = end;
    return ret;
}


// factory

void* CreateFrameSinkAudio()
{
    return static_cast<Fwrap::IFrameSinkAudio*>(new FrameSinkAudioImpl());
}