using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Text;
using System.IO;

using FFmpegNetAux;
using FFmpegNetMediaSource;
using FFmpegNetPlayer;
using AppTools;


namespace Player
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        // RTSP is not technically a protocol handler in libavformat, it is a demuxer and muxer.
        // listen_timeout
        // Set maximum timeout(in seconds) to establish an initial connection. Setting listen_timeout > 0 sets rtsp_flags to ‘listen’.
        // Default is -1 which means an infinite timeout when ‘listen’ mode is set.

        const string RtspProtoPrefix = "rtsp://";
        const string RtspTimeoutKey = "listen_timeout";
        const int RtspTimeoutDefSec = 5;

        const string RtpOptionKey = "protocol_whitelist";
        const string RtpOptionVal = "file,rtp,udp";

        const string ConcatFormat = "concat";

        const string FileProtoPrefix = "file:";

        //const string PrefHWAccel = "DXVA2";

        const string JpegFileType = "jpg";

        const string JpegFileFormat = "jpeg_pipe";

        const int RendParamsIntervalMs = 800;

        const int DefJpegQualityLevel  = 75;

        const double PositionThreshold = 0.001;

        static readonly string[] OscZoomScales =
        {
            "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8"
        };

        static readonly string[] GpuCodecs = { "QSV", "CUVID" };
        static readonly string[] Threads = { "auto", "2", "3", "4", "6", "8", "10", "12", "14", "16" };

        static readonly int[] RewindFps = { 2, 5, 10, 20, 30, 50, 100, 150 };

        static readonly string[] JpegFileExtentions = { "jpg", "jpeg" };

        const int FpsIndexDef = 2;

        bool m_SkipCheckedUnchecked;
        bool m_PlayErr;
        bool m_StopNoSeek;

        bool     m_StopOnEof = true;
        OptionsX m_OptionsX;

        double m_CurrPos;
        bool   m_Unloading;

        bool   m_PlayerEnabled;

        bool   m_LastFrame;

        static bool IsRtspProto(string url) => url.StartsWith(RtspProtoPrefix);

        static bool IsFileProto(string url) => url.StartsWith(FileProtoPrefix);

        bool IsFile => comboSrcType.SelectedIndex == 0;

        bool IsCam => comboSrcType.SelectedIndex == 1;

        bool IsUrl => comboSrcType.SelectedIndex == 2;

        bool IsRtpFlag => (bool)checkBoxSrcSdp.IsChecked;

        bool Benchmark => IsFile && (bool)checkBoxBenchmark.IsChecked;

        bool UseVideoStrm => comboDecVid.SelectedIndex > 0;

        bool UseAudioStrm => comboDecAud.SelectedIndex > 0;

        bool Rewinding => (bool)buttonRewind.IsChecked;

        void InitPlayerControls()
        {
            foreach (int k in RewindFps)
            {
                comboFps.Items.Add(k);
            }
            comboFps.SelectedIndex = FpsIndexDef;
            m_OptionsX = new OptionsX();
            EnablePlayerGui(false);
        }

        void ForceToggleButton(ToggleButton btn, bool chcked)
        {
            if ((bool)btn.IsChecked != chcked)
            {
                m_SkipCheckedUnchecked = true;
                btn.IsChecked = chcked;
                m_SkipCheckedUnchecked = false;
            }
        }

        void SwitchToggleButton(ToggleButton btn)
        {
            ForceToggleButton(btn, !(bool)btn.IsChecked);
        }

        static string OptionsToIniFormat(string[] opts)
        {
            StringBuilder sb = new StringBuilder(64);
            for (int i = 0; i < opts.Length; i += 2)
            {
                sb.AppendFormat("{0}={1}\n", opts[i], opts[i + 1]); // ini format:  key=value\n...
            }
            return sb.ToString();
        }

        static void AddDecoderOptions(PrmNmOpts decItem, KeyValList decExParams)
        {
            if (decItem.HasOptions)
            {
                decExParams.Add("options", OptionsToIniFormat(decItem.Options));
            }
        }

        static void CheckDecExParams(ref KeyValList decExParams, bool video = true)
        {
            if (decExParams == null)
            {
                decExParams = new KeyValList();
                decExParams.Add("mediatype", video ? "video" : "audio");
            }
        }

        static void AddNameFmtOpts(object comboItem, KeyValList decExParams)
        {
            var item = (PrmNmOpts)comboItem;
            decExParams.Add("name", item.Name);
            decExParams.AddNoEmpty("format", item.Format);
            AddDecoderOptions(item, decExParams);
        }

// play item params

        void SetFormatOptions(DemuxerParams dmp, bool format, bool options)
        {
            if (comboFormats.SelectedIndex > 0)
            {
                var itm = (PrmNmOpts)comboFormats.SelectedItem;
                if (format)
                {
                    dmp.Format = itm.Name;
                }
                if (options)
                {
                    dmp.SetOptions(itm.Options);
                }
            }
        }

        DemuxerParams GetDemuxPrms(string url, bool video, bool audio, ref bool alive)
        {
            DemuxerParams dmp = null;
            if (IsFile)
            {
                dmp = new DemuxerParams(FileHlp.NormPath(url), video, audio && !Benchmark);
                SetFormatOptions(dmp, true, true);
                if (FileHlp.CheckExtention(url, JpegFileExtentions))
                {
                    dmp.Format = JpegFileFormat;
                }
            }
            else if (IsCam)
            {
                dmp = new DemuxerParams(url, video, audio);
                dmp.Format = ParamsTools.DShowFormat;
                SetFormatOptions(dmp, false, true);
                alive = true;
            }
            else if (IsUrl)
            {
                if (IsRtpFlag)
                {
                    dmp = new DemuxerParams(url, true, true);
                    dmp.AddOption(RtpOptionKey, RtpOptionVal);
                    alive = true;
                }
                else
                {
                    dmp = new DemuxerParams(url, video, audio);
                    SetFormatOptions(dmp, true, true);
                    if (IsRtspProto(url))
                    {
                        dmp.AddOption(RtspTimeoutKey, RtspTimeoutDefSec.ToString());
                    }
                    if (!IsFileProto(url))
                    {
                        alive = true;
                    }
                }
            }
            return dmp;
        }

        string GetDecPrmsVideo()
        {
            KeyValList decExParams = null;
            if (comboDecVid.SelectedIndex > 1) // by name
            {
                CheckDecExParams(ref decExParams);
                AddNameFmtOpts(comboDecVid.SelectedItem, decExParams);
            }
            if (comboGpu.SelectedIndex > 0 && comboDecVid.SelectedIndex == 1) // gpu decoder
            {
                CheckDecExParams(ref decExParams);
                decExParams.Add("name", "*" + comboGpu.SelectedItem.ToString().ToLower());
            }
            if (comboHWAccel.SelectedIndex > 0) // hwaccel
            {
                CheckDecExParams(ref decExParams);
                decExParams.Add("hwaccel", comboHWAccel.SelectedItem.ToString().ToLower());
            }
            if (comboThreads.SelectedIndex > 0) // threads
            {
                CheckDecExParams(ref decExParams);
                decExParams.Add("threads", comboThreads.SelectedItem.ToString());
            }
            return decExParams?.ToString();
        }

        string GetDecPrmsAudio()
        {
            KeyValList decExParams = null;
            if (comboDecAud.SelectedIndex > 1) // by name
            {
                CheckDecExParams(ref decExParams, false);
                AddNameFmtOpts(comboDecAud.SelectedItem, decExParams);
                return decExParams.ToString();
            }
            return decExParams?.ToString();
        }

        FrameParamsVideo GetFramePrmsVideo(string url)
        {
            var prms = new FrameParamsVideo();
            if (comboFiltVid.SelectedIndex > 0)
            {
                var flt = (PrmFilter)comboFiltVid.SelectedItem;
                prms.FilterStr = ProcSubtitFilter(flt.Filter, url);
                prms.FpsFactor = flt.FpsFactor;
                prms.Seq = flt.Seq != 0;
            }
            return prms;

            //    vidPrms.Width = 450;
            //    vidPrms.Height = 450;
        }

        FrameParamsAudio GetFramePrmsAudio()
        {
            var prms = new FrameParamsAudio();

            if (comboFiltAud.SelectedIndex > 0)
            {
                prms.FilterStr = ((PrmFilter)comboFiltAud.SelectedItem).Filter;
            }

            return prms;

            //    audPrms.Chann = 1;
            //    audPrms.SampleRate = 22050;
        }

        ModeX GetModeX(bool alive)
        {
            var modex = new ModeX();
            modex.Alive = alive;
            if (modex.Alive || Benchmark)
            {
                modex.HoldPresentationTime = false;
            }
            if (Benchmark && (bool)checkBoxSkipVideoRend.IsChecked)
            {
                modex.SkipVideoRendering = true;
            }
 
            return modex;
        }

        PlayItemPrms GetPlayItem()
        {
            var pi = new PlayItemPrms();
            string url = comboUrl.Text;

            if (!string.IsNullOrEmpty(url))
            {
                bool video = UseVideoStrm;
                bool audio = UseAudioStrm;
                bool alive = false;

                pi.DemuxPrms = GetDemuxPrms(url, video, audio, ref alive);

                pi.DecPrmsVideo = GetDecPrmsVideo();
                pi.DecPrmsAudio = GetDecPrmsAudio();

                pi.FramePrmsVideo = GetFramePrmsVideo(url);
                pi.FramePrmsAudio = GetFramePrmsAudio();

                pi.ModeX    = GetModeX(alive);
                pi.OptionsX = m_OptionsX;
            }

            return pi;
        }

// 

        string ProcSubtitFilter(string flt, string url)
        {
            string ret = null;
            if (flt.IndexOf("subtitles=default") == 0)
            {
                string path = null;
                if (IsFile)
                {
                    path = url.Replace('\\', '/');
                    if (char.IsLetter(path[0]) && path[1] == ':')
                    {
                        path = path.Insert(1, @"\");
                    }
                }
                else
                {
                    path = url;
                }
                ret = $"subtitles='{path}'";

                int scPos = flt.IndexOf(':');
                if (scPos > 0)
                {
                    string s = flt.Substring(scPos + 1);
                    if (!string.IsNullOrEmpty(s) && int.TryParse(s, out int ind))
                    {
                        ret += $":si={ind}";
                    }
                }
            }
            else
            {
                ret = flt;
            }
            return ret;
        }

        void UpdateRecentListControl(int selInd)
        {
            var items = m_RecListCurr.GetItems();
            comboUrl.Items.Clear();
            foreach (var item in items)
            {
                comboUrl.Items.Add(item);
            }
            if (selInd < 0)
            {
                comboUrl.Text = string.Empty;
            }
            comboUrl.SelectedIndex = selInd;
        }

        void UpdateRecentList(string url)
        {
            m_RecListCurr.AddItem(url);
            UpdateRecentListControl(0);
        }

        void DisplayRendParams()
        {
            string s = m_Player.GetRendParams();
            if (s != null)
            {
                LogNet.PrintInfo("DisplayRendParams, params={0}", s);
                labelRendParams.Content = s;
            }
            else
            {
                LogNet.PrintInfo("DisplayRendParams, null");
            }
        }

        bool CheckDelayedAction(ToggleButton btn = null)
        {
            bool ret = !m_Player.DelayedActionInProgress();
            if (!ret)
            {
                App.ShowMessageExcl(this, ResStr1.Msg8);
                if (btn != null)
                {
                    SwitchToggleButton(btn);
                }
            }
            return ret;
        }

        bool Load()
        {
            bool loaded = false;
            m_PlayErr = false;
            m_StopNoSeek = false;
            m_LastFrame = false;
            PlayItemPrms pi = GetPlayItem();
            if (pi.IsValid)
            {
                if (m_Player.Build(pi))
                {
                    ForceToggleButton(buttonPlayPause, true);
                    UpdateLoadedState(true);
                    SetSourceInfo(pi);

                    UpdateRecentList(pi.DemuxPrms.Url);
                    UpdateOsc(pi.DemuxPrms);
                    m_CurrPos = 0.0;
                    loaded = true;
                    if (m_Player.HasMedia)
                    {
                        m_DelayedAct.Begin(DisplayRendParams, RendParamsIntervalMs);
                    }
                    else
                    {
                        App.ShowMessageExcl(this, ResStr1.Msg1);
                    }
                }
                else
                {
                    App.ShowMessageErr(this, ResStr1.Msg3);
                }
            }
            else
            {
                App.ShowMessageErr(this, ResStr1.Msg4);
            }
            return loaded;
        }

        void Unload()
        {
            if (m_Player.HasSource)
            {
                if (CheckDelayedAction(buttonLoadUnload))
                {
                    EnablePlayerGui(false);
                    if (!Rewinding)
                    {
                        m_Player.Unload();
                        m_OscMan.Close(true);
                        ForceToggleButton(buttonPlayPause, false);
                    }
                    else
                    {
                        UncheckRewindUnloading();
                        m_DelayedAct.Begin(Unload, StopRewindIntervalMs);
                    }
                }
                else
                {
                    LogNet.PrintError("Unload: CheckDelayedAction");
                }
            }
        }

        void UpdateOsc(DemuxerParams prms)
        {
            if (prms != null && m_Player.HasAudio && IsFile)
            {
                m_OscMan.SetParams(prms);
                if ((bool)checkBoxOsc.IsChecked)
                {
                    m_OscMan.Start();
                }
            }
        }

        void UpdateFilterControls(bool loaded)
        {
            m_EqParams?.Reset();
            m_DrawTextParams?.Reset();
            if (loaded) 
            {
                if (GetFilterCommandVideo() > 0)
                {
                    buttonFilterCmdVideo.IsEnabled = true;
                    labelFiltVid.IsEnabled = true;
                    labelFiltVid.Target = buttonFilterCmdVideo;
                }
                else
                {
                    buttonFilterCmdVideo.IsEnabled = false;
                    labelFiltVid.IsEnabled = false;
                    labelFiltVid.Target = null;
                }

                if (GetFilterCommandAudio() > 0)
                {
                    buttonFilterCmdAudio.IsEnabled = true;
                    labelFiltAud.IsEnabled = true;
                    labelFiltAud.Target = buttonFilterCmdAudio;
                }
                else
                {
                    buttonFilterCmdAudio.IsEnabled = false;
                    labelFiltAud.IsEnabled = false;
                    labelFiltAud.Target = null;
                }
            }
            else // unloaded
            {
                buttonFilterCmdVideo.IsEnabled = false;
                labelFiltVid.IsEnabled = true;
                labelFiltVid.Target = comboFiltVid;

                buttonFilterCmdAudio.IsEnabled = false;
                labelFiltAud.IsEnabled = true;
                labelFiltAud.Target = comboFiltAud;

                CloseFilterCommands();
            }
        }

        void CloseFilterCommands()
        {
            FilterCmdWindow0.CloseEx();
            FilterCmdWindow1.CloseEx();
            FilterCmdWindow2.CloseEx();
        }

        void UpdateLoadedState(bool loaded)
        {
            if (loaded)
            {
                buttonLoadUnload.Content = ResStr1.BtnUnload;
                if (m_Player.HasMedia)
                {
                    if (m_Player.CanSeek)
                    {
                        sliderPosition.Activate(m_Player.Duration);
                    }
                    m_PlayerWatcher.Start();
                    m_CurrTimeMan.Start();
                    if (m_Player.HasVideo && !(bool)checkBoxSkipVideoRend.IsChecked)
                    {
                        panelBcs.IsEnabled = true;
                    }
                    if (Benchmark)
                    {
                        m_BenchmarkStart = DateTime.Now;
                    }
                }
            }
            else // unloaded
            {
                buttonLoadUnload.Content = ResStr1.BtnLoad;
                buttonPlayPause.IsEnabled = false;
                buttonStop.IsEnabled = false;
                buttonSaveFrame.IsEnabled = false;
                buttonNextFrame.IsEnabled = false;
                buttonPrevFrame.IsEnabled = false;
                buttonRewind.IsEnabled = false;
                sliderPosition.Deactivate();

                m_PlayerWatcher.Stop();
                m_CurrTimeMan.Stop();
                SetSourceInfo(null);
                labelRendParams.Content = string.Empty;

                m_BcsMan.Reset(checkBoxBcs);
                panelBcs.IsEnabled = false;
                ClearBenchmark();
            }
            UpdateFilterControls(loaded);
            m_AppState.Unloaded = !loaded;
        }

        void EnablePlayerGui(bool enable)
        {
            m_PlayerEnabled = enable;
            UpdatePlayerState();
        }

        void UpdatePlayerState()
        {
            bool pp = m_PlayerEnabled && m_Player.HasMedia && !m_Player.Alive && !Rewinding;
            bool stopped = m_Player.IsStopped;
            buttonPlayPause.IsEnabled = pp && (Benchmark ? stopped : true);
            buttonStop.IsEnabled = pp && !stopped;

            bool pv = m_PlayerEnabled && m_Player.HasVideo && m_Player.IsPaused && !Rewinding;
            bool pf = pv && m_Player.CanSeek;
            buttonSaveFrame.IsEnabled = pv;
            buttonNextFrame.IsEnabled = pf;
            buttonPrevFrame.IsEnabled = pf;
            buttonRewind.IsEnabled    = pf || Rewinding;

            if (sliderPosition.Activated)
            {
                sliderPosition.ShowThumb(!stopped && !Benchmark && !Rewinding);
                if (stopped)
                {
                    ResetPosition();
                }
            }

            if ((bool)checkBoxBcs.IsChecked)
            {
                m_BcsMan.Enable(!stopped);
            }
        }

        bool TryClose()
        {
            bool ret = true;
            Unload();
            if (m_Player.HasSource)
            {
                m_Closing = true;
                ret = false;
            }
            CloseFilterCommands();

            return ret;
        }

        void DelayedActionEnd(PosFlag posFlags) // callback from player, end of delayed operation
        {
            if (!m_Closing)
            {
                EnablePlayerGui(true);

                if (posFlags == PosFlag.None) // stop, unload
                {
                    if (!m_Player.HasSource)
                    {
                        UpdateLoadedState(false);
                    }
                }
                else // position
                {
                    if (m_Player.HasVideo && m_Player.IsPaused)
                    {
                        if (posFlags == PosFlag.End)
                        {
                            m_LastFrame = true;
                        }
                        buttonNextFrame.IsEnabled = !m_LastFrame;
                        buttonPrevFrame.IsEnabled = posFlags != PosFlag.Begin;
                        buttonRewind.IsEnabled = buttonNextFrame.IsEnabled;
                    }
                }
                if (Benchmark && m_Player.IsPlaying)
                {
                    ClearBenchmark();
                    m_BenchmarkStart = DateTime.Now;
                }
                if (m_LastFrame)
                {
                    buttonNextFrame.IsEnabled = false;
                    buttonRewind.IsEnabled = false;
                    buttonPlayPause.IsEnabled = false;
                }
            }
            else
            {
                this.Close();
            }
        }

        void buttonLoad_Checked(object sender, EventArgs e)
        {
            if (CheckDelayedAction(buttonLoadUnload))
            {
                if (!Load())
                {
                    buttonLoadUnload.IsChecked = false;
                }
            }
            else
            {
                LogNet.PrintError("Load: CheckDelayedAction");
            }
        }

        void buttonLoad_Unchecked(object sender, EventArgs e) => Unload();

        void checkBoxOsc_Checked(object sender, EventArgs e)
        {
            m_OscMan.Start();
            if (m_Player.IsPaused)
            {
                m_OscMan.SetCursorPosition(m_Player.GetPosition());
            }
        }

        void checkBoxOsc_Unchecked(object sender, EventArgs e) => m_OscMan.Close(false);
 
// play/pause/stop

        void PlayPause()
        {
            if (!m_SkipCheckedUnchecked)
            {
                if (CheckDelayedAction(buttonPlayPause))
                {
                    EnablePlayerGui(false);
                    m_Player.PlayPause();
                }
                else
                {
                    LogNet.PrintError("PlayPause: CheckDelayedAction");
                }
            }
        }

        void DisplayBenchmark(double pos)
        {
            if (pos >= 0.0)
            {
                double cur = (DateTime.Now - m_BenchmarkStart).TotalSeconds;
                double pp = (pos > 0.0) ? pos : m_Player.Duration;
                double res = (pp > 0.0) ? cur * 100.0 / pp : 0.0;
                string s = (res > 10.0) ? $"{res:F0}" : $"{res:F1}";
                labelBenchmark.Content = $"Benchmark: {s}% ({cur:F1} sec)";
                labelBenchmark.Visibility = Visibility.Visible;
                m_CurrTimeMan.Stop();
                LogNet.PrintInfo("Benchmark={0}%, framePos={1:F3}, time={2:F3}", s, pos, cur);
            }
            else
            {
                labelBenchmark.Content = null;
                labelBenchmark.Visibility = Visibility.Hidden;
                m_BenchmarkStart = new DateTime();
            }
        }

        void DisplayBenchmark()
        {
            if (Benchmark)
            {
                DisplayBenchmark(m_Player.LastVideoFramePosition);
            }
        }

        void ClearBenchmark()
        {
            DisplayBenchmark(-1.0);
        }

        void Stop()
        {
            if (CheckDelayedAction())
            {
                m_LastFrame = false;
                DisplayBenchmark();
                EnablePlayerGui(false);
                m_Player.Stop();
                ForceToggleButton(buttonPlayPause, false);
            }
            else
            {
                LogNet.PrintError("Stop, CheckDelayedAction");
            }
        }

        void UncheckRewindUnloading()
        {
            m_Unloading = true;
            buttonRewind.IsChecked = false;
            m_Unloading = false;
        }

        private void buttonRewind_Checked(object sender, EventArgs e)
        {
            if (CheckDelayedAction(buttonRewind))
            {
                EnablePlayerGui(false);
                int fps = (int)comboFps.SelectedItem;
                m_Player.StartRewinder(fps);
                sliderPosition.ShowThumb(false);
                LogNet.PrintInfo("Rewind, start, fps={0}", fps);
            }
            else
            {
                LogNet.PrintError("Rewind start, CheckDelayedAction");
            }
        }

        private void buttonRewind_Unchecked(object sender, EventArgs e)
        {
            if (CheckDelayedAction(buttonRewind))
            {
                EnablePlayerGui(false);
                m_Player.StopRewinder(m_Unloading);
                sliderPosition.ShowThumb(!m_Unloading);
                LogNet.PrintInfo("Rewind, stop{0}", m_Unloading ? ", unloading" : "");
            }
            else
            {
                LogNet.PrintError("Rewind stop, CheckDelayedAction");
            }
        }

        void cmdRewind_Executed(object sender, EventArgs e) => Toggle(buttonRewind);

        void cmdFps_Executed(object sender, EventArgs e)
        {
            if (comboFps.IsEnabled)
            {
                comboFps.Focus();
            }
        }

        private void comboFps_SelectionChanged(object sender, EventArgs e)
        {
            if (Rewinding)
            {
                int fps = (int)comboFps.SelectedItem;
                m_Player.StartRewinder(fps);
            }
        }

        private void buttonNextFrame_Click(object sender, EventArgs e)
        {

            if (CheckDelayedAction())
            {
                EnablePlayerGui(false);
                m_Player.GoToNextFrame();
            }
            else
            {
                LogNet.PrintError("NextFrame, CheckDelayedAction");
            }
        }

        private void buttonPrevFrame_Click(object sender, EventArgs e)
        {
            if (CheckDelayedAction())
            {
                m_LastFrame = false;
                EnablePlayerGui(false);
                m_Player.GoToPrevFrame();
            }
            else
            {
                LogNet.PrintError("PrevFrame, CheckDelayedAction");
            }
        }

        void buttonPlayPause_Checked(object sender, EventArgs e) => PlayPause();

        void buttonPlayPause_Unchecked(object sender, EventArgs e) => PlayPause();

        void buttonStop_Click(object sender, EventArgs e) => Stop();

        void UpdatePosition()
        {
            if (m_PlayerEnabled)
            {
                double pos = m_Player.GetPosition();
                if (Math.Abs(pos - m_CurrPos) > PositionThreshold)
                {
                    m_CurrPos = pos;
                    SetPosition(m_CurrPos);
                }
                if (Rewinding && m_Player.EndOfVideo)
                {
                    LogNet.PrintInfo("UpdatePosition, stop rewinding");
                    m_LastFrame = true;
                    buttonRewind.IsChecked = false;
                }
            }
        }

        void SetPosition(double pos)
        {
            sliderPosition.SetPosition(pos);
            m_OscMan.SetCursorPosition(pos);
            m_CurrTimeMan.Update(pos);
        }

        void ResetPosition()
        {
            sliderPosition.SetPosition(0.0);
            m_OscMan.SetCursorPosition(0.0);
            m_CurrTimeMan.Reset();
        }

        void CheckPlayerState() // m_PlayerWatcher.m_Timer.Tick
        {
            if (!m_Player.HasSource)
            {
                return;
            }
            if (!m_Player.Error)
            {
                if (!m_Player.EndOfData)
                {
                    if (!m_Player.IsStopped)
                    {
                        if (sliderPosition.Activated)
                        {
                            UpdatePosition();
                        }
                        else if (m_Player.IsPlaying)
                        {
                            m_CurrTimeMan.Update();
                        }
                    }
                }
                else // eof
                {
                    bool stop = true;
                    if (m_Player.HasVideo)
                    {
                        if (m_Player.CanSeek)
                        {
                            if (!m_StopOnEof && !Benchmark)
                            {
                                if (m_Player.IsPlaying)
                                {
                                    LogNet.PrintInfo("CheckPlayerState, pause on eof");
                                    m_LastFrame = true;
                                    buttonPlayPause.IsChecked = false;
                                    buttonNextFrame.IsEnabled = false;
                                }
                                stop = false;
                            }
                        }
                        else // cannot seek
                        {
                            if (!m_StopNoSeek)
                            {
                                LogNet.PrintInfo("CheckPlayerState, no seek eof");
                                m_StopNoSeek = true;
                                buttonPlayPause.IsEnabled = false;
                                buttonStop.IsEnabled = false;
                                buttonPrevFrame.IsEnabled = false;
                                buttonNextFrame.IsEnabled = false;
                                buttonRewind.IsEnabled = false;
                                buttonSaveFrame.IsEnabled = true;
                            }
                            stop = false;
                        }
                    } //  HasVideo

                    if (stop && m_Player.IsPlaying)
                    {
                        LogNet.PrintInfo("CheckPlayerState, stop on eof");
                        Stop();
                    }
                } // end of
            }
            else // error
            {
                if (!m_PlayErr)
                {
                    m_PlayErr = true;
                    LogNet.PrintError("CheckPlayerState, error");
                    App.ShowMessageErr(this, ResStr1.Msg9);
                    buttonPlayPause.IsEnabled = false;
                }
            }
        }

// position

        void SetForcedPosition(double pos, bool set)
        {
            if (set)
            {
                if (CheckDelayedAction())
                {
                    m_LastFrame = false;
                    EnablePlayerGui(false);
                    m_PosTracker.Disable();
                    m_Player.SetPosition(pos);
                    LogNet.PrintInfo("SetForcedPosition, pos={0:F3}", pos);
                }
                else
                {
                    LogNet.PrintError("ForcedPosition, CheckDelayedAction");
                }
            }
            else
            {
                m_PosTracker.ShowPosition(pos);
            }
        }

        void sliderPosition_ForcedPosition(double pos, bool force) => SetForcedPosition(pos, force);

// sound volume
        void sliderSoundVolume_ValueChanged(object sender, EventArgs e) => m_Player?.SetAudioVolume(sliderSoundVolume.Position);

        void buttonMute_Checked(object sender, EventArgs e) => m_Player.SetMute(true);

        void buttonMute_Unchecked(object sender, EventArgs e) => m_Player.SetMute(false);

// info
        void EnableSourceInfo(bool enable)
        {
            panelSrcInfo.Visibility = enable ? Visibility.Visible : Visibility.Hidden;
            scrollSrcInfo.VerticalScrollBarVisibility = enable ? ScrollBarVisibility.Auto : ScrollBarVisibility.Disabled;
        }

        void SetSourceInfo(PlayItemPrms playItem)
        {
            if (playItem != null)
            {
                var mst = new MediaSourceTxt(m_Player.MediaSourceInfo, playItem.DemuxPrms.Url);
                textVideoInfo.Text = mst.Video;
                textAudioInfo.Text = mst.Audio;
                textDurInfo.Text  = mst.Duration;
                textSizeInfo.Text = mst.Size;
                EnableSourceInfo(true);
            }
            else
            {
                EnableSourceInfo(false);
                textVideoInfo.Text = string.Empty;
                textAudioInfo.Text = string.Empty;
                textDurInfo.Text  = string.Empty;
                textSizeInfo.Text = string.Empty;
            }
        }

        void srcInfoButton_Click(object sender, EventArgs e) => MediaSourceWindow.Launch(this, m_Player.GetMediaSourceHdr());
//
        PrmFilter GetFilterItem(ComboBox comboFilters, bool hasMedia)
        {
            PrmFilter ret = null;
            if (hasMedia && comboFilters.SelectedIndex > 0)
            {
                ret = (PrmFilter)comboFilters.SelectedItem;
            }
            return ret;
        }

        int GetFilterCommand(ComboBox comboFilters, bool hasMedia)
        {
            PrmFilter itm = GetFilterItem(comboFilters, hasMedia);
            return itm != null ? itm.Commands : -1;
        }

        int GetFilterCommandVideo() => GetFilterCommand(comboFiltVid, m_Player.HasVideo);

        int GetFilterCommandAudio() => GetFilterCommand(comboFiltAud, m_Player.HasAudio);

        void SendFilterCommand(bool video, string filter, string comm, string arg)
        {
            bool res = video
                ? m_Player.SendFilterCommandVideo(filter, comm, arg)
                : m_Player.SendFilterCommandAudio(filter, comm, arg);
            if (!res)
            {
                App.ShowMessageErr(this, ResStr1.Msg5);
            }
        }

        void LaunchFilterCommDlgVideo()
        {
            var itm = GetFilterItem(comboFiltVid, m_Player.HasVideo);
            if (itm != null)
            {
                switch (itm.Commands)
                {
                case 10:
                    FilterCmdWindow1.Launch(this, m_EqParams, SendFilterCommand);
                    break;
                case 20:
                    var info = m_Player.MediaSourceInfo;
                    FilterCmdWindow2.Launch(this, info.Width, info.Height, m_DrawTextParams, SendFilterCommand);
                    break;
                case 1:
                    FilterCmdWindow0.Launch(this, itm.Filter, true, SendFilterCommand);
                    break;
                }
            }
        }

        void LaunchFilterCommDlgAudio()
        {
            var itm = GetFilterItem(comboFiltAud, m_Player.HasAudio);
            if (itm != null)
            {
                int cmd = itm.Commands;
                if (cmd == 0)
                {
                    FilterCmdWindow0.Launch(this, itm.Filter, false, SendFilterCommand);
                }
            }
        }

        void buttonFilterCmdVideo_Click(object sender, EventArgs e) => LaunchFilterCommDlgVideo();

        void buttonFilterCmdAudio_Click(object sender, EventArgs e) => LaunchFilterCommDlgAudio();

// misc
        void SaveCurrentFrame()
        {
            string path = null, fileType = null;
            if (FileDlg.LaunchSaveFileDialogFrame(this, ref path, ref fileType))
            {
                int qualityLevel = (fileType == JpegFileType)
                    ? JpegQLevWindow.Launch(this, DefJpegQualityLevel)
                    : 0;
                if (qualityLevel >= 0)
                {
                    LogNet.PrintInfo("SaveCurrentFrame, fileType={0}, qualityLevel={1}, path={2}", fileType, qualityLevel, path);
                    try
                    {
                        m_Player.SaveCurrentFrame(path, fileType, qualityLevel);
                    }
                    catch (Exception exc)
                    {
                        LogNet.PrintExc(exc, "SaveCurrentFrame");
                        App.ShowMessageErr(this, ResStr1.Msg6);
                    }
                }
            }
        }

        void SrcTypeChanged()
        {
            int ind = comboSrcType.SelectedIndex;
            m_RecListCurr = (ind == 0)
                ? m_RecListFile
                : (ind == 1)
                    ? m_RecListCam
                    : IsRtpFlag ? m_RecListSdp : m_RecListUrl;
            m_AppState.IsFile = (ind == 0);
            UpdateRecentListControl(-1);
            checkBoxSrcSdp.IsEnabled = IsUrl;
            buttonBrowse.IsEnabled = !IsUrl || (bool)checkBoxSrcSdp.IsChecked;
        }

        void SdpChanged(bool check)
        {
            m_RecListCurr = check ? m_RecListSdp : m_RecListUrl;
            UpdateRecentListControl(-1);
            buttonBrowse.IsEnabled = check;
        }

        void checkBoxSrcSdp_Checked(object sender, EventArgs e) => SdpChanged(true);

        void checkBoxSrcSdp_Unchecked(object sender, EventArgs e) => SdpChanged(false);

        void buttonSaveFrame_Click(object sender, EventArgs e) => SaveCurrentFrame();

        void comboSrcType_SelectionChanged(object sender, EventArgs e) => SrcTypeChanged();

        void buttonPlayerOpts_Click(object sender, EventArgs e) => OptionsExWindow.Launch(this, ref m_StopOnEof, m_OptionsX);

    } // partial class MainWindow

} // namespace Player
