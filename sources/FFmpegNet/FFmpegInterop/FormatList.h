#pragma once

// ---------------------------------------------------------------------
// File: FormatList.h
// Classes: FormatInfo, FormaList
// Purpose: managed wrapper for native IFormaList implementation
// Module: FFmpegInterop - managed wrappers for native object from FFmpegWrapper
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "include/FormatListIface.h"

#include "CommTypes.h"
#include "ImplPtr.h"


namespace FFmpegInterop
{
    public ref class FormatInfo sealed 
    {
        literal StrType^ MuxPrefix = "*";

        using InfoPrt = const Fwrap::IFormatInfo* const;

        InfoPrt m_Info;

    internal:
        FormatInfo(InfoPrt p);

    public:
        property bool Demuxer { bool get(); }
        property StrType^ Name { StrType^ get(); }
        property StrType^ LongName { StrType^ get(); }
        property StrType^ Mime { StrType^ get(); }
        property StrType^ Extentions { StrType^ get(); }
        property StrType^ PrivOptions { StrType^ get(); }
        property StrType^ PrivOptionsEx { StrType^ get(); }

        virtual StrType^ ToString() override;

    }; // ref class FormatInfo

// -------------------------------------------------------------------

    using FormatListBase = ImplPtr<Fwrap::IFormatList>;

    public ref class FormatList sealed : FormatListBase
    {
    public:
        FormatList();

        property int Count { int get(); }
        property FormatInfo^ default[int]{ FormatInfo ^ get(int i); }
        property StrType^ ContextOptionsEx { StrType^ get(); }

    }; // ref class FormatList

} // namespace FFmpegInterop
