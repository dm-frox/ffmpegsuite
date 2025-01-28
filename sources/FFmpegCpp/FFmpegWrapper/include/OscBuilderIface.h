#pragma once

// ---------------------------------------------------------------------
// File: OscBuilderIface.h
// Classes: IOscBuilder (abstract)
// Purpose: interface for the object which builds oscillogram for the audio stream
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "BaseIface.h"

namespace Fwrap
{
    class IOscBuilder : public IBase
    {
    protected:
        IOscBuilder() = default;
        ~IOscBuilder() = default;

    public:
        virtual int Build(const char* url, const char* fmt, const char* ops, int oscLen) = 0;
        virtual const void* Data() const = 0;
        virtual int Length() const = 0;
        virtual double Progress() const = 0;
        virtual bool EndOfData() const = 0;
        virtual double SourceDuration() const = 0;

        static IOscBuilder* CreateInstance();

    }; // class IOscBuilder

} // namespace Fwrap
