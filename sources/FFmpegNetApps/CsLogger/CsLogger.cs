using System;
using System.Text;
using System.IO;
using System.Threading;
using System.Diagnostics;

using DebugStream = System.Diagnostics.Debug;


namespace CsLogger
{
    internal enum Level
    {
        Self = 0, Error = 1, Warning, Info, Extended, Debug
    }
//----------------------------------------------------------------------
// main class
    public sealed class SLogImpl : IDisposable
    {
        const string    Version         = "4.00";
        const string    IntExcFormat    = "!!! Internal exception !!!, format={0}";
        const string    IntExcExc       = "!!! Internal exception !!!, exception while exception logging";
        const string    FormatAltPath   = @"{0}\{1}";
        const string    DefExtention    = ".log";
        const string    XmlPrmRootName  = "CsLogger";
        const string    XmlPrmNodeName  = "Params";
// config parameters
        bool            m_FlushLine     = false;
        bool            m_DebugOut      = false;
        long            m_Flags         = 0;
        Encoding        m_Encoding      = Encoding.UTF8; 
        BackupMan       m_BackupMan;
//
        int             m_LogInst       = 0;
//
        int             m_OverflowCount = 0;
        string          m_ConfigName    = string.Empty;
//
        StreamWriter        m_Writer        = null;
        LinePrefixFormatter m_LinePrefix    = null;
        bool                m_Ready         = false;

        object m_Sync;

// IDisposable impl
        void IDisposable.Dispose()
        {
            Close();
        }
//
        internal SLogImpl()
        {
            m_Writer       = StreamWriter.Null;
            m_BackupMan    = new BackupMan();
            m_LinePrefix   = new LinePrefixFormatter();
            m_Sync         = new object();
        }

//
        internal bool Ready
        {
            get { return m_Ready; }
        }

        public int BackupCount
        {
            get { return m_BackupMan.BackupCount; }
            set { lock (m_Sync) { m_BackupMan.BackupCount = value; } }
        }

        public int MaxFileSize
        {
            get { return m_BackupMan.MaxFileSize; }
            set { lock (m_Sync) { m_BackupMan.MaxFileSize = value; } }
        }

        public bool BackupSize
        {
            get { return m_BackupMan.BackupSize; }
            set { lock (m_Sync) { m_BackupMan.BackupSize = value; } }
        }

        public bool FlushLine
        {
            get { return m_FlushLine; }
            set { lock (m_Sync) { m_FlushLine = value; } }
        }

        public Encoding Encoding
        {
            get { return m_Encoding; }
            set { lock (m_Sync) { m_Encoding = value; } }
        }

        public long Flags
        {
            get { long ret = 0; lock (m_Sync) { ret = m_Flags; } return ret; }
            set { lock (m_Sync) { m_Flags = value; } }
        }

        public bool ThreadLabel
        {
            get { return m_LinePrefix.ThreadLabel; }
            set { lock (m_Sync) { m_LinePrefix.ThreadLabel = value; } }
        }

        internal bool DebugOut
        {
            get { return m_DebugOut; }
            set { lock (m_Sync) { m_DebugOut = value; } }
        }
//
        public string LogFile
        {
            get { return m_BackupMan.FullPath; }
        }

        public string Folder
        {
            get { return m_BackupMan.Folder; }
        }

        public int LogInst
        {
            get { return m_LogInst; }
        }
// установка параметров из текстового файла
        void LoadConfigParams(string configFile)
        {
            if (!string.IsNullOrEmpty(configFile))
            {
                string cfgPath = GetFilePath(configFile, false);
                if (ConfigFile.GetParams(cfgPath, XmlPrmRootName, XmlPrmNodeName, SetParamFromText))
                {
                    m_ConfigName = Path.GetFileName(cfgPath);
                }
                else
                {
                    m_ConfigName = "<error>";
                }
            }
            else
            {
                m_ConfigName = string.Empty;
            }
        }

        void SetParamFromText(string name, string svalue) // from config fileName
        {
            switch (name)
            {
            case "BackupCount":
            case "MaxFileSize":
            case "BackupSize":
                m_BackupMan.SetParamFromText(name, svalue);
                break;
            case "ConfigName":
                m_ConfigName = svalue;
                break;
            case "FlushLine":
                ConfigFile.ParseBool(svalue, ref m_FlushLine);
                break;
            case "CodePage":
                ConfigFile.ParseCodePage(svalue, ref m_Encoding);
                break;
            case "Flags":
                ConfigFile.ParseFlags(svalue, ref m_Flags);
                break;
            case "ThreadLabel":
                {
                    bool thLab = false;
                    if (ConfigFile.ParseBool(svalue, ref thLab))
                        m_LinePrefix.ThreadLabel = thLab;
                }
                break;
            case "DebugOutput":
                ConfigFile.ParseBool(svalue, ref m_DebugOut);
                break;
            }
        }
// log opening
        static string GetFilePath(string path, bool createDir)
        {
            string ret = Path.IsPathRooted(path) 
                ? Path.GetFullPath(path)
                : Path.GetFullPath(Path.Combine(AppDomain.CurrentDomain.BaseDirectory, path));
            if (Path.GetExtension(ret).Length == 0)
                ret += DefExtention;
            if (createDir)
                Directory.CreateDirectory(Path.GetDirectoryName(ret));
            return ret;
        }

        bool OpenLogFile(string file)
        {
            string fullPath  = GetFilePath(file, true);
            string iniFolder = Path.GetDirectoryName(fullPath);
            string fileName  = Path.GetFileName(fullPath);
            string currFolder = iniFolder;

            for (int k = 2; k < 9; ++k)
            {
                m_LogInst = k - 1;
                try
                {
                    Directory.CreateDirectory(currFolder);
                    m_BackupMan.Init(currFolder, fileName);
                    m_BackupMan.Backup();
                    Begin(); 
                    break;
                }
                catch
                {
                    currFolder = string.Format(FormatAltPath, iniFolder, k);
                }
             }
             return m_Ready;
        }

        void Begin() 
        {
            m_Writer.Close();
            var fs = new FileStream(m_BackupMan.FullPath, FileMode.CreateNew, FileAccess.Write, FileShare.Read);
            m_Writer = new StreamWriter(fs, m_Encoding);
            m_Writer.AutoFlush = false;
            m_Ready = true;
            LogBegin();
        }

        void LogBegin()
        {
            LogSelf("Log file begin: {0}, log version={1}, overflow count={2}, app inst={3}, config file={4}",
                DateTime.Now.ToString(), Version, m_OverflowCount, m_LogInst, m_ConfigName);
            LogSelf("Log params: fileSize={0}, backup={1}, totBckpSize={2}, flushLine={3}, threadLab={4}, flags=0x{5:X}, encoding={6}/{7}",
                m_BackupMan.MaxFileSize, m_BackupMan.BackupCount, ToLog(m_BackupMan.BackupSize),
                ToLog(m_FlushLine), ToLog(m_LinePrefix.ThreadLabel), m_Flags,
                m_Encoding.EncodingName, m_Encoding.CodePage);
        }

        public bool Open(string file, string configFile = null)
        {
            bool ret = false;
            Close();
            lock (m_Sync)
            {
                LoadConfigParams(configFile);
                ret = OpenLogFile(file);
            }
            return ret;
        }
// closing
        public void Close()
        {
            lock (m_Sync)
            {
                End(false);
                m_BackupMan.Clear();
            }
        }

        internal void End(bool checkOverflow)
        {
            int size = (int)m_Writer.BaseStream.Length;
            if (!checkOverflow || size >= m_BackupMan.MaxFileSize)
            {
                LogSelf("Log end: size={0}, overflow={1}", size, checkOverflow);
                if (checkOverflow)
                    ++m_OverflowCount;
                m_Ready = false;
                m_Writer.Close();
                m_Writer = StreamWriter.Null;
                if (checkOverflow)
                {
                    try
                    {
                        m_BackupMan.Backup();
                        Begin(); //1);
                    }
                    catch
                    {
                    }
                }
            }
        }

// writing to log
        void LogSelf(string format, params object[] prms)
        {
            lock (m_Sync)
            {
                m_LinePrefix.Write(m_Writer, Level.Self);
                m_Writer.WriteLine(format, prms);
            }
        }

        internal void SkipLine()
        {
            m_Writer.WriteLine();
        }

        internal void Log(Level lev, string format, object[] prms)
        {
            lock (m_Sync)
            {
                bool noExc = true;
                try
                {
                    m_LinePrefix.Write(m_Writer, lev);
                    m_Writer.WriteLine(format, prms);
                }
                catch
                {
                    noExc = false;
                    m_Writer.WriteLine(IntExcFormat, format);
                }
                DoFlushLine();
                if (m_DebugOut)
                {
                    DebugStream.WriteLine(noExc
                        ? string.Format(format, prms)
                        : string.Format(IntExcFormat, format));
                }
                End(true);
            }
        }

        void Log(Level lev, string msg)
        {
            lock (m_Sync)
            {
                m_LinePrefix.Write(m_Writer, lev);
                m_Writer.WriteLine(msg);
            }
        }

        void WriteLineData(string s1, string s2)
        {
            m_Writer.Write(s1);
            if (s2 != null)
                m_Writer.Write(s2);
            m_Writer.Write(m_Writer.NewLine);
            if (m_DebugOut)
            {
                DebugStream.Write(s1);
                if (s2 != null)
                    DebugStream.Write(s2);
                DebugStream.Write(m_Writer.NewLine);
            }
        }

        public void E(string format, params object[] prms)
        {
            if (m_Ready)
                Log(Level.Error, format, prms);
        }

        public void E(string msg)
        {
            if (m_Ready)
                Log(Level.Error, msg);
        }

        public void W(string format, params object[] prms)
        {
            if (m_Ready)
                Log(Level.Warning, format, prms);
        }

        public void W(string msg)
        {
            if (m_Ready)
                Log(Level.Warning, msg);
        }

        public void I(string format, params object[] prms)
        {
            if (m_Ready)
                Log(Level.Info, format, prms);
        }

        public void I(string msg)
        {
            if (m_Ready)
                Log(Level.Info, msg);
        }
 
        public void X(string format, params object[] prms)
        {
            if (m_Ready)
                Log(Level.Extended, format, prms);
        }

        public void X(string msg)
        {
            if (m_Ready)
                Log(Level.Extended, msg);
        }

        public bool CanLogX(long flags)
        {
            return (flags & m_Flags) != 0;
        }

        [Conditional("DMLOG_DEBUG")]
        public void D(string format, params object[] prms)
        {
             if (m_Ready)
                Log(Level.Debug, format, prms);
        }

        [Conditional("DMLOG_DEBUG")]
        public void D(string msg)
        {
            if (m_Ready)
                Log(Level.Debug, msg);
        }

        public void Exc(string msg, Exception exc)
        {
            if (m_Ready)
            {
                lock (m_Sync)
                {
                    try
                    {
                        m_LinePrefix.Write(m_Writer, Level.Error);
                        WriteLineData(string.IsNullOrEmpty(msg) ? "Exception:" : msg, null);
                        WriteLineData(" -- Type      =", exc.GetType().Name);
                        WriteLineData(" -- Message   =", exc.Message);
                        WriteLineData(" -- Source    =", exc.Source);
                        WriteLineData(" -- TargetSite=", exc.TargetSite.ToString());
                        WriteLineData(" -- StackTrace:", null);
                        WriteLineData(exc.StackTrace, null);
                    }
                    catch
                    {
                        m_LinePrefix.Write(m_Writer, Level.Error);
                        WriteLineData(IntExcExc, null);
                    }
                    m_Writer.Flush();
                }
            }
        }
// flush line
        internal void DoFlushLine()
        {
            if (m_Ready && m_FlushLine)
                m_Writer.Flush();
        }

        public void Flush()
        {
            if (m_Ready && !m_FlushLine)
            {
                lock (m_Sync)
                {
                    m_Writer.Flush();
                }
            }
        }
// temp fileName
        string GetTempFileName(string fprefix)
        {
            DateTime dt = DateTime.Now;
            string fileName = string.Format("{0}{1:D2}.{2:D2}-{3:D2}.{4:D2}.{5:D2}.{6:D3}",
                    fprefix ?? string.Empty,
                    dt.Day, dt.Month, dt.Hour, dt.Minute, dt.Second, dt.Millisecond);
            return Path.Combine(m_BackupMan.Folder, fileName);
        }

        public FileStream GetTempFile(string fprefix)
        {
            FileStream ret = null;
            if (m_Ready)
            {
                lock (m_Sync)
                {
                    var file_ = new object[1];
                    for (int i = 0; i < 2; ++i)
                    {
                        try
                        {
                            if (i == 1) // second attempt
                            {
                                Thread.Sleep(40);
                            }
                            string file = GetTempFileName(fprefix);
                            ret = new FileStream(file, FileMode.CreateNew, FileAccess.Write, FileShare.None);
                            W("Temp file={0}", file);
                            break;
                        }
                        catch
                        {
                            E("Cannot create temp file, prefix={0}", fprefix ?? string.Empty);
                        }
                    }
                }
            }
            return ret;
        }
// misc
        static internal char ToLog(bool v)
        {
            return v ? 'T' : 'F';
        }

        public TextWriter TextWriter
        {
            get { return m_Writer; }
        }


    } // class SLogImpl

} // namespace CsLogger

