#pragma once
// ---------------------------------------------------------------------
// File: FrameHubTools.h
// Classes: FrameProc, MultiInput
// Purpose: helpers for FrameHub
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <vector>

#include "PumpSwitchIface.h"

#include "ffmpeg_fwd.h"
#include "NonCopyable.h"


namespace Fcore
{
    class IFrameProcessor;
    class FrameConv;
    class StreamReader;

    // class to process frames in place
    class FrameProc : NonCopyable
    {
        IFrameProcessor* m_FrameProc{ nullptr };
        AVFrame*         m_Frame{ nullptr };
    public:
        FrameProc();
        ~FrameProc();
        bool IsEnabled() const { return m_Frame ? true : false; }
        bool Enable(IFrameProcessor* frameProc);
        int ProcFrame(AVFrame* frame, FrameConv& frameConv);

    }; // class FrameProc

    // class to suppurt multi input filters for filter graph
    class MultiInput : NonCopyable, Ftool::IPumpSwitchW
    {
        std::vector<Ftool::IPumpSwitchW*> m_Ptrs;
    public:
        MultiInput(bool player);
        ~MultiInput();
        void Add(StreamReader* strm);
// IPumpSwitchW impl
    private:
        void EnablePumpWrite(bool enable) override final;

    public:
        Ftool::IPumpSwitchW* PumpSwitch();

    }; // class MultiInput

} // namespace Fcore