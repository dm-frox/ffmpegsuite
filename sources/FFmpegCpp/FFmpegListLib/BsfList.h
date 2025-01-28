#pragma once

// ---------------------------------------------------------------------
// File: BsfList.h
// Classes: BsfInfo, BsfList
// Purpose: classes to give information about bitstream filters
// Module: FFmpegListLib - ffmpeg component lists
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <string>

#include "include/BsfListIface.h"

#include "ffmpeg_fwd.h"
#include "ListBase.h"


namespace Flist
{
    class BsfInfo : public ListItemInfoBaseOpt<Fwrap::IBsfInfo>
    {
// data
    private:
        const AVBitStreamFilter* const m_Bsf;
        mutable std::string            m_CodecIds{};

// ctor, dtor, copying
    public:
        explicit BsfInfo(const AVBitStreamFilter* bsf);
        ~BsfInfo();
// operations
    private:
        static std::string GetCodecIds(const AVBitStreamFilter* bsf);
    public:
        const char* GetOpts(bool priv, bool ex, std::string& optStr) const;

// Fwrap::IBsfInfo impl
    private:
        const char* Name() const override final;
        const char* PrivOptions() const override final;
        const char* PrivOptionsEx() const override final;
        const char* CodecIds() const override final;

    }; // class BsfInfo

    class BsfList : public ListBaseOpt<BsfInfo>
    {
// ctor, dtor, copying
    public:
        BsfList();
        ~BsfList();
// properties
    public:
        const char* CtxOptsEx() const;
// operations
    public:
        void Setup();
    private:
        static size_t GetCapacity();

    }; // class BsfList

} // namespace Flist

