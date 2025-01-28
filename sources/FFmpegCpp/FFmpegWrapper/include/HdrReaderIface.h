#pragma once

// ---------------------------------------------------------------------
// File: HdrReaderIface.h
// Classes: IHdrReader (abstract)
// Purpose: interface for the object which reads the media source header 
// and gives information about the media source
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "HdrReaderBaseIface.h"

namespace Fwrap
{
    class IHdrReader : public IHdrReaderBase
    {
    protected:
        IHdrReader() = default;
        ~IHdrReader() = default;

    public:
        virtual int Open(const char* url, const char* fmt, const char* ops) = 0;

        static IHdrReader* CreateInstance();

    }; // class IHdrReader

} // namespace Fwrap
