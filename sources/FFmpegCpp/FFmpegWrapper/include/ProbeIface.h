#pragma once

// ---------------------------------------------------------------------
// File: ProbeIface.h
// Classes: IProbe (abstract)
// Purpose: interface for the test object
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "BaseIface.h"

namespace Fwrap
{

    class IProbe : public IBase
    {
    protected:
        IProbe() = default;
        ~IProbe() = default;

    public:
        virtual const char* Version() const = 0;
        virtual int Do(int par1, const char* par2) = 0;

        virtual void TestMultiString(const char* par) = 0;

        virtual void SortInt(int* arr, int len) = 0;
        virtual void SortInt2(const int* inArr, int len, int* outArr) = 0;
        virtual int LogDevList() = 0;
        virtual void TestSubtit(const char* path, const char* lang, int count) = 0;

        static IProbe* CreateInstance();

    }; // class IProbe

} // namespace Fwrap