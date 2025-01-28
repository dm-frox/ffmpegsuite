#pragma once

// ---------------------------------------------------------------------
// File: SampFmtList.h
// Classes: SampFmtInfo, SampFmtnList
// Purpose: classes to give information about supported sample formats
// Module: FFmpegListLib - ffmpeg component lists
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "include/SampFmtListIface.h"

#include "ffmpeg.h"
#include "ListBase.h"


namespace Flist
{
    class SampFmtInfo : public ListItemInfoBase<Fwrap::ISampFmtInfo>
    {
// data
    private:
        const char* const m_Name;
        const char* const m_Descr;
        int const         m_BytesPerSamp;
        bool const        m_IsPlanar;
// ctor, dtor, copying
    public:
        SampFmtInfo(AVSampleFormat sampFmt, const char* decr);
        ~SampFmtInfo();

// Fwrap::ISampFmtInfo impl
    private:
        const char* Name() const override;
        const char* Descr() const override;
        int  BytesPerSamp() const override;
        bool IsPlanar() const override;

    }; // SampFmtInfo

    class SampFmtList : public ListBase<SampFmtInfo>
    {
// ctor, dtor, copying
    public:
        SampFmtList();
        ~SampFmtList();
// operations
    public:
        void Setup();

    }; // class SampFmtList

} // namespace Fcor