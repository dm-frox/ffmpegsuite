#pragma once

// ---------------------------------------------------------------------
// File: OscBuilderImpl.h
// Classes: OscBuilderImpl
// Purpose: IOscBuilder implementation
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "OscBuilder.h"

#include "include/OscBuilderIface.h"


namespace Fwrap
{
    class OscBuilderImpl : IOscBuilder
    {
        Fapp::OscBuilder m_OscBuilder;

        OscBuilderImpl();
        ~OscBuilderImpl();

// IOscBuilder impl
        int Build(const char* url, const char* fmt, const char* ops, int oscLen) override final;
        const void* Data() const override final;
        int Length() const override final;
        double Progress() const override final;
        bool EndOfData() const override final;
        double SourceDuration() const override final;
// factory
        friend IOscBuilder* IOscBuilder::CreateInstance();

    }; // class OscBuilderImpl

} // namespace Fwrap

