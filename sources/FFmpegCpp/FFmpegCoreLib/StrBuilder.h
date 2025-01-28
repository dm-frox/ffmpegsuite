#pragma once

// ---------------------------------------------------------------------
// File: StrBuilder.h
// Classes: StrBuilder
// Purpose: a helper to build string
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2023
// ---------------------------------------------------------------------

#include <string>

#include "NonCopyable.h"

namespace Fcore
{
    class StrBuilder : NonCopyable
    {
        const std::string m_Sepr;

    protected:
        std::string m_Str{};

    public:
        StrBuilder(int capacity, const char* sepr);
        StrBuilder(int capacity);

        ~StrBuilder();

        void Append(const char* item);
        std::string Str() const;

        static std::string FormatBytes(const void* bytes, int count);

    private:
        static char Hex(unsigned int i)
        {
            constexpr unsigned int dbase = 10u;

            return char(i < dbase ? '0' + i : 'a' + (i - dbase));
        }

    }; //class StrBuilder

} // namespace Fcore
