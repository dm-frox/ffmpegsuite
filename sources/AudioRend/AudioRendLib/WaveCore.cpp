#include "pch.h"

#include "WaveCore.h"

#include "include/FrameSinkAudioIface.h"

#include <mmreg.h>

#include <string>

// ---------------------------------------------------------------------

template<typename T>
T Clamp0(T val, T maxVal)
{
    T ret = val > (T)0 ? val : (T)0;
    return ret > maxVal ? maxVal : ret;
}

std::string ErrCodeToStr(MMRESULT res, const char* msg);

// ---------------------------------------------------------------------

const DWORD WaveCore::InvalidPos  = ULONG_MAX;

const int WaveCore::DefBuffDurationMs = 200;
const int WaveCore::BuffAlignSamp     = 32;

const int  WaveCore::MaxVolume    = 0xFFFF;
const double WaveCore::MaxVolumeD = MaxVolume;

const int WaveCore::WaitMarker = 11;

DWORD WaveCore::MakeVolume(int vol)
{
    return MAKELONG(vol, vol);
}

// MMRESULT waveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume);
// dwVolume
// The low - order word contains the left - channel volume setting, 
// and the high - order word contains the right - channel setting.
// A value of 0xFFFF represents full volume, and a value of 0x0000 is silence.

WaveCore::WaveCore()
    : m_Volume(MakeVolume(MaxVolume))
{}

WaveCore::~WaveCore() = default;

bool WaveCore::SetFrameSrc(IFrameSrc* src)
{
    bool ret = false;
    if (!m_WaveOut.IsValid())
    {
        m_FrameSrc = src;
        ret = true;
    }
    return ret;
}

bool WaveCore::InitBuffers(const WAVEFORMATEX& fmt, int buffDurMs)
{
    int buffDur = (buffDurMs > 0) ? buffDurMs : DefBuffDurationMs;
    int buffSize = fmt.nAvgBytesPerSec * buffDur / 1000;
    int buffAlign = fmt.nBlockAlign * BuffAlignSamp;
    buffSize = ((buffSize / buffAlign) + 1) * buffAlign;
    return m_WaveBuffers.Init(buffSize);
}

bool WaveCore::Open(int chann, int sampleRate, int bytesPerSample, int buffDurMs, int devId)
{
    bool ret = false;
    if (!m_WaveOut.IsValid() && m_FrameSrc)
    {
        if (((chann == 1) || (chann == 2)) 
            && (sampleRate > 0) 
            && ((bytesPerSample == 2) || (bytesPerSample == 4)))
        {
            WAVEFORMATEX  fmt;
            WaveFormat_Fill(chann, sampleRate, bytesPerSample, fmt);

            if (WaveOutOpen(fmt, devId) && InitBuffers(fmt, buffDurMs))
            {
                m_Chann = chann;
                m_SampleRate = sampleRate;
                m_Bps = bytesPerSample;

                m_State = PlayerState::Stopped;
                m_RendStarted = false;
                SetPositionType(fmt);
                SetVolume();

                ret = true;
            }
        }
    }
    return ret;
}

void WaveCore::Close()
{
    AutoLock al(m_MtxWave);

    m_WaveOut.Delete();
    m_WaveBuffers.Free();
    m_Chann = 0;
    m_SampleRate = 0;
    m_Bps = 0;
    m_State = PlayerState::OutOfSource;
}

bool WaveCore::WaveOutOpen(const WAVEFORMATEX& fmt, int devId)
{
    bool ret = false;

    AutoLock al(m_MtxWave);

    DWORD flags = CALLBACK_FUNCTION;
    HWAVEOUT h = (HWAVEOUT)nullptr;
    MMRESULT res = ::waveOutOpen(&h, 
        (devId >= 0) ? (UINT)devId : WAVE_MAPPER,
        &fmt, 
        (DWORD_PTR)WaveCore::WaveOutProc, 
        (DWORD_PTR)this, 
        flags);

    if (MMR2Bool(res, "waveOutOpen"))
    {
        m_WaveOut.Assign(h);
        ret = true;
    }
    return ret;
}

void WaveCore::SetPositionType(const WAVEFORMATEX& fmt)
{
    memset(&m_MMTIME, 0, sizeof m_MMTIME);
    m_MMTIME.wType = TIME_SAMPLES;
    m_PositionFactor = 1.0 / fmt.nSamplesPerSec; // to seconds
}

DWORD WaveCore::Position()
{
    DWORD ret = InvalidPos;
    if (m_WaveOut.IsValid())
    {
        AutoLock al(m_MtxWave);
        MMRESULT res = ::waveOutGetPosition(m_WaveOut, &m_MMTIME, sizeof MMTIME);
        if (MMR2Bool(res, "waveOutGetPosition"))
        {
            ret = m_MMTIME.u.sample;
        }
    }
    return ret;
}

double WaveCore::PositionSec()
{
    DWORD pos = Position();
    return (pos != InvalidPos) 
        ? (double)pos * m_PositionFactor 
        : 0.0;
}

bool WaveCore::EndPause()
{
    AutoLock al(m_MtxWave);

    if (m_State == PlayerState::Paused)
    {
        MMRESULT res = ::waveOutRestart(m_WaveOut);
        if (MMR2Bool(res, "waveOutRestart"))
        {
            m_State = PlayerState::Playing;
        }
    }

    return m_State == PlayerState::Playing;
}

bool WaveCore::BeginPause()
{
    AutoLock al(m_MtxWave);

    if (m_State == PlayerState::Playing)
    {
        MMRESULT res = ::waveOutPause(m_WaveOut);
        if (MMR2Bool(res, "waveOutPause"))
        {
            m_State = PlayerState::Paused;
        }
    }

    return m_State == PlayerState::Paused;
}

bool WaveCore::Reset()
{
    bool ret = false;

    if (m_WaveOut.IsValid())
    {
        AutoLock al(m_MtxWave);

        MMRESULT res = ::waveOutReset(m_WaveOut);
        ret = MMR2Bool(res, "waveOutReset");
        m_State = PlayerState::Stopped;
        m_RendStarted = false;
    }

    return ret;
}

bool WaveCore::Stop()
{
    AutoLock al(m_MtxWave);

    if (m_State == PlayerState::Playing || m_State == PlayerState::Paused)
    {
        MMRESULT res = ::waveOutReset(m_WaveOut);
        if (MMR2Bool(res, "waveOutReset"))
        {
            m_State = PlayerState::Stopped;
        }
    }

    return (m_State == PlayerState::Stopped);
}

int WaveCore::Volume()
{
    int ret = 0;

    DWORD vol = 0;
    MMRESULT res = ::waveOutGetVolume(m_WaveOut, &vol);
    if (MMR2Bool(res, "waveOutGetVolume"))
    {
        ret = vol & MaxVolume;
    }
    return ret;
}

bool WaveCore::SetVolume()
{
    bool ret = false;
    if (IsReady())
    {
        MMRESULT res = ::waveOutSetVolume(m_WaveOut, m_Volume);
        if (MMR2Bool(res, "waveOutSetVolume"))
        {
            ret = true;
        }
    }
    return ret;
}

bool WaveCore::SetVolume(int volume)
{
    int x = Clamp0(volume, MaxVolume);
    m_Volume = MakeVolume(x);
    return SetVolume();
}

bool WaveCore::SetVolumeSq(double volLin) 
{
    double vol = Clamp0(volLin, 1.0); // 0.0 <= vol && vol <= 1.0
    double vol2 = vol * vol;
    int volume = (int)(vol2 * MaxVolumeD);
    return SetVolume(volume);
}

bool WaveCore::WriteBuffer(WAVEHDR* hdr)
{
    bool ret = false;
    AutoLock al(m_MtxWave);

    MMRESULT res = ::waveOutPrepareHeader(m_WaveOut, hdr, sizeof WAVEHDR);
    if (MMR2Bool(res, "waveOutPrepareHeader"))
    {
        res = ::waveOutWrite(m_WaveOut, hdr, sizeof WAVEHDR);
        ret = MMR2Bool(res, "waveOutWrite");
    }
    return ret;
}

bool WaveCore::WriteBuffer(WaveBuffer* buff)
{
    bool ret = false;
    bool start = !m_RendStarted;
    buff->UpdateHdr(start);
    buff->Lock();
    if (WriteBuffer(buff->GetHdr()))
    {
        if (start)
        {
            m_State = PlayerState::Playing;
            m_RendStarted = true;
            Log("begin rendering", 0);
        }
        ret = true;
    }
    else
    {
        buff->Unlock();
    }
    return ret;
}

WaveBuffer* WaveCore::GetFreeBuffer()
{
    WaveBuffer* ret = m_WaveBuffers.GetFreeBuffer();
    if (ret)
    {
        ret->Reset();
    }
    return ret;
}

bool WaveCore::BuffersAreFree() const
{ 
    return m_WaveBuffers.BuffersAreFree();
}

int WaveCore::BufferSize() const
{ 
    return m_WaveBuffers.BufferSize();
}

void WaveCore::WomDone(DWORD dwParam)
{
    AutoLock al(m_MtxCallback);

    if (m_WaveBuffers.WomDone(dwParam))
    {
        m_FrameSrc->NotifyFreeBuffer(true);
    }
}

void WaveCore::Log(const char* str, int r) const
{
    m_FrameSrc->Log(str, r);
}

// static
void CALLBACK WaveCore::WaveOutProc(
        HWAVEOUT /*hwo*/, 
        UINT uMsg,         
        DWORD_PTR dwInstance,  
        DWORD dwParam1,    
        DWORD /*dwParam2*/ )
{
    if (uMsg == WOM_DONE)
    {
        auto p = reinterpret_cast<WaveCore*>(dwInstance);
        p->WomDone(dwParam1);
    }
}

//WOM_DONE 
//dwParam1 = (DWORD) lpwvhdr 
//dwParam2 = reserved 

bool WaveCore::MMR2Bool(MMRESULT res, const char* msg) const
{
    if (res != MMSYSERR_NOERROR)
    {
        Log(ErrCodeToStr(res, msg).c_str(), res);
        return false;
    }
    return true;
}

// static
void WaveCore::WaveFormat_Fill(int chann, int sampleRate, int bytesPerSample, WAVEFORMATEX& fmt)
{
    memset(&fmt, 0, sizeof fmt);

    fmt.nChannels       = static_cast<WORD>(chann);
    fmt.nSamplesPerSec  = sampleRate;
    fmt.nBlockAlign     = static_cast<WORD>(bytesPerSample * fmt.nChannels);
    fmt.nAvgBytesPerSec = fmt.nBlockAlign * fmt.nSamplesPerSec;
    fmt.wBitsPerSample  = static_cast<WORD>(bytesPerSample * 8);

    WORD fTag = WAVE_FORMAT_UNKNOWN;
    switch (bytesPerSample)
    {
    case 2:
        fTag = WAVE_FORMAT_PCM;
        break;
    case 4:
        fTag = WAVE_FORMAT_IEEE_FLOAT;
        break;
    }
    fmt.wFormatTag = fTag;
}

