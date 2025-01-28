using System;
using System.IO;
using System.Globalization;

using Timer = System.Windows.Threading.DispatcherTimer;


namespace Player
{
    class PlayerWatcher
    {
        const int DefIntervalMs = 200;

        Timer m_Timer;

        internal PlayerWatcher(Action checkState, int intervalMs = 0)
        {
            m_Timer = new Timer() { Interval = TimeSpan.FromMilliseconds(intervalMs > 0 ? intervalMs : DefIntervalMs) };
            m_Timer.Tick += delegate { checkState(); };
        }

        internal void Start()
        {
            m_Timer.IsEnabled = true;
        }

        internal void Stop()
        {
            m_Timer.IsEnabled = false;
        }

    } // class PlayerWatcher

    static class FileHlp
    {
        internal static string NormPath(string path) => (new FileInfo(path)).FullName;

        internal static bool CheckExtention(string path, string[] exts)
        {
            bool ret = false;
            int dotPos = path.LastIndexOf('.');
            if (dotPos > 0)
            {
                string ext = path.Substring(dotPos + 1);
                if (!string.IsNullOrEmpty(ext))
                {
                    foreach (var e in exts)
                    {
                        if (string.Compare(e, ext, true, CultureInfo.InvariantCulture) == 0)
                        {
                            ret = true;
                            break;
                        }
                    }
                }
            }
            return ret;
        }
    }

    class DrawTextParams
    {
        internal string Text { get; set; }
        internal int Fontsize { get; set; }
        internal int Fontcolor { get; set; }
        internal int X { get; set; }
        internal int Y { get; set; }

        internal DrawTextParams()
        {
            Reset();
        }

        internal void Reset()
        {
            Text = "";
            Fontsize = 0;
            Fontcolor = 0;
            X = 0;
            Y = 0;
        }

    } // class DrawTextParams

    class EqParams
    {
        internal double Brightness { get; set; }

        internal double Contrast { get; set; }

        internal double Saturation { get; set; }

        internal EqParams()
        {
            Reset();
        }

        internal void Reset()
        {
            Brightness = 0.0;
            Contrast   = 1.0;
            Saturation = 1.0;
        }

    } //  class EqParams

} // namespace Player