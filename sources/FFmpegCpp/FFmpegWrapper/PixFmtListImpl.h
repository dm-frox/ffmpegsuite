#pragma once

// ---------------------------------------------------------------------
// File: PixFmtListImpl.h
// Classes: PixFmtListmpl
// Purpose: IPixFmtList implementation
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2023
// ---------------------------------------------------------------------

#include "PixFmtList.h"

#include "include/PixFmtListIface.h"


namespace Fwrap
{
    class PixFmtListImpl : IPixFmtList
    {
        Flist::PixFmtList m_PixFmtList;

        PixFmtListImpl();
        ~PixFmtListImpl();

// IPixFmtList impl
        void Setup() override final;
        int Count() const override final;
        const IPixFmtInfo* operator[](int i) const override final;
// factory
        friend IPixFmtList* IPixFmtList::CreateInstance();

    }; // class PixFmtListImpl

} // namespace Fwrap

