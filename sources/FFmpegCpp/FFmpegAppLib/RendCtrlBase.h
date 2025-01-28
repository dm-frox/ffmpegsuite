#pragma once

// ---------------------------------------------------------------------
// File: RendCtrlBase.h
// Classes: base class for RendCtrlVideo, RendCtrlAudio
// Purpose: class to support video/audio rendering
// Module: FFmpegAppLib - aplications based on clases from FFmpegCoreLib
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <atomic>

#include "ProceedIface.h"
#include "ProceedPump.h"
#include "MediaTypeHolder.h"
#include "FrameDataIface.h"

namespace Fapp
{
    class RendCtrlBase : public Fcore::MediaTypeHolder
    {
// types
    protected:       
        using IFrameData = Fcore::IFrameData;
        using IProceed   = Ftool::IProceed;

// data
    private:
        std::atomic<double> m_RendPos{ 0.0 };

    protected:
        static const int  MaxSleepInterval;

        Ftool::ProceedPump m_Pump;
        IFrameData*        m_FrameHub{ nullptr };
        bool               m_HoldPresentationTime{ false };

// ctor, dtor, copying
    public:
        RendCtrlBase(AVMediaType mediaType);
        ~RendCtrlBase();
// properties
    public:
        bool HasFrameHub() const { return m_FrameHub != nullptr; }
        bool Proceeding() const { return m_Pump.Proceeding(); }
        double RendPos() const { return m_RendPos.load(); }
    protected:
        void SetRendPos(double pos) { m_RendPos.store(pos); }
        void IncRendPos(double dur) { m_RendPos.store(RendPos() + dur); }
        Ftool::IPumpSwitchR* PumpSwitchR() { return m_Pump.PumpSwitchR(); }
        Ftool::IPumpSwitchW* PumpSwitchW() { return m_Pump.PumpSwitchW(); }
// operations
    public:
        bool CreatePump();
        void ClosePump();
        bool PumpError() const;
        bool EndOfData() const;

    protected:
        void InitPump(IProceed* pr, bool useCondVar, const char* tag, bool logEx = false);
        void AttachFrameHub(IFrameData* frameHub);

        static const char* Ptr2Log(void* ptr) { return ptr ? "yes" : "null"; }

    }; // class RendCtrlBase 

} // namespace Fapp