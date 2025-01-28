#pragma once

// ---------------------------------------------------------------------
// File: QueueBase.h
// Classes: QueueBase
// Purpose: based on CircularArray FIFO container with synchronization and pump control, 
// Module: FFmpegToolsLib - auxiliary classes, functions
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <mutex>

#include "CircularArray.h"
#include "PumpCtrl.h"

namespace Ftool
{

    class QueueBase
    {
// types
    private:
        using ElemType      = CircularArray::ElemType;
        using ElemConstType = CircularArray::ElemConstType;
        using ClearFunc     = CircularArray::ClearFunc;
        using Mutex         = std::mutex;
        using AutoLock      = std::lock_guard<Mutex>;

// data
    private:
        Mutex&          m_Mutex;
        CircularArray   m_Queue;

        ReadPumpCtrl    m_ReadPumpCtrl;
        WritePumpCtrl   m_WritePumpCtrl;
 // ctor, dtor, copying
    protected:
        QueueBase(Mutex& mutex, int buffLen);
        ~QueueBase();
// properties
    public:
        int  BufferLength() const { return m_Queue.BufferLength(); }
        int  DataLength() const;
        int  DataLengthNoSync() const { return m_Queue.DataLength(); }
// operations
    public:
        bool AllocBuffer(int size) { return m_Queue.AllocBuffer(size); }
        void EnableReadPumpCtrl(IPumpSwitchR* pumpSwitch);
        void EnableWritePumpCtrl(IPumpSwitchW* pumpSwitch, int thresholdOff, bool strict = true);
    protected:
        bool Push(ElemType item);
        bool Pop(ElemType& item);
        bool Peek(ElemConstType& item) const;
        void Drop(ClearFunc clearFunc);
        int Clear(ClearFunc clearFunc);
        int Shrink(int len, ClearFunc clearFunc);
    private:
        bool OnPush(int len);
        bool OnPop(int len);

    }; // class QueueBase

} // namespace Ftool

