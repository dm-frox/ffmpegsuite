#pragma once

// ---------------------------------------------------------------------
// File: ImplPtr.h
// Classes: DllFlag, ImplPtr
// Purpose: managed template, smart pointer to a native implementation
// Module: FFmpegInterop - managed wrappers for native object from FFmpegWrapper
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

//#define USE_FINALIZATOR_BLOCKING_

namespace FFmpegInterop
{
    public ref class DllFlag
    {
        static bool Loaded_ = false;
    public:
        static void SetLoaded(bool loaded)
        {
            Loaded_ = loaded;
        }
        static bool Loaded()
        {
            return Loaded_;
        }

    }; // ref class DllFlag

    template <typename T>
    public ref class ImplPtr : System::IDisposable
    {
        T* const m_Ptr;

        void Delete()
        {
            if (m_Ptr)
            {
                m_Ptr->Delete();
            }
        }

        ~ImplPtr() { Delete(); }
#ifdef USE_FINALIZATOR_BLOCKING_
        !ImplPtr() { if (DllFlag::Loaded()) Delete(); }
#else
        !ImplPtr() { Delete(); }
#endif

    protected:

        ImplPtr(T* p) : m_Ptr(p) {}

        T* Ptr() { return m_Ptr; }

    }; // ref class ImplPtr

} // namespace FFmpegInterop
