#pragma once

// ---------------------------------------------------------------------
// File: FrameSourceEx.h
// Classes: FrameSourceEx
// Purpose: extends FrameSource for MediaPlayerstem
// Module: FFmpegAppLib - aplications based on clases from FFmpegCoreLib
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "FrameSource.h"

namespace Fcore
{
    class IFrameProcessor;
    class IFrameData;
    class IFrameConv;
}

namespace Ftool
{
    class ProceedPump;
}

namespace Fapp
{

    class FrameSourceEx : public FrameSource
    {
// ctor, dtor, copying
    public:
        FrameSourceEx(bool useFilterGraphAlways, bool cnvFrameSequentially, bool player);
        FrameSourceEx();
        ~FrameSourceEx();
// properties
    public:

        Fcore::IFrameData* FrameDataVideo();
        Fcore::IFrameData* FrameDataAudio();

        Fcore::IFrameConv* FrameConvVideo();
// operations
    public:
        void SetFrameProcVideo(Fcore::IFrameProcessor* frameProc);
        void SetFrameProcAudio(Fcore::IFrameProcessor* frameProc);

        int SendFilterCommandVideo(const char* tar, const char* cmd, const char* arg);
        int ReconfigureFilterGraphVideo(const char* filterString);

        int SendFilterCommandAudio(const char* tar, const char* cmd, const char* arg);
        int ReconfigureFilterGraphAudio(const char* filterString);

        void SetPositionAudio(double pos, bool paused);
        bool SeekingInProgressAudio() const;
        void SetSeekPumpAudio();

    }; // class FrameSourceEx

} // namespace Fapp
