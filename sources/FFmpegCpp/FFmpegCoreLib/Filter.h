#pragma once

// ---------------------------------------------------------------------
// File: Filter.h
// Classes: ISinkFrameFlt (abstract), FilterBase, FilterT (template), FilterInput, FilterOutput
// Purpose: wrappers for AVFilterContext, classec to support input/output filters in filter graph
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <string>
#include <vector>

#include "BaseIface0.h"

#include "MediaTypeHolder.h"
#include "FrameTools.h"

namespace Fcore
{
    class FilterGraph;
    class ChannLayout;

    class ISinkFrameFlt : Ftool::IBase0
    {
    protected:
        ISinkFrameFlt() = default;
        ~ISinkFrameFlt() = default;

    public:
        virtual int ForwardFrame(AVFrame* frame, int outputIdx) = 0;

    }; // class ISinkFrameFlt

    class FilterBase : public MediaTypeHolder
    {
// data
    private:
        static const int MaxInstNameLen;

        int const            m_Index;
        FilterGraph&         m_Graph;

        AVFilterContext*     m_FilterCtx{ nullptr };

        std::string      m_FiltName{};
        std::string      m_InstName{};
        std::string      m_Args{};
// ctor, dtor, copying
    protected:
        FilterBase(AVMediaType mediaType, int idx, FilterGraph& graph);
        ~FilterBase();
// properties
    public:
        int  Index() const { return m_Index; }
    protected:
        AVFilterContext* FilterContext() { return m_FilterCtx; }
        const char* GenInstName(const char* id, const char* cname) const;
        char* GenId(const char* idBase) const;
// operations
    public:
        static int LinkDirect(FilterBase& in, FilterBase& out);

        void FreeFilter();
        int  RecreateFilter();

    protected:
        bool Assert(const char* msg) const;
        bool AssertFirst(const char* msg) const;

        int CreateFilter(const char* filtName, const char* instName, const char* args);
        int SetPrivOptionAsBin(const char* optName, int optVal);
        void InitInOutItem(AVFilterInOut* fltListItem, char* id) const;

    }; // class FilterBase

    template<class T>
    class FilterT : public FilterBase
    {
// ctor, dtor, copying
    protected:
        FilterT(AVMediaType mediaType, int idx, FilterGraph& graph)
            : FilterBase(mediaType, idx, graph)
        {}
        FilterT() = default;
// operations
    public:
        void InitInOutItem(AVFilterInOut* fltListItem) const
        {
            FilterBase::InitInOutItem(fltListItem, GenId(T::IdBase));
        }
    protected:
        int  CreateFilter(const char* args)
        {
            return FilterT<T>::CreateFilter(IsVideo() ? T::FilterNameVideo : T::FilterNameAudio, args);
        }
    private:
        int  CreateFilter(const char* filtName, const char* args)
        {
            return FilterBase::CreateFilter(filtName, FilterBase::GenInstName(T::IdBase, filtName), args);
        }

    }; // template class FilterT

    class FilterInput : public FilterT<FilterInput>
    {
// data
    public:
        static const char FilterNameVideo[];
        static const char FilterNameAudio[];
        static const char IdBase[];
    private:
        int m_Timebase_N{ 0 };
        int m_Timebase_D{ 0 };
// ctor, dtor, copying
    public:
        FilterInput(AVMediaType mediaType, int idx, FilterGraph& graph);
        ~FilterInput();
// properties
    public:
        void GetTimebase(AVRational& timeBase) const;
    private:
        void SetTimebase(AVRational timeBase);
// operations
    public:
        int CreateFilterVideo(int width, int height, AVPixelFormat pixFmt, AVRational timeBase, AVRational frameRate);
        int CreateFilterAudio(const ChannLayout& channLayout, int sampleRate, AVSampleFormat sampFmt, AVRational timeBase);
        int AddFrame(AVFrame* src);
        int SendFrame(AVFrame* frame);

    }; // class FilterInput

    class FilterOutput : public FilterT<FilterOutput>
    {
// data
    public:
        static const char FilterNameVideo[];
        static const char FilterNameAudio[];
        static const char IdBase[];
    private:
        static const char OptNameVideo[];
        static const char OptNameAudio[];

        int   m_Format{ -1 };
        Frame m_Frame{};
// ctor, dtor, copying
    public:
        FilterOutput(AVMediaType mediaType, int idx, FilterGraph& graph);
        ~FilterOutput();
// operations
    public:
        int CreateFilterVideo(AVPixelFormat pixFmt);
        int CreateFilterAudio(AVSampleFormat sampFmt);

        int RecreateFilterVideo()
        {
            return CreateFilterVideo(static_cast<AVPixelFormat>(m_Format));
        }

        int RecreateFilterAudio()
        {
            return CreateFilterAudio(static_cast<AVSampleFormat>(m_Format));
        }

        int GetFrame(AVFrame* outFrame);
        int ReceiveFrame(ISinkFrameFlt* sink);
        int RemoveFrames();
        bool NoOutputFrames();

    }; // class FilterOutput

} // namespace Fcore


