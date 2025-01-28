#pragma once

// ---------------------------------------------------------------------
// File: ProtocolListImpl.h
// Classes: ProtocolListImpl
// Purpose: IProtocolList implementation
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "ProtocolList.h"

#include "include/ProtocolListIface.h"


namespace Fwrap
{
    class ProtocolListImpl : IProtocolList
    {
        Flist::ProtocolList m_ProtocolList;

        ProtocolListImpl();
        ~ProtocolListImpl();

// IProtocolList impl
        void Setup() override final;
        int Count() const override final;
        const IProtocolInfo* operator[](int i) const override final;
// factory
        friend IProtocolList* IProtocolList::CreateInstance();

    }; // class ProtocolListImpl

} // namespace Fwrap

