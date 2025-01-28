#pragma once

// ---------------------------------------------------------------------
// File: Probe.h
// Classes: Probe
// Purpose: test class
// Module: FFmpegAppLib - aplications based on clases from FFmpegCoreLib
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

namespace Fapp
{
    class Probe
    {
// ctor, dtor, copying
    public:
        Probe();
        ~Probe();
// operations
    public:
        const char* Version() const;
        int Do(int par1, const char* par2);
        void TestMultiString(const char* ms);
        int LogDevList();
        void SortInt(int* arr, int len);
        void SortInt2(const int* inArr, int len, int* outArr);
        void TestSubtit(const char* path, const char* lang, int count);

    }; // class Probe

} // namespace Fapp
