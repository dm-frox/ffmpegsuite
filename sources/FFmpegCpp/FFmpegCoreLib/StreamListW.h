#pragma once

// ---------------------------------------------------------------------
// File: StreamListW.h
// Classes: StreamListW
// Purpose: container to store pointers to StreamWriter
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "StreamWriter.h"
#include "StreamListBase.h"

namespace Fcore
{
    class StreamListW : public StreamListBaseT<StreamWriter>
    {
        using Base = StreamListBaseT<StreamWriter>;

// ctor, dtor, copying
    public:
        StreamListW() = default;
        ~StreamListW() = default;
// properties
    public:
        bool EndOfData() const
        {
            return Base::CheckAll(&StreamWriter::EndOfData);
        }

    }; // class StreamListW

} // namespace Fcore