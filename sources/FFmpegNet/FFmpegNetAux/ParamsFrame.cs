// ---------------------------------------------------------------------
// File: ParamsFrame.cs
// Classes: FrameParamsVideo, FrameParamsAudio
// Purpose: helpers to store parametrs for Muxer from FFmpegInterop.dll
// Module: FFmpegNetCore - .NET assembly to provide more comfortable interface to FFmpegInterop.dll
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

using System;


namespace FFmpegNetAux
{
    public class FrameParamsVideo
    {
        int    m_Width;
        int    m_Height;
        string m_PixelFormats;
        string m_FilterStr;
        int    m_FpsFactor;
        bool   m_Seq;

        public FrameParamsVideo()
        {}

        public int Width { get => m_Width; set => m_Width = value; }

        public int Height { get => m_Height; set => m_Height = value; }

        public string PixelFormats => m_PixelFormats;

        public string FilterStr { get => m_FilterStr; set => m_FilterStr = value; }

        public int FpsFactor { get => m_FpsFactor; set => m_FpsFactor = value; }

        public bool Seq { get => m_Seq; set => m_Seq = value; }

        public bool NeedSeq => !string.IsNullOrEmpty(m_FilterStr) && (m_FpsFactor != 0.0 || m_Seq);

    } // class FrameParamsVideo

    public class FrameParamsAudio 
    {
        int    m_Chann;
        int    m_SampleRate;
        string m_SampleFormat;
        string m_ChannLayout;
        string m_FilterStr;

        public FrameParamsAudio()
        {}

        public int Chann { get => m_Chann; set => m_Chann = value; }

        public int SampleRate { get => m_SampleRate; set => m_SampleRate = value; }

        public string SampleFormat { get => m_SampleFormat; set => m_SampleFormat = value; }

        public string ChannLayout { get => m_ChannLayout; set => m_ChannLayout = value; }

        public string FilterStr { get => m_FilterStr; set => m_FilterStr = value; }

    } // class FrameParamsAudio
 
} // namespace FFmpegNetAux


