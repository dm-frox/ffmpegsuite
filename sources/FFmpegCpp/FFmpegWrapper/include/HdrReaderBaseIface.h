#pragma once

// ---------------------------------------------------------------------
// File: HdrReaderBaseIface.h
// Classes: IHdrReaderBase (abstract), CStrEx
// Purpose: base interface for the object which reads the media source header 
// and gives information about the media source
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "BaseIface.h"

namespace Fwrap
{

    struct CStrEx
    {
        const char* Str;
        int         Len;
    };

    class IHdrReaderBase : public IBase
    {
    protected:
        IHdrReaderBase() = default;
        ~IHdrReaderBase() = default;

    public:
        virtual int StreamCount() const = 0;
        virtual CStrEx GetInfo() const = 0;
        virtual CStrEx GetMetadata() const = 0;
        virtual CStrEx GetChapters() const = 0;
        virtual CStrEx GetStreamInfo(int strmIdx) const = 0;
        virtual CStrEx GetStreamMetadata(int strmIdx) const = 0;

    }; // class IHdrReaderBase

} // namespace Fwrap
