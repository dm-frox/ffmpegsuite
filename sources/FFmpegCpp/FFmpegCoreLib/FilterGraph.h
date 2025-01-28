#pragma once

// ---------------------------------------------------------------------
// File: FilterGraph.h
// Classes: FilterGraph 
// Purpose: wrapper for AVFilterGraph
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <mutex>

#include "VectorPtr.h"
#include "Filter.h"
#include "MediaTypeHolder.h"


namespace Fcore
{
    class FrameHub;
    class ChannLayout;

    class FilterGraph
    {
// types
    private:
        using Mutex    = std::mutex;
        using AutoLock = std::lock_guard<Mutex>;
// data
    private:
        Mutex                   m_Mutex{};

        AVFilterGraph*          m_Graph{ nullptr };
        std::string             m_Name{};

        VectorPtrT<FilterInput>  m_Inputs{};
        VectorPtrT<FilterOutput> m_Outputs{};

        std::string             m_FilterString{};
        bool                    m_RebuldOnReset{ true };
        int                     m_InputNullCntr{ 0 };

        AVMediaType m_MediaTypeInput{ AVMEDIA_TYPE_UNKNOWN };
        AVMediaType m_MediaTypeOutput{ AVMEDIA_TYPE_UNKNOWN };

// ctor, dtor, copying
    public:
        FilterGraph();
        ~FilterGraph();
// properties
    public:
        const char* Name() const { return m_Name.c_str(); }
        bool IsReady() const { return m_Graph ? true : false; }
        AVFilterGraph* Get() { return m_Graph; }
// operations
    public:
        int Create(AVMediaType mediaTypeInput, AVMediaType mediaTypeOutput, const char* name);
        int Create(AVMediaType mediaType);
        void DisableRebuldOnReset(){ m_RebuldOnReset = false; }
        void Close();

        int InputCount() const { return m_Inputs.Count(); }
        int OutputCount() const { return m_Outputs.Count(); }

        int AddInputVideo(int width, int height, AVPixelFormat pixFmt, AVRational timeBase, AVRational frameRate);
        int AddInputAudio(int sampleRate, AVSampleFormat sampFmt, const ChannLayout& channLayout, AVRational timeBase);

        int AddOutputVideo(AVPixelFormat pixFmt);
        int AddOutputAudio(AVSampleFormat sampleFmt);

        int ConfigureFilterGraph(const char* filterString);
        int ReconfigureFilterGraph(const char* filterString);

        int SendCommand(const char* target, const char* cmd, const char* arg);

        int FilterFrame(AVFrame* inFrame, int inputIdx, ISinkFrameFlt* sink);
        int FilterFrameOne2One(AVFrame* inFrame, AVFrame* outFrame);
        void Reset();

    private:
        int RemoveOutputFrames();
        int Rebuild(const char* filterString);
        void LogFilters() const;

        bool AssertGraph(const char* msg) const;
        bool AssertInput(const char* msg) const;
        bool AssertOutput(const char* msg) const;
        bool AssertInputIndex(int outputIdx, const char* msg) const;
        bool AssertOutputIndex(int inputIdx, const char* msg) const;

    }; // class FilterGraph

} // namespace Fcore 
