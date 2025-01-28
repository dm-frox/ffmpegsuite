#pragma once

// ---------------------------------------------------------------------
// File: SampFmtListImpl.h
// Classes: SampFmtListmpl
// Purpose: ISampFmtList implementation
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2023
// ---------------------------------------------------------------------

#include "SampFmtList.h"

#include "include/SampFmtListIface.h"


namespace Fwrap
{
    class SampFmtListImpl : ISampFmtList
    {
        Flist::SampFmtList m_SampFmtList;

        SampFmtListImpl();
        ~SampFmtListImpl();

// IPixFmtList impl
        void Setup() override final;
        int Count() const override final;
        const ISampFmtInfo* operator[](int i) const override final;
// factory
        friend ISampFmtList* ISampFmtList::CreateInstance();

    }; // class SampFmtListImpl

} // namespace Fwrap

