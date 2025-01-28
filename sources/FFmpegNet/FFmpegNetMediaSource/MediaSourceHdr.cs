// ---------------------------------------------------------------------
// File: MediaSourceHdr.cs
// Classes: HdrItem, MediaSourceHdr, MediaSourceRep (static)
// Purpose: read, store and save media source information using HdrReader from FFmpegInterop.dll
// Module: FFmpegNetCore - .NET assembly to provide more comfortable interface to FFmpegInterop.dll
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

using System;
using System.IO;

using FFmpegInterop;
using FFmpegNetAux;

namespace FFmpegNetMediaSource
{
    public struct HdrItem
    {
        public KeyValList Info;
        public KeyValList Metadata;
        public KeyValList Chapters;

    } // struct HdrItem

    public class MediaSourceHdr
    {
        readonly HdrItem   m_Root;
        readonly HdrItem[] m_Streams;

        public MediaSourceHdr(string url, string format, string options)
        {
            using (var hdrReader = new HdrReader())
            {
                hdrReader.Open(url, format, options);
                m_Root.Info = KeyValList.FromString(hdrReader.GetInfo());
                m_Root.Info.Add("File size", MediaSourceTxt.GetFileSize(url, false));
                m_Root.Metadata = KeyValList.FromString(hdrReader.GetMetadata());
                m_Root.Chapters = KeyValList.FromString(hdrReader.GetChapters());
                m_Streams = new HdrItem[hdrReader.StreamCount];
                for (int i = 0, n = m_Streams.Length; i < n; ++i)
                {
                    m_Streams[i].Info = KeyValList.FromString(hdrReader.GetStreamInfo(i));
                    m_Streams[i].Metadata = KeyValList.FromString(hdrReader.GetStreamMetadata(i));
                }
            }
        }

        public MediaSourceHdr(MediaPlayerStem mediaPlayerStem, string filePath)
        {
            m_Root.Info = KeyValList.FromString(mediaPlayerStem.GetInfo());
            m_Root.Info.Add("File size", MediaSourceTxt.GetFileSize(filePath, false));
            m_Root.Metadata = KeyValList.FromString(mediaPlayerStem.GetMetadata());
            m_Root.Chapters = KeyValList.FromString(mediaPlayerStem.GetChapters());
            m_Streams = new HdrItem[mediaPlayerStem.StreamCount];
            for (int i = 0, n = m_Streams.Length; i < n; ++i)
            {
                m_Streams[i].Info = KeyValList.FromString(mediaPlayerStem.GetStreamInfo(i));
                m_Streams[i].Metadata = KeyValList.FromString(mediaPlayerStem.GetStreamMetadata(i));
            }
        }

        public HdrItem Root => m_Root;

        public HdrItem[] Streams => m_Streams;

    } // class MediaSourceHdr

    public static class MediaSourceRep
    {
        const int IndentStep = 4;

        static void WriteKeyValList(StreamWriter strm, KeyValList kvList, int indent)
        {
            if (kvList.Length > 0)
            {
                string inds = new string(' ', indent * IndentStep);
                for (int i = 0; i < kvList.Length; ++i)
                {
                    var kv = kvList[i];
                    strm.WriteLine("{0}{1,-16} {2}", inds, kv.Key, kv.Value);
                }
            }
        }

        static void WriteHdrItem(StreamWriter strm, HdrItem item)
        {
            strm.WriteLine("    Info:");
            WriteKeyValList(strm, item.Info, 2);
            if (item.Metadata.Length > 0)
            {
                strm.WriteLine("    Metadata:");
                WriteKeyValList(strm, item.Metadata, 2);
            }
        }

        public static void Write(MediaSourceHdr msh, StreamWriter strm)
        {
            strm.WriteLine("Media source:");
            WriteHdrItem(strm, msh.Root);
            for (int i = 0, n = msh.Streams.Length; i < n; ++i)
            {
                strm.WriteLine("Stream #{0}", i);
                WriteHdrItem(strm, msh.Streams[i]);
            }
        }

    } // static class MediaSourceRep

} // namespace FFmpegNetMediaSource
