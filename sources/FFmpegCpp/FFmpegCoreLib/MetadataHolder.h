#pragma once

// ---------------------------------------------------------------------
// File: MetadataHolder.h
// Classes: MetadataHolder
// Purpose: helper to extract and store metadata
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 22.01.2020
// ---------------------------------------------------------------------

#include <string>

#include "ffmpeg_fwd.h"
#include "NonCopyable.h"

namespace Fcore
{
    class MetadataHolder : NonCopyable
    {
// data
        std::string  m_Metadata;
// ctor, dtor, copying
    public:
        MetadataHolder();
        ~MetadataHolder();
// properties
    public:
        const char* GetMetadata(int& len) const;
// operations
    public:
        int ExtractMetadata(const AVDictionary* pDict);

    }; // class MetadataHolder

} // namespace Fcore