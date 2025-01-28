#include "pch.h"

#include "WaveBuffer.h"

const uintptr_t WaveBuffer::IdMask = 0xFFFF'FFFF;

WaveBuffer::WaveBuffer()
    : m_Id(GetId())
{
    ::memset(&m_Hdr, 0, sizeof m_Hdr);
}

DWORD WaveBuffer::GetId() const
{
    auto ptr = reinterpret_cast<uintptr_t>(&m_Hdr);
    return static_cast<DWORD>(ptr & IdMask);  // ptr -> DWORD !!
}

WaveBuffer::~WaveBuffer() = default;

bool WaveBuffer::Init(int size)
{
    bool ret = false;
    if (size > 0)
    {
        if (SBuffer::Alloc(size))
        {
            m_Hdr.lpData = SBuffer::Buffer();
            Unlock();
            ret = true;
        }
    }
    return ret;
}

void WaveBuffer::Reset()
{
    SBuffer::Zero();
    m_Size = 0;
}

void WaveBuffer::UpdateHdr(bool start)
{
    m_Hdr.dwBufferLength = m_Size;
    m_Hdr.dwFlags = start ? WHDR_BEGINLOOP : 0;
    m_Hdr.dwLoops = start ? 1 : 0;
}

bool WaveBuffer::WomDone(DWORD dwParam)
{
    if (dwParam == m_Id)
    {
        Unlock();
        return true;
    }
    return false;
}

int WaveBuffer::AddData(const void* data, int size)
{
    int ret = 0;
    if (data && size > 0)
    {
        int rest = SBuffer::BufferSize() - m_Size;
        if (rest > 0)
        {
            int toAdd = (size <= rest) ? size : rest;
            ::memcpy(SBuffer::Buffer() + m_Size, data, toAdd);
            m_Size += toAdd;
            ret = toAdd;
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------------
// 

WaveBuffers::WaveBuffers() = default;

WaveBuffers::~WaveBuffers() = default;

int WaveBuffers::BufferSize() const
{
    return m_Buffers[0].BufferSize();
}

bool WaveBuffers::BuffersAreFree() const
{
    for (const auto& buff : m_Buffers)
    {
        if (!buff.IsFree())
            return false;
    }
    return true;
}

bool WaveBuffers::Init(int size)
{
    for (auto& buff : m_Buffers)
    {
        if (!buff.Init(size))
            return false;
    }
    return true;
}

WaveBuffer* WaveBuffers::GetFreeBuffer()
{
    for (auto& buff : m_Buffers)
    {
        if (buff.IsFree())
            return &buff;
    }
    return nullptr;
}

bool WaveBuffers::WomDone(DWORD dwParam)
{
    for (auto& buff : m_Buffers)
    {
        if (buff.WomDone(dwParam))
            return true;
    }
    return false;
}

void WaveBuffers::Free()
{
    for (auto& buff : m_Buffers)
    {
        buff.Unlock();
        buff.Free();
    }
}
