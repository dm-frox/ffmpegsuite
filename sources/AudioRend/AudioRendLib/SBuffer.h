#pragma once

// ---------------------------------------------------------------------
// File: SBuffer.h
// Classes: SBuffer
// Purpose: base class for WaveBuffer, FrameTailBuff
// Module: AudioRendLib - audio renderer based on windows waveform audio API (winmm.lib)
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <malloc.h>
#include <memory.h>


class SBuffer
{
// data
private:
    char*   m_pBuff{ nullptr };
    int     m_BuffSize{ 0 };

// ctor, dtor, copying
protected:
    SBuffer() = default;
    ~SBuffer() { ::free(m_pBuff); }

    SBuffer(const SBuffer&) = delete;
    SBuffer& operator=(const SBuffer&) = delete;
// properties
protected:
    char* Buffer() { return m_pBuff; }
    const char* Buffer() const { return m_pBuff; }
public:
    int  BufferSize() const { return m_BuffSize; }
    bool IsAllocated() const { return m_BuffSize > 0; }
// operations
protected:
    void Zero()
    {
        if (m_pBuff)
        {
            ::memset(m_pBuff, 0, m_BuffSize);
        }
    }

public:
    bool Alloc(int size)
    {
        bool ret = false;
        if (m_pBuff == nullptr && size >= 0)
        {
            if (void* p = ::malloc(size))
            {
                m_pBuff = static_cast<char*>(p);
                m_BuffSize = size;
                Zero();
                ret = true;
            }
        }
        return ret;
    }

    void Free()
    {
        ::free(m_pBuff);
        m_BuffSize = 0;
    }

}; // class SBuffer

