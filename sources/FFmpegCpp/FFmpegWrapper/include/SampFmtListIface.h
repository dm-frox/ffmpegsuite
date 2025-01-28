#pragma once

// ---------------------------------------------------------------------
// File: SampFmtListIface.h
// Classes: ISampFmtInfo (abstract), ISampFmtList (abstract)
// Purpose: interfaces for the objects which give information about the sample formats
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "ListBaseIface.h"

namespace Fwrap
{
    class ISampFmtInfo : public IListItemInfoBase2
    {
    protected:
        ISampFmtInfo() = default;
        ~ISampFmtInfo() = default;

    public:
        virtual int  BytesPerSamp() const = 0;
        virtual bool IsPlanar() const = 0;

    }; // class ISampFmtInfo

    class ISampFmtList : public IListBase
    {
    protected:
        ISampFmtList() = default;
        ~ISampFmtList() = default;

    public:
        virtual const ISampFmtInfo* operator[](int i) const = 0;

        static ISampFmtList* CreateInstance();

    }; // class ISampFmtList

} // namespace Fwrap
