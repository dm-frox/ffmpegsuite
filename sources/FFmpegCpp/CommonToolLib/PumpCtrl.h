#pragma once

// ---------------------------------------------------------------------
// File: PumpCtrl.h
// Classes: PumpCtrlBase, ReadCtrl, WriteCtrl
// Purpose: helpers to switch ProceedPump
// Module: FFmpegToolsLib - auxiliary classes, functions
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

namespace Ftool
{
    class IPumpSwitchR;
    class IPumpSwitchW;

    class PumpCtrlBase
    {
// data
    protected:
        int          m_ThresholdOff{ 0 };
        int          m_ThresholdOn{ 0 };
// ctor, dtor, copying
        PumpCtrlBase(int thresholdOff, int thresholdOn);
        PumpCtrlBase() = default;
        PumpCtrlBase(const PumpCtrlBase&) = default;
        PumpCtrlBase& operator = (const PumpCtrlBase&) = default;
        ~PumpCtrlBase() = default;

    }; // class PumpCtrlBase

    class ReadPumpCtrl : public PumpCtrlBase
    {
// data
    private:
        IPumpSwitchR* m_PumpSwitch{ nullptr };
// ctor, dtor, copying
    public:
        ReadPumpCtrl(IPumpSwitchR* pumpSwitch);
        ReadPumpCtrl() = default;
        ReadPumpCtrl(const ReadPumpCtrl&) = default;
        ReadPumpCtrl& operator = (const ReadPumpCtrl&) = default;
        ~ReadPumpCtrl() = default;
// operations
    public:
        void OnPush(int len);
        void OnPop(int len);
        void Disable();

    }; // class ReadPumpCtrl

    class WritePumpCtrl : public PumpCtrlBase
    {
// data
    private:
        IPumpSwitchW* m_PumpSwitch{ nullptr };
        bool  m_Strict{ false };
// ctor, dtor, copying
    public:
        WritePumpCtrl(IPumpSwitchW* pumpSwitch, int thresholdOff, bool strict);
        WritePumpCtrl() = default;
        WritePumpCtrl(const WritePumpCtrl&) = default;
        WritePumpCtrl& operator = (const WritePumpCtrl&) = default;
        ~WritePumpCtrl() = default;
// operations
    public:
        void OnPush(int len);
        void OnPop(int len);

    }; // class WritePumpCtrl

} // namespace Ftool
