#pragma once

// ---------------------------------------------------------------------
// File: PumpSwitchIface.h
// Classes: IPumpSwitchR, IPumpSwitchW  (abstract)
// Purpose: interfaces which shall implement ProceedPump to pause/resume proceeding
// Module: FFmpegToolsLib - auxiliary classes, functions
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "BaseIface0.h"

namespace Ftool
{
    class IPumpSwitchR : IBase0
    {
    protected:
        IPumpSwitchR() = default;
        ~IPumpSwitchR() = default;
    public:
        virtual void EnablePumpRead(bool enable) = 0;

    }; // class IPumpSwitchR

    class IPumpSwitchW : IBase0
    {
    protected:
        IPumpSwitchW() = default;
        ~IPumpSwitchW() = default;
    public:
        virtual void EnablePumpWrite(bool enable) = 0;

    }; // class IPumpSwitchW

} // namespace Ftool
