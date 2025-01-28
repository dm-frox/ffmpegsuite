// ---------------------------------------------------------------------
// File: OscControl.cs
// Classes: OscControl, OscCursor
// Purpose: controls for audio oscillogram vizualization
// Module: FFmpegNetPlayer.dll - .NET assembly which provides media player functionality
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Controls;
using System.Globalization;


namespace FFmpegNetPlayer

{
    public class OscControl : Control
    {
        static readonly Pen NullLine = new Pen(Brushes.Magenta, 0.5);

        static readonly SolidColorBrush ProgressBrush  = new SolidColorBrush(Color.FromArgb(50, 0, 250, 0));
        static readonly SolidColorBrush ProgrBkgnBrush = new SolidColorBrush(Color.FromRgb(0, 0, 0));

        const double FontHeight = 32;
        static readonly Typeface TxtTypeface = new Typeface("Segoe UI");
        const string ProgressText = "Oscillogram...";
        const string NotApplText  = "Not applied";
        const double MinTextOrig = 4.0;

        ImageSource m_ImageSource;

        double m_OscYScale;
        double m_Progress;
        FormattedText m_FmtTextProgress;
        FormattedText m_FmtTextNotAppl;
        double m_PixelsPerDip;
        bool   m_SourceLoaded;
        bool   m_OscFlagOn;

        public OscControl()
        {
            this.ClipToBounds = true;
            m_PixelsPerDip = VisualTreeHelper.GetDpi(this).PixelsPerDip;
        }

        internal void SetSourceLoaded(bool loaded)
        {
            m_SourceLoaded = loaded;
            InvalidateVisual();
        }
        internal void SetOscFlag(bool on)
        {
            m_OscFlagOn = on;
            InvalidateVisual();
        }
        internal void SetOscYScale(double scale)
        {
            if (scale != m_OscYScale)
            {
                m_OscYScale = scale;
                InvalidateVisual();
            }
        }

        protected override void OnRender(DrawingContext dc)
        {
            Rect rendRect = new Rect(new Point(0.0, 0.0), RenderSize);
            dc.DrawRectangle(Background, null, rendRect);
            if (m_ImageSource != null)
            {
                double ymid = RenderSize.Height * 0.5;
                bool yscale = (m_OscYScale != 1.0);

                if (yscale)
                {
                    var trans = new ScaleTransform(1.0, m_OscYScale, 0.0, ymid);
                    dc.PushTransform(trans);
                }

                dc.DrawImage(m_ImageSource, rendRect);

                if (yscale)
                {
                    dc.Pop();
                }

                dc.DrawLine(NullLine, new Point(0.0, ymid), new Point(RenderSize.Width, ymid));
            }
            else if (0.0 < m_Progress && m_Progress <= 1.0)
            {
                if (m_FmtTextProgress == null)
                {
                    m_FmtTextProgress = GetFmtText(ProgressText);
                }
                dc.DrawRectangle(ProgrBkgnBrush, null, rendRect);
                dc.DrawText(m_FmtTextProgress, CalcTextPoint(m_FmtTextProgress));
                dc.DrawRectangle(ProgressBrush, null, new Rect(0.0, 0.0, m_Progress * RenderSize.Width, RenderSize.Height));
            }
            else if (m_SourceLoaded && m_OscFlagOn)
            {
                if (m_FmtTextNotAppl == null)
                {
                    m_FmtTextNotAppl = GetFmtText(NotApplText);
                }
                dc.DrawText(m_FmtTextNotAppl, CalcTextPoint(m_FmtTextNotAppl));
            }
        }

        Point CalcTextPoint(FormattedText ftxt)
        {
            double txtX = Math.Max((RenderSize.Width - ftxt.Width) * 0.5, MinTextOrig);
            double txtY = Math.Max((RenderSize.Height - ftxt.Height) * 0.5, MinTextOrig);
            return new Point(txtX, txtY);
        }

        internal void SetProgress(double progress)
        {
            if (m_ImageSource == null)
            {
                m_Progress = progress;
                InvalidateVisual();
            }
        }

        internal void SetImageSource(ImageSource imageSource)
        {
            m_ImageSource = imageSource;
            m_Progress = 0.0;
            m_FmtTextProgress = null;
            InvalidateVisual();
        }

        FormattedText GetFmtText(string text)
        {
            return new FormattedText(
                text,
                CultureInfo.InvariantCulture,
                FlowDirection.LeftToRight,
                TxtTypeface,
                FontHeight,
                Brushes.Orange, m_PixelsPerDip);
        }

    } // class OscControl

    public class OscCursor : Control
    {
        static readonly Pen PosMark = new Pen(Brushes.Red, 1.5);

        double m_Duration;
        double m_Position;

        public OscCursor()
        {}

        internal void SetDuration(double val) => m_Duration = val;

        internal void Deactivate()
        {
            m_Duration = -1.0;
            InvalidateVisual();
        }

        internal void SetPosition(double pos)
        {
            m_Position = pos;
            InvalidateVisual();
        }

        protected override void OnRender(DrawingContext dc)
        {
            Rect rendRect = new Rect(new Point(0.0, 0.0), RenderSize);
            dc.DrawRectangle(Brushes.Transparent, null, rendRect);
            if (m_Duration > 0.0 && (0.0 <= m_Position && m_Position <= m_Duration))
            {
                double x = m_Position / m_Duration * RenderSize.Width;
                dc.DrawLine(PosMark, new Point(x, 0.0), new Point(x, RenderSize.Height));
            }
        }

    } // class OscCursor

} // namespace FFmpegNetPlayer
