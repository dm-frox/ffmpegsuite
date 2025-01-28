#pragma once

// ---------------------------------------------------------------------
// File: FormatListImpl.h
// Classes: FormatListImpl
// Purpose: IFormatList implementation
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "FormatList.h"

#include "include/FormatListIface.h"


namespace Fwrap
{
    class FormatListImpl : IFormatList
    {
        Flist::FormatList m_FormatList;

        FormatListImpl();
        ~FormatListImpl();

// IFormatList impl
        void Setup() override final;
        int Count() const override final;
        const IFormatInfo* operator[](int i) const override final;
        const char* ContextOptionsEx() const override final;
// factory
        friend IFormatList* IFormatList::CreateInstance();

    }; //class FormatListImpl

} // namespace Fwrap
