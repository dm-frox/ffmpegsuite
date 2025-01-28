#pragma once

// ---------------------------------------------------------------------
// File: CodecDec.h
// Classes: ISinkFrameDec (abstract), Decoder, ReopenParams (nested)
// Purpose: decoder for avcodec_send_packet/avcodec_receive_frame decoding scheme
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "BaseIface0.h"

#include "CodecBase.h"
#include "CodecDecHW.h"
#include "FrameTools.h"


namespace Fcore
{
    class IDecoder;
    class ParamsEx;

    class ISinkFrameDec : Ftool::IBase0
    {
    protected:
        ISinkFrameDec() = default;
        ~ISinkFrameDec() = default;

    public:
        virtual int ForwardFrame(AVFrame* frm) = 0;

    }; // class ISinkFrameDec

    class Decoder : public CodecBase
    {
    // types
        class ReopenParams
        {
        // data
        private:
            const AVCodec*  m_Codec{ nullptr };
            std::string     m_Options{};
            AVPixelFormat   m_PixFmt{ AV_PIX_FMT_NONE };

        // ctor, dtor, copying
        public:
            ReopenParams() = default;
            ~ReopenParams() = default;
            ReopenParams(const ReopenParams&) = delete;
            ReopenParams& operator=(const ReopenParams&) = delete;
        // properties
        public:
            bool IsEnabled() const { return m_Codec ? true : false; }
            const AVCodec* Codec() const { return m_Codec; }
            AVPixelFormat PixFmt() const { return m_PixFmt; }
            const char* Options() const { return m_Options.c_str(); }
        // operations
        public:
            void Enable(const AVCodecContext* codecCtx, const std::string& options);

        }; // nested class ReopenMode

    // data
    private:
        static const char EncoderMetaKey[];
        static const char ReopenSuffix[];
        static const bool SkipBadPackets;

        static const AVPixelFormat PrefPixFmtDef;
        static const AVPixelFormat PrefPixFmtAlt;

        AVStream&         m_Stream;

        Frame             m_Frame{};
        IDecoder*         m_Decoder{ nullptr };

        CodecHW           m_CodecHW{};

        ReopenParams      m_ReopenParams{};
        
        int               m_PacktCntr{ 0 };
        int               m_FrameCntr{ 0 };

    // ctor, dtor, copying
    public:
        explicit Decoder(AVStream& strm);
        ~Decoder();
    // operations
        int  OpenDecoder(const char* decParamsEx);
        int  DecodePacket(const AVPacket* pkt, ISinkFrameDec* sink);
        void Reset();
        const char* GetEncoderNameFromMetadata() const;
        bool IsOpened() const { return m_Decoder ? true : false; }

        int DecodeSubtitle(AVPacket* pkt, AVSubtitle* sub, int* got);
        static void FreeSubtitle(AVSubtitle* sub);
    private:
        const AVCodecParameters& CodecParams() const { return *m_Stream.codecpar; }
        AVCodecID CodecId() const { return CodecParams().codec_id; }
        const AVCodec* FindDecoderByName(const char* decName) const;
        const AVCodec* FindDecoder(const char* decName, bool& byName) const;
        int  AllocAndInitContext();
        bool InitHWAccel(const char* hwName, bool isGpu);
        void UpdatePixFormat(AVPixelFormat prefPixFmt, bool isGpu);
        void UpdateSampFormat(AVSampleFormat prefSampFmt);
        void InitContextEx(const ParamsEx& paramsEx, bool byName);
        int TryExternalDecoder();
        void DeleteDecoder();

        bool CloseOnReset() const;
        int ReopenDecoder();
        bool ResetDecoder();

    }; // class Decoder

} // namespace Fcore
