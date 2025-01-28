#pragma once

// ---------------------------------------------------------------------
// File: CodecList.h
// Classes: CodecInfo, CodecNode, CodecList
// Purpose: classes to give information about supported supported codecs
// Module: FFmpegListLib - ffmpeg component lists
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <string>

#include "include/CodecListIface.h"

#include "ffmpeg.h"
#include "ListBase.h"


namespace Flist
{
    class CodecInfo : public ListItemInfoBaseOpt<Fwrap::ICodecInfo>
    {
// data
    private:
        static const char* const GpuSuffixes[];

        const AVCodec* const m_Codec;

        mutable std::string  m_Formats;
        mutable std::string  m_HWAccels;
// ctor, dtor, copying
    public:
        explicit CodecInfo(const AVCodec* codec);
        ~CodecInfo();
// operations
    private:
        static std::string SetFormats(const AVCodec* codec);
        static std::string SetHWAccel(const AVCodec* codec);
        static bool IsGpuCodec(const char* name);
        const char* GetPrivOpts(bool ex, std::string& optStr) const;
    public:
        static const char* GetCtxOptsEx(std::string& optStr);

// ICodecInfo impl
    private:
        const char* Name() const override final;
        const char* PrivOptions() const override final;
        const char* PrivOptionsEx() const override final;
        const char* LongName() const override final;
        bool IsDecoder() const override final;
        bool IsExperimental() const override final;
        const char* Formats() const override final;
        const char* HWAccels() const override final;

    }; // class CodecInfo

    class CodecNode : public ListItemInfoBase<Fwrap::ICodecNode>
    {
// data
    private:
        const AVCodecDescriptor* const m_CodecDescr;

        std::vector<CodecInfo>         m_Decoders{};
        std::vector<CodecInfo>         m_Encoders{};

// ctor, dtor, copying
    public:
        explicit CodecNode(const AVCodecDescriptor* codecDescr);
        ~CodecNode();
// properties
    public:
        AVCodecID Id() const { return m_CodecDescr->id; }
// operations
    private:
        bool CheckProp(int prop) const;
    public:
        void AddCodec(const AVCodec* codec);

// ICodecNode impl
    private:
        const char* Name() const override final;
        const char* LongName() const override final;
        const char* MediaTypeStr() const override final;
        int CodecCount() const override final;
        int DecoderCount() const override final;
        int EncoderCount() const override final;
        int MediaType() const override final;
        bool Lossless() const override final;
        bool Lossy() const override final;
        bool IntraOnly() const override final;
        const Fwrap::ICodecInfo* Decoder(int i) const override final;
        const Fwrap::ICodecInfo* Encoder(int i) const override final;

    }; // class CodecNode

    class CodecList :  public ListBaseOpt<CodecNode>
    {
// types
    private:
        using CodecsType = std::vector<const AVCodec*>;
// data
    private:
        int                 m_CodecCount{ 0 };

// ctor, dtor, copying
    public:
        CodecList();
        ~CodecList();
// properties
    public:
        int CodecCount() const { return m_CodecCount; }
        const char* CtxOptsEx() const;
// operations
    public:
        void Setup();
    private:
        void SetupNodes(const CodecsType& codecs);
        static size_t GetCapacityCodecs();
        static size_t GetCapacityNodes();

    }; // class CodecList

} // namespace Flist
