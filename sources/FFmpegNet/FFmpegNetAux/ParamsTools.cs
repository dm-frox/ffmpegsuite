// ---------------------------------------------------------------------
// File: ParamsTools.cs
// Classes: ParamsTools (static)
// Purpose: helpers to create DemuxerParams instances
// Module: FFmpegNetCore - .NET assembly to provide more comfortable interface to FFmpegInterop.dll
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

using System;
using System.Text;
using System.IO;

namespace FFmpegNetAux
{
    public static class ParamsTools
    {
        const string ConcatListItemTempl = "file '{0}'";
        const string ConcatCommentTempl  = "# FFmpegTools: {0}";

        const string MediaTypeVideo = "video";
        const string MediaTypeAudio = "audio";

        public const string DShowFormat    = "dshow";
        public const string ConcatFormat   = "concat";
        public const string RawVideoFormat = "rawvideo";

        const int MaxMetadataLen = 128;

// url reader params creation

        const string DevVideo = "video=";
        const string DevAudio = "audio=";
        const char   DevSepr  = ':';

        public static string ComposeDShowUrl(string devVideo, string devAudio)
        {
            var sb = new StringBuilder(64);
            if (!string.IsNullOrEmpty(devVideo))
            {
                sb.Append(DevVideo);
                sb.Append(devVideo);
            }
            if (!string.IsNullOrEmpty(devAudio))
            {
                if (sb.Length > 0)
                {
                    sb.Append(DevSepr);
                }
                sb.Append(DevAudio);
                sb.Append(devAudio);
            }
            return sb.ToString();
        }

        public static DemuxerParams[] CreateParams(string[] urls)
        {
            var prms = new DemuxerParams[(urls != null) ? urls.Length : 0];
            for (int i = 0; i < prms.Length; ++i)
            {
                prms[i] = new DemuxerParams(urls[i]);
            }
            return prms;
        }

        static string GetDevUrl(string mediaType, string devName) => $"{mediaType}={devName}";

        static string GetDevUrlVideoAudio(string devNameVideo, string devNameAudio) => 
            $"{MediaTypeVideo}={devNameVideo}:{MediaTypeAudio}={devNameAudio}";

        static void SetVideoOptions(OptionList opts, int width, int height, int framerate)
        {
            if (width > 0 && height > 0)
                opts.AddOption("video_size", string.Format("{0}x{1}", width, height));
            if (framerate > 0)
                opts.AddOption("framerate", framerate.ToString());
        }

        public static DemuxerParams CreateDevVideoParams(string devName, int width = 0, int height = 0, int framerate = 0)
        {
            var prms = new DemuxerParams(GetDevUrl(MediaTypeVideo, devName), true, false);
            prms.Format = DShowFormat;
            SetVideoOptions(prms, width, height, framerate);
            return prms;
        }

        public static DemuxerParams CreateDevParams(string devNameVideo, string devNameAudio)
        {
            var prms = new DemuxerParams(GetDevUrlVideoAudio(devNameVideo, devNameAudio), true, true);
            prms.Format = DShowFormat;
            return prms;
        }
        
        static void SetAudioOptions(OptionList opts, int chann, int sampleRate)
        {
            if (chann > 0)
                opts.AddOption("channels", chann.ToString());
            if (sampleRate > 0)
                opts.AddOption("sample_rate", sampleRate.ToString());
        }

        public static DemuxerParams CreateRawPcmParams(string url, RawPcmFormat rawPcmEnum, int chann, int sampleRate)
        {
            var prms = new DemuxerParams(url, false, true);
            prms.Format = rawPcmEnum.ToString();
            SetAudioOptions(prms, chann, sampleRate);
            return prms;
        }

        public static DemuxerParams CreateRawPcmParams(string url, int chann, int sampleRate) => 
            CreateRawPcmParams(url, RawPcmFormat.s16le, chann, sampleRate);

// concat support

        static string GenFileListString(string[] list, string comment)
        {
            var sb = new StringBuilder(list.Length * 64);
            sb.AppendFormat(ConcatCommentTempl, comment);
            sb.AppendLine();
            foreach (var path in list)
            {
                if (!string.IsNullOrEmpty(path))
                {
                    sb.AppendFormat(ConcatListItemTempl, path);
                    sb.AppendLine();
                }
            }
            return sb.ToString();
        }

        static void WriteFileList(string[] list, string path, string comment)
        {
            var flist = GenFileListString(list, comment);
            if (!string.IsNullOrEmpty(flist))
            {
                var chars = Encoding.UTF8.GetBytes(flist);
                using (var fs = new FileStream(path, FileMode.Create, FileAccess.Write, FileShare.None))
                {
                    fs.Write(chars, 0, chars.Length);
                }
            }
        }

        public static DemuxerParams CreateConcatParams(string[] list, string listPath, string comment)
        {
            WriteFileList(list, listPath, !string.IsNullOrEmpty(comment) ? comment : string.Empty);
            return new DemuxerParams(listPath, true, true) { Format = ConcatFormat };
        }

// misc

        public static string FormatTimeSec(int timeSec, bool noHour)
        {
            int h = timeSec / 3600;
            int m = (timeSec / 60) % 60;
            int s = timeSec % 60;
            return (noHour && h == 0)
                ? $"{m}:{s:D2}"
                : $"{h}:{m:D2}:{s:D2}";
        }

        public static string FormatPosition(double pos, bool noHour) => FormatTimeSec((int)Math.Round(pos), noHour);

        public static string TruncateMetadata(string meta) =>
            (meta.Length <= MaxMetadataLen) ? meta : meta.Substring(0, MaxMetadataLen) + "(...)";

    } // class ParamsTools

    public enum RawPcmFormat
    {
        alaw,
        mulaw,

        f32be,
        f32le,
        f64be,
        f64le,

        s16be,
        s16le,
        s24be,
        s24le,
        s32be,
        s32le,
        s8,

        u16be,
        u16le,
        u24be,
        u24le,
        u32be,
        u32le,
        u8

    } // enum RawPcmFormat

} // namespace FFmpegNetAux

