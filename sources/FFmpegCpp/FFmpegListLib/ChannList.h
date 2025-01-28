#pragma once

// ---------------------------------------------------------------------
// File: ChannList.h
// Classes: ChannInfo, ChannList
// Purpose: classes to give information about standard channel layouts
// Module: FFmpegListLib - ffmpeg component lists
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <string>

#include "include/ChannListIface.h"

#include "ffmpeg.h"
#include "ListBase.h"


namespace Flist
{
    class ChannInfo : public ListItemInfoBase<Fwrap::IChannInfo>
    {
// data
    private:
        std::string m_Name;
        std::string m_Descr;
        const int   m_Chann;
        const uint64_t m_Mask;

// ctor, dtor, copying
    public:
        explicit ChannInfo(const AVChannelLayout* channLayout);
        ~ChannInfo();

// Fwrap::IChannInfo impl
    private:
        const char* Name() const override final;
        const char* Descr() const override final;
        int Chann() const override final;
        uint64_t    Mask() const override final;

    }; // class ChannInfo

    class ChannList : public ListBase<ChannInfo>
    {
// ctor, dtor, copying
    public:
        ChannList();
        ~ChannList();
// operations
    public:
        void Setup();
    private:
        static size_t GetCapacity();

    }; // class ChannList

} // namespace Flist
