#pragma once

// ---------------------------------------------------------------------
// File: CodecList.h
// Classes: CodecInfo, CodecNode, CodecList
// Purpose: managed wrapper for native ICodecList implementation
// Module: FFmpegInterop - managed wrappers for native object from FFmpegWrapper
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "include/CodecListIface.h"

#include "CommTypes.h"
#include "ImplPtr.h"


namespace FFmpegInterop
{
    public ref class CodecInfo sealed
    {
        using InfoPrt = const Fwrap::ICodecInfo* const;

        InfoPrt m_Info;

    internal:
        CodecInfo(InfoPrt p);

    public:
        property StrType^ Name { StrType^ get(); }
        property StrType^ LongName { StrType^ get(); }

        property bool Decoder { bool get(); }
        property bool Experimental { bool get(); }

        property StrType^ Formats { StrType^ get(); }
        property StrType^ HWAccels { StrType^ get(); }
        property StrType^ PrivOptions { StrType^ get(); }
        property StrType^ PrivOptionsEx { StrType^ get(); }

        virtual StrType^ ToString() override;

    }; // ref class CodecInfo

    public ref class CodecNode sealed
    {
        using NodePrt = const Fwrap::ICodecNode* const;

        NodePrt m_Node;

    public:
        CodecNode(NodePrt p);

        property StrType^ Name { StrType^ get(); }
        property StrType^ LongName { StrType^ get(); }
        property StrType^ MediaTypeStr { StrType^ get(); }

        property int CodecCount { int get(); }
        property int DecoderCount { int get(); }
        property int EncoderCount { int get(); }
        property int MediaType { int get(); }

        property bool Lossless { bool get(); }
        property bool Lossy { bool get(); }
        property bool IntraOnly { bool get(); }

        CodecInfo^ Decoder(int i);
        CodecInfo^ Encoder(int i);

        virtual StrType^ ToString() override;

    }; // ref class CodecNode

    using CodecListBase = ImplPtr<Fwrap::ICodecList>;

    public ref class CodecList sealed : CodecListBase
    {
    public:
        CodecList();

        property int Count { int get(); }
        property CodecNode^ default[int]{ CodecNode ^ get(int i); }
        property int CodecCount { int get(); }
        property StrType^ ContextOptionsEx { StrType^ get(); }

    }; // ref class CodecList

} // namespace FFmpegInterop
