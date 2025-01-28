using System;
using System.Text;
using System.IO;

using FFmpegNetAux;
using FFmpegInterop;

namespace Explorer
{
    static class Tests
    {
        internal static string GetMultistring(string[] items)
        {
            var sb2 = new StringBuilder(64);
            for (int j = 0; j < items.Length; j += 2)
            {
                sb2.Append(items[j]);
                sb2.Append('\n');
                sb2.Append(items[j + 1]);
                sb2.Append('\0');

            }
            sb2.Append('\0');
            return sb2.ToString();
        }

        internal static void TestMultistring()
        {
            string[] t1 = { "pixel_format", "bgra", "video_size", "320x240", "framerate", "10" };
            string[] t2 = { "Nickname", "Crazy Rabbit", "Color", "Lightgreen" };

            using (var probe = new Probe())
            {
                probe.TestMultiString(GetMultistring(t1));
                probe.TestMultiString(GetMultistring(t2));
            }
        }

        internal static void TestProbe()
        {
            using (var probe = new Probe())
            {
                probe.Do(125, "Godzilla");
                int[] arr = new int[] { 5, 2, 8, 1, 4, 3, 6, 7 };
                int[] arrs = new int[arr.Length];//probe.SortInt(arr);
                probe.SortInt2(arr, arrs);
                LogNet.PrintInfo("Probe, SortInt:");
                for (int i = 0; i < arrs.Length; ++i)
                    LogNet.PrintInfo(" -- {0}  {1}", arr[i], arrs[i]);
                int rr = probe.LogDevList();
                LogNet.PrintInfo("Probe, LogDevList: {0}", rr);
            }
        }

        internal static void TestSubtit()
        {
            using (var probe = new Probe())
            {
                //probe.TestSubtit(@"E:\A-data\Media\1\Imposters.S01E01.720p.HDTV.x264-FLEET_eztv__1486564451.mkv", "0", 10);
                //probe.TestSubtit(@"E:\A-data\Media\1\Sintel_DivXPlus_6500kbps.mkv", "eng", 8);
                probe.TestSubtit(@"E:\A-data\Media\1\SubTit\Teremok.1937.WEBRip.(6.37).25 fps_utf8.srt", "", 20);
            }
        }

        internal static void TestProbe2()
        {
            using (Probe probe = new Probe())
            {
                probe.Do(125, "Godzilla");
            }
        }

        internal static void PrintDecoders(CodecNode node)
        {
            for (int i = 0, n = node.DecoderCount; i < n; ++i)
            {
                LogNet.PrintInfo(" dec  {0}", node.Decoder(i));
            }
        }

        internal static void PrintEncoders(CodecNode node)
        {
            for (int i = 0, n = node.EncoderCount; i < n; ++i)
            {
                LogNet.PrintInfo(" enc  {0}", node.Encoder(i));
            }
        }

        internal static void TestCodecList()
        {
            using (var codecs = new CodecList())
            {
                LogNet.PrintInfo("TestCodecList: codec count={0}, node count={1}", codecs.CodecCount, codecs.Count);
                for (int i = 0, n = codecs.Count; i < n; ++i)
                {
                    CodecNode node = codecs[i];
                    LogNet.PrintInfo(" --  {0,3} {1,18} {2,8} {3}/{4}   {5}",
                        i, node, node.MediaTypeStr, node.DecoderCount, node.EncoderCount, node.LongName);

                    //PrintDecoders(node);
                    //PrintEncoders(node);
                }

            }
        }

        internal static void TestHdrReader(string path)
        {
            //if (!string.IsNullOrEmpty(path))
            //{
            //    MediaSourceRep.Write(new MediaSourceHdr(path, null, null), @"E:\AllLogs\ffmpeg\Lite15\Explorer\HdrReader.txt");
            //}
        }

        internal static void TestProtocols()
        {
            using (var plst = new ProtocolList())
            {
                int n = plst.Count;
                LogNet.PrintInfo("TestProtocols, count={0}", n);
                for (int i = 0; i < n; ++i)
                {
                    ProtocolInfo pi = plst[i];
                    LogNet.PrintInfo(" -- {0}  {1}/{2}", pi.Name, pi.IsInput, pi.IsOutput);
                }
            }
        }

        internal static void TestDataMuxer()
        {
            using (var dmx = new AudioArrMuxer())
            {
                try
                {
                    dmx.Build(
                        @"E:\A-data\Media\1\trans6\Wildlife-video.mkv",
                        @"E:\A-data\Media\1\trans6\Raw Pcm16(sin440)-mono-11025-30s.sw",
                        @"E:\A-data\Media\1\trans6\DataMux.mkv");
                }
                catch
                {
                    LogNet.PrintError("DataMuxer.Build()");
                }
            }
        }

        internal static void TestChannList()
        {
            using (var chl = new ChannList())
            {
                using (var f = File.CreateText(@"E:\A-data\Media\1\work\chann_list.txt"))
                {
                    int n = chl.Count;
                    //LogNet.PrintInfo("TestChannList, count={0}", n);
                    f.WriteLine("TestChannList, count={0}", n);
                    for (int i = 0; i < n; ++i)
                    {
                        ChannInfo pi = chl[i];
                        //LogNet.PrintInfo(" -- {0,2}  {1,10} {2,3} {3}", pi.Chann, pi.Descr, pi.Mask, pi.ChannsDescr);
                        f.WriteLine(" -- {0,2}  {1,15} {2,14:X} {3}", pi.Chann, pi.Name, pi.Mask, pi.Descr);
                    }
                }

            }

        }

        internal static bool BuildAudioArrMuxer(AudioArrMuxer muxer)
        {
            bool ret = false;
            try
            {
                muxer.Build(
                    @"E:\A-data\Media\1\trans6\Wildlife-video.mkv",
                    @"E:\A-data\Media\1\trans6\Calleso_30s.mp3",
                    @"E:\A-data\Media\1\trans6\DataMux2.mkv");
                ret = true;
            }
            catch
            {
                LogNet.PrintError("DataMuxer.Build()");
            }
            return ret;
        }

    } // static class Tests

} // namespace Explorer

// Wildlife-video.mkv
// @"E:\A-data\Media\1\bin\Raw h264(micarr)640x480-25fps-300s.h264",
// @"E:\A-data\Media\1\bin\stolbov_300s.wav",
// @"E:\A-data\Media\1\bin\DataMux.mkv");
