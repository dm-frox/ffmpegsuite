using System;
using System.IO;

using FFmpegInterop;

namespace Explorer
{
    static class FormatWriter
    {
        internal static void Write(StreamWriter sw, FormatList formats, string txt)
        {
            sw.WriteLine(txt);
            string name = string.Empty;
            for (int i = 0, n = formats.Count; i < n; ++i)
            {
                FormatInfo info = formats[i];
                string nn = info.Name;
                if (nn != name)
                {
                    name = nn;
                    sw.WriteLine("{0}", name);
                }
                sw.WriteLine("  {0, -8} {1}", info.Demuxer ? "Demuxer:" : "Muxer:", info.LongName);
                if (!string.IsNullOrEmpty(info.Extentions))
                {
                    sw.WriteLine("     Exts: {0}", info.Extentions);
                }
                if (!string.IsNullOrEmpty(info.Mime))
                {
                    sw.WriteLine("     Mime: {0}", info.Mime);
                }
            }
        }

    } // class FormatWriter

} // namespace Explorer
