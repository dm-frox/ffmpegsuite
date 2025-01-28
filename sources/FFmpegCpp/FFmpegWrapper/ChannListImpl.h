#pragma once

// ---------------------------------------------------------------------
// File: ChannListImpl.h
// Classes: ChannListImpl
// Purpose: IChannList implementation
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "ChannList.h"

#include "include/ChannListIface.h"


namespace Fwrap
{
    class ChannListImpl : IChannList
    {
        Flist::ChannList m_ChannList;

        ChannListImpl();
        ~ChannListImpl();

// IChannList impl
        void Setup() override final;
        int Count() const override final;
        const IChannInfo* operator[](int i) const override final;
// factory
        friend IChannList* IChannList::CreateInstance();

    }; // class ChannListImpl

} // namespace Fwrap

