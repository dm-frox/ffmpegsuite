#pragma once

// ---------------------------------------------------------------------
// File: OscBuilder.h
// Classes: OscBuilder
// Purpose: managed wrapper for native IOscBuilder implementation
// Module: FFmpegInterop - managed wrappers for native object from FFmpegWrapper
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------


#include "include/OscBuilderIface.h"

#include "CommTypes.h"
#include "ImplPtr.h"


namespace FFmpegInterop
{
    using OscBuilderBase = ImplPtr<Fwrap::IOscBuilder>;

    public ref class OscBuilder sealed : OscBuilderBase
    {
    public:
        OscBuilder();

        void Build(StrType^ url, StrType^ fmt, StrType^ ops, int oscLen);
        property PtrType Data { PtrType get(); }
        property int Length { int get(); }
        property double Progress { double get(); }
        property bool EndOfData { bool get(); }
        property double SourceDuration { double get(); }

    }; // ref class OscBuilder

} // namespace FFmpegInterop
