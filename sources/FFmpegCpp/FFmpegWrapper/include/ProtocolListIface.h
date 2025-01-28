#pragma once

// ---------------------------------------------------------------------
// File: ProtocolListIface.h
// Classes: IProtocolInfo (abstract), IProtocolList (abstract)
// Purpose: interfaces for the objects which give information about supported protocols
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "ListBaseIface.h"

namespace Fwrap
{
    class IProtocolInfo : public IListItemInfoBase1
    {
    protected:
        IProtocolInfo() = default;
        ~IProtocolInfo() = default;

    public:
        virtual bool IsInput() const = 0;
        virtual bool IsOutput() const = 0;

    }; // class IProtocolInfo

    class IProtocolList : public IListBase
    {
    protected:
        IProtocolList() = default;
        ~IProtocolList() = default;

    public:
        virtual const IProtocolInfo* operator[](int i) const = 0;

        static IProtocolList* CreateInstance();

    }; // class IProtocolList

} // namespace Fwrap
