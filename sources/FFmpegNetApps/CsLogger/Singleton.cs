using System;
using System.IO;
using System.Text;
using System.Diagnostics;
using System.Threading;

namespace CsLogger
{
    public static class SLog
    {
        internal static SLogImpl TheLogger;

        static SLog() // static ctor
        {
            TheLogger = new SLogImpl();
        }

        public static SLogImpl GetInstance()
        {
            return new SLogImpl();
        }

        public static bool Ready
        {
            get { return TheLogger.Ready; }
        }
// config params
        public static int BackupCount
        {
            get { return TheLogger.BackupCount; }
            set { TheLogger.BackupCount = value; }
        }

        public static int MaxFileSize
        {
            get { return TheLogger.MaxFileSize; }
            set { TheLogger.MaxFileSize = value; }
        }

        public static bool BackupSize
        {
            get { return TheLogger.BackupSize; }
            set { TheLogger.BackupSize = value; }
        }

        public static bool FlushLine
        {
            get { return TheLogger.FlushLine; }
            set { TheLogger.FlushLine = value; }
        }

        public static Encoding Encoding
        {
            get { return TheLogger.Encoding; }
            set { TheLogger.Encoding = value; }
        }

        public static int CodePage
        {
            set { TheLogger.Encoding = Encoding.GetEncoding(value); }
        }

        public static long Flags
        {
            get { return TheLogger.Flags; }
            set { TheLogger.Flags = value; }
        }

        public static bool ThreadLabel
        {
            get { return TheLogger.ThreadLabel; }
            set { TheLogger.ThreadLabel = value; }
        }

// initialization, closing
        public static bool Open(string file, string configFile = null)
        {
            bool ret = false;
            if (TheLogger.Open(file, configFile))
            {
                LogEnvInfo();
                ret = true;
            }
            return ret;
        }

        public static void Close()
        {
            TheLogger.Close();
        }

        // logging

        public static void SkipLine()
        {
            TheLogger.SkipLine();
        }

        public static void E(string format, params object[] prms)
        {
            Level lev = Level.Error;
            TheLogger.Log(lev, format, prms);
        }

        public static void E(string msg)
        {
            TheLogger.E(msg);
        }

        public static void W(string format, params object[] prms)
        {
            Level lev = Level.Warning;
            TheLogger.Log(lev, format, prms);
        }

        public static void W(string msg)
        {
            TheLogger.W(msg);
        }

        public static void I(string format, params object[] prms)
        {
            Level lev = Level.Info;
            TheLogger.Log(lev, format, prms);
        }

        public static void I(string msg)
        {
            TheLogger.I(msg);
        }

        public static void X(string format, params object[] prms)
        {
            Level lev = Level.Extended;
            TheLogger.Log(lev, format, prms);
        }

        public static void X(string msg)
        {
            TheLogger.X(msg);
        }

        public static bool CanLogX(long flags)
        {
            return TheLogger.CanLogX(flags);
        }

        [Conditional("DMLOG_DEBUG")]
        public static void D(string format, params object[] prms)
        {
            Level lev = Level.Debug;
            TheLogger.Log(lev, format, prms);
        }

        [Conditional("DMLOG_DEBUG")]
        public static void D(string msg)
        {
            TheLogger.D(msg);
        }

        public static void Exc(string msg, Exception exc)
        {
            TheLogger.Exc(msg, exc);
        }

        public static void Exc(Exception exc)
        {
            TheLogger.Exc(null, exc);
        }

        static void LogEnvInfo()
        {
            TheLogger.I("OS ver={0}, .NET ver={1}, Proc count={2}, Comp/User={3}/{4}",
                Environment.OSVersion.ToString(),
                Environment.Version.ToString(),
                Environment.ProcessorCount,
                Environment.MachineName,
                Environment.UserName);
        }

        public static void LogLangInfo(object langId) // langId - app defined language id, may be null
        {
            TheLogger.I("LangId={0}, CurrentCulture={1}, CurrentUICulture={2}",
               (langId != null) ? langId.ToString() : "<null>",
               Thread.CurrentThread.CurrentCulture.EnglishName,
               Thread.CurrentThread.CurrentUICulture.EnglishName);
        }

// additional services
        public static void Flush()
        {
            TheLogger.Flush();
        }

        public static string LogFolder
        {
            get { return TheLogger.Folder; }
        }

        public static int LogInst
        {
            get { return TheLogger.LogInst; }
        }

        public static TextWriter TextWriter
        {
            get { return TheLogger.TextWriter; }
        }

// temp fileName
        public static FileStream GetTempFile(string fprefix)
        {
            return TheLogger.GetTempFile(fprefix);
        }

        static string FormatTimeMs(int time) // in milliseconds
        {
            int sec = (time + 500) / 1000,
                min = sec / 60,
                hour = min / 60,
                days = hour / 24;
            return (days == 0)
                ? string.Format("{0}:{1:D2}:{2:D2}", hour, min % 60, sec % 60)
                : string.Format("{0} days {1}:{2:D2}:{3:D2}", days, hour % 24, min % 60, sec % 60);
        }

    } // static class SLog

} // namespace CsLogger


