#pragma once

// ---------------------------------------------------------------------
// File: HWrap.h
// Classes: HWrap
// Purpose: HWAVEOUT wrapper
// Module: AudioRendLib - audio renderer based on windows waveform audio API (winmm.lib)
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

template <typename H, typename D, H InvalidValue = (H)0>
class HWrap
{
// data
private:
    H m_Value;

// ctor, dtor, copying
public:
    HWrap()
        : m_Value(InvalidValue)
    {}

    explicit HWrap(H val)
        : m_Value(val)
    {}

    ~HWrap()
    {
        if (IsValid())
        {
            D()(m_Value);
        }
    }

    HWrap(const HWrap&) = delete;

    HWrap& operator=(const HWrap&) = delete;

// properties
public:

    operator H() const
    {
        return m_Value;
    }
        
    bool IsValid() const
    {
        return (m_Value != InvalidValue);
    }

    UINT_PTR ToUIntPtr() const
    {
        return (UINT_PTR)m_Value;
    }

// operations
public:
    void Delete()
    {
        if (IsValid())
        {
            D()(m_Value);
            m_Value = InvalidValue;
        }
    }

    bool Assign(H val)
    {
        bool ret = false;
        if (!IsValid())
        {
            m_Value = val;
            ret = true;
        }
        return ret;
    }

    inline
    static BOOL ToBOOL(bool v)
    {
        return v ? TRUE : FALSE;
    }

}; // class HWrap
