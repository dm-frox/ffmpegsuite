using System;
using System.Windows;
using System.Windows.Threading;

using AppTools;
using FFmpegNetAux;

namespace Player
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {

        internal const string AppName  = "FFmpeg Player";
        const string AppSubFolder    = "player";
        const string LogFileNameNet  = "NetPlayer.log";
        internal const string LogFileNameWrap = "CppPlayer.log";

        static StartParams StartPrms;

        static AppInst     AppInst;
        
        internal static StartParams StartParams => StartPrms;

        internal static bool IsFirstAppInst => AppInst.IsFirstAppInst;

        private void Application_Startup(object sender, StartupEventArgs e)
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

        private void Application_Exit(object sender, ExitEventArgs e)
        {
            LogNet.PrintInfo("Application_Exit");
            LogNet.Close();
            AppInst.Close();
        }

        private void Application_DispatcherUnhandledException(object sender, DispatcherUnhandledExceptionEventArgs e)
        {
            LogNet.PrintExc(e.Exception, "Player, Application Unhandled Exception");
            StartPrms.LogUnhandledException(e.Exception);
            e.Handled = true;
            ShowMessage("Application unhandled exception.");
        }

        internal static void ShowMessage(string msg)
        {
            MessageBox.Show(msg, AppName);
        }

        internal static void ShowMessageExcl(Window owner, string msg)
        {
            MessageBox.Show(owner, msg, AppName, MessageBoxButton.OK, MessageBoxImage.Exclamation);
        }

        internal static void ShowMessageErr(Window owner, string msg)
        {
            MessageBox.Show(owner, msg, AppName, MessageBoxButton.OK, MessageBoxImage.Error);
        }

    } // partial class App
    
} // namespace Player

//public static event Microsoft.Win32.PowerModeChangedEventHandler PowerModeChanged;
//public class PowerModeChangedEventArgs : EventArgs
//public Microsoft.Win32.PowerModes Mode { get; }
//public enum PowerModes
//Resume 1
//Операционная система готова выйти из приостановленного состояния.
//StatusChange	2	
//Событие уведомления о состоянии режима питания было вызвано операционной системой. Оно может указывать на слабый заряд батареи или ее зарядку, на переход от питания переменного тока к питанию от батареи или на другие изменения статуса источника питания системы.
//Suspend	3	
//Операционная система готова приостановить работу.
