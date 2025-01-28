#pragma once

// ---------------------------------------------------------------------
// File: PixFmtList.h
// Classes: PixFmtInfo, PixFmtList
// Purpose: classes to give information about supported pixel formats
// Module: FFmpegListLib - ffmpeg component lists
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <string>

#include "include/PixFmtListIface.h"

#include "ffmpeg.h"
#include "ListBase.h"


namespace Flist
{
    class PixFmtInfo : public ListItemInfoBase<Fwrap::IPixFmtInfo>
    {
// data
    private:
        AVPixelFormat m_PixFmt;

        const char* const m_Name;
        const std::string m_Descr;
        const int  m_CompCount;
        const int  m_PlaneCount;
        const int  m_BitPerPix;
        const bool m_HWAccel;
        const bool m_Palette;
        const bool m_Bitstream;
        const uint32_t m_CompDepth;
// ctor, dtor, copying
    public:
        explicit PixFmtInfo(const AVPixFmtDescriptor* pfd);
        ~PixFmtInfo();
// operations
    private:
        static uint32_t PackDepth(const AVPixFmtDescriptor* pfd);

// Fwrap::IPixFmtInfo impl
    private:
        const char* Name() const override;
        const char* Descr() const override;
        int  CompCount() const override;
        int  PlaneCount() const override;
        int  BitPerPix() const override;
        bool HWAccel() const override;
        bool Palette() const override;
        bool Bitstream() const override;
        uint32_t CompDepth() const override;

    }; // class PixFmtInfo

    class PixFmtList : public ListBase<PixFmtInfo>
    {
// ctor, dtor, copying
    public:
        PixFmtList();
        ~PixFmtList();
// operations
    public:
        void Setup();
    private:
        static size_t GetCapacity();

    }; // class PixFmtList

} // namespace Flist
