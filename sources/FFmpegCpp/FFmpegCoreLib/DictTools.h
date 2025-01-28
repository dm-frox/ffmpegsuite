#pragma once

// ---------------------------------------------------------------------
// File: DictTools.h
// Classes: DictHolder, DictIterator
// Purpose: helpers to work with AVDictionary
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "ffmpeg.h"
#include "NonCopyable.h"

namespace Fcore
{
    class DictHolder : NonCopyable
    {
// data
    private:
        static const int  Flags;

        AVDictionary* m_Dict;
// ctor, dtor, copying
    public:
        explicit DictHolder(const char* keyVal);
        ~DictHolder();
// properties
    public:
        bool IsEmpty() const { return m_Dict == nullptr; }
        int  Count() const;
// operations
    public:
        AVDictionary* Move();

        const char* operator[](const char* key) const;

        void Log() const;
        void Log(const char* func, const char* descr) const;

        static int Count(const AVDictionary* dict);
        static void Log(const AVDictionary* dict, const char* func, const char* descr, int logLev = -1);
        static void LogNotFound(AVDictionary* dict, const char* func);
        static bool FindKey(const AVDictionary* dict, const char* key, const char*& val);
        static AVDictionary* Copy(const AVDictionary* dict);

    }; // class DictHolder

    class DictIterator
    {
// data
    private:
        static const char NullKey[];
        static const int  Flags;

        const AVDictionary* const m_Dict;
        const AVDictionaryEntry* m_Entry{ nullptr };
// ctor, dtor, copying
    public:
        explicit DictIterator(const AVDictionary* dict);
        ~DictIterator();
// operations
    public:
        int  Count() const;
        bool Next(const char*& key, const char*& val);

    }; // class DictIterator

} // namespace Fcore
