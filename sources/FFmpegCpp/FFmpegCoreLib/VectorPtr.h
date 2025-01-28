#pragma once

// ---------------------------------------------------------------------
// File: VectorPtr.h
// Classes: VectorPtrT (template)
// Purpose: base container to store pointers to StreamReader, StreamWriter and other objects
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <vector>

#include "NonCopyable.h"

namespace Fcore
{

    template<class T> 
    class VectorPtrT : NonCopyable
    {
// types
        using VectorImpl = std::vector<T*>;
// data
        static const int DefCapacity = 2;

        VectorImpl m_Data;

    protected:
        const VectorImpl& Data() const { return m_Data; }
// ctor, dtor, copying
    public:
        VectorPtrT() = default;

        ~VectorPtrT()
        {
            for (T* itr : m_Data)
            {
                delete itr;
            }
        }
// properties
    public:
        int  Count() const { return (int)m_Data.size(); }
        bool IsEmpty() const { return m_Data.empty(); }
        bool IndexIsValid(int idx) const { return 0 <= idx && idx < Count(); }
// operations
    public:
        void Reserve(int n) { m_Data.reserve((n > 0) ? n : DefCapacity); }

        T& Back() { return *m_Data.back(); }
        const T& Back() const { return *m_Data.back(); }

        T& operator[](int idx) { return *m_Data[idx]; }
        const T& operator[](int idx) const { return *m_Data[idx]; }

        void AppendItem(T* s)
        {
            m_Data.push_back(s);
        }

    }; // class VectorPtrT

} // namespace Fcore

