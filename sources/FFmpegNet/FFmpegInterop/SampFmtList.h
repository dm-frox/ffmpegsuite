#pragma once

// ---------------------------------------------------------------------
// File: SampFmtList.h
// Classes: SampFmtInfo, SampFmtList
// Purpose: managed wrapper for native ISampFmtList implementation
// Module: FFmpegInterop - managed wrappers for native object from FFmpegWrapper
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "include/SampFmtListIface.h"

#include "CommTypes.h"
#include "ImplPtr.h"


namespace FFmpegInterop
{
    public ref class SampFmtInfo sealed
    {
        using InfoPrt = const Fwrap::ISampFmtInfo* const;

        InfoPrt m_Info;

    internal:
        SampFmtInfo(InfoPrt p);

    public:
        property StrType^ Name { StrType^ get(); }
        property StrType^ Descr { StrType^ get(); }
        property int  BytesPerSamp { int get(); }
        property bool IsPlanar { bool get(); }

    }; // ref class SampFmtInfo

// -------------------------------------------------------------------

    using SampFmtListBase = ImplPtr<Fwrap::ISampFmtList>;

    public ref class SampFmtList sealed : SampFmtListBase
    {
    public:
        SampFmtList();

        property int Count { int get(); }
        property SampFmtInfo^ default[int]{ SampFmtInfo^ get(int i); }

    }; // ref class SampFmtList

} // namespace FFmpegInterop



