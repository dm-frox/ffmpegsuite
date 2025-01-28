#pragma once

// ---------------------------------------------------------------------
// File: ProceedIface.h
// Classes: IProceed (abstract)
// Purpose: interface which used in ProceedPump
// Module: FFmpegToolsLib - auxiliary classes, functions
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------


#include "BaseIface0.h"

namespace Ftool
{
    class IProceed : IBase0
    {
    public:
        static const int EndOfDataMarker  = 1;
        static const int ProceedException = -2000;

    protected:
        IProceed() = default;
        ~IProceed() = default;

    public:
        virtual int Proceed() = 0;

    }; // class IProceed

} // namespace Ftool
