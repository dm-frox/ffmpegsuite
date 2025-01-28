using System;
using System.Xml;
using System.Text;

namespace AppTools
{
    public class PrmEncBase : PrmBase
    {
        public readonly string   Encoder;
        public readonly long     Bitrate;
        public readonly string[] Options;
        public readonly string   FilterStr;


        public bool HasOptions => Options != null && Options.Length > 0;

        protected PrmEncBase(string displayName, string encoder, long bitrate, string[] options, string filter)
            : base(displayName)
        {
            Encoder = encoder;
            Bitrate = bitrate;
            Options = options;
            FilterStr = filter;
        }

        static long ParseBitrate(string bitrateStr)
        {
            if (bitrateStr == null)
            {
                return 0;
            }
            long ret = 0;
            string bs = null;
            long factor = 1;
            int len = bitrateStr.Length;
            char sfx = (len > 0) ? bitrateStr[len - 1] : '0';
            if (!char.IsDigit(sfx))
            {
                bs = bitrateStr.Substring(0, len - 1);
                switch (char.ToUpper(sfx))
                {
                case 'K':
                    factor = 1000;
                    break;
                case 'M':
                    factor = 1_000_000;
                    break;
                case 'G':
                    factor = 1_000_000_000;
                    break;
                }
            }

            if (long.TryParse(bs ?? bitrateStr, out long val))
            {
                ret = val * factor;
            }

            return ret;
        }

        protected static long GetBitrate(XmlAttributeCollection attrs)
        {
            return ParseBitrate(GetAttrStr(attrs, "Bitrate"));
        }

    } // class PrmEncBase

    class AttrBase
    {
        public readonly string DisplayName;
        public readonly string Encoder;
        public readonly bool Copying;

        static bool CheckCopying(string enc)
        {
            bool ret = false;
            if (enc.Length == 1)
            {
                char x = enc[0];
                if (x == 'c' || x == 'C' ||
                    x == 'с' || x == 'С') // russian !!
                {
                    ret = true;
                }
            }
            return ret;
        }

        public AttrBase(XmlAttributeCollection attr)
        {
            DisplayName = PrmBase.GetDisplayName(attr);
            Encoder = PrmBase.GetAttrStr(attr, "Name");
            Copying = CheckCopying(Encoder);
        }

    } // AttrBase

    public class PrmEncVideo : PrmEncBase
    {
        public readonly string Preset;
        public readonly int Crf;

        public readonly int Width;
        public readonly int Height;
        public readonly string PixelFormat;
        public readonly int    Fps;


        public PrmEncVideo(string displayName, string encoder,
            string pixelFormat,
            long bitrate, string preset, int crf, string[] options, 
            int fps, string filter, int width, int height)
            : base(displayName, encoder, bitrate, options, filter)
        {
            PixelFormat = pixelFormat;
            Preset = preset;
            Crf = crf;
            Width = width;
            Height = height;
            Fps = fps;
        }

        public PrmEncVideo(string displayName) // Packet copying
            : this(displayName, string.Empty, string.Empty, 0, null, -1, null, 0, null, 0, 0)
        {}

        public static PrmEncVideo[] Read(string path)
        {
            var nodes = GetNodes(path);
            int n = Count(nodes);
            var items = new PrmEncVideo[n];
            for (int i = 0; i < n; ++i)
            {
                var node  = nodes[i];
                var attrs = node.Attributes;
                var ba = new AttrBase(attrs);
                items[i] = ba.Copying
                    ? new PrmEncVideo(ba.DisplayName) // Packet copying
                    : new PrmEncVideo(
                        ba.DisplayName,
                        ba.Encoder,
                        GetAttrStr(attrs, "PixelFormat"),
                        GetBitrate(attrs),
                        GetAttrStr(attrs, "Preset"),
                        GetAttrInt(attrs, "Crf", -1),
                        GetKeyValList(node),
                        GetAttrInt(attrs, "Fps"),
                        GetAttrStr(attrs, "Filter"),
                        GetAttrInt(attrs, "Width"),
                        GetAttrInt(attrs, "Height")
                        );
            }
            return items;
        }

    } // class PrmEncVideo

    public class PrmEncAudio : PrmEncBase
    {
        public readonly int    Chann;
        public readonly int    SampleRate;
        public readonly string SampleFormat;
        public readonly string ChannLayout;

        public PrmEncAudio(string displayName, string encoder,
            string sampleFormat,
            long bitrate, string[] options, string filter, int chann, int sampleRate, string channLayout)
            : base(displayName, encoder, bitrate, options, filter)
        {
            Chann = chann;
            SampleRate = sampleRate;
            SampleFormat = sampleFormat;
            ChannLayout = channLayout;
        }

        public PrmEncAudio(string displayName) // Packet copying
            : this(displayName, string.Empty, string.Empty, 0, null, null, 0, 0, null)
        { }

        public static PrmEncAudio[] Read(string path)
        {
            var nodes = GetNodes(path);
            int n = Count(nodes);
            var items = new PrmEncAudio[n];
            for (int i = 0; i < n; ++i)
            {
                var node  = nodes[i];
                var attrs = node.Attributes;
                var ba = new AttrBase(attrs);
                items[i] = ba.Copying
                    ? new PrmEncAudio(ba.DisplayName) // Packet copying
                    : new PrmEncAudio(
                        ba.DisplayName,
                        ba.Encoder,
                        GetAttrStr(attrs, "SampleFormat"),
                        GetBitrate(attrs),
                        GetKeyValList(node),
                        GetAttrStr(attrs, "Filter"),
                        GetAttrInt(attrs, "Channels"),
                        GetAttrInt(attrs, "SampleRate"),
                        GetAttrStr(attrs, "ChannLayout")
                        );
            }
            return items;
        }

    } // class PrmEncAudio

    public class PrmMetadata : PrmBase
    {
        public readonly string Metadata;

        public PrmMetadata(string displayName, string metadata)
            : base(displayName)
        {
            Metadata = metadata;
        }

        static string GetMetadataString(string[] item, char kvDelim, char pairDelim, bool multiStr)
        {
            var sb = new StringBuilder(64);
            for (int j = 0; j < item.Length; j += 2)
            {
                sb.Append(item[j]);
                sb.Append(kvDelim);
                sb.Append(item[j + 1]);
                sb.Append(pairDelim);
            }
            if (multiStr)
            {
                sb.Append(pairDelim);
            }
            return sb.ToString();
        }

        public static PrmMetadata[] Read(string path)
        {
            var nodes = GetNodes(path);
            int n = Count(nodes);
            var items = new PrmMetadata[n];
            for (int i = 0; i < n; ++i)
            {
                string[] item = GetKeyValList(nodes[i]);
                items[i] = new PrmMetadata(
                    GetMetadataString(item, '=', ';', false),
                    GetMetadataString(item, '\n', '\0', true)
                    );
            }
            return items;
        }

    } // class PrmMetadata

} // namespace AppTools
