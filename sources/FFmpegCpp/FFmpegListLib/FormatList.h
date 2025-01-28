#pragma once

// ---------------------------------------------------------------------
// File: FormatList.h
// Classes: FormatNode, FormatList
// Purpose: classes to give information about supported formats, demuxers, muxers
// Module: FFmpegListLib - ffmpeg component lists
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <string>

#include "include/FormatListIface.h"

#include "ffmpeg.h"
#include "OptionTools.h"
#include "ListBase.h"


namespace Flist
{
    class FormatInfo : public ListItemInfoBaseOpt<Fwrap::IFormatInfo>
    {
// data
    private:
        const AVInputFormat* const  m_Demux;
        const AVOutputFormat* const m_Muxer;
// ctor, dtor, copying
    public:
        explicit FormatInfo(const AVInputFormat* dem, const AVOutputFormat* mux);
        ~FormatInfo();
// operations
    private:
        const char* GetPrivOpts(bool ex, std::string& optStr) const;
    public:
        static const char* GetCtxOptsEx(std::string& optStr);

// Fwrap::IFormatInfo impl
    private:
        const char* Name() const override final;
        const char* PrivOptions() const override final;
        const char* PrivOptionsEx() const override final;
        bool IsDemuxer() const override final;
        const char* LongName() const override final;
        const char* Mime() const override final;
        const char* Extentions() const override final;

    }; // class FormatNode

    class FormatList : public ListBaseOpt<FormatInfo>
    {
// ctor, dtor, copying
    public:
        FormatList();
        ~FormatList();
// properties
    public:
        const char* CtxOptsEx() const;
// operations
    public:
        void Setup();
    private:
        static size_t GetCapacity();

    }; // class FormatList

} // namespace Flist
