using System;
using System.Windows;
using System.Windows.Controls.Primitives;
using System.Windows.Threading;
using System.Windows.Media.Imaging;
using System.ComponentModel;

using Microsoft.Win32;

using FFmpegInterop;
using FFmpegNetAux;
using FFmpegNetPlayer;
using AppTools;


namespace Player
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        const string HelpFile = "Player.html";
        const string DocUrl   = "https://ffmpeg.org/documentation.html";
        const string ImgPath  = "pack://application:,,,/Images/";


        const int InitFFmpegIntervalMs = 200;
        const int StopRewindIntervalMs = 120;

        const int OscStateIntervalMs = 240;
        const int OscWidth           = 2000;
        const int OscHeight          = 80;
        const double OscAmpFactor    = 0.75;

        const int RecListLen = 8;

        PlayerWatcher   m_PlayerWatcher;
        DelayedAction1  m_DelayedAct;

        MediaPlayerNet   m_Player;
        OscMan           m_OscMan;
        CurrTimeMan      m_CurrTimeMan;
        PosTracker       m_PosTracker;

        AppState         m_AppState;

        BcsMan           m_BcsMan;

        RecentStrList     m_RecListFile;
        RecentStrList     m_RecListCam;
        RecentStrList     m_RecListSdp;
        RecentStrList     m_RecListUrl;
        RecentStrListBase m_RecListCurr;

        bool     m_Closing;

        DateTime m_BenchmarkStart;

        EqParams       m_EqParams;
        DrawTextParams m_DrawTextParams;


        public MainWindow()
        {
            InitializeComponent();

            this.Title = StartParams.GetAppTitle(App.AppName, App.IsFirstAppInst);
            m_AppState = new AppState();
            this.DataContext = m_AppState;

            m_PlayerWatcher = new PlayerWatcher(CheckPlayerState);
            m_DelayedAct    = new DelayedAction1();

            var stopImage  = LoadBitmap("img-stop.png");
            var audioImage = LoadBitmap("img-audio.png");
            var videoImage = LoadBitmap("img-video.png");
            var infoImage  = LoadBitmap("img-info.png");

            videoControl.Init(scrollVideo);
            LogNet.PrintInfo("MainWindow: pixelsPerDip={0}", videoControl.PixelsPerDip);

            m_Player = new MediaPlayerNet(videoControl,
                stopImage, audioImage, videoImage, infoImage,
                DelayedActionEnd);

            SystemEvents.PowerModeChanged += Window_PowerModeChanged;

            m_CurrTimeMan = new CurrTimeMan(labelCurrTime);
            m_PosTracker = new PosTracker(labelPosTracker);

            m_OscMan = new OscMan(oscControl, oscCursor, OscWidth, OscHeight, OscAmpFactor, OscStateIntervalMs);

            m_RecListFile = CreateRecentStrList("files.txt", true);
            m_RecListCam  = CreateRecentStrList("cams.txt", false);
            m_RecListSdp  = CreateRecentStrList("sdps.txt", true);
            m_RecListUrl  = CreateRecentStrList("urls.txt", false);
            m_RecListCurr = null;

            sliderPosition.ForcedPosition += sliderPosition_ForcedPosition;

            string psFile = App.StartParams.BcsEffectFilePath;
            LogNet.PrintInfo("MainWindow: BCS effect file={0}", psFile);
            m_BcsMan = new BcsMan(videoControl, sliderB, sliderC, sliderS, psFile);

            comboSrcType.Fill(ResStr1.ComboTypes.Split(','));
            comboGpu.Fill(GpuCodecs, HeadItems.None);
            comboThreads.Fill(Threads, HeadItems.Default);
            comboFormats.Fill(PrmNmOpts.Read(GetPrmPath("Formats.xml")), HeadItems.Default);
            comboFiltVid.Fill(PrmFilter.Read(GetPrmPath("FiltersVideo.xml")), HeadItems.None);
            comboFiltAud.Fill(PrmFilter.Read(GetPrmPath("FiltersAudio.xml")), HeadItems.None);
            comboDecAud.Fill(PrmNmOpts.Read(GetPrmPath("DecodersAudio.xml")), HeadItems.None | HeadItems.Default, 1);
            comboDecVid.Fill(PrmNmOpts.Read(GetPrmPath("DecodersVideo.xml")), HeadItems.None | HeadItems.Default, 1);
            comboOscZoom.Fill(OscZoomScales);
            UpdateLoadedState(false);

            EnableSourceInfo(false);
            ClearBenchmark();

            labelRendParams.Content = string.Empty;

            m_Closing = false;

            m_BenchmarkStart = new DateTime();

            btnTest.Visibility = Visibility.Collapsed;

            m_EqParams = new EqParams();
            m_DrawTextParams = new DrawTextParams();

            InitPlayerControls();
        }

        void InitFFmpeg()
        {
            if (FFmpegConfig.Init(App.StartParams, "LogParams.xml", App.LogFileNameWrap, App.IsFirstAppInst))
            {
                string[] r = Core.GetHWAccels().ToUpper().Split(' ');
                comboHWAccel.Fill(r, HeadItems.None);
            }
            else
            {
                App.ShowMessageErr(this, ResStr1.Msg2);
            }
        }

        static string GetPrmPath(string file) => App.StartParams.GetParamsFilePath(file);

        static BitmapImage LoadBitmap(string fileName)
        {
            return new BitmapImage(new Uri(ImgPath + fileName));
        }

        static RecentStrList CreateRecentStrList(string fileName, bool ignoreCase)
        {
            return new RecentStrList(RecListLen, App.StartParams.GetDataFilePath(fileName), ignoreCase);
        }

        void Window_Loaded(object sender, EventArgs e)
        {
            m_RecListFile.Load();
            m_RecListCam.Load();
            m_RecListUrl.Load();
            m_RecListSdp.Load();

            SrcTypeChanged();

            m_DelayedAct.Begin(InitFFmpeg, InitFFmpegIntervalMs);
        }

        void Window_Closing(object sender, CancelEventArgs e)
        {
            if (!Rewinding)
            {
                if (TryClose())
                {
                    LogNet.PrintInfo("Window_Closing");
                }
                else
                {
                    e.Cancel = true;
                    LogNet.PrintInfo("Window_Closing, delayed");
                }
            }
            else
            {
                UncheckRewindUnloading();
                panelPlayer.IsEnabled = false;
                LogNet.PrintInfo("Window_Closing, Rewinding");
                e.Cancel = true;
                m_DelayedAct.Begin(Close, StopRewindIntervalMs);
            }
        }

        void ForsePause(bool sync)
        {
            if ((bool)buttonPlayPause.IsChecked)
            {
                buttonPlayPause.IsChecked = false;
                LogNet.PrintInfo("ForsePause, sync={0}", sync);
            }
        }

        void Window_PowerModeChanged(object sender, PowerModeChangedEventArgs e)
        {
            LogNet.PrintInfo("PowerModeChanged, {0}", e.Mode.ToString());
            if (e.Mode == PowerModes.Suspend)
            {
                if (CheckAccess())
                {
                    ForsePause(true);
                }
                else
                {
                    Dispatcher.BeginInvoke(new Action(()=>ForsePause(false)));
                }
            }
        }

        void Window_Closed(object sender, EventArgs e)
        {
            LogNet.PrintInfo("Window_Closed, begin...");
            m_Player.Unload();
            Core.Close();
            if (App.IsFirstAppInst)
            {
                m_RecListFile.Save();
                m_RecListCam.Save();
                m_RecListUrl.Save();
                m_RecListSdp.Save();
            }

            SystemEvents.PowerModeChanged -= Window_PowerModeChanged;

            LogNet.PrintInfo("Window_Closed, end");
        }

        void SetPathUrl()
        {
            string path = null;
            bool rr = false;
            if (IsFile)
            {
                rr = FileDlg.LaunchOpenFileDialog(this, ref path);
            }
            else if (IsCam)
            {
                rr = DShowDevListWindow.Launch(this, Core.GetDShowDevList, ParamsTools.ComposeDShowUrl, ref path);
            }
            else if (IsUrl)
            {
                if (IsRtpFlag)
                {
                    rr = FileDlg.LaunchOpenFileDialogRtp(this, ref path);
                }
            }
            if (rr && !string.IsNullOrEmpty(path))
            {
                comboUrl.Text = path;
            }
        }

        void buttonFileDialog_Click(object sender, EventArgs e) => SetPathUrl();

        void checkBox_BCS_Checked(object sender, EventArgs e)
        {
            if (!m_Player.IsStopped)
            {
                m_BcsMan.Enable(true);
            }
        }

        void checkBox_BCS_Unchecked(object sender, EventArgs e) => m_BcsMan.Enable(false);

        void resetBCS_Click(object sender, EventArgs e) => m_BcsMan.Reset();

        void sliderB_ValueChanged(object sender, EventArgs e) => m_BcsMan.UpdateB(textSliderB);

        void sliderC_ValueChanged(object sender, EventArgs e) => m_BcsMan.UpdateC(textSliderC);
        
        void sliderS_ValueChanged(object sender, EventArgs e) => m_BcsMan.UpdateS(textSliderS);

        void comboOscZoom_SelectionChanged(object sender, EventArgs e) => m_OscMan.SetOscYScale(comboOscZoom.SelectedIndex + 1.0);

        void checkBoxPix2Pix_Checked(object sender, EventArgs e) => videoControl.SetPix2Pix(true);
 
        void checkBoxPix2Pix_Unchecked(object sender, EventArgs e) => videoControl.SetPix2Pix(false);

        void checkBoxBenchmark_Unchecked(object sender, EventArgs e) => checkBoxSkipVideoRend.IsChecked = false;

        void cmdHelp_Executed(object sender, EventArgs e) => Misc.OpenDoc(App.StartParams.GetHelpFilePath(HelpFile));
 
        void cmdDocs_Executed(object sender, EventArgs e) => Misc.OpenDoc(DocUrl);

        void Toggle(ToggleButton btn)
        {
            if (btn.IsEnabled)
            {
                btn.IsChecked = !(bool)btn.IsChecked;
            }
        }

        void cmdMute_Executed(object sender, EventArgs e) => Toggle(buttonMute);

        void cmdVolDown_Executed(object sender, EventArgs e) => sliderSoundVolume.ChangeVolume(false);

        void cmdVolUp_Executed(object sender, EventArgs e) => sliderSoundVolume.ChangeVolume(true);

        void cmdPlayPause_Executed(object sender, EventArgs e) => Toggle(buttonPlayPause);

        void cmdPix2Pix_Executed(object sender, EventArgs e) => Toggle(checkBoxPix2Pix);

        void cmdLoadUnload_Executed(object sender, EventArgs e) => Toggle(buttonLoadUnload);

        private void btnTest_Click(object sender, EventArgs e)
        {
            //LogNet.PrintInfo("Test sliderPos: activated={0}, enabled={1}", sliderPosition.Activated, sliderPosition.IsEnabled);
        }
    } // partial class MainWindow

} // namespace Player

