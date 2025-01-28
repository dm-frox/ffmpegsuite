// ---------------------------------------------------------------------
// File: TranscoderNet.cs
// Classes: TranscoderNet
// Purpose: transcoder
// Module: FFmpegNetTranscoder - .NET assembly which provides transcoder functionality
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------


using System;

using FFmpegInterop;
using FFmpegNetAux;

namespace FFmpegNetTranscoder
{
    public class TranscoderNet : IDisposable
    {

        Transcoder m_Transcoder;

        public TranscoderNet(bool useFilterGraph) => m_Transcoder = new Transcoder(useFilterGraph);

        public void Dispose() => m_Transcoder.Dispose();

        public bool EndOfData => m_Transcoder.EndOfData;

        public bool Error => m_Transcoder.Error;

        public double Duration => m_Transcoder.Duration;

        public double Position => m_Transcoder.Position;

        public void CancelInput() => m_Transcoder.CancelInput();

        public MediaSourceInfo MediaSourceInfo => m_Transcoder.SourceInfo;

        public void Run() => m_Transcoder.Run();

        public void Pause() => m_Transcoder.Pause();

        public bool Build(
            DemuxerParams readerParams,
            MuxerParams writerParams,
            string metadata, string metadataVideo, string metadataAudio,
            FrameParamsVideo vidParams, EncoderParamsVideo vidEncParams,
            FrameParamsAudio audParams, EncoderParamsAudio audEncParams,
            bool useVideo, bool useAudio)
        {
            bool ret = false;
            try
            {
                BuildImpl(
                    readerParams,
                    writerParams,
                    metadata, metadataVideo, metadataAudio,
                    vidParams, vidEncParams,
                    audParams, audEncParams,
                    useVideo, useAudio);
                ret = true;
            }
            catch (Exception exc)
            {
                LogNet.PrintExc(exc, "Transcoder1.Build");
            }
            return ret;
        }

        void BuildImpl(
            DemuxerParams readerParams,
            MuxerParams writerParams,
            string metadata, string metadataVideo, string metadataAudio,
            FrameParamsVideo vidFrmParams, EncoderParamsVideo vidEncParams,
            FrameParamsAudio audFrmParams, EncoderParamsAudio audEncParams,
            bool useVideo, bool useAudio)
        {
            if (!useVideo && !useAudio)
                throw new ArgumentException("Transcoder.BuildImpl, no media streams");

            if (useVideo)
            {
                if (vidFrmParams != null)
                {
                    m_Transcoder.SetFrameParamsVideo(vidFrmParams.Width, vidFrmParams.Height, vidFrmParams.PixelFormats, vidFrmParams.FilterStr, vidFrmParams.FpsFactor);
                }
                if (vidEncParams != null)
                {
                    m_Transcoder.SetEncoderParamsVideo(vidEncParams.Name, vidEncParams.Bitrate, vidEncParams.Preset, vidEncParams.Crf, vidEncParams.GetOptionString());
                }
            }

            if (useAudio)
            {
                if (audFrmParams != null)
                {
                    m_Transcoder.SetFrameParamsAudio(audFrmParams.Chann, audFrmParams.SampleRate, audFrmParams.SampleFormat, audFrmParams.ChannLayout, audFrmParams.FilterStr);
                }
                if (audEncParams != null)
                {
                    m_Transcoder.SetEncoderParamsAudio(audEncParams.Name, audEncParams.Bitrate, audEncParams.GetOptionString());
                }
            }

            m_Transcoder.SetMetadata(
                metadata, 
                useVideo ? metadataVideo : string.Empty,
                useAudio ? metadataAudio : string.Empty);

            m_Transcoder.Build(
                readerParams.Url, readerParams.Format, readerParams.GetOptionString(),
                writerParams.Url, writerParams.Format, writerParams.GetOptionString(),
                useVideo, useAudio);
        }

    } // public class Transcoder1

} // namespace FFmpegNetTranscoder


