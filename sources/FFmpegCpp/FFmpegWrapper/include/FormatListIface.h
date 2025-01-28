#pragma once

// ---------------------------------------------------------------------
// File: FormatListIface.h
// Classes: IFormatInfo (abstract), IFormatList (abstract)
// Purpose: interfaces for the objects which give information about supported formats
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "ListBaseIface.h"

namespace Fwrap
{
    class IFormatInfo : public IListItemInfoBase1
    {
    protected:
        IFormatInfo() = default;
        ~IFormatInfo() = default;

    public:
        virtual bool IsDemuxer() const = 0;
        virtual const char* LongName() const = 0;
        virtual const char* Mime() const = 0;
        virtual const char* Extentions() const = 0;

    }; // class IFormatInfo
 
    class IFormatList : public IListBaseEx
    {
    protected:
        IFormatList() = default;
        ~IFormatList() = default;

    public:
        virtual const IFormatInfo* operator[](int i) const = 0;

        static IFormatList* CreateInstance();

    }; // class IFormatList

} // namespace Fwrap

