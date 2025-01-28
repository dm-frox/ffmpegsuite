#pragma once

// ---------------------------------------------------------------------
// File: FilterListImpl.h
// Classes: FilterListImpl
// Purpose: IFilterList implementation
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "FilterList.h"

#include "include/FilterListIface.h"


namespace Fwrap
{
    class FilterListImpl : IFilterList
    {
        Flist::FilterList m_FilterList;

        FilterListImpl();
        ~FilterListImpl();

// IFilterList impl
        void Setup() override final;
        int Count() const override final;
        const IFilterInfo* operator[](int i) const override final;
        const char* ContextOptionsEx() const override final;
// factory
        friend IFilterList* IFilterList::CreateInstance();

    }; // class FilterListImpl

} // namespace Fwrap

