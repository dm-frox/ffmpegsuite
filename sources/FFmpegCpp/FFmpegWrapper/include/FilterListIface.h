#pragma once

// ---------------------------------------------------------------------
// File: FilterListIface.h
// Classes: IFilterInfo (abstract), IFilterList (abstract)
// Purpose: interfaces for the objects which give information about supported filters
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "ListBaseIface.h"


namespace Fwrap
{
    class IFilterInfo : public IListItemInfoBase1
    {
    protected:
        IFilterInfo() = default;
        ~IFilterInfo() = default;

    public:
        virtual const char* Descr() const = 0;
        virtual int InCount() const = 0;
        virtual int OutCount() const = 0;
        virtual bool InVideo() const = 0;
        virtual bool OutVideo() const = 0;
        virtual bool Cmd() const = 0;

    }; // class IFilterInfo

    class IFilterList : public IListBaseEx
    {
    protected:
        IFilterList() = default;
        ~IFilterList() = default;

    public:
        virtual const IFilterInfo* operator[](int i) const = 0;

        static IFilterList* CreateInstance();

    }; // class IFilterList

} // namespace Fwrap


