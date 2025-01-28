#pragma once

// ---------------------------------------------------------------------
// File: ProbeImpl.h
// Classes: ProbeImpl
// Purpose: IProbe implementation
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "Probe.h"

#include "include/ProbeIface.h"


namespace Fwrap
{
    class ProbeImpl : IProbe
    {
        Fapp::Probe m_Probe;

        ProbeImpl();
        ~ProbeImpl();
// IProbe impl
        const char* Version() const override final;
        int Do(int par1, const char* par2) override final;
        void TestMultiString(const char* par) override final;
        void SortInt(int* arr, int len) override final;
        void SortInt2(const int* inArr, int len, int* outArr) override final;
        int LogDevList() override final;
        void TestSubtit(const char* path, const char* lang, int count) override final;
// factory
        friend IProbe* IProbe::CreateInstance();

    }; //class ProbeImpl

} // namespace Fwrap

