#pragma once

// ---------------------------------------------------------------------
// File: ProtocoList.h
// Classes: ProtocolInfo, ProtocoList
// Purpose: managed wrapper for native IProtocoList implementation
// Module: FFmpegInterop - managed wrappers for native object from FFmpegWrapper
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "include/ProtocolListIface.h"

#include "CommTypes.h"
#include "ImplPtr.h"


namespace FFmpegInterop
{
    public ref class ProtocolInfo sealed 
    {
        using InfoPrt = const Fwrap::IProtocolInfo* const;

        InfoPrt m_Info;

    internal:
        ProtocolInfo(InfoPrt p);

    public:
        property StrType^ Name { StrType^ get(); }
        property bool IsInput { bool get(); }
        property bool IsOutput { bool get(); }
        property StrType^ PrivOptions { StrType^ get(); }
        property StrType^ PrivOptionsEx { StrType^ get(); }

        virtual StrType^ ToString() override;

    }; // ref class ProtocolInfo

// -------------------------------------------------------------------

    using ProtocolListBase = ImplPtr<Fwrap::IProtocolList>;

    public ref class ProtocolList sealed : ProtocolListBase
    {
    public:
        ProtocolList();

        property int Count { int get(); }
        property ProtocolInfo^ default[int]{ ProtocolInfo^ get(int i); }

    }; // ref class ProtocolList

} // namespace FFmpegInterop

