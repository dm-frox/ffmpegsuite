#pragma once

// ---------------------------------------------------------------------
// File: ChannLayout.h
// Classes: ChannLayout
// Purpose: wrapper for AVChannelLayout
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2022
// ---------------------------------------------------------------------

#include "ffmpeg.h"
#include "NonCopyable.h"

namespace Fcore
{
    class ChannLayout : NonCopyable
    {
    // data
    private:
        static const ChannLayout Null;

        AVChannelLayout m_ChannLayout;

    // ctor, dtor, copying
    public:
        explicit ChannLayout(int chann = 0);
        ~ChannLayout();

    // properties
    public:
        bool IsNull() const { return m_ChannLayout.nb_channels == 0; }
        int Chann() const { return m_ChannLayout.nb_channels; }
        uint64_t Mask() const { return m_ChannLayout.u.mask; }

        AVChannelLayout* ToPtr() { return &m_ChannLayout; }
        const AVChannelLayout* ToPtr() const { return &m_ChannLayout; }

        static const ChannLayout& GetNull() { return Null; }

    // operations
    public:
        bool operator==(const ChannLayout& other) const;
        bool operator!=(const ChannLayout& other) const { return !operator==(other); }

        void Clear();
        int Set(const AVChannelLayout* src);
        int Set(const ChannLayout& src);
        int SetFromString(const char* name);
        int SetFromChann(int chann);
        int ToString(char* buff, int bsize) const;
        int CopyTo(AVChannelLayout& channLayout) const;

        static const AVChannelLayout* Find(const AVChannelLayout* vals, const ChannLayout& pref);

    }; // class ChannLayout

} // namespace Fcore
