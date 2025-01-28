#pragma once

// ---------------------------------------------------------------------
// File: OscBuilder.h
// Classes: OscBuilder
// Purpose: oscillogram builder
// Module: FFmpegAppLib - aplications based on clases from FFmpegCoreLib
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "OscBuilderData.h"
#include "FrameSourceEx.h"

namespace Fapp
{
    class OscBuilder
    {
// data
    private:
        static const int  Chann;
        static const char SampleFormat[];

        FrameSourceEx  m_FrameSource;
        OscBuilderData m_OscBuilderData;
// ctor, dtor, copying
    public:
        OscBuilder();
        ~OscBuilder();
// properties
    public:
        const void* Data() const;
        int Length() const;
        double Progress() const;
        bool EndOfData() const;
        double SourceDuration() const;
// operations
    public:
        int Build(const char* url, const char* fmt, const char* ops, int oscLen);

    }; // class OscBuilder

} // namespace Fapp

