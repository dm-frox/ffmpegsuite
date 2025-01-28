#pragma once

// ---------------------------------------------------------------------
// File: Transcoder.h
// Classes: Transcoder
// Purpose: managed wrapper for native ITranscoder implementation
// Module: FFmpegInterop - ProtocolInfo
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "include/TranscoderIface.h"

#include "CommTypes.h"
#include "MediaSourceInfo.h"
#include "ImplPtr.h"

namespace FFmpegInterop
{
    using TranscoderBase = ImplPtr<Fwrap::ITranscoder>;


    public ref class Transcoder sealed : TranscoderBase
    {
        MediaSourceInfo^ m_MediaSourceInfo;
    public:
        Transcoder(bool useFilterGraph);

        void SetFrameParamsVideo(int width, int height, StrType^ pixFmt, StrType^ vidFilter, int fps);

        void SetEncoderParamsVideo(StrType^ vidEnc, int64_t vidBitrate, StrType^ preset, int crf, StrType^ vidOpts);

        void SetFrameParamsAudio(int chann, int sampleRate, StrType^ sampFmt, StrType^ channLayout, StrType^ audFilter);

        void SetEncoderParamsAudio(StrType^ audEnc, int64_t audBitrate, StrType^ audOpts);

        void SetMetadata(StrType^ metadata, StrType^ metadataVideo, StrType^ metadataAudio);

        void Build(
            StrType^ srcUrl, StrType^ srcFormat, StrType^ srcOptions,
            StrType^ dstUrl, StrType^ dstFormat, StrType^ dstOptions,
            bool useVideo, bool useAudio);

        property bool EndOfData { bool get(); }
        property bool Error { bool get(); }
        property double Duration { double get(); }
        property double Position { double get(); }

        void CancelInput();

        void Run();
        void Pause();

        property MediaSourceInfo^ SourceInfo { MediaSourceInfo^ get(); }

    }; // ref class Transcoder

} // namespace FFmpegInterop


