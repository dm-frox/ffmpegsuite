#pragma once


// ---------------------------------------------------------------------
// File: ChannListIface.h
// Classes: IChannInfo (abstract), IChannList (abstract)
// Purpose: interfaces for the objects which give information about standerd channel layouts
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2023
// ---------------------------------------------------------------------

#include "ListBaseIface.h"

namespace Fwrap
{
    class IChannInfo : public IListItemInfoBase2
    {
    protected:
        IChannInfo() = default;
        ~IChannInfo() = default;

    public:
        virtual int Chann() const = 0;
        virtual uint64_t Mask() const = 0;

    }; // classt IChannInfo

    class IChannList : public IListBase
    {
    protected:
        IChannList() = default;
        ~IChannList() = default;

    public:
        virtual const IChannInfo* operator[](int i) const = 0;

        static IChannList* CreateInstance();

    }; // class IChannList

} // namespace Fwrap
