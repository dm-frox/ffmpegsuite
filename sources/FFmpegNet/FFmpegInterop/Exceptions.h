#pragma once

// ---------------------------------------------------------------------
// File: Exceptions.h
// Classes: function and macros
// Purpose: helpers to raise exceptions
// Module: FFmpegInterop - managed wrappers for native object from FFmpegWrapper
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

namespace FFmpegInterop
{

    void RaiseExc(const char* func, const char* msg);
    void RaiseExc(const char* func, const char* msg, int errCode);
    void AssertPtr(const char* func, const void* ptr);
    void AssertRes(const char* func, int res);
    void AssertGen(const char* func, const char* msg, bool cond);
    void AssertGen(const char* func, const char* msg, const void* cond);

} // namespace FFmpegInterop

#define RAISE_EXC(m)     RaiseExc(__FUNCTION__, (m))
#define RAISE_EXC2(m, c) RaiseExc(__FUNCTION__, (m), (c))
#define ASSERT_PTR(p)    AssertPtr(__FUNCTION__, (p))
#define ASSERT_RES(r)    AssertRes(__FUNCTION__, (r))
#define ASSERT_GEN(c, m) AssertGen(__FUNCTION__, (m), (c))
#define ASSERT_REF(r)    AssertGen(__FUNCTION__, "null reference", (r != nullptr))
#define ASSERT_IMPL()    AssertPtr(__FUNCTION__, Ptr())

