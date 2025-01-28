#pragma once

// ---------------------------------------------------------------------
// File: SysDevColl.h
// Class: SysDevItem, SysDevColl, SysDevIter
// Purpose: fetch DirectShow devices (Windows only)
// Module: FFmpegToolsLib - auxiliary classes, functions
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <string>

struct ICreateDevEnum;
struct IEnumMoniker;
struct IPropertyBag;
struct IBaseFilter;

namespace Ftool
{
    class SysDevIter;

    class SysDevItem
    {
        // data
    private:
        static const wchar_t PropName[];
        static const wchar_t PropGuid[];
        static const wchar_t PropFcc[];
        static const wchar_t Error[];

        IPropertyBag* m_PropBag{ nullptr };
        IBaseFilter* m_Filter{ nullptr };

        // ctor, dtor, copying
    private:
        SysDevItem();
        ~SysDevItem();
    public:
        SysDevItem(const SysDevItem&) = delete;
        SysDevItem& operator=(const SysDevItem&) = delete;
        // properties
    private:
        bool IsValid() const { return m_PropBag ? true : false; }
        void SetPropBag(IPropertyBag* propBag) { m_PropBag = propBag; }
        void SetFilter(IBaseFilter* filter) { m_Filter = filter; }
    public:
        bool GetString(const wchar_t* propName, std::wstring& val) const;
        bool GetName(std::wstring& val) const { return GetString(PropName, val); }
        bool GetGuid(std::wstring& val) const { return GetString(PropGuid, val); }
        bool GetFcc(std::wstring& val) const { return GetString(PropFcc, val); }
        IBaseFilter* GetFilter() const { return m_Filter; }
        bool GetFilterName(std::wstring& val) const;
        // operations
    private:
        void Release();

        friend class SysDevColl;

    }; // class SysDevItem

    class SysDevColl
    {
        // data
    private:
        const void* const m_CatId;
        bool const        m_FetchFilter;
        IEnumMoniker* m_EnumMon{ nullptr };
        SysDevItem        m_Current{};

        // ctor, dtor, copying
    public:
        explicit SysDevColl(const void* catId, bool fetchFilter = false);
        ~SysDevColl();
        SysDevColl(const SysDevColl&) = delete;
        SysDevColl& operator=(const SysDevColl&) = delete;
        // properties
    private:
        const SysDevItem& Current() const { return m_Current; }
        const bool IsValid() const { return m_Current.IsValid(); }
    public:
        SysDevIter begin();
        const SysDevIter end() const;
        // operations
    public:
        void Close();
    private:
        bool Init();
        bool Next();

        friend class SysDevIter;

    }; // class SysDevColl

    class SysDevIter // lite iterator, range for 
    {
        // data
    private:
        SysDevColl* const m_SysDevColl;

        // ctor, dtor, copying
    private:
        explicit SysDevIter(SysDevColl* sysDevColl = nullptr)
            : m_SysDevColl(sysDevColl)
        {}
    public:
        SysDevIter(const SysDevIter&) = default;
        ~SysDevIter() = default;
        SysDevIter& operator=(const SysDevIter&) = delete;
        // operations
    public:
        SysDevIter& operator++() { m_SysDevColl->Next(); return *this; }

        const SysDevItem& operator*() const { return m_SysDevColl->Current(); }

        bool operator!=(const SysDevIter& rh) const
        {
            return m_SysDevColl
                ? m_SysDevColl->IsValid()
                : (rh.m_SysDevColl ? rh.m_SysDevColl->IsValid() : false); 
        }

        friend class SysDevColl;

    }; // class SysDevIter

} // namespace Ftool

