#pragma once

// ---------------------------------------------------------------------
// File: ThreadBase.h
// Classes: ThreadBase
// Purpose: thread helpers
// Module: FFmpegToolsLib - auxiliary classes, functions
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

namespace Ftool
{
    // static
    class ThreadTools
    {
    public:

        ThreadTools() = delete;

        static void Sleep(int intervalMs);
        static int GetHwConcurrency();

    }; // class ThreadBase

} // namespace Ftool
