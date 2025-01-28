#pragma once

// ---------------------------------------------------------------------
// File: BaseIface0.h
// Classes: IBase0 (abstract)
// Purpose: base class for other interface classes
// Module: FFmpegToolsLib - auxiliary classes, functions
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

namespace Ftool
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


} // namespace Ftool
