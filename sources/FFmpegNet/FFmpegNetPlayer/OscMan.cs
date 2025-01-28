// ---------------------------------------------------------------------
// File: OscMan.cs
// Classes: OscMan
// Purpose: interaction between all audio oscillogram related objects
// Module: FFmpegNetPlayer.dll - .NET assembly which provides media player functionality
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

using System;
using System.Windows.Threading;

using FFmpegNetAux;

namespace FFmpegNetPlayer
{
    public class OscMan
    {
        const double IniProgress = 0.001;

        readonly int m_OscWidth;
        readonly int m_OscHeight;
        readonly int m_TimerIntervalMs;

        DispatcherTimer m_TimerOsc;
        OscBuilderNet   m_OscBuilder;
        OscBitmap       m_OscBitmap;
        DemuxerParams   m_OscUrlParams;

        OscControl      m_OscControl;
        OscCursor       m_OscCursor;

        public OscMan(OscControl oscControl, OscCursor oscCursor, int width, int height, double ampFactor, int timerIntervalMs)
        {
            m_OscWidth = width;
            m_OscHeight = height;
            m_TimerIntervalMs = timerIntervalMs;

            m_OscBuilder = new OscBuilderNet();
            m_OscBitmap  = new OscBitmap(ampFactor);

            m_TimerOsc = new DispatcherTimer();
            m_TimerOsc.Interval = TimeSpan.FromMilliseconds(m_TimerIntervalMs);
            m_TimerOsc.Tick += delegate { CheckOscState(); };

            m_OscControl = oscControl;
            m_OscCursor  = oscCursor;
        }

        public void SetCursorPosition(double pos) => m_OscCursor.SetPosition(pos);

        public void SetOscYScale(double scale) => m_OscControl.SetOscYScale(scale);

        public void SetParams(DemuxerParams prms)
        {
            m_OscUrlParams = prms;
            m_OscControl.SetSourceLoaded(m_OscUrlParams != null);
        }

        void CheckOscState()
        {
            if (m_OscBuilder.EndOfSource)
            {
                m_TimerOsc.IsEnabled = false;
                m_OscBitmap.Build(m_OscHeight, m_OscBuilder);
                m_OscControl.SetImageSource(m_OscBitmap.Bitmap);
                m_OscBuilder.Close();
            }
            else
            {
                var progr = m_OscBuilder.Progress;
                m_OscControl.SetProgress(progr);
            }
        }

        public bool Start()
        {
            bool ret = false;
            if (m_OscUrlParams != null)
            {
                if (m_OscBuilder.Build(m_OscUrlParams, m_OscWidth))
                {
                    m_TimerOsc.IsEnabled = true;
                    m_OscControl.SetProgress(IniProgress);
                    m_OscControl.SetOscFlag(true);
                    m_OscCursor.SetDuration(m_OscBuilder.SourceDuration);
                    ret = true;
                }
            }
            return ret;
        }

        public void Close(bool unloaded)
        {
            m_TimerOsc.IsEnabled = false;
            m_OscBuilder.Close();
            m_OscBitmap.Close();
            m_OscControl.SetImageSource(null);
            m_OscControl.SetOscFlag(false);
            m_OscCursor.Deactivate();
            if (unloaded)
            {
                SetParams(null);
            }
        }

    } // class OscMan

} // namespace FFmpegNetPlayer
