#pragma once

// ---------------------------------------------------------------------
// File: Probe.h
// Classes: Probe
// Purpose: managed wrapper for native IProbe implementation
// Module: FFmpegInterop - managed wrappers for native object from FFmpegWrapper
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------


#include "include/ProbeIface.h"

#include "CommTypes.h"
#include "ImplPtr.h"


namespace FFmpegInterop
{
    using ProbeBase = ImplPtr<Fwrap::IProbe>;

    public ref class Probe sealed : ProbeBase
    {
    public:
        Probe();

        StrType^ Version();
        int Do(int param1, StrType^ param2);
        void TestMultiString(StrType^ opts);
        array<int>^ SortInt(array<int>^ arr);
        void SortInt2(array<int>^ inArr, array<int>^ outArr);
        int LogDevList();
        void TestSubtit(StrType^ path, StrType^ lang, int count);

    }; //  ref class Probe : ProbeBase

} // namespace FFmpegInterop
