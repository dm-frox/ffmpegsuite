#pragma once

// ---------------------------------------------------------------------
// File: CodecListIface.h
// Classes: ICodecInfo (abstract), ICodecNode (abstract), ICodecList (abstract)
// Purpose: interfaces for the objects which give information about supported codecs
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "ListBaseIface.h"

namespace Fwrap
{
    class ICodecInfo : public IListItemInfoBase1
    {
    protected:
        ICodecInfo() = default;
        ~ICodecInfo() = default;

    public:
        virtual const char* LongName() const = 0;
        virtual bool IsDecoder() const = 0;
        virtual bool IsExperimental() const = 0;
        virtual const char* Formats() const = 0;
        virtual const char* HWAccels() const = 0;

    }; // class ICodecInfo

    class ICodecNode
    {
    protected:
        ICodecNode() = default;
        ~ICodecNode() = default;

    public:
        virtual const char* Name() const = 0;
        virtual const char* LongName() const = 0;
        virtual const char* MediaTypeStr() const = 0;
        virtual int CodecCount() const = 0;
        virtual int DecoderCount() const = 0;
        virtual int EncoderCount() const = 0;
        virtual int MediaType() const = 0;
        virtual bool Lossless() const = 0;
        virtual bool Lossy() const = 0;
        virtual bool IntraOnly() const = 0;
        virtual const ICodecInfo* Decoder(int i) const = 0;
        virtual const ICodecInfo* Encoder(int i) const = 0;

    }; // class ICodecNode

    class ICodecList : public IListBaseEx
    {
    protected:
        ICodecList() = default;
        ~ICodecList() = default;

    public:
        virtual const ICodecNode* operator[](int i) const = 0;
        virtual int CodecCount() const = 0;

        static ICodecList* CreateInstance();

    }; // class ICodecList

} // namespace Fwrap
