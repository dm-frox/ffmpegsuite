#pragma once

// ---------------------------------------------------------------------
// File: AudioArrMuxer.h
// Classes: AudioArrMuxer
// Purpose: test class to write audio stream with a large number of channels
// Module: FFmpegCoreLib - managed wrappers for native object from FFmpegWrapper
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2022
// ---------------------------------------------------------------------


#include "include/AudioArrMuxerIface.h"

#include "CommTypes.h"
#include "ImplPtr.h"


namespace FFmpegInterop
{
    using DataMuxerBase = ImplPtr<Fwrap::IAudioArrMuxer>;

    public ref class AudioArrMuxer sealed : DataMuxerBase
    {
    public:
        AudioArrMuxer();

        void Build(StrType^ urlVideo, StrType^ urlAudio, StrType^ urlDst);
        void Close();

        property bool EndOfData { bool get(); }
        property bool Error { bool get(); }

    }; // public ref class AudioArrMuxer

} // namespace FFmpegInterop

