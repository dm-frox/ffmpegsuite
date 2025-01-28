#pragma once

// ---------------------------------------------------------------------
// File: TranscoderImpl.h
// Classes: TranscoderImpl
// Purpose: ITranscoder implementation
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "Transcoder.h"

#include "include/TranscoderIface.h"

namespace Fwrap
{
    class TranscoderImpl : ITranscoder
    {
        Fapp::Transcoder m_Transcoder;

        explicit TranscoderImpl(bool useFilterGraph);
        ~TranscoderImpl();

// ITranscoder impl
        void Delete() override final;
        void SetFrameParamsVideo(int width, int height, const char* pixFmt, const char* vidFilter, int fps) override final;
        void SetEncoderParamsVideo(const char* vidEnc, int64_t vidBitrate, const char* preset, int crf, const char* vidOpts) override final;
        void SetFrameParamsAudio(int chann, int sampleRate, const char* sampFmt, const char* channLayout, const char* audFilter) override final;
        void SetEncoderParamsAudio(const char* audEnc, int64_t audBitrate, const char* audOpts) override final;
        void SetMetadata(const char* metadata, const char* metadataVideo, const char* metadataAudio) override final;
            
        int Build(
            const char* srcUrl, const char* srcFormat, const char* srcOptions,
            const char* dstUrl, const char* dstFormat, const char* dstOptions,
            bool useVideo, bool useAudio
            ) override final;
            
        bool EndOfData() const override final;
        bool Error() const override final;
        double Duration() const override final;
        double Position() const override final;
        void CancelInput() override final;

        void Run() override final;
        void Pause() override final;

        const IMediaSourceInfo* MediaSourceInfo() const override final;
// factory        
        friend ITranscoder* ITranscoder::CreateInstance(bool useFilterGraph);

    }; // class TranscoderImpl

} // namespace Fwrap
