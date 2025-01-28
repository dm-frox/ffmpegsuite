#pragma once

// ---------------------------------------------------------------------
// File: BsfList.h
// Classes: BsfInfo, BsfList
// Purpose: managed wrapper for native IBsfList implementation
// Module: FFmpegInterop - managed wrappers for native object from FFmpegWrapper
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "include/BsfListIface.h"

#include "CommTypes.h"
#include "ImplPtr.h"


namespace FFmpegInterop
{
    public ref class BsfInfo sealed
    {
        using InfoPrt = const Fwrap::IBsfInfo* const;

        InfoPrt m_Info;

    internal:
        BsfInfo(InfoPrt p);

    public:
        property StrType^ Name { StrType^ get(); }
        property StrType^ CodecIds { StrType^ get(); }
        property StrType^ PrivOptions { StrType^ get(); }
        property StrType^ PrivOptionsEx { StrType^ get(); }

        virtual StrType^ ToString() override;

    }; // ref class BsfInfo

// -------------------------------------------------------------------

    using BsfListBase = ImplPtr<Fwrap::IBsfList>;

    public ref class BsfList sealed : BsfListBase
    {
    public:
        BsfList();

        property int Count { int get(); }
        property BsfInfo^ default[int] { BsfInfo^ get(int i); }
        property StrType^ ContextOptionsEx { StrType^ get(); }

    }; // ref class BsfList

} // namespace FFmpegInterop

