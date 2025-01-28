#pragma once

// ---------------------------------------------------------------------
// File: CodecDec.h
// Classes: Decoder, ReopenMode
// Purpose: decoder for avcodec_send_packet/avcodec_receive_frame decoding scheme
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "CodecBase.h"
#include "CodecDecHW.h"
#include "FrameTools.h"
#include "NonCopyable.h"

namespace Fcore
{
    class IFrameSink;
    class IDecoder;
    class ParamsEx;

    class Decoder : public CodecBase
    {
    // types
        class ReopenMode
        {
        // data
        private:
            AVCodecContext* m_CodecCtx{ nullptr };
            const AVCodec*  m_Codec{ nullptr };
            std::string     m_Options{};
            bool            m_Closed{ false };

        // ctor, dtor, copying
        public:
            ReopenMode();
            ~ReopenMode();
            ReopenMode(const ReopenMode&) = delete;
            ReopenMode& operator=(const ReopenMode&) = delete;
        // properties
        public:
            bool Enabled() const { return m_CodecCtx ? true : false; }
        private:
            const char* MediaType() const;
        // operations
        public:
            void Enable(AVCodecContext* codecCtx, const char* options);
            void CloseDecoder();
            int ReopenDecoder();

        }; // nested class ReopenMode

    // data
    private:
        static const char EncoderMetaKey[];
        static const bool SkipBadPackets;

        static const AVPixelFormat PrefPixFmtDef;
        static const AVPixelFormat PrefPixFmtAlt;

        AVStream&         m_Stream;

        const AVCodec*    m_Codec{ nullptr };
        std::string       m_Options{};

        IFrameSink* const m_FrameSink;
        Frame             m_Frame{};
        IDecoder*         m_Decoder{ nullptr };

        CodecHW           m_CodecHW{};

        //ReopenMode        m_ReopenMode{};
        
        int               m_PacktCntr{ 0 };
        int               m_FrameCntr{ 0 };

    // ctor, dtor, copying
    public:
        Decoder(AVStream& strm, IFrameSink* frameSink);
        ~Decoder();
    // operations
    public:
        int  OpenDecoder(const char* decParamsEx);
        int  DecodePacket(const AVPacket* pkt);
        void Reset();
        const char* GetEncoderNameFromMetadata() const;
        bool IsOpened() const { return m_Decoder ? true : false; }

        int DecodeSubtitle(AVPacket* pkt, AVSubtitle* sub, int* got);
        static void FreeSubtitle(AVSubtitle* sub);
    private:
        void CloseDecoder();
        int ReopenDecoder();

        const AVCodecParameters& CodecParams() const { return *m_Stream.codecpar; }
        AVCodecID CodecId() const { return CodecParams().codec_id; }
        const AVCodec* FindDecoderByName(const char* decName) const;
        const AVCodec* FindDecoder(const char* decName, bool& byName) const;
        int  AllocAndInitContext();
        bool InitHWAccel(const char* hwName, bool isGpu);
        void UpdatePixFormat(AVPixelFormat prefPixFmt, bool byName, bool hw);
        void UpdateSampFormat(AVSampleFormat prefSampFmt, bool byName);
        void InitContextEx(const ParamsEx& paramsEx, bool byName);
        int TryExternalDecoder();
        void DeleteDecoder();
        bool CheckReopenMode() const;

    }; // class Decoder

} // namespace Fcore
