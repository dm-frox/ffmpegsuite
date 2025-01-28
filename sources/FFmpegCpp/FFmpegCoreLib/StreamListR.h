#pragma once

// ---------------------------------------------------------------------
// File: StreamListR.h
// Classes: StreamListR
// Purpose: container to store pointers to StreamReader
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "StreamReader.h"
#include "StreamListBase.h"

#include "Logger.h"

namespace Fcore
{
    class StreamListR : public StreamListBaseT<StreamReader>
    {
        using Base = StreamListBaseT<StreamReader>;

// ctor, dtor, copying
    public:
        StreamListR() = default;
        ~StreamListR() = default;

// operations
    public:
        void Reset()
        {
            Base::ForEach(&StreamReader::Reset);
        }

    }; // class StreamListR

} // namespace Fcore