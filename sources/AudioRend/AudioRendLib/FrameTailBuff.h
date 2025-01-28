#pragma once

// ---------------------------------------------------------------------
// File: FrameTailBuff.h
// Classes: FrameTailBuff
// Purpose: auxiliary buffer
// Module: AudioRendLib - audio renderer based on windows waveform audio API (winmm.lib)
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2024
// ---------------------------------------------------------------------

#include "SBuffer.h"

class FrameTailBuff : public SBuffer
{
// data
private:
	int m_DataSize{ 0 };
	int m_Offset{ 0 };

// ctor, dtor, copying
public:
	FrameTailBuff();
    ~FrameTailBuff();
// properties
public:
	const void* Data() const { return Buffer() + m_Offset; }
	int DataSize() const { return m_DataSize; }
	bool IsEmpty() const { return m_DataSize == 0; }
// operations
public:
	void Set(const void* data, int offset, int size);
	void Reset();
	void MoveFront(int size);
};

