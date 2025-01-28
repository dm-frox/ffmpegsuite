#pragma once

// ---------------------------------------------------------------------
// File: ChannList.h
// Classes: ChannInfo, ChannList
// Purpose: managed wrapper for native IChannList implementation
// Module: FFmpegInterop - managed wrappers for native object from FFmpegWrapper
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------


#include "include/ChannListIface.h"

#include "CommTypes.h"
#include "ImplPtr.h"


namespace FFmpegInterop
{
    public ref class ChannInfo sealed
    {
        using InfoPrt = const Fwrap::IChannInfo* const;

        InfoPrt m_Info;

    internal:
        ChannInfo(InfoPrt p);

    public:
        property StrType^ Name { StrType^ get(); }
        property StrType^ Descr { StrType^ get(); }
        property int Chann { int get(); }
        property long long Mask { long long get(); }

    }; // ref class ChannInfo

// -------------------------------------------------------------------

    using ChannListBase = ImplPtr<Fwrap::IChannList>;

    public ref class ChannList sealed : ChannListBase
    {
    public:
        ChannList();

        property int Count { int get(); }
        property ChannInfo^ default[int]{ ChannInfo^ get(int i); }

    }; // ref class ChannList

} // namespace FFmpegInterop


