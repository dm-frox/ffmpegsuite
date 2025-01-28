#pragma once

// ---------------------------------------------------------------------
// File: MultiString.h
// Classes: MultiString
// Purpose: helper to store key-value pairs
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 22.01.2020
// ---------------------------------------------------------------------

#include <string>
#include <cstring>

#include "ffmpeg_fwd.h"
#include "NonCopyable.h"

namespace Fcore
{
    class MultiString : NonCopyable
    {
// data
    private:
        static const char   KeyValDelim;
        static const char   KeyValDelimIni;
        static const char   PairDelim;
        static const char   PairDelimIni[];
        static const size_t DefItemLen;
        static const int    DictFlags;
        static const int    DefPrecision;

        std::string  m_MultiString{};
        int          m_Count{ 0 };
// ctor, dtor, copying
    public:
        MultiString();
        ~MultiString();
// operations
    public:
        void Reserve(int capacity);
        void Reset();
        void SetEnd();
        void Add(const char* key, const char* val);
        void Add(const char* key, int32_t val);
        void Add(const char* key, uint32_t val);
        void Add(const char* key, int64_t val);
        void Add(const char* key, uint64_t val);
        void Add(const char* key, bool val);
        void AddTime(const char* key, double val, double val2 = -1.0);
        void AddFramerate(const char* key, AVRational val);
        int  AddDictionary(const AVDictionary* dict);
        void SetDictionary(const AVDictionary* pDict);

        static AVDictionary* CreateDictionary(const char* keyVals);
        static void Dump(const char* keyVals);
        static void Assign(std::string& dst, const char* src);
    private:
        template<typename T>
        void Add_(const char* key, T val);
// properties
    public:
        int Count() const { return m_Count; }
        const char* Str() const { return m_MultiString.c_str(); }
        int Length() const { return (int)m_MultiString.length(); }
        bool IsEmpty() const { return m_MultiString.empty(); }

        static int FindLength(const char* str);
        static std::string PackDictionary(const AVDictionary* pDict, int& count);
        static std::string FromIniFormat(const char* strIni);
        static std::string ToString(const std::string& multiStr);

    }; // class MultiString

    class MultiStringIterator
    {
        const char* m_Curr;
    public:
        MultiStringIterator(const char* multiStr) : m_Curr(multiStr ? multiStr : "") {}
        const char* Curr() const { return m_Curr; }
        const char* Next() { return (m_Curr += (std::strlen(m_Curr) + 1)); }

    }; // class MultiStringIterator

} // namespace Fcore

