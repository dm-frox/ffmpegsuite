using System;
using System.IO;

using FFmpegInterop;

namespace Explorer
{
    static class CodecWriter
    {
        static void WriteCodec(StreamWriter sw, CodecInfo ci)
        {
            sw.WriteLine("    {0, -12} {1} {2}", ci.Name, ci.LongName, ci.Experimental ? "[Experimental]" : "");
            if (!string.IsNullOrEmpty(ci.Formats))
            {
                sw.WriteLine("      Fmts: {0}", ci.Formats);
            }
            if (!string.IsNullOrEmpty(ci.HWAccels))
            {
                sw.WriteLine("      Accs: {0}", ci.HWAccels);
            }
        }

        static string Flags(CodecNode node)
        {
            return string.Format("{0}{1}{2}",
                node.Lossless ? "S" : "",
                node.Lossy ? "L" : "",
                node.IntraOnly ? "I" : "");
        }

        static void Write(StreamWriter sw, CodecList codecs, MediaType mediaType)
        {

            sw.WriteLine("{0} codecs --------------------------", mediaType.ToString());
            for (int i = 0, n = codecs.Count; i < n; ++i)
            {
                CodecNode node = codecs[i];

                if (node.MediaType == (int)mediaType)
                {

                    sw.WriteLine("{0, -12} Flags: {1}, Descr: {2}", node.Name, Flags(node), node.LongName);

                    for (int j = 0, m = node.DecoderCount; j < m; ++j)
                    {
                        if (j == 0)
                        {
                            sw.WriteLine("  Decoders:");
                        }
                        WriteCodec(sw, node.Decoder(j));
                    }
                    for (int j = 0, m = node.EncoderCount; j < m; ++j)
                    {
                        if (j == 0)
                        {
                            sw.WriteLine("  Encoders:");
                        }
                        WriteCodec(sw, node.Encoder(j));
                    }
                }
            }
        }

        internal static void Write(StreamWriter sw, CodecList codecs)
        {
            sw.WriteLine("Codecs, flags: L - lossy, S - lossless, I - intra frame only");
            Write(sw, codecs, MediaType.Video);
            Write(sw, codecs, MediaType.Audio);
            Write(sw, codecs, MediaType.Subtitle);
            Write(sw, codecs, MediaType.Data);
        }

    } // class CodecWriter

} // namespace Explorer
