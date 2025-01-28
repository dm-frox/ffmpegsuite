#pragma once

// ---------------------------------------------------------------------
// File: Marshal.h
// Classes: NativeString
// Purpose: native strings - .NET strings convertion
// Module: FFmpegInterop - managed wrappers for native object from FFmpegWrapper
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <vcclr.h>
#include <cstring>
#include <cstdlib>

namespace FFmpegInterop
{

// System::String -> const char* (UTF8)
    class NativeString
    {
        using Mar = System::Runtime::InteropServices::Marshal;
        using Enc = System::Text::Encoding;
        using Str = System::String;
        using Ptr = System::IntPtr;

        Ptr m_Buff;
        int m_Length;

        char* Buff() { return static_cast<char*>(m_Buff.ToPointer()); }

    public:

        NativeString(const NativeString&) = delete;
        NativeString& operator=(const NativeString&) = delete;

        NativeString(Str^ str)
        {
            if (!Str::IsNullOrEmpty(str))
            {
                auto chars = Enc::UTF8->GetBytes(str);
                int len = chars->Length;
                m_Buff = Mar::AllocHGlobal(len + 1);
                Mar::Copy(chars, 0, m_Buff, len);
                *(Buff() + len) = '\0';
                m_Length = len;
            }
            else
            {
                m_Length = 0;
            }
        }

        ~NativeString()
        {
            if (m_Length > 0)
            {
                Mar::FreeHGlobal(m_Buff);
            }
        }

        const char* CStr()
        {
            return m_Length > 0
                ? Buff()
                : reinterpret_cast<char*>(&m_Length);
        }

        int Length() const
        {
            return m_Length;
        }

// const char* (UTF8) -> System::String 
        static Str^ GetString(const char* cstr, int len)
        {
            int charCount = cstr ? ((len > 0) ? len : (int)std::strlen(cstr)) : 0;
            if (charCount > 0)
            {
                auto chars = gcnew array<unsigned char>(charCount);
                Mar::Copy(Ptr(const_cast<char*>(cstr)), chars, 0, charCount);
                return Enc::UTF8->GetString(chars);
            }
            else
            {
                return Str::Empty;
            }
        }

        template<typename T> // T - FWrap::CStrEx
        static Str^ GetStringEx(T p)
        {
            return GetString(p.Str, p.Len);
        }

    }; // class NativeString

// System::String -> wchar_t* in stack buffer
    template<size_t N>
    class ToWCharsN
    {
        wchar_t m_Buff[N];

    public:
        ToWCharsN(System::String^ src)
        {
            if (src != nullptr && src->Length > 0)
            {
                pin_ptr<const wchar_t> pp = PtrToStringChars(src);
                wcscpy_s(m_Buff, pp);
            }
            else
            {
                m_Buff[0] = L'\0';
            }
        }

        ~ToWCharsN() = default;

        operator const wchar_t*() const { return m_Buff; }
        int BuffLen() const { return N; }
        int StrLen() const { wcslen(m_Buff); }

    }; // class ToWCharsN

    using ToWChars = ToWCharsN<_MAX_PATH>; // to store path

} // namespace FFmpegInterop

// Attention! This is a dangerous macro to convert System::String^ to const char* in UTF8
// Use only as a rvalue argument. E.g. Foo(TO_CHARS(s));
// Don't store return value. E.g. const char* x = TO_CHARS(s); // ERROR !!

#define TO_CHARS(str) FFmpegInterop::NativeString(str).CStr()

#define TO_WCHARS(str) FFmpegInterop::ToWCharsN<_MAX_PATH>(str)


// MSDN

// static void Copy (byte[] source, int startIndex, IntPtr destination, int length);
// Copies data from a one - dimensional, managed 8 - bit unsigned integer array to an unmanaged memory pointer

// static void Copy(IntPtr source, byte[] destination, int startIndex, int length);
// Copies data from an unmanaged memory pointer to a managed 8 - bit unsigned integer array.

// StringToHGlobalUni is useful for custom marshaling or for use when mixing managed and unmanaged code.
// Because this method allocates the unmanaged memory required for a string, 
// always free the memory by calling FreeHGlobal.
// This method provides the opposite functionality of Marshal.PtrToStringUni.
// This method copies embedded null characters, and includes a terminating null character.


