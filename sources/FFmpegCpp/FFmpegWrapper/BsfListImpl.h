#pragma once

// ---------------------------------------------------------------------
// File: BsfListImpl.h
// Classes: BsfListImpl
// Purpose: IBsfList implementation
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "BsfList.h"

#include "include/BsfListIface.h"


namespace Fwrap
{
    class BsfListImpl : IBsfList
    {
        Flist::BsfList m_BsfList;

        BsfListImpl();
        ~BsfListImpl();

// IBsfList impl
        void Setup() override final;
        int Count() const override final;
        const IBsfInfo* operator[](int i) const override final;
        const char* ContextOptionsEx() const override final;
// factory
        friend IBsfList* IBsfList::CreateInstance();

    }; // class BsfListImpl

} // namespace Fwrap