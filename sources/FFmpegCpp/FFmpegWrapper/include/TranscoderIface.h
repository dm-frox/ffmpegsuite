#pragma once

// ---------------------------------------------------------------------
// File: TranscoderIface.h
// Classes: ITranscoder (abstract)
// Purpose: interface for the transcoder object
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "BaseIface.h"

namespace Fwrap
{
    class IMediaSourceInfo;

    class ITranscoder : public IBase
    {
    protected:
        ITranscoder() = default;
        ~ITranscoder() = default;

    public:
        
        virtual void SetFrameParamsVideo(int width, int height, const char* pixFmt, const char* vidFilter, int fps) = 0;
        virtual void SetEncoderParamsVideo(const char* vidEnc, int64_t vidBitrate, const char* preset, int crf, const char* vidOpts) = 0;
        virtual void SetFrameParamsAudio(int chann, int sampleRate, const char* sampFmt, const char* channLayout, const char* audFilter) = 0;
        virtual void SetEncoderParamsAudio(const char* audEnc, int64_t audBitrate, const char* audOpts) = 0;
        virtual void SetMetadata(const char* metadata, const char* metadataVideo, const char* metadataAudio) = 0;
            
        virtual int Build(
            const char* srcUrl, const char* srcFormat, const char* srcOptions,
            const char* dstUrl, const char* dstFormat, const char* dstOptions,
            bool useVideo, bool useAudio
            ) = 0;
            
        virtual bool EndOfData() const = 0;
        virtual bool Error() const = 0;
        virtual double Duration() const = 0;
        virtual double Position() const = 0;
        virtual void CancelInput() = 0;

        virtual void Run() = 0;
        virtual void Pause() = 0;

        virtual const IMediaSourceInfo* MediaSourceInfo() const = 0;
        
        static ITranscoder* CreateInstance(bool useFilterGraph);

    }; // class ITranscoder

} // namespace Fwrap
