#pragma once

// ---------------------------------------------------------------------
// File: StreamListBase.h
// Classes: StreamListBaseT (template), base class for StreamListR, StreamListW
// Purpose: base container to store pointers to StreamReader or StreamWriter
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "VectorPtr.h"

namespace Fcore
{
    class StreamBase;

    template<class T> // T - StreamReader or StreamWriter
    class StreamListBaseT : public VectorPtrT<T>
    {
// types
    private:
        using Base = VectorPtrT<T>;

 // ctor, dtor, copying
    protected:
        StreamListBaseT() = default;
        ~StreamListBaseT() = default;

// operations
    protected:
        void ForEach(void (T::*pmem)())
        {
            for (T* pstrm : Base::Data())
            {
                if (pstrm->IsActivated())
                {
                    (pstrm->*pmem)();
                }
            }
        }

        template<typename S>
        void ForEach(void (T::*pmem)(S), S s)
        {
            for (T* pstrm : Base::Data())
            {
                if (pstrm->IsActivated())
                {
                    (pstrm->*pmem)(s);
                }
            }
        }

        bool CheckAll(bool (T::*pmem)() const) const
        {
            bool ret = true;
            for (const T* pstrm : Base::Data())
            {
                if (pstrm->IsActivated())
                {
                    if (!(pstrm->*pmem)())
                    {
                        ret = false;
                        break;
                    }
                }
            }
            return ret;
        }

        void ForEach_(void (StreamBase::*pmem)(bool), bool par)
        {
            for (T* pstrm : Base::Data())
            {
                if (pstrm->NeedsPump())
                {
                    (pstrm->*pmem)(par);
                }
            }
        }

        void ForEach_(void (StreamBase::* pmem)())
        {
            for (T* pstrm : Base::Data())
            {
                if (pstrm->NeedsPump())
                {
                    (pstrm->*pmem)();
                }
            }
        }

    public:
        bool CreatePump()
        {
            bool ret = true;
            for (T* pstrm : Base::Data())
            {
                if (pstrm->NeedsPump())
                {
                    if (!pstrm->CreatePump())
                    {
                        ret = false;
                        break;
                    }
                }
            }
            return ret;
        }

        bool PumpError() const
        {
            bool ret = false;
            for (const T* pstrm : Base::Data())
            {
                if (pstrm->NeedsPump())
                {
                    if (pstrm->PumpError())
                    {
                        ret = true;
                        break;
                    }
                }
            }
            return ret;
        }

        void Run()
        {
            ForEach_(&StreamBase::Run);
        }

        void Pause()
        {
            ForEach_(&StreamBase::Pause);
        }

        void Stop(bool skipPause)
        {
            ForEach_(&StreamBase::Stop, skipPause);
        }

        void ClosePump()
        {
            ForEach_(&StreamBase::ClosePump);
        }

    }; // class StreamListBaseT

} // namespace Fcore
