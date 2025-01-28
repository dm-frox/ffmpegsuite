#pragma once

// ---------------------------------------------------------------------
// File: ProtocolList.h
// Classes: ProtocolInfo, ProtocolList
// Purpose: classes to give information about supported protocols
// Module: FFmpegListLib - ffmpeg component lists
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <string>
#include <cstring>

#include "include/ProtocolListIface.h"

#include "ListBase.h"


namespace Flist
{
    class ProtocolInfo : public ListItemInfoBaseOpt<Fwrap::IProtocolInfo>
    {
// data
    private:
        const char* const m_Name;
        mutable bool m_IsInput;
        mutable bool m_IsOutput;
// ctor, dtor, copying
    public:
        explicit ProtocolInfo(const char* name);
        ProtocolInfo(const ProtocolInfo& itm);
        ~ProtocolInfo();
// operations
    public:
        void SetInput() const { m_IsInput = true; }
        void SetOutput() const { m_IsOutput = true; }

        bool operator<(const ProtocolInfo& rh) const { return std::strcmp(m_Name, rh.m_Name) < 0; }

    private:
        const char* GetPrivOpts(bool ex, std::string& optStr) const;

// Fwrap::IProtocolInfo impl
    private:
        const char* Name() const override final;
        const char* PrivOptions() const override final;
        const char* PrivOptionsEx() const override final;
        bool IsInput() const override final;
        bool IsOutput() const override final;

    }; // class ProtocolInfo

    class ProtocolList : public ListBase<ProtocolInfo> 
    {
// ctor, dtor, copying
    public:
        ProtocolList();
        ~ProtocolList();
// operations
    public:
        void Setup();

    }; // class ProtocolList

} // namespace Flist

