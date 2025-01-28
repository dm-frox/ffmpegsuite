// ---------------------------------------------------------------------
// File: ParamsEnc.cs
// Classes: EncoderParamsBase, EncoderParamsAudio, EncoderParamsAudio
// Purpose: helpers to store parametrs for Muxer from FFmpegInterop.dll
// Module: FFmpegNetCore - .NET assembly to provide more comfortable interface to FFmpegInterop.dll
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

using System;

namespace FFmpegNetAux
{
    public class EncoderParamsBase : OptionList
    {
        readonly string m_Name;
        long            m_Bitrate;

        protected EncoderParamsBase(string encoderName) => m_Name = encoderName;

        public string Name => m_Name;

        public long Bitrate { get => m_Bitrate; set => m_Bitrate = value; }

        public bool IsValid => !string.IsNullOrEmpty(m_Name);
 
    } // class EncoderParamsBase

    public class EncoderParamsVideo : EncoderParamsBase
    {
        string m_Preset;
        int    m_Crf;

        public EncoderParamsVideo(string encoderName)
            : base(encoderName)
        {}

        public string Preset { get => m_Preset; set => m_Preset = value; }

        public int Crf { get => m_Crf; set => m_Crf = value; }

    } // class EncoderParamsVideo

    public class EncoderParamsAudio : EncoderParamsBase
    {
        public EncoderParamsAudio(string encoderName)
            : base(encoderName)
        {}

    } // class EncoderParamsAudio

} // namespace FFmpegNetAux

