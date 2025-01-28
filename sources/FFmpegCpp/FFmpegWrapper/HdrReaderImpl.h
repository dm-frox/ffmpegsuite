#pragma once

// ---------------------------------------------------------------------
// File: HdrReaderImpl.h
// Classes: HdrReaderImpl
// Purpose: IHdrReader implementation
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------


#include "Demuxer.h"

#include "include/HdrReaderIface.h"

namespace Fwrap
{

    class HdrReaderImpl : IHdrReader
    {
        Fcore::Demuxer m_Demuxer;

        HdrReaderImpl();
        ~HdrReaderImpl();

// IFormatList impl
        int StreamCount() const override final;
        CStrEx GetInfo() const override final;
        CStrEx GetMetadata() const override final;
        CStrEx GetChapters() const override final;
        CStrEx GetStreamInfo(int strmIdx) const override final;
        CStrEx GetStreamMetadata(int strmIdx) const override final;

        int Open(const char* url, const char* fmt, const char* ops) override final;
// factory
        friend IHdrReader* IHdrReader::CreateInstance();

    }; // class HdrReaderImpl

} // namespace Fwrap
