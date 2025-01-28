#pragma once

// ---------------------------------------------------------------------
// File: NonCopyable.h
// Classes: NonCopyable
// Purpose: base class for non copyable classes
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 25.01.2020
// ---------------------------------------------------------------------

namespace Fcore
{
    class NonCopyable
    {
// ctor, dtor, copying
    protected:
        NonCopyable() = default;
        ~NonCopyable() = default;

        NonCopyable(const NonCopyable&) = delete;
        NonCopyable& operator=(const NonCopyable&) = delete;

        NonCopyable(NonCopyable&&) = delete;
        NonCopyable& operator=(NonCopyable&&) = delete;

    }; // class NonCopyable

} // namespace Fcore