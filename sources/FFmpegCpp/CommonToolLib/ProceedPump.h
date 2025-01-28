#pragma once

// ---------------------------------------------------------------------
// File: ProceedPump.h
// Classes: ProceedPump
// Purpose: pump to call IProceed::Proceed() 
// Module: FFmpegToolsLib - auxiliary classes, functions
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <atomic>

#include "PumpSwitchIface.h"

namespace Ftool
{
    class IProceed;

    class ProceedPump : IPumpSwitchR, IPumpSwitchW
    {
// types
    private:
        using Thread     = std::thread;
        using AtomicBool = std::atomic<bool>;
        using Mutex      = std::mutex;
        using AutoLock   = std::lock_guard<Mutex>;
        using UniqLock   = std::unique_lock<Mutex>;
        using CondVar    = std::condition_variable;
// data
    private:
        static const int  PauseInterval;
        static const int  StopInterval;
        static const char DefaultTag[];

        Thread      m_Thread;
        Mutex       m_Mutex;
        CondVar     m_CondVar;

        IProceed*   m_Proceeder{ nullptr };
        bool        m_UseCondVar{ false };
        std::string m_Tag;

        AtomicBool  m_IsActive{ false };
        AtomicBool  m_EndOfData{ false };
        AtomicBool  m_IsError{ false };
        AtomicBool  m_Proceeding{ false };

        bool        m_IsRunning{ false };
        bool        m_CanProceedR{ true };
        bool        m_CanProceedW{ true };

        bool        m_LogEx{ false };

// ctor, dtor, copying
    public:
        ProceedPump();
        ~ProceedPump();
// properties
    public:
        const char* Tag() const;
        bool IsActive() const;
        bool PumpError() const;
        bool EndOfData() const;
        void EnableUseCondVar();
        bool Proceeding() const;
    private:
        bool IsRunning();
// operations
    public:
        void Init(IProceed* pr, bool useCondVar, const char* name, bool logEx);

        bool CreatePump();
        void Run();
        void Pause();
        void Stop(bool skipPause);
        void ClosePump();
        void LogState();

    private:
        void InitFlags();
        int Proceed();
        int ProcResult(int res);
        void SetRunning(bool running);
        bool CanProceed() const;
        void DoJob(); // thread function
        bool EnableRead(bool enable);
        bool EnableWrite(bool enable);

// IPumpSwitchR impl
    private:
        void EnablePumpRead(bool enable) override final;
// IPumpSwitchW impl
    private:
        void EnablePumpWrite(bool enable) override final;
    public:
        IPumpSwitchR* PumpSwitchR() { return static_cast<IPumpSwitchR*>(this); }
        IPumpSwitchW* PumpSwitchW() { return static_cast<IPumpSwitchW*>(this); }

    }; // class ProceedPump

} // namespace Ftool


