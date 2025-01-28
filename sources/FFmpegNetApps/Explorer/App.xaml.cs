using System;
using System.Windows;
using System.Windows.Threading;


using AppTools;
using FFmpegNetAux;

namespace Explorer
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {

        internal const string AppName = "FFmpeg Explorer";
        const string AppSubFolder = "explorer";
        const string LogFileNameNet = "NetExpl.log";
        internal const string LogFileNameWrap = "CppExpl.log";

        static StartParams StartPrms;
        static AppInst     AppInst;

        internal static bool IsFirstAppInst => AppInst.IsFirstAppInst;

        internal static StartParams StartParams => StartPrms;

        private void App_Startup(object sender, StartupEventArgs e)
        {
            StartPrms = new StartParams(AppSubFolder, e.Args);
            AppInst = new AppInst(AppSubFolder);

            if (IsFirstAppInst)
            {
                if (LogNet.Open(LogNetImpl.Default, StartPrms.GetLogFilePath(LogFileNameNet)))
                {
                    LogNet.PrintInfo(
                        $"{AppName}: platform={StartParams.PlatformConfigTag}, dev={StartPrms.Dev}, app root={StartPrms.AppRoot}");
                }
            }
        }

        private void App_Exit(object sender, ExitEventArgs e)
        {
            LogNet.PrintInfo("App_Exit");
            LogNet.Close();
            AppInst.Close();
        }

        private void App_DispatcherUnhandledException(object sender, DispatcherUnhandledExceptionEventArgs e)
        {
            LogNet.PrintExc(e.Exception, "Application Unhandled Exception");
            StartPrms.LogUnhandledException(e.Exception);
            e.Handled = true;
            ShowMessage("Application Unhandled Exception.");
        }

        internal static MessageBoxResult ShowMessage(string msg, bool yesNo = false)
        {
            return (!yesNo)
                ? MessageBox.Show(msg, AppName)
                : MessageBox.Show(msg, AppName, MessageBoxButton.YesNo, MessageBoxImage.Question);
        }

    } // class App

} // namespace Explorer
