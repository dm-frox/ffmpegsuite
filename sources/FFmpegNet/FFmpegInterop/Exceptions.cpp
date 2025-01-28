#include "pch.h"

#include "Exceptions.h"

#include "CommTypes.h"

using ExcType = System::Exception;

namespace
{
    ref class StrHelper
    {
        literal StrType^ Fmt1 = L"{0}: {1}";
        literal StrType^ Fmt2 = L"{0}: {1}, error code={2}";
        literal StrType^ Fmt3 = L"{0}: null pointer";
        literal StrType^ Fmt4 = L"{0}: bad result={1}";
        literal StrType^ Fmt5 = L"{0}: bad index={1} (max={2})";

    public:

        static StrType^ Compose1(const char* func, const char* msg)
        {
            return StrType::Format(Fmt1, gcnew StrType(func), gcnew StrType(msg));
        }

        static StrType^ Compose2(const char* func, const char* msg, int errCode)
        {
            return StrType::Format(Fmt2, gcnew StrType(func), gcnew StrType(msg), errCode);
        }

        static StrType^ Compose3(const char* func)
        {
            return StrType::Format(Fmt3, gcnew StrType(func));
        }

        static StrType^ Compose4(const char* func, int res)
        {
            return StrType::Format(Fmt4, gcnew StrType(func), res);
        }

        static StrType^ Compose5(const char* func, int idx, int maxVal)
        {
            return StrType::Format(Fmt5, gcnew StrType(func), idx, maxVal);
        }

    }; // ref class StrHelper

} // namespace anon

namespace FFmpegInterop
{
    void RaiseExc(const char* func, const char* msg)
    {
        throw gcnew ExcType(StrHelper::Compose1(func, msg));
    }

    void RaiseExc(const char* func, const char* msg, int errCode)
    {
        throw gcnew ExcType(StrHelper::Compose2(func, msg, errCode));
    }

    void AssertPtr(const char* func, const void* ptr)
    {
        if (!ptr)
            throw gcnew ExcType(StrHelper::Compose3(func));
    }

    void AssertRes(const char* func, int res)
    {
        if (res < 0)
            throw gcnew ExcType(StrHelper::Compose4(func, res));
    }

    void AssertGen(const char* func, const char* msg, bool cond)
    {
        if (!cond)
            throw gcnew ExcType(StrHelper::Compose1(func, msg));
    }

    void AssertGen(const char* func, const char* msg, const void* cond)
    {
        if (!cond)
            throw gcnew ExcType(StrHelper::Compose1(func, msg));
    }

} // namespace FFmpegInterop