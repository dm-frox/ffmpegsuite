#pragma once

// ---------------------------------------------------------------------
// File: HdrReader.h
// Classes: HdrReader
// Purpose: managed wrapper for native IHdrReader implementation
// Module: FFmpegInterop - managed wrappers for native object from FFmpegWrapper
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "include/HdrReaderIface.h"

#include "CommTypes.h"
#include "MediaSourceInfo.h"
#include "ImplPtr.h"

namespace FFmpegInterop
{
    using HdrReaderBase = ImplPtr<Fwrap::IHdrReader>;

    public ref class HdrReader sealed : HdrReaderBase
    {
    public:
        HdrReader();

        void Open(StrType^ url, StrType^ fmt, StrType^ ops);

        property int StreamCount { int get(); }

        StrType^ GetInfo();
        StrType^ GetMetadata();
        StrType^ GetChapters();

        StrType^ GetStreamInfo(int strmIdx);
        StrType^ GetStreamMetadata(int strmIdx);

    }; // public ref class HdrReader

} // namespace FFmpegInterop
