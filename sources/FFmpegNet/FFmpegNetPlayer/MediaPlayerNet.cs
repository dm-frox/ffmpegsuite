// ---------------------------------------------------------------------
// File: MediaPlayerNet.cs
// Classes: enum PosFlag, MediaPlayerNet
// Purpose: media player
// Module: FFmpegNetPlayer.dll - .NET assembly which provides media player functionality
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

using System;
using System.Windows.Media;

using FFmpegInterop;
using FFmpegNetAux;
using FFmpegNetMediaSource;
using RendVideo;


namespace FFmpegNetPlayer
{
    public enum PosFlag
    {
        None,
        Neutral,
        End,
        Begin,

    } // enum PosFlag

    public partial class MediaPlayerNet : IDisposable
    {
        enum State { OutOfSource, Stopped, Running, Paused };

        const double AudioVolumeThreshold = 0.05;
        const double DefAudioVolume  = 0.75;
        const double MuteAudioVolume = 0.0;

        const int MaxChannAudio      = 2;
        const int ReduceSampleFormat = 1;

        const int RunDelayIntervalMs  = 200;
        const int DelayTickIntervalMs = 40;

        static readonly char[] AudDevSepr = { '\n' };


        MediaPlayerStem m_MediaPlayerStem;
        VideoControl    m_VideoControl;
        VideoRenderer   m_VideoRenderer;
        MediaSourceHdr  m_MediaSourceHdr;


        ImageSource m_StopImage;
        ImageSource m_AudioImage;
        ImageSource m_VideoImage;
        ImageSource m_InfoImage;

        DelayedAction<bool>   m_DelayedAct;
        DelayedAction<double> m_DelayedActPos;
        DelayedAction1        m_DelayedActRun;

        Action<PosFlag> m_GuiCallback;

        bool   m_FrameStepMode;
        bool   m_Rewinding;
        bool   m_DelayedRun;

        double m_Duration;
        bool   m_Alive;
        string m_FilePath;

        State m_State;

        double m_AudioVolume;
        bool   m_Mute;

        public MediaPlayerNet(VideoControl videoControl,
            ImageSource stopImage, ImageSource audioImage, ImageSource videoImage, ImageSource infoImage,
            Action<PosFlag> guiCallback)
        {
            m_VideoControl = videoControl;
            m_StopImage    = stopImage;
            m_AudioImage   = audioImage;
            m_VideoImage   = videoImage;
            m_InfoImage    = infoImage;
            m_GuiCallback  = guiCallback;

            m_VideoRenderer = new VideoRenderer(m_VideoControl);
            m_DelayedAct    = new DelayedAction<bool>(DelayTickIntervalMs);
            m_DelayedActPos = new DelayedAction<double>(DelayTickIntervalMs);
            m_DelayedActRun = new DelayedAction1(RunDelayIntervalMs);

            m_State = State.OutOfSource;
            m_AudioVolume = DefAudioVolume;
        }

        void IDisposable.Dispose() => Unload();

// --------------------------------------------------------------------
// properties

        bool Stem => m_MediaPlayerStem != null;

        public bool HasVideo => Stem && m_MediaPlayerStem.SourceInfo.HasVideo;

        public bool HasAudio => Stem && m_MediaPlayerStem.SourceInfo.HasAudio;

        public bool HasMedia => HasVideo || HasAudio;

        public double LastVideoFramePosition => m_MediaPlayerStem.VideoRendPosition;

        public MediaSourceInfo MediaSourceInfo => m_MediaPlayerStem?.SourceInfo;

        public bool Alive => m_Alive;

        public bool CanSeek => Stem && (m_MediaPlayerStem.SourceInfo.CanSeek && m_MediaPlayerStem.SourceInfo.Duration > 0.0);

        public double Duration => m_Duration;

        public bool Error => Stem && m_MediaPlayerStem.PumpError;

        public bool EndOfData => Stem && m_MediaPlayerStem.EndOfData;

        public bool EndOfVideo => Stem && m_MediaPlayerStem.EndOfVideo;

        public bool HasSource => Stem && m_State != State.OutOfSource;

        public bool IsStopped => m_State == State.Stopped;

        public bool IsPlaying => m_State == State.Running;

        public bool IsPaused => m_State == State.Paused;

        public MediaSourceHdr GetMediaSourceHdr()
        {
            if (m_MediaSourceHdr == null && Stem)
            {
                m_MediaSourceHdr = new MediaSourceHdr(m_MediaPlayerStem, m_FilePath);
            }
            return m_MediaSourceHdr;
        }

// --------------------------------------------------------------------
// building

        public bool Build(PlayItemPrms prms)
        {
            return Build(prms.DemuxPrms,
                prms.DecPrmsVideo, prms.FramePrmsVideo,
                prms.DecPrmsAudio, prms.FramePrmsAudio,
                prms.ModeX, prms.OptionsX);
        }

        bool Build(DemuxerParams readerPrms,
            string decPrmsVideo, FrameParamsVideo vidPrms, 
            string decPrmsAudio, FrameParamsAudio audPrms,
            ModeX modex, OptionsX optx)
        {
            bool ret = false;
            try
            {
                BuildImpl(readerPrms,
                   decPrmsVideo, vidPrms, 
                   decPrmsAudio, audPrms,
                   modex, optx);
                SetAudioVolume();
                Play2();
                ret = true;
            }
            catch (Exception exc)
            {
                LogNet.PrintExc(exc, "MediaPlayerNet, Build");
                Close();
            }
            return ret;
        }

        void BuildImpl(DemuxerParams readerPrms, 
            string decPrmsVideo, FrameParamsVideo vidPrms, 
            string decPrmsAudio, FrameParamsAudio audPrms,
            ModeX modex, OptionsX optx)
        {
            LogNet.SkipLine();
            LogNet.PrintInfo("MediaPlayerNet, BuildImpl, V/A={0}/{1}, url={2}",
                readerPrms.UseVideo, readerPrms.UseAudio, readerPrms.Url);

            if (HasSource)
                throw new Exception("MediaPlayerNet, BuildImpl, bad state");

            m_Alive = modex.Alive;

            bool forceConvSeq = readerPrms.UseVideo && vidPrms != null && vidPrms.NeedSeq;

            m_MediaPlayerStem = new MediaPlayerStem(optx.UseFilterGraphAlways, optx.ConvFrameSequentially || forceConvSeq); 

            if (readerPrms.UseVideo)
            {
                if (vidPrms != null)
                {
                    m_MediaPlayerStem.SetFrameParamsVideo(vidPrms.Width, vidPrms.Height, VideoRenderer.PixelFormat, vidPrms.FilterStr, vidPrms.FpsFactor);
                }
                else
                {
                    m_MediaPlayerStem.SetFrameParamsVideo(0, 0, VideoRenderer.PixelFormat, null, 0);
                }
            }
            if (readerPrms.UseAudio && audPrms != null)
            {
                m_MediaPlayerStem.SetRendModeAudio(MaxChannAudio, ReduceSampleFormat);
                m_MediaPlayerStem.SetFrameParamsAudio(audPrms.Chann, audPrms.SampleRate, string.Empty, audPrms.ChannLayout, audPrms.FilterStr);
            }

            m_MediaPlayerStem.Build(
                readerPrms.Url, readerPrms.Format, readerPrms.GetOptionString(),
                readerPrms.UseVideo, decPrmsVideo, readerPrms.UseAudio, decPrmsAudio);

            IntPtr rendVideo = (m_MediaPlayerStem.SourceInfo.HasVideo && !modex.SkipVideoRendering)
                ? RendVideoInterop.FrameSinkVideoNet.CreateInstanse(m_VideoRenderer)
                : IntPtr.Zero;

            IntPtr rendAudio = m_MediaPlayerStem.SourceInfo.HasAudio
                ? AudioRendInterop.FrameSinkAudioNet.CreateInstanse()
                : IntPtr.Zero;

            m_MediaPlayerStem.SetupRendering(
                rendVideo, modex.HoldPresentationTime, optx.SyncByAudio, optx.SwapFields,
                rendAudio, optx.AudioDevId);

            SetAuxImages(modex.SkipVideoRendering);

            m_Duration = !m_Alive ? m_MediaPlayerStem.SourceInfo.Duration : 0.0;
            m_FilePath = !m_Alive ? readerPrms.Url : null;
            m_Rewinding = false;
            m_FrameStepMode = false;
            m_DelayedRun = false;
            m_MediaSourceHdr = null;
            m_State = State.Stopped;

            LogNet.PrintInfo("MediaPlayerNet, BuildImpl, OK, source: {0}; duration={1}; max dur={2}",
                FormatSourceParams(),
                ParamsTools.FormatPosition(m_Duration, false),
                (m_MediaPlayerStem.HasAudio && optx.SyncByAudio)
                    ? ParamsTools.FormatPosition((double)uint.MaxValue / m_MediaPlayerStem.SampleRate, false)
                    : "N/A");
        }

        void SetAuxImages(bool skipRend)
        {
            if (m_VideoControl != null)
            {
                m_VideoControl.SetStopImage(m_StopImage);
                var img = !m_MediaPlayerStem.SourceInfo.HasVideo
                    ? (m_MediaPlayerStem.SourceInfo.HasAudio
                        ? m_AudioImage
                        : m_InfoImage)
                    : (skipRend
                        ? m_VideoImage
                        : null);
                if (img != null)
                {
                    m_VideoControl.SetNoVideoImage(img);
                    m_VideoControl.InvalidateVisual();
                }
            }
        }

        string FormatSourceParams()
        {
            string vid = m_MediaPlayerStem.HasVideo
                        ? $"{m_MediaPlayerStem.Width}x{m_MediaPlayerStem.Height}, {m_MediaPlayerStem.PixFmtStr}"
                        : "No";
            string aud = m_MediaPlayerStem.HasAudio
                        ? $"{m_MediaPlayerStem.Chann}-{m_MediaPlayerStem.SampleRate}, {m_MediaPlayerStem.SampFmtStr}"
                        : "No";
            return $"Video: {vid}; Audio: {aud}";
        }

        public string GetRendParams()
        {
            string ret = null;
            if (HasSource)
            {
                try
                {
                    string vid = null;
                    if (m_MediaPlayerStem.HasVideo)
                    {
                        int w = m_VideoRenderer.Width;
                        int h = m_VideoRenderer.Height;
                        string f = m_VideoRenderer.PixelFormatStrT;
                        if (w > 0 && h > 0 && f != null)
                        {
                            vid = $"{w}x{h}, {f}";
                        }
                        else
                        {
                            vid = "0x0";
                        }
                    }
                    else
                    {
                        vid = "No stream";
                    }

                    string aud = null;
                    if (m_MediaPlayerStem.HasAudio)
                    {
                        int c = m_MediaPlayerStem.RendChann;
                        int r = m_MediaPlayerStem.RendSampleRate;
                        string f = m_MediaPlayerStem.SampFmtStr;
                        if (c > 0 && r > 0 && !string.IsNullOrEmpty(f))
                        {
                            aud = $"{c}-{r}, {f}";
                        }
                        else
                        {
                            aud = "0-0";
                        }
                    }
                    else
                    {
                        aud = "No stream";
                    }

                    if (vid != null && aud != null)
                    {
                        ret = $"Video: {vid}; Audio: {aud}";
                    }
                }
                catch (Exception exc)
                {
                    LogNet.PrintExc(exc, "MediaPlayerNet, GetRendParams");
                }
            }
            return ret;
        }

        void ToggleMute(bool mute)
        {
            if (mute)
            {
                m_MediaPlayerStem.SetAudioVolume(MuteAudioVolume);
            }
            else
            {
                SetAudioVolume();
            }
        }

        void EnabelFrameStepMode(bool enable)
        {
            if (HasAudio && (enable != m_FrameStepMode))
            {
                m_FrameStepMode = enable;
                ToggleMute(m_FrameStepMode);
            }
        }

        void EnabelRewinding(bool enable)
        {
            if (HasAudio && (enable != m_Rewinding))
            {
                m_Rewinding = enable;
                ToggleMute(m_Rewinding);
            }
        }

        void Close()
        {
            if (Stem)
            {
                m_MediaPlayerStem.Dispose();
                m_MediaPlayerStem = null;
                m_VideoControl?.Reset();
                m_VideoRenderer.Close();

                m_Alive = false;
                m_Duration = 0.0;
                m_FilePath = null;
                m_Rewinding = false;
                m_FrameStepMode = false;
                m_DelayedRun = false;
                m_MediaSourceHdr = null;
                m_State = State.OutOfSource;
            }
        }

        void NotifyGui(PosFlag posFlags) => m_GuiCallback?.Invoke(posFlags);

        public bool DelayedActionInProgress()
        {
            return Stem && (m_DelayedAct.IsEnabled || m_DelayedActPos.IsEnabled || m_DelayedActRun.IsEnabled);
        }

// --------------------------------------------------------------------
// play/pause

        bool Proceeding() => m_MediaPlayerStem.Proceeding;

        public void PlayPause() // from GUI
        {
            if (HasSource)
            {
                if (IsPlaying)
                {
                    Pause();
                }
                else if (IsPaused)
                {
                    Play();
                }
                else if (IsStopped)
                {
                    Play2();
                }
            }
        }

        void Pause()
        {
            m_MediaPlayerStem.Pause();
            m_DelayedAct.Begin(true, Proceeding, EndPause);
        }

        void EndPause(bool prm)
        {
            m_State = State.Paused;
            NotifyGui(PosFlag.None);
        }

        void NotifyRun()
        {
            m_DelayedActRun.Begin(()=>NotifyGui(PosFlag.None));
        }

        void Run()
        {
            LogNet.PrintInfo("MediaPlayerNet, Run");
            m_MediaPlayerStem.Run();
            m_State = State.Running;
            NotifyRun();
        }

        void Play()
        {
            if (!m_FrameStepMode)
            {
                Run();
            }
            else
            {
                LogNet.PrintInfo("MediaPlayerNet, PlayPause, DelayedRun");
                m_DelayedRun = true;
                ShiftVideoPosition(0);
            }
        }

        void Play2()
        {
            m_VideoControl?.Stop(false);
            Run();
        }

// --------------------------------------------------------------------
// stop/unload

        public void Stop() => BeginStop(false); // from GUI

        public void Unload() => BeginStop(true); // from GUI

        void BeginStop(bool unload)
        {
            if (HasSource)
            {
                if (IsPlaying)
                {
                    m_MediaPlayerStem.Pause();
                }
                m_DelayedAct.Begin(unload, Proceeding, EndStop);
            }
        }

        void EndStop(bool unload)
        {
            m_MediaPlayerStem.Stop();
            m_VideoRenderer.ResetPicture();
            m_VideoControl?.Stop(true);
            EnabelFrameStepMode(false);

            if (unload)
            {
                Close();
            }
            else
            {
                m_State = State.Stopped;
            }

            NotifyGui(PosFlag.None);
        }

// --------------------------------------------------------------------
// rewinder

        public void StartRewinder(int fps)
        {
            if (HasVideo)
            {
                EnabelRewinding(true);
                m_MediaPlayerStem.StartRewinder(fps);
                m_MediaPlayerStem.Run();
                NotifyRun();
            }
        }

        public void StopRewinder(bool unloading)
        {
            if (HasVideo)
            {
                m_MediaPlayerStem.Pause();
                m_DelayedAct.Begin(unloading, Proceeding, EndStopRewinder);
            }
        }

        void EndStopRewinder(bool unloading)
        {
            m_MediaPlayerStem.StopRewinder();
            if (!unloading)
            {
                EnabelRewinding(false);
                ShiftVideoPosition(0);
            }
            else
            {
                m_Rewinding = false;
            }
        }

// --------------------------------------------------------------------
// position

        public void SetPosition(double pos) // from GUI
        {
            EnabelFrameStepMode(false);
            BeginSetPosition(pos);
        }

        void BeginSetPosition(double pos)
        {
            if (HasSource && CanSeek)
            {
                if (IsPlaying)
                {
                    m_MediaPlayerStem.Pause();
                }
                m_DelayedActPos.Begin(pos, Proceeding, СontinueSetPosition);
            }
        }

        bool SeekingInProgress() => m_MediaPlayerStem.SeekingInProgress;

        void BeginSeeking(bool paused) => m_DelayedAct.Begin(paused, SeekingInProgress, EndSetPosition);

        void СontinueSetPosition(double pos)
        {
            bool paused = IsPaused;
            m_MediaPlayerStem.SetPosition(pos, paused);
            m_MediaPlayerStem.Run();
            BeginSeeking(paused);
        }

        void EndSetPosition(bool paused)
        {
            if (paused)
            {
                m_MediaPlayerStem.Pause();
            }
            if (m_DelayedRun)
            {
                m_DelayedRun = false;
                Run();
            }
            NotifyGui(m_MediaPlayerStem.EndOfVideo
                ? PosFlag.End 
                : ((m_MediaPlayerStem.VideoRendPosition == 0.0)
                    ? PosFlag.Begin
                    : PosFlag.Neutral));
        }

        public void GoToNextFrame() // from GUI
        {
            ShiftVideoPositionFwd(1);
        }

        void ShiftVideoPositionFwd(int k)
        {
            if ((k > 0) && HasVideo && IsPaused)
            {
                EnabelFrameStepMode(true);
                if (m_MediaPlayerStem.ShiftVideoPositionFwd(k))
                {
                    m_MediaPlayerStem.Run();
                    BeginSeeking(true);
                }
                else
                {
                    NotifyGui(PosFlag.End);
                }
            }
        }

        public void GoToPrevFrame() // from GUI
        {
            if (m_MediaPlayerStem.VideoRendPosition > 0.0)
            {
                ShiftVideoPosition(-1);
            }
            else
            {
                NotifyGui(PosFlag.Begin);
            }
        }

        void ShiftVideoPosition(int frameCount)
        {
            if (HasVideo && IsPaused)
            {
                double newPos = m_MediaPlayerStem.GetShiftedVideoPosition(frameCount);
                if (newPos >= 0.0)
                {
                    SetPosition(newPos);
                }
                else
                {
                    LogNet.PrintError("MediaPlayerNet, ShiftVideoPosition, count={0}", frameCount);
                }
            }
        }

        public double GetPosition() // from GUI
        {
            double ret = 0.0;
            if (HasSource)
            {
                ret = m_DelayedActPos.IsEnabled
                    ? m_DelayedActPos.Param
                    : m_MediaPlayerStem.Position;
            }
            return ret;
        }

// --------------------------------------------------------------------
// audio volume, devices

        void SetAudioVolume() => m_MediaPlayerStem?.SetAudioVolume(!m_Mute ? m_AudioVolume : MuteAudioVolume);

        void UpdateAudioVolume()
        {
            if (!m_FrameStepMode && !m_Rewinding)
            {
                SetAudioVolume();
            }
        }

        public void SetAudioVolume(double vol) // Gui
        {
            if (Math.Abs(m_AudioVolume - vol) >= AudioVolumeThreshold || vol < AudioVolumeThreshold)
            {
                m_AudioVolume = vol;
                UpdateAudioVolume();
            }
        }

        public void SetMute(bool mute) // Gui
        {
            if (mute != m_Mute)
            {
                m_Mute = mute;
                UpdateAudioVolume();
            }
        }

        public static string[] GetAudioDeviceList()
        {
            string devList = AudioRendInterop.FrameSinkAudioNet.GetWaveDeviceList();
            return devList.Split(AudDevSepr, StringSplitOptions.RemoveEmptyEntries);
        }

// --------------------------------------------------------------------
// filter control

        public bool SendFilterCommandVideo(string tar, string cmd, string arg) =>
            Stem && m_MediaPlayerStem.SendFilterCommandVideo(tar, cmd, arg);
        public bool SendFilterCommandAudio(string tar, string cmd, string arg) =>
            Stem && m_MediaPlayerStem.SendFilterCommandAudio(tar, cmd, arg);

        public void ReconfigureFilterGraphVideo(string filterString) =>
            m_MediaPlayerStem?.ReconfigureFilterGraphVideo(filterString);

        public void ReconfigureFilterGraphAudio(string filterString) =>
            m_MediaPlayerStem?.ReconfigureFilterGraphAudio(filterString);

// --------------------------------------------------------------------

        public void SaveCurrentFrame(string path, string fileType, int qualityLevel)
        {
            if (m_VideoRenderer.PictureIsSet)
            {
                var src = m_VideoRenderer.BitmapSrc;
                switch (fileType)
                {
                case "jpg":
                    PlayerTools.SaveBitmapInJpegFile(src, path, qualityLevel);
                    break;
                case "png":
                    PlayerTools.SaveBitmapInPngFile(src, path);
                    break;
                default:
                    throw new Exception("MediaPlayerNet, SaveCurrentFrame, bad file type");
                }
            }
            else
            {
                throw new Exception("MediaPlayerNet, SaveCurrentFrame, no picture");
            }
        }

    } //  class MediaPlayerNet

} // FFmpegNetPlayer
