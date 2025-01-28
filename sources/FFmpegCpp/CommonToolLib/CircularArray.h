#pragma once

// ---------------------------------------------------------------------
// File: CircularArray.h
// Classes: CircularArray (header only)
// Purpose: limited size FIFO container for pointers
// Module: FFmpegToolsLib - auxiliary classes, functions
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <cstring>
#include <malloc.h>

namespace Ftool
{
    class CircularArray
    {
// types
    public:
        using ElemType      = void*;
        using ElemConstType = const void*;
        using ClearFunc     = void(ElemType);
// data
    private:
        ElemType* m_Buffer{ nullptr };
        int       m_BuffLen{ 0 };
        int       m_DataLen{ 0 };
        int       m_WritePos{ 0 };
        int       m_ReadPos{ 0 };

// memory operations
    private:
        static ElemType* Alloc(int len) { return static_cast<ElemType*>(::malloc(sizeof(ElemType) * len)); }
        static void Free(ElemType* p) { ::free(p); }
        static void Zero(ElemType* p, int count) { std::memset(p, 0, count * sizeof(ElemType)); }
// ctor, dtor, copying
    public:
        explicit CircularArray(int len) { AllocBuff(len); }
        ~CircularArray() { Free(m_Buffer); }
        CircularArray(const CircularArray&) = delete;
        CircularArray& operator=(const CircularArray&) = delete;
        CircularArray(CircularArray&&) = delete;
        CircularArray& operator=(CircularArray&&) = delete;
// properties
    private:
        bool IsNotFull() const { return m_DataLen < m_BuffLen; }
        bool IsNotEmpty() const { return m_DataLen > 0; }
    public:
        bool IsEmpty() const { return !IsNotEmpty(); }
        bool IsFull() const { return !IsNotFull(); }
        int  DataLength() const { return m_DataLen; }
        int  BufferLength() const { return m_BuffLen; }
 // operations
    private:
        void Zero() 
        { 
            Zero(m_Buffer, m_BuffLen); 
        }

        bool AllocBuff(int len)
        {
            bool ret = false;
            if (len > 0)
            {
                if (auto p = Alloc(len))
                {
                    m_Buffer = p;
                    m_BuffLen = len;
                    Zero();
                    ret = true;
                }
            }
            return ret;
        }

        void IncPos(int& pos) const
        {
            ++pos;
            if (pos == m_BuffLen)
            {
                pos = 0;
            }
        }

    public:
        bool AllocBuffer(int len) 
        { 
            return (m_BuffLen == 0) ? AllocBuff(len) : false; 
        }

        int Push(ElemType item)
        {
            int ret = -1;
            if (IsNotFull())
            {
                m_Buffer[m_WritePos] = item;
                IncPos(m_WritePos);
                ret = ++m_DataLen;
            }
            return ret;
        }

        int Pop(ElemType& item)
        {
            int ret = -1;
            if (IsNotEmpty())
            {
                ElemType* rr = m_Buffer + m_ReadPos;
                item = *rr;
                Zero(rr, 1);
                IncPos(m_ReadPos);
                ret = --m_DataLen;
            }
            return ret;
        }

        bool Peek(ElemConstType& item) const
        {
            bool ret = false;
            if (IsNotEmpty())
            {
                item = m_Buffer[m_ReadPos];
                ret = true;
            }
            else
            {
                item = nullptr;
            }
            return ret;
        }

        int Drop(ClearFunc clearFunc)
        {
            ElemType item = nullptr;
            int len = Pop(item);
            if (len >= 0)
            {
                clearFunc(item);
            }
            return len;
        }

        int Shrink(int newLen, ClearFunc clearFunc)
        {
            int nn = (newLen > 0) ? newLen : 0;
            while (m_DataLen > nn)
            {
                void* item = nullptr;
                Pop(item);
                clearFunc(item);
            }
            return m_DataLen;
        }

        int Clear(ClearFunc clearFunc)
        {
            int ret = m_DataLen;
            if (ret > 0)
            {
                Shrink(0, clearFunc);
                m_WritePos = 0;
                m_ReadPos = 0;
            }
            return ret;
        }

    }; // class CircularArray

} // namespace Ftool
