using System;
using System.IO;
using System.Text;

using FFmpegInterop;

namespace AppTools
{
    public class StartParams
    {
        const bool IsDebug =
#if DEBUG
        true;
#else
        false;
#endif
        const string DevKey = "/dev";

        // <app_root> from the folder with  *.exe
        const string RelRootDev = @"..\..\..\..\..\.."; // <app_root>\sources\FFmpegNetApps\<app_name>\bin\<platform_tag>\<config_dir>
        const string RelRoot    = @"..\..";             // <app_root>\bin\<platform_tag>

        const string CompanyName     = "dm_frox";
        const string ProductName     = "FFmpegSuite";
        const string LogSubFolder    = "_LOGS";
        const string DataSubFolder   = "data";

        const string ParamsSubFolder = "params";
        const string DocSubFolder    = "doc";
        const string HelpSubFolder   = "help";

        const string UnhExcFileName  = "_UnhExc.log";

        const string Platformf64     = "64";
        const string X64BinSubFolder = @"x64\";

        public static readonly string PlatformTag = Core.PlatformTag();

        static readonly bool Is64 = PlatformTag.IndexOf(Platformf64) >= 0;

        public static readonly string PlatformConfigTag  = PlatformTag + (IsDebug ? " (Debug)" : string.Empty);

// FFmpegWrapper.dll location relatively <app_root>

        static readonly string ConfigDir = IsDebug ? @"Debug" : @"Release";

        static readonly string PlatformSubDir   = Is64 ? X64BinSubFolder : string.Empty;

        static readonly string WrapSubFolderDev = @"sources\" + PlatformSubDir + ConfigDir;
        // relatively <app_root>
        static readonly string WrapSubFolder    = @"bin\" + PlatformTag;

// end FFmpegWrapper.dll

        static readonly string BcsEffectRelPathDev = @"sources\FFmpegNetApps\Player\BCS.ps";
        static readonly string BcsEffectRelPath    = WrapSubFolder + @"\BCS.ps";

        // relatively <app_root>
        static readonly string FFmpegSubFolder     = @"ffmpeg\bin\" + PlatformTag;

        bool   m_Dev;
        string m_AppRoot;
        string m_FFmpegFolder;
        string m_FFmpegWrapFolder;
        string m_ParamsFolder;
        string m_DocFolder;
        string m_LogFolder;
        string m_DataFolder;

        public StartParams(string appSubfolder, string[] cmdLineArgs)
        {
            SetupAppDataFolders(appSubfolder);
            SetupAppRoot(cmdLineArgs);
            m_ParamsFolder = Path.Combine(m_AppRoot, ParamsSubFolder, appSubfolder);
            m_DocFolder = Path.Combine(m_AppRoot, DocSubFolder);
        }

        void SetupAppDataFolders(string appSubfolder)
        {
            var sf = Environment.SpecialFolder.LocalApplicationData;
            string appDataRoot = CreateDir(Environment.GetFolderPath(sf), CompanyName, ProductName, appSubfolder);
            m_LogFolder = CreateDir(appDataRoot, LogSubFolder);
            m_DataFolder = CreateDir(appDataRoot, DataSubFolder);
        }

        static string CreateDir(params string[] folders)
        {
            var path = Path.Combine(folders);
            DirectoryInfo di = Directory.CreateDirectory(path);
            return di.FullName;
        }

        void SetupAppRoot(string[] cmdLineArgs)
        {
            foreach (var s in cmdLineArgs)
            {
                if (s == DevKey)
                {
                    m_Dev = true; // started from IDE
                    break;
                }
            }

            string rt = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, m_Dev ? RelRootDev : RelRoot);
            m_AppRoot = Path.GetFullPath(rt);

            m_FFmpegFolder = Path.Combine(m_AppRoot, FFmpegSubFolder);
            m_FFmpegWrapFolder = Path.Combine(m_AppRoot, m_Dev ? WrapSubFolderDev : WrapSubFolder);
        }

        public static string GetAppTitle(string appName, bool firstInst)
        {
            var tit = appName;
            if (!Is64)
            {
                tit += (" - ") + PlatformTag;
            }
            if (IsDebug) 
            {
                tit += " (Debug)";
            }
            if (!firstInst)
            {
                tit += " (#)";
            }
            return tit;
        }

        public bool Dev => m_Dev;

        public string AppRoot => m_AppRoot;

        public string FFmpegFolder => m_FFmpegFolder;

        public string FFmpegWrapFolder => m_FFmpegWrapFolder;

        public string BcsEffectFilePath => Path.Combine(m_AppRoot, m_Dev ? BcsEffectRelPathDev : BcsEffectRelPath);

        public string GetParamsFilePath(string file) => Path.Combine(m_ParamsFolder, file);

        public string GetHelpFilePath(string file) => Path.Combine(m_DocFolder, HelpSubFolder, file);

        public string GetLogFilePath(string fileName) => Path.Combine(m_LogFolder, fileName);

        public string GetDataFilePath(string fileName) => Path.Combine(m_DataFolder, fileName);

        public void LogUnhandledException(Exception exc) => File.WriteAllText(GetLogFilePath(UnhExcFileName), FormatException(exc));

        static string FormatException(Exception exc)
        {
            StringBuilder sb = new StringBuilder(2 * 1024);
            sb.AppendLine(exc.Message);
            sb.AppendFormat(" -- Type      ={0}", exc.GetType().Name);
            sb.AppendLine();
            sb.AppendFormat(" -- Source    ={0}", exc.Source);
            sb.AppendLine();
            sb.AppendFormat(" -- TargetSite={0}", exc.TargetSite.ToString());
            sb.AppendLine();
            sb.AppendLine(" -- StackTrace:");
            sb.AppendLine(exc.StackTrace);

            return sb.ToString();
        }

    } //  class StartParams

} // namespace AppTools

