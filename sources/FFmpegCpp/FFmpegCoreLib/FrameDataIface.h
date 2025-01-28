#pragma once

// ---------------------------------------------------------------------
// File: FrameDataIface.h
// Classes: IFrameData (abstract), IFrameConv (abstract)
// Purpose: interfaces to fetch and convert frames from FrameHub queue
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "ffmpeg_fwd.h"
#include "BaseIface0.h"


namespace Ftool
{
    class IPumpSwitchR;
    class FrameConvBase;
}

namespace Fcore
{
    class IFrameData : Ftool::IBase0
    {
    protected:
        IFrameData() = default;
        ~IFrameData() = default;

    public:
        virtual void SetReadFramePump(Ftool::IPumpSwitchR* readPump) = 0;

        virtual double VideoFramePos()  const = 0;
 
        virtual const void* PeekVideoFrame(int& width, int& height, int& stride, double& pos) const = 0;
        virtual AVFrame* PeekVideoFrame(double& pos) = 0;
        virtual const void* PeekAudioFrame(int& size, int& chann, int& sampleRate, double& pos) const = 0;
        virtual void DropFrame() = 0;

        virtual int FrameCount() const = 0;

        static double EndOfVideoMarker() { return -1.0; }
        static int EndOfAudioMarker() { return -1; }

    }; // class IFrameData

    class IFrameConv : Ftool::IBase0
    {
    protected:
        IFrameConv() = default;
        ~IFrameConv() = default;

    public:
        virtual int  Convert(AVFrame* inFrame, AVFrame* outFrame) = 0;
        virtual bool FilterIsUsed() const = 0;

    }; // class IFrameConv
    
} // namespace Fcore

