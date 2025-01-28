#pragma once

// ---------------------------------------------------------------------
// File: PixFmtList.h
// Classes: PixFmtInfo, PixFmtList
// Purpose: managed wrapper for native IPixFmtList implementation
// Module: FFmpegInterop - managed wrappers for native object from FFmpegWrapper
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "include/PixFmtListIface.h"

#include "CommTypes.h"
#include "ImplPtr.h"


namespace FFmpegInterop
{
    public ref class PixFmtInfo sealed
    {
        using InfoPrt = const Fwrap::IPixFmtInfo* const;

        InfoPrt m_Info;

    internal:
        PixFmtInfo(InfoPrt p);

    public:
        property StrType^ Name { StrType^ get(); }
        property StrType^ Descr { StrType^ get(); }
        property int CompCount { int get(); }
        property int PlaneCount { int get(); }
        property int BitPerPix { int get(); }
        property bool HWAccel { bool get(); }
        property bool Palette { bool get(); }
        property bool Bitstream { bool get(); }
        property uint32_t CompDepth { uint32_t get(); }

    }; // ref class PixFmtInfo

// -------------------------------------------------------------------

    using PixFmtListBase = ImplPtr<Fwrap::IPixFmtList>;

    public ref class PixFmtList sealed : PixFmtListBase
    {
    public:
        PixFmtList();

        property int Count { int get(); }
        property PixFmtInfo^ default[int]{ PixFmtInfo^ get(int i); }

    }; // ref class PixFmtList

} // namespace FFmpegInterop



