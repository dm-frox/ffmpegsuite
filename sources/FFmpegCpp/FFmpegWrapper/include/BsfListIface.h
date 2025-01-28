#pragma once

// ---------------------------------------------------------------------
// File: BsfListIface.h
// Classes: IBsfInfo (abstract), IBsfList (abstract)
// Purpose: interfaces for the objects which give information about bitstream filters
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "ListBaseIface.h"

namespace Fwrap
{
    class IBsfInfo : public IListItemInfoBase1
    {
    protected:
        IBsfInfo() = default;
        ~IBsfInfo() = default;

    public:
        virtual const char* CodecIds() const = 0;

    }; // class IBsfInfo

    class IBsfList : public IListBaseEx
    {
    protected:
        IBsfList() = default;
        ~IBsfList() = default;

    public:
        virtual const IBsfInfo* operator[](int i) const = 0;

        static IBsfList* CreateInstance();

    }; // class IBsfList

} // namespace Fwrap

