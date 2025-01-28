#pragma once

// ---------------------------------------------------------------------
// File: DllLoader.h
// Classes: DllLoaderBase, DllLoader, NPtr
// Purpose: native helpers to load FFmpegWrapper
// Module: FFmpegInterop - managed wrappers for native object from FFmpegWrapper
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

namespace FFmpegInterop
{
    class DllLoaderBase
    {
        static void* const  InvalidValue;
        static const int	NoError;

        void* m_hModule;
        int   m_ErrCode;

    public:
        DllLoaderBase();
        ~DllLoaderBase();

        DllLoaderBase(const DllLoaderBase&) = delete;
        DllLoaderBase& operator=(const DllLoaderBase&) = delete;

        bool Load(const wchar_t* dllPath);
        void Unload();
        bool IsLoaded() const;
        void* Get();
        int GetErrCode() const;

    }; // class DllLoaderBase

    class DllLoader
    {
        typedef void* (*P_DLL_ENTRY_POINT)();
//
        DllLoaderBase m_Lib;
    public:
        DllLoader();
//
        void Unload() { m_Lib.Unload(); }
        int  GetErrCode() const { return m_Lib.GetErrCode(); }

        bool Load(const wchar_t* dllPath, const wchar_t* dllFolderEx);
        void* GetIface(const char* dllEntryPointName);
    private:
        static void RemoveBackSeparator(wchar_t* str);

    }; // class DllLoader

    template <typename T>
    class NPtr
    {
        T* m_Ptr;

    public:
        NPtr() : m_Ptr(new T()) {}
        ~NPtr() { delete m_Ptr; }

        T* operator->() const { return m_Ptr; }
        T* Move() { T* ret = m_Ptr; m_Ptr = nullptr;  return ret; }

        NPtr(const NPtr&) = delete;
        NPtr& operator=(const NPtr&) = delete;

    }; // class NPtr

} // namespace FFmpegInterop

using DllLoaderPtr = FFmpegInterop::NPtr<FFmpegInterop::DllLoader>;

