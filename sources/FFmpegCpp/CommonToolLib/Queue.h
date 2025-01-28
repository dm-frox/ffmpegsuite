#pragma once

// ---------------------------------------------------------------------
// File: Queue.h
// Classes: QueueT (template)
// Purpose: based on QueueBase FIFO container for typed pointers 
// Module: FFmpegToolsLib - auxiliary classes, functions
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "QueueBase.h"

namespace Ftool
{
    template<typename T, void(*PClearFunc)(T*)>
    class QueueT : public QueueBase
    {
// types
    private:
        using CT   = const T;
        using Base = QueueBase;

    public:
        template<typename TT>
        struct ElemEx
        {
            TT* const  Ptr;
            bool const Valid;

            ElemEx(TT* p, bool valid) : Ptr(p), Valid(valid) {}
            explicit operator bool() const { return Valid; }
        };

// ctor, dtor, copying
    public:
        QueueT(std::mutex& mutex, int buffLen)
            : Base(mutex, buffLen)
        {}

        ~QueueT()
        {
            Base::Clear(CF);
        }
// operations
    private:
        static void CF(void* p)
        {
            (*PClearFunc)(static_cast<T*>(p));
        }
    public:
        bool Push(T* item)
        {
            return Base::Push(item);
        }

        ElemEx<T> Pop()
        {
            void* pp = nullptr;
            bool res = Base::Pop(pp);
            return { static_cast<T*>(pp), res };
        }

        ElemEx<CT> Peek() const
        {
            const void* pp = nullptr;
            bool res = Base::Peek(pp);
            return { static_cast<CT*>(pp), res };
        }

        void Drop()
        {
            Base::Drop(CF);
        }

        int Clear()
        {
            return Base::Clear(CF);
        }

        int Shrink(int newLen)
        {
            return Base::Shrink(newLen, CF);
        }

    }; // template class QueueT

} // namespace Ftool

