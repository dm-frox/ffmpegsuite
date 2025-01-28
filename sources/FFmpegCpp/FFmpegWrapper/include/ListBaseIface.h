#pragma once

// ---------------------------------------------------------------------
// File: ListBaseIface.h
// Classes: IListItemInfoBase0 (abstract), IListItemInfoBase1 (abstract), IListItemInfoBase2 (abstract), 
//     IListBase (abstract), IListBaseEx (abstract)
// Purpose: base classes for the other list interface classes
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "BaseIface.h"

namespace Fwrap
{

    class IListItemInfoBase0 
    {
    protected:
        IListItemInfoBase0() = default;
        virtual ~IListItemInfoBase0() = default;
        // copy/move - default

    public:
        virtual const char* Name() const = 0;

    }; // class IListItemInfoBase0

    class IListItemInfoBase1 : public IListItemInfoBase0
    {
    protected:
        IListItemInfoBase1() = default;
        ~IListItemInfoBase1() = default;

    public:
        virtual const char* PrivOptions() const = 0;
        virtual const char* PrivOptionsEx() const = 0;

    }; // class IListItemInfoBase1

    class IListItemInfoBase2 : public IListItemInfoBase0
    {
    protected:
        IListItemInfoBase2() = default;
        ~IListItemInfoBase2() = default;

    public:
        virtual const char* Descr() const = 0;

    }; // class IListItemInfoBase2

    class IListBase : public IBase
    {
    protected:
        IListBase() = default;
        ~IListBase() = default;

    public:
        virtual void Setup() = 0;
        virtual int Count() const = 0;

    }; // class IListBase

    class IListBaseEx : public IListBase
    {
    protected:
        IListBaseEx() = default;
        ~IListBaseEx() = default;

    public:
        virtual const char* ContextOptionsEx() const = 0;

    }; // class IListBaseEx

} // namespace Fwrap

