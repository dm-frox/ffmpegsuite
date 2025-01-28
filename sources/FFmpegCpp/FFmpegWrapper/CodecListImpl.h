#pragma once

// ---------------------------------------------------------------------
// File: CodecListImpl.h
// Classes: CodecListImpl
// Purpose: ICodecList implementation
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "CodecList.h"

#include "include/CodecListIface.h"


namespace Fwrap
{
    class CodecListImpl : ICodecList
    {
        Flist::CodecList m_CodecList;

        CodecListImpl();
        ~CodecListImpl();

// ICodecList impl
        void Setup() override final;
        int Count() const override final;
        const ICodecNode* operator[](int i) const override final;
        int CodecCount() const override final;
        const char* ContextOptionsEx() const override final;
// factory
        friend ICodecList* ICodecList::CreateInstance();

    }; // class CodecListImpl

} // namespace Fwrap

