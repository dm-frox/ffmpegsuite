#include "pch.h"

#include "FrameTailBuff.h"

FrameTailBuff::FrameTailBuff() = default;
FrameTailBuff::~FrameTailBuff() = default;

void FrameTailBuff::Set(const void* data, int offset, int size)
{
	if (IsAllocated() && data && offset >= 0 && size > 0)
	{
		auto srcBegin = static_cast<const char*>(data) + offset;
		if (size <= BufferSize())
		{
			::memcpy(Buffer(), srcBegin, size);
			m_DataSize = size;
		}
		else // data will be lost
		{
			SBuffer::Zero();
			m_DataSize = BufferSize();
		}
		m_Offset = 0;
	}
}

void FrameTailBuff::Reset()
{
	m_Offset = 0;
	m_DataSize = 0;
}

void FrameTailBuff::MoveFront(int size)
{
	if (size >= 0)
	{
		int newSize = m_DataSize - size;
		if (newSize > 0)
		{
			m_Offset = m_Offset + size;
			m_DataSize = newSize;
		}
		else
		{
			Reset();
		}
	}
}