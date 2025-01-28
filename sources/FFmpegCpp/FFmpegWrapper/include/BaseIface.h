#pragma once

// ---------------------------------------------------------------------
// File: BaseIface.h
// Classes: IBase0 (abstract), IBase (abstract)
// Purpose: base classes for other interface classes
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

namespace Fwrap
{
    class IBase0
    {
    protected:
        IBase0() = default;
        virtual ~IBase0() = default;

        IBase0(const IBase0&) = delete;
        IBase0& operator=(const IBase0&) = delete;
        IBase0(IBase0&&) = delete;
        IBase0& operator=(IBase0&&) = delete;

    }; // class IBase0

    class IBase : IBase0
    {
    protected:
        IBase() = default;
        ~IBase() = default;

    public:
        virtual void Delete();

    }; // class IBase

} // namespace Fwrap

