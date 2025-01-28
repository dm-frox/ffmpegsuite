#pragma once

// ---------------------------------------------------------------------
// File: FilterList.h
// Classes: FilterInfo, FilterList
// Purpose: managed wrapper for native IFilterList implementation
// Module: FFmpegInterop - managed wrappers for native object from FFmpegWrapper
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------


#include "include/FilterListIface.h"

#include "CommTypes.h"
#include "ImplPtr.h"


namespace FFmpegInterop
{
    public ref class FilterInfo sealed
    {
        using InfoPrt = const Fwrap::IFilterInfo* const;

        InfoPrt m_Info;

    internal:
        FilterInfo(InfoPrt p);

    public:
        property StrType^ Name { StrType^ get(); }
        property StrType^ Descr { StrType^ get(); }
        property int InCount { int get(); }
        property int OutCount { int get(); }
        property bool InVideo { bool get(); }
        property bool OutVideo{ bool get(); }
        property bool Cmd { bool get(); }
        property StrType^ PrivOptions { StrType^ get(); }
        property StrType^ PrivOptionsEx { StrType^ get(); }

    }; // ref class FilterInfo

// -------------------------------------------------------------------

    using FilterListBase = ImplPtr<Fwrap::IFilterList>;

    public ref class FilterList sealed : FilterListBase
    {
    public:
        FilterList();

        property int Count { int get(); }
        property FilterInfo^ default[int]{ FilterInfo^ get(int i); }
        property StrType^ ContextOptionsEx { StrType^ get(); }
 
    }; // ref class FilterList

} // namespace FFmpegInterop

