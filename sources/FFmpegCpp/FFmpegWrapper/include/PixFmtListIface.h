#pragma once


// ---------------------------------------------------------------------
// File: PixFmtListIface.h
// Classes: IPixFmtInfo (abstract), IPixFmtList (abstract)
// Purpose: interfaces for the objects which give information about the pixel formats
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "ListBaseIface.h"

namespace Fwrap
{
    class IPixFmtInfo : public IListItemInfoBase2
    {
    protected:
        IPixFmtInfo() = default;
        ~IPixFmtInfo() = default;

    public:
        virtual int  CompCount() const = 0;
        virtual int  PlaneCount() const = 0;
        virtual int  BitPerPix() const = 0;
        virtual bool HWAccel() const = 0;
        virtual bool Palette() const = 0;
        virtual bool Bitstream() const = 0;
        virtual uint32_t CompDepth() const = 0;

    }; // class IPixFmtInfo

    class IPixFmtList : public IListBase
    {
    protected:
        IPixFmtList() = default;
        ~IPixFmtList() = default;

    public:
        virtual const IPixFmtInfo* operator[](int i) const = 0;

        static IPixFmtList* CreateInstance();

    }; // class IPixFmtList

} // namespace Fwrap

