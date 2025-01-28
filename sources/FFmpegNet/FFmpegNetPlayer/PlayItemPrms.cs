
// ---------------------------------------------------------------------
// File: PlayItemPrms.cs
// Classes: ModeX, OptionsX, PlayItemPrms
// Purpose: play item params
// Module: FFmpegNetPlayer.dll - .NET assembly which provides media player functionality
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

using System;

using FFmpegNetAux;


namespace FFmpegNetPlayer
{
    public class ModeX
    {
        public bool Alive = false;
        public bool HoldPresentationTime = true;
        public bool SkipVideoRendering = false;

    } // class ModeX

    public class OptionsX
    {
        public bool SyncByAudio = true;
        public bool UseFilterGraphAlways = false;
        public bool ConvFrameSequentially = false;
        public bool SwapFields = false;
        public int  AudioDevId = -1;

    }; // class OptionsX


    public class PlayItemPrms
    {
        public DemuxerParams    DemuxPrms;
        public string           DecPrmsVideo;
        public string           DecPrmsAudio;
        public FrameParamsVideo FramePrmsVideo;
        public FrameParamsAudio FramePrmsAudio;
        public ModeX            ModeX;
        public OptionsX         OptionsX;


        public bool IsValid => (DemuxPrms != null) && DemuxPrms.IsValid;

    } // class PlayItemPrms

} // namespace FFmpegNetPlayer
