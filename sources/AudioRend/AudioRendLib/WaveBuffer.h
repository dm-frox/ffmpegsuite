#pragma once

// ---------------------------------------------------------------------
// File: WaveBuffer.h
// Classes: WaveBuffer, WaveBuffers
// Purpose: audio buffer for rendering
// Module: AudioRendLib - audio renderer based on windows waveform audio API (winmm.lib)
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <atomic>

#include <mmsystem.h>

#include "SBuffer.h"


class WaveBuffer : public SBuffer
{
// data
private:
    static const uintptr_t IdMask;

    const DWORD       m_Id;
    WAVEHDR	          m_Hdr;
    std::atomic<bool> m_Free{ true };
    int               m_Size{ 0 };

// ctor, dtor, copying
public:
    WaveBuffer();
    ~WaveBuffer();
// properties
public:
    bool IsFree() const { return m_Free; }
    void* Buffer() { return m_Hdr.lpData; }
    WAVEHDR* GetHdr() { return &m_Hdr; }
    bool IsFull() const { return m_Size == SBuffer::BufferSize(); }
private:
    DWORD GetId() const;
// operations
public:
    bool Init(int size);
    void UpdateHdr(bool start);

    void Lock() { m_Free = false; }
    void Unlock() { m_Free = true; }

    void Reset();
    bool WomDone(DWORD dwParam);

    int AddData(const void* data, int size);

}; // class WaveBuffer

class WaveBuffers
{
// data
private:
    static const int Count = 3;

    WaveBuffer m_Buffers[Count];

public:
// ctor, dtor, copying
    WaveBuffers();
    ~WaveBuffers();
// properties
public:
    int BufferSize() const;
    bool BuffersAreFree() const;
// operations
public:
    bool Init(int size);
    WaveBuffer* GetFreeBuffer();
    bool WomDone(DWORD dwParam);
    void Free();

}; // class WaveBuffers
