using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Threading;
using System.ComponentModel;
using System.IO;

using Microsoft.Win32;

using FFmpegInterop;
using FFmpegNetAux;
using FFmpegNetMediaSource;
using FFmpegNetTranscoder;
using AppTools;
using System.Windows.Documents;

namespace Transcoder
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        enum Mode { None, Transcoding};

        const int TimerInterval = 400;

        const bool UseFilterGraph = false;

        const string HelpFile = "Transcoder.html";

        const string DocUrl = @"https://ffmpeg.org/documentation.html";

        CurrTimeMan m_CurrTimeMan;

        DispatcherTimer m_Timer;

        TranscoderNet m_Transcoder;

        bool m_Canceled;
        bool m_End;

        Mode m_Mode;
        bool m_IniMode;

        public MainWindow()
        {
            InitializeComponent();

            this.Title = StartParams.GetAppTitle(App.AppName, App.IsFirstAppInst); 

            m_CurrTimeMan = new CurrTimeMan(labelCurrTime);

            m_Timer = new DispatcherTimer() { Interval = TimeSpan.FromMilliseconds(TimerInterval) };
            m_Timer.Tick += delegate { CheckState(); };

            comboSrcType.Fill(ResStr1.ComboTypes.Split(','));

            comboFormatsSrc.Fill(PrmNmOpts.Read(GetParamsPath("FormatsSrc.xml")), HeadItems.Default);
            comboFormatsDst.Fill(PrmNmOpts.Read(GetParamsPath("FormatsDst.xml")), HeadItems.Default);

            comboEncVideo.Fill(PrmEncVideo.Read(GetParamsPath("EncodersVideo.xml")), HeadItems.None);
            comboEncAudio.Fill(PrmEncAudio.Read(GetParamsPath("EncodersAudio.xml")), HeadItems.None);

            comboMeta.Fill(PrmMetadata.Read(GetParamsPath("Metadata.xml")), HeadItems.None);
            comboMetaVideo.Fill(PrmMetadata.Read(GetParamsPath("MetadataVideo.xml")), HeadItems.None);
            comboMetaAudio.Fill(PrmMetadata.Read(GetParamsPath("MetadataAudio.xml")), HeadItems.None);

            labelResult.Content = string.Empty;

            SystemEvents.PowerModeChanged += Window_PowerModeChanged;
            panelSrcInfo.Visibility = Visibility.Hidden;
        }

        void InitFFmpeg()
        {
            if (FFmpegConfig.Init(App.StartParams, "LogParams.xml", App.LogFileNameWrap, App.IsFirstAppInst))
            {
                labelFFmpeg.Content = ResStr1.Lab2;
            }
            else
            {
                App.ShowMessageErr(this, ResStr1.Msg1);
            }
        }

        void Window_Loaded(object sender, RoutedEventArgs e)
        {
            m_IniMode = true;
            m_Timer.IsEnabled = true;
        }

        void Window_Closed(object sender, EventArgs e)
        {
            if (m_Transcoder != null)
            {
                m_Transcoder.Dispose();
            }
            SystemEvents.PowerModeChanged -= Window_PowerModeChanged;
            Core.Close();
        }

        void Window_Closing(object sender, CancelEventArgs e)
        {
            if (m_Mode != Mode.None)
            {
                e.Cancel = true;
            }
        }

        static string GetParamsPath(string file) => App.StartParams.GetParamsFilePath(file);

        static string GetMetadata(ComboBox cb)
        {
            return (cb.SelectedIndex > 0) ? ((PrmMetadata)cb.SelectedItem).Metadata : null;
        }

        void buttonTest_Click(object sender, RoutedEventArgs e)
        {
            ForsePause(true);
        }

        void ForsePause(bool sync)
        {
            if (m_Transcoder != null)
            {
                m_Transcoder.Pause();
                LogNet.PrintInfo("ForsePause, sync={0}", sync);
                labelResult.Content = ResStr1.Lab1;
                buttonResume.IsEnabled = true;
                m_CurrTimeMan.Suspend();
                m_Timer.Stop();
            }
        }

        void buttonResume_Click(object sender, RoutedEventArgs e)
        {
            if (m_Transcoder != null)
            {
                m_Transcoder.Run();
                labelResult.Content = "";
                buttonResume.IsEnabled = false;
                m_CurrTimeMan.Resume();
                m_Timer.Start();
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
                    Dispatcher.BeginInvoke(new Action(() => ForsePause(false)));
                }
            }
        }

        void buttonSrcFileDialog_Click(object sender, RoutedEventArgs e)
        {
            string path = null;
            bool rr = false;
            if (comboSrcType.SelectedIndex == 0)
            {
                int filtInd = 0; 
                rr = FileDlg.LaunchOpenFileDialog(this, filtInd, ref path);
            }
            else if (comboSrcType.SelectedIndex == 1)
            {
                rr = DShowDevListWindow.Launch(this, Core.GetDShowDevList, ParamsTools.ComposeDShowUrl, ref path);
            }
            if (rr)
            {
                textBoxSrcPath.Text = path;
            }
        }

        void buttonDstFileDialog_Click(object sender, RoutedEventArgs e)
        {
            string path = null;
            if (FileDlg.LaunchSaveFileDialogTrans(this, false, ref path))
            {
                textBoxDstPath.Text = path;
            }
        }

        bool CanWrite()
        {
            bool ret = false;
            if (!File.Exists(textBoxDstPath.Text))
            {
                ret = true;
            }
            else
            {
                if (App.ShowMessageQ(this, ResStr1.Msg2) == MessageBoxResult.Yes)
                {
                    ret = true;
                }
            }
            return ret;
        }

        void buttonStart_Checked(object sender, RoutedEventArgs e)
        {
            labelResult.Content = string.Empty;
            string msg = null;
            bool res = false;
            if (VerifyStartTrans())
            {
                res = StartTranscoding();
            }
            else
            {
                msg = ResStr1.Msg3;
            }
            if (res)
            {
                buttonStart.Content = ResStr1.BtnStop;
                panelTransParams.IsEnabled = false;
                panelSrcInfo.Visibility = Visibility.Visible;
            }
            else
            {
                buttonStart.IsChecked = false;
            }

            if (msg != null)
            {
                App.ShowMessageErr(this, msg);
            }
        }

        void buttonStart_Unchecked(object sender, RoutedEventArgs e)
        {
            bool err = false;
            if (m_Mode == Mode.Transcoding)
            {
                err = m_Transcoder.Error;
                if (m_End)
                {
                    StopTranscoding(true);
                    CloseProcessing();
                }
                else
                {
                    StopTranscoding(false);
                    m_Canceled = true;
                    buttonStart.IsEnabled = false;
                    buttonStart.Content = "Wait...";
                }
            }
            if (err)
            {
                App.ShowMessageErr(this, ResStr1.Msg4);
            }
        }

        bool VerifyStartTrans()
        {
            bool ret =
                !string.IsNullOrEmpty(textBoxSrcPath.Text) &&
                !string.IsNullOrEmpty(textBoxDstPath.Text) &&
                (comboEncVideo.SelectedIndex > 0 || comboEncAudio.SelectedIndex > 0);
            if (ret)
            {
                if (!CanWrite())
                    ret = false;
            }
            //if (ret)
            //{
            //    if ((bool)checkBoxVideo.IsChecked && comboEncVideo.SelectedIndex < 0)
            //        ret = false;
            //}
            //if (ret)
            //{
            //    if ((bool)checkBoxAudio.IsChecked && comboEncAudio.SelectedIndex < 0)
            //        ret = false;
            //}
            return ret;
        }

        void CloseProcessing()
        {
            buttonStart.Content = ResStr1.BtnStart;
            buttonStart.IsEnabled = true;
            panelTransParams.IsEnabled = true;
            m_Mode = Mode.None;
            m_Timer.IsEnabled = false;
            DisableProgress();
            LogNet.PrintInfo("End transcoding");
        }

        void CheckState() // tick
        {
            if (m_IniMode)
            {
                m_Timer.IsEnabled = false;
                m_IniMode = false;
                InitFFmpeg();
            }
            else
            {
                bool end = CheckState(out double pos);
                if (end)
                {
                    //LoggerNet.PrintInfo("CheckState, end detected ===============");
                    labelResult.Content = string.Format("Expended time={0}", m_CurrTimeMan.CurrentTimeStr);
                    m_CurrTimeMan.Stop();
                    m_End = true;
                    if (!m_Canceled)
                    {
                        buttonStart.IsChecked = false;
                    }
                    else
                    {
                        StopTranscoding(true);
                        CloseProcessing();
                    }
                }
                else
                {
                    SetProgress(pos);
                    m_CurrTimeMan.Update();
                }
            }
        }

        void GetParamsVideo(ref FrameParamsVideo frm, ref EncoderParamsVideo enc)
        {
            var tp = (PrmEncVideo)comboEncVideo.SelectedItem;
            if (tp != null)
            {
                frm = new FrameParamsVideo() { Width = tp.Width, Height = tp.Height, FilterStr = tp.FilterStr, FpsFactor = tp.Fps };
                enc = new EncoderParamsVideo(tp.Encoder) { Bitrate = tp.Bitrate, Preset = tp.Preset, Crf = tp.Crf };
                if (tp.HasOptions)
                {
                    enc.SetOptions(tp.Options);
                }
            }
        }

        void GetParamsAudio(ref FrameParamsAudio frm, ref EncoderParamsAudio enc)
        {
            var tp = (PrmEncAudio)comboEncAudio.SelectedItem;
            if (tp != null)
            {
                frm = new FrameParamsAudio() { 
                    Chann = tp.Chann, SampleRate = tp.SampleRate, 
                    SampleFormat = tp.SampleFormat, ChannLayout = tp.ChannLayout,
                    FilterStr = tp.FilterStr };
                enc = new EncoderParamsAudio(tp.Encoder) { Bitrate = tp.Bitrate };
                if (tp.HasOptions)
                {
                    enc.SetOptions(tp.Options);
                }
            }
        }

        void StopTranscoding(bool shutDown)
        {
            if (m_Transcoder != null)
            {
                if (shutDown)
                {
                    m_Transcoder.Dispose();
                    m_Transcoder = null;
                }
                else
                {
                    m_Transcoder.CancelInput();
                }
            }
        }

        bool StartTranscoding()
        {
            LogNet.PrintInfo("StartTranscoding ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
            m_Canceled = false;
            m_End = false;

            bool ret = false;
            StopTranscoding(true);
            m_Transcoder = new TranscoderNet(UseFilterGraph);

            m_Mode = Mode.None;

            bool useVideo = comboEncVideo.SelectedIndex > 0;
            bool useAudio = comboEncAudio.SelectedIndex > 0;

            var readerParams = new DemuxerParams(textBoxSrcPath.Text, useVideo, useAudio);
            if (comboFormatsSrc.SelectedIndex > 0)
            {
                var itm = (PrmNmOpts)comboFormatsSrc.SelectedItem;
                readerParams.Format = itm.Name;
                readerParams.SetOptions(itm.Options);
            }
            if (comboSrcType.SelectedIndex == 1)
            {
                readerParams.Format = ParamsTools.DShowFormat;
            }

            var writerParams = new MuxerParams(textBoxDstPath.Text);
            if (comboFormatsDst.SelectedIndex > 0)
            {
                var itm = (PrmNmOpts)comboFormatsDst.SelectedItem;
                readerParams.Format = itm.Name;
                readerParams.SetOptions(itm.Options);
            }

            FrameParamsVideo frmVid = null;
            EncoderParamsVideo encVid = null;
            if (useVideo)
            {
                GetParamsVideo(ref frmVid, ref encVid);
            }

            FrameParamsAudio frmAud = null;
            EncoderParamsAudio encAud = null;
            if (useAudio)
            {
                GetParamsAudio(ref frmAud, ref encAud);
            }

            string metaGen = GetMetadata(comboMeta);
            string metaVid = useVideo ? GetMetadata(comboMetaVideo) : null;
            string metaAud = useAudio ? GetMetadata(comboMetaAudio) : null;

            if (m_Transcoder.Build(
                readerParams, writerParams, 
                metaGen, metaVid, metaAud, 
                frmVid, encVid, 
                frmAud, encAud,
                useVideo, useAudio))
            {
                m_CurrTimeMan.Start();
                EnableProgress(m_Transcoder.Duration);
                SetSourceInfo(m_Transcoder);
                m_Timer.IsEnabled = true;

                m_Mode = Mode.Transcoding;
                LogNet.PrintInfo("Begin transcoding ...");

                ret = true;
            }
            else
            {
                StopTranscoding(true);
                LogNet.PrintError("Failed to start transcoding.");
                App.ShowMessageErr(this, ResStr1.Msg5);
            }

            return ret;
        }

        bool CheckState(out double pos)
        {
            bool ret = false;
            if (m_Mode == Mode.Transcoding)
            {
                ret = m_Transcoder.EndOfData || m_Transcoder.Error;
                pos = m_Transcoder.Position;
            }
            else
            {
                pos = 0.0;
            }
            return ret;
        }

        void EnableProgress(double dur)
        {
            if (dur > 0.0)
            {
                progress.IsIndeterminate = false;
                progress.Value = 0.0;
                progress.Maximum = dur;
            }
            else
            {
                progress.IsIndeterminate = true;
            }
        }

        void DisableProgress()
        {
            EnableProgress(1.0);
        }

        void SetProgress(double pos)
        {
            if (!progress.IsIndeterminate)
            {
                if (0.0 <= pos && pos <= progress.Maximum)
                {
                    progress.Value = pos;
                }
            }
        }

        void SetSourceInfo(TranscoderNet transcoder)
        {
            if (transcoder != null)
            {
                var mst = new MediaSourceTxt(transcoder.MediaSourceInfo, textBoxSrcPath.Text);
                textVideoInfo.Text = mst.Video;
                textAudioInfo.Text = mst.Audio;
                textDurInfo.Text = mst.Duration;
                textSizeInfo.Text = mst.Size;
            }
            else
            {
                textVideoInfo.Text = string.Empty;
                textAudioInfo.Text = string.Empty;
                textDurInfo.Text = string.Empty;
                textSizeInfo.Text = string.Empty;
            }
        }

        void comboSrcType_SelectionChanged(object sender, EventArgs e) => buttonDstFileDialog.IsEnabled = (comboSrcType.SelectedIndex != 2);

        void cmdHelp_Executed(object sender, EventArgs e) => Misc.OpenDoc(App.StartParams.GetHelpFilePath(HelpFile)); 

        void cmdDocs_Executed(object sender, EventArgs e) => Misc.OpenDoc(DocUrl);

    } // partial class MainWindow

} // namespace Transcoder

