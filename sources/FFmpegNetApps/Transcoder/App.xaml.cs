using System;
using System.Windows;
using System.Windows.Threading;

using AppTools;
using FFmpegNetAux;

namespace Transcoder
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        internal const string AppName         = "FFmpeg Transcoder";
        const string AppSubFolder             = "transcoder";
        const string LogFileNameNet           = "NetTrans.log";
        internal const string LogFileNameWrap = "CppTrans.log";

        static StartParams StartPrms;
        static AppInst     AppInst;


        internal static StartParams StartParams => StartPrms;

        internal static bool IsFirstAppInst => AppInst.IsFirstAppInst;

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

        internal static void ShowMessage(string msg)
        {
            MessageBox.Show(msg, AppName);
        }

        internal static void ShowMessageErr(Window owner, string msg)
        {
            MessageBox.Show(owner, msg, AppName, MessageBoxButton.OK, MessageBoxImage.Error);
        }

        internal static MessageBoxResult ShowMessageQ(Window owner, string msg)
        {
            return MessageBox.Show(owner, msg, AppName, MessageBoxButton.YesNo, MessageBoxImage.Question);
        }

    } // partial class App

} // namespace Transcoder
