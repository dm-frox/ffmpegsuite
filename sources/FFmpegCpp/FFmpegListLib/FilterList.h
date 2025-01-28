#pragma once

// ---------------------------------------------------------------------
// File: FilterList.h
// Classes: FilterInfo, FilterList
// Purpose: classes to give information about supported filters
// Module: FFmpegListLib - ffmpeg component lists
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <string>

#include "include/FilterListIface.h"

#include "ffmpeg.h"
#include "ListBase.h"


namespace Flist
{
    class FilterInfo : public ListItemInfoBaseOpt<Fwrap::IFilterInfo>
    {
// data
    private:
        const char* const m_Name;
        const char* const m_Descr;
        const int m_InCount;
        const int m_OutCount;
        const bool m_InVideo;
        const bool m_OutVideo;
        const bool m_Cmd;
// ctor, dtor, copying
    public:
        explicit FilterInfo(const AVFilter* filter);
        ~FilterInfo();
// operations
    public:
        const char* GetOpts(bool priv, bool ex, std::string& optStr) const;
    private:
        static bool IsVideo(int count, const AVFilterPad* pads);

// Fwrap::IFilterInfo impl
    private:
        const char* Name() const override;
        const char* PrivOptions() const override final;
        const char* PrivOptionsEx() const override final;
        const char* Descr() const override;
        int InCount() const override;
        int OutCount() const override;
        bool InVideo() const override;
        bool OutVideo() const override;
        bool Cmd() const override;

    }; // class FilterInfo

    class FilterList : public ListBaseOpt<FilterInfo> 
    {
// ctor, dtor, copying
    public:
        FilterList();
        ~FilterList();
// properties
    public:
        const char* CtxOptsEx() const;
// operations
    public:
        void Setup();
    private:
        static size_t GetCapacity();

    }; // FilterList

} // namespace Flist
