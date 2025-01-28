#pragma once

// ---------------------------------------------------------------------
// File: MediaPlayerStem.h
// Classes: MediaPlayerStem
// Purpose: managed wrapper for native IMediaPlayerStem implementation
// Module: FFmpegInterop - managed wrappers for native object from FFmpegWrapper
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "include/MediaPlayerStemIface.h"

#include "CommTypes.h"
#include "MediaSourceInfo.h"
#include "ImplPtr.h"
#include "Enums.h"


namespace FFmpegInterop
{
    using MediaPlayerBoxBase = ImplPtr<Fwrap::IMediaPlayerStem>;

    public ref class MediaPlayerStem sealed : MediaPlayerBoxBase
    {
        MediaSourceInfo^ m_MediaSourceInfo;

    public:
        MediaPlayerStem(bool useFilterGraphAlways, bool convFrameSequentially);

        void SetFrameParamsVideo(int width, int height, StrType^ pixFmt, StrType^ vidFilter, int fpsFactor);
        void SetRendModeAudio(int maxChann, int reduceSampleFormat);
        void SetFrameParamsAudio(int chann, int sampleRate, StrType^ sampFmt, StrType^ channLayout, StrType^ audFilter);

        void Build(StrType^ url, StrType^ fmt, StrType^ fmtOps,
            bool useVideo, StrType^ decPrmsVideo, bool useAudio, StrType^ decPrmsAudio);

        void SetupRendering(
            PtrType rendVideo, bool holdPresentationTime, bool synkByAudio, bool swapFields, 
            PtrType rendAudio, int audioDevId);

        void SetPosition(double pos, bool onPause);
        property bool SeekingInProgress { bool get(); }
        double GetShiftedVideoPosition(int frameCount);
        bool ShiftVideoPositionFwd(int frameCount);
        void SetAudioVolume(double vol);

        property double Position { double get(); }
        property double VideoRendPosition { double get(); }
        property bool Proceeding { bool get(); }
        property bool EndOfData { bool get(); }
        property bool EndOfVideo { bool get(); }

        property MediaSourceInfo^ SourceInfo { MediaSourceInfo^ get(); }

        property bool HasVideo { bool get(); }
        property int Width { int get(); }
        property int Height { int get(); }
        property StrType^ PixFmtStr { StrType^ get(); }

        property bool HasAudio { bool get(); }
        property int Chann { int get(); }
        property int SampleRate { int get(); }
        property StrType^ SampFmtStr { StrType^ get(); }

        property int RendWidth { int get(); }
        property int RendHeight { int get(); }

        property int RendChann { int get(); }
        property int RendSampleRate { int get(); }

        property int StreamCount { int get(); }

        StrType^ GetInfo();
        StrType^ GetMetadata();
        StrType^ GetChapters();

        StrType^ GetStreamInfo(int strmIdx);
        StrType^ GetStreamMetadata(int strmIdx);

        void Run();
        void Pause();
        void Stop();
        property bool PumpError { bool get(); }

        void StartRewinder(int fps);
        void StopRewinder();

        bool SendFilterCommandVideo(StrType^ tar, StrType^ cmd, StrType^ arg);
        void ReconfigureFilterGraphVideo(StrType^ filterString);

        bool SendFilterCommandAudio(StrType^ tar, StrType^ cmd, StrType^ arg);
        void ReconfigureFilterGraphAudio(StrType^ filterString);

    }; // class MediaPlayerStem

} // namespace FFmpegInterop
