// ---------------------------------------------------------------------
// File: MediaSourceTxt.cs
// Classes: MediaSourceTxt
// Purpose: helper to visualize MediaSourceInfo information from  FFmpegInterop.dll
// Module: FFmpegNetCore - .NET assembly to provide more comfortable interface to FFmpegInterop.dll
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

using System;
using System.Text;
using System.IO;

using FFmpegInterop;
using FFmpegNetAux;

namespace FFmpegNetMediaSource
{
    public class MediaSourceTxt
    {
        const double K = 1024;
        const double M = K * K;

        const string FileProto = "file:";
        const string NoMedia   = "No stream or not used";

        readonly string m_Video;
        readonly string m_Audio;
        readonly string m_Duration;
        readonly string m_Size;

        public MediaSourceTxt(MediaSourceInfo info, string path)
        {
            m_Video = GetVideo(info);
            m_Audio = GetAudio(info);
            m_Duration = GetDuration(info);
            m_Size = GetFileSize(path, true);
        }

        public string Video => m_Video;

        public string Audio => m_Audio;
 
        public string Duration => m_Duration;

        public string Size => m_Size;

        static string GetVideo(MediaSourceInfo info)
        {
            StringBuilder sb = new StringBuilder(64);
            if (info.HasVideo)
            {
                sb.AppendFormat("{0}x{1}; {2:F3} frm/s", info.Width, info.Height, info.Fps);
                sb.AppendLine();
                sb.AppendFormat("decoder: {0}", info.VideoCodec);
                sb.AppendLine();
                sb.AppendFormat("pix.fmt: {0}", info.PixelFormat);
                if (info.VideoBitrate > 0)
                {
                    sb.AppendLine();
                    sb.AppendFormat("bitrate: {0}", FormatBitrate(info.VideoBitrate));
                }
            }
            else
            {
                sb.Append(NoMedia);
            }
            return sb.ToString();
        }

        static string GetAudio(MediaSourceInfo info)
        {
            StringBuilder sb = new StringBuilder(64);
            if (info.HasAudio)
            {
                sb.AppendFormat("{0} chann; {1} samp/s", info.Chann, info.SampleRate);
                sb.AppendLine();
                sb.AppendFormat("decoder: {0}", info.AudioCodec);
                sb.AppendLine();
                sb.AppendFormat("samp.fmt: {0}", info.SampleFormat);
                if (info.AudioBitrate > 0)
                {
                    sb.AppendLine();
                    sb.AppendFormat("bitrate: {0}", FormatBitrate(info.AudioBitrate));
                }
            }
            else
            {
                sb.Append(NoMedia);
            }
            return sb.ToString();
        }

        static string GetDuration(MediaSourceInfo info)
        {
            double dur = info.Duration;
            StringBuilder sb = new StringBuilder(64);
            sb.Append("Duration: ");
            if (dur > 0.0)
            {
                sb.Append(ParamsTools.FormatPosition(info.Duration, true));
                sb.AppendFormat(" ({0:F1} sec)", dur);
            }
            else
            {
                sb.Append("0");
            }
            return sb.ToString();
        }

        internal static string GetFileSize(string path, bool prefix)
        {
            string ret = null;
            if (!string.IsNullOrEmpty(path))
            {
                if (path.StartsWith(FileProto))
                {
                    path = path.Substring(FileProto.Length);
                }
                try
                {
                    var fi = new FileInfo(path);
                    ret = FormatFileSize(fi.Length, prefix);
                }
                catch
                {
                }
            }
            return ret ?? "N/A";
        }

        static string FormatFileSize(long flen, bool prefix)
        {
            string pref = prefix ? "File size: " : string.Empty;
            bool k = flen < K;
            string unit = k ? "KB" : "MB";
            double size = (double)flen / (k ? K : M);
            return $"{pref}{size:F3} {unit}";
        }

        static string FormatBitrate(long bitrate)
        {
            string ret = "";
            if (bitrate < 1000)
            {
                ret = bitrate.ToString();
            }
            else if (bitrate < 1_000_000)
            {
                double br = bitrate / 1000.0;
                ret = $"{br:F0} K";
            }
            else
            {
                double br = bitrate / 1_000_000.0;
                ret = (br < 10.0) ? $"{br:F1} M" : $"{br:F0} M";
            }
            return ret;
        }

    } // class SourceInfoText

} // namespace FFmpegNetAux


