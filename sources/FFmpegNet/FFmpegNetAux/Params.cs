// ---------------------------------------------------------------------
// File: Params.cs
// Classes: OptionList, DemuxerParams, MuxerParams
// Purpose: helpers to store parametrs for Demuxer and Muxer from FFmpegInterop.dll
// Module: FFmpegNetCore - .NET assembly to provide more comfortable interface to FFmpegInterop.dll
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

using System;

using OptsDict = System.Collections.Generic.Dictionary<string, string>;


namespace FFmpegNetAux
{
    public class OptionList
    {
        KeyValList m_Options;

        public OptionList() => m_Options = new KeyValList();

        protected int OptionCount => m_Options.Length;

        public string GetOptionString() => m_Options.ToString();

        public void AddOption(string key, string val) => m_Options.Add(key, val);

        public void SetOptions(string[] opts) // key0, value0, key1, value1, ...
        {
            m_Options.Clear();

            if (opts == null || opts.Length == 0)
                return;

            if ((opts.Length & 1) != 0)
                throw new Exception("OptionList.SetOptions, option list length must be even");

            for (int i = 0; i < opts.Length; i += 2)
            {
                AddOption(opts[i], opts[i + 1]);
            }
        }

        public void SetOptions(OptsDict opts)
        {
            m_Options.Clear();

            if (opts == null || opts.Count == 0)
                return;

            foreach (var kv in opts)
            {
                m_Options.Add(kv);
            }
        }

    } // class OptionList

    public class DemuxerParams : OptionList
    {
        readonly string m_Url;
        string m_Format;
        bool m_UseVideo;
        bool m_UseAudio;

        public DemuxerParams(string url, bool useVideo = true, bool useAudio = true)
        {
            m_Url = url;
            m_Format = string.Empty;
            m_UseVideo = useVideo;
            m_UseAudio = useAudio;
        }

        public bool IsValid => !string.IsNullOrEmpty(m_Url);
   
        public string Url => m_Url;
        
        public string Format { get => m_Format; set => m_Format = value; }

        public bool UseVideo { get => m_UseVideo; set => m_UseVideo = value; }

        public bool UseAudio { get => m_UseAudio; set => m_UseAudio = value; }

    } // class DemuxerParams

    public class MuxerParams : OptionList
    {
        const string DefFormat = "";

        readonly string m_Url;
        readonly string m_Format;


        public MuxerParams(string url, string format)
        {
            m_Url = url;
            m_Format = format;
        }

        public MuxerParams(string url)
            : this(url, DefFormat)
        {}

        public bool IsValid => !string.IsNullOrEmpty(m_Url);

        public string Url => m_Url;

        public string Format => m_Format;

    } // class MuxerParams

} // namespace FFmpegNetAux
