// ---------------------------------------------------------------------
// File: PositionControl.cs
// Classes: PlayerSliderPos, PlayerSliderSndVol
// Purpose: Sliders to control play position and sound volume
// Module: FFmpegNetPlayer.dll - .NET assembly which provides media player functionality
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

using System;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;

using FFmpegNetAux;

namespace FFmpegNetPlayer
{
    public class PlayerSliderPos : RangeBase
    {
        const double UpdThresholdDur = 0.002; // 2 ms 

        const double DefThumbCornRad = -1.0;
        const double ThumbBorderThickness     = 0.5;
        const double ThumbBorderThicknessOver = 1.5;
        const double DefSlotRelHeight = 0.3;
        const double DefThumbRelWidth = 1.0; // square

        const int ToolTipShowDuration = 2000;

        readonly static Brush DefBackground   = Brushes.Gray;
        readonly static Brush DefSlotBrush    = Brushes.DimGray;
        readonly static Brush DefCurrPosBrush = Brushes.LightSkyBlue;

        readonly static Brush DefThumbBrush = new SolidColorBrush(Color.FromArgb(80, 0, 192, 255));
        readonly static Brush DefThumbBrushOver = Brushes.LightCyan;
        readonly static Brush DefThumbBrushDrag = Brushes.Gold;

        readonly static Brush DefThumbBorderBrush = Brushes.Cyan;

        readonly static Pen NullPen = new Pen();

        Brush m_CurrPosBrush;
        Brush m_SlotBrush;

        double m_SlotRelHeight;

        Brush m_ThumbBrush;
        Brush m_ThumbBrushOver;
        Brush m_ThumbBrushDrag;

        Pen m_ThumbPen;
        Pen m_ThumbPenOver;

        double m_RelThumbWidth;
        double m_ThumbCornRad;

        bool   m_ThumbIsVisible;

        double m_Duration;
        double m_DurToValueFactor;
        double m_UpdThresholdVal;

        double m_DragVal;
        bool   m_SyncValAndDragVal;

        bool   m_Over;
        bool   m_OverThumb;

        double m_MouseX;

        public event Action<double, bool> ForcedPosition;

        protected PlayerSliderPos(bool customToolTip)
        {
            m_CurrPosBrush = DefCurrPosBrush;
            m_SlotBrush    = DefSlotBrush;

            m_RelThumbWidth = DefThumbRelWidth;
            m_ThumbCornRad  = DefThumbCornRad;

            m_ThumbBrush = DefThumbBrush;
            m_ThumbBrushOver = DefThumbBrushOver;
            m_ThumbBrushDrag = DefThumbBrushDrag;

            SetThumbBorder(DefThumbBorderBrush);

            m_SlotRelHeight = DefSlotRelHeight;

            base.Minimum = 0.0;
            base.Maximum = 1.0;
            base.SmallChange = 0.05;
            base.LargeChange = 0.25;
            base.Value = 0.0;
            base.Background = DefBackground;
            base.Focusable = false;

            if (customToolTip)
            {
                base.ToolTip = " ";
                base.AddHandler(ToolTipService.ToolTipOpeningEvent, new RoutedEventHandler(ToolTip_Opening));
                base.SetValue(ToolTipService.ShowDurationProperty, ToolTipShowDuration);
            }
        }

        public PlayerSliderPos()
            : this(true)
        {}

        void ToolTip_Opening(object sender, RoutedEventArgs e)
        {
            if (m_ThumbIsVisible)
            {
                double pos = PositionFromValue(ValueFromMouseX(m_MouseX));
                base.ToolTip = ParamsTools.FormatPosition(pos, true);
            }
        }

        void SetThumbBorder(Brush brush)
        {
            m_ThumbPen = new Pen(brush, ThumbBorderThickness);
            m_ThumbPenOver = new Pen(brush, ThumbBorderThicknessOver);
        }

        public double SlotRelHeight { get => m_SlotRelHeight; set => m_SlotRelHeight = value; }

        public double ThumbCornRad { get => m_ThumbCornRad; set => m_ThumbCornRad = value; }

        public double ThumbRelWidth { get => m_RelThumbWidth; set => m_RelThumbWidth = value; }

        public Brush ThumbBrush { get => m_ThumbBrush; set => m_ThumbBrush = value; }

        public Brush ThumbBrushOver { get => m_ThumbBrushOver; set => m_ThumbBrushOver = value; }

        public Brush ThumbBrushDrag { get => m_ThumbBrushDrag; set => m_ThumbBrushDrag = value; }

        public Brush CurrentPosBrush { get => m_CurrPosBrush; set => m_CurrPosBrush = value; }

        public Brush SlotBrush { get => m_SlotBrush; set => m_SlotBrush = value; }

        double Normalize(double val) => Math.Min(Math.Max(val, 0.0), 1.0);

        public void SetPosition(double pos)
        {
            if (Activated)
            {
                double newValue = ValueFromPosition(pos);
                if (Math.Abs(base.Value - newValue) > m_UpdThresholdVal)
                {
                    base.Value = newValue;
                    if (m_ThumbIsVisible)
                    {
                        CheckOverThumb(m_MouseX);
                    }
                    base.InvalidateVisual();
                }
            }
        }

        double ValueFromPosition(double pos) => pos * m_DurToValueFactor;

        double PositionFromValue(double val) => val * m_Duration;

        public double Position => PositionFromValue(base.Value);

        public void Activate(double dur)
        {
            if (dur > 0.0)
            {
                m_Duration = dur;
                m_DurToValueFactor = 1.0 / m_Duration;
                m_UpdThresholdVal = ValueFromPosition(UpdThresholdDur);
            }
            else
            {
                m_Duration = 0.0;
                m_DurToValueFactor = 0.0;
                m_UpdThresholdVal = 0.0;
            }
            m_ThumbIsVisible = false;
            base.Value = 0.0;
            base.IsEnabled = false;
            base.InvalidateVisual();
        }

        public void Deactivate() => Activate(0.0);

        public bool Activated => m_Duration > 0.0;

        public void ShowThumb(bool show)
        {
            if (Activated && (show != m_ThumbIsVisible))
            {
                m_ThumbIsVisible = show;
                base.IsEnabled = m_ThumbIsVisible;
                base.InvalidateVisual();
            }
        }

        protected void SyncValAndDragVal()
        {
            m_SyncValAndDragVal = true;
        }

        public bool ThumbIsVisible => m_ThumbIsVisible;

        double ThumbHeight => base.ActualHeight;

        double ThumbWidth => ThumbHeight * m_RelThumbWidth;

        double SlotWidth => base.ActualWidth - ThumbWidth;

        double ThumbWidthHalf => ThumbWidth * 0.5;

        double SlotX => ThumbWidthHalf;

        double ValueFromMouseX(double mouseX)
        {
            double slotWidth = SlotWidth;
            double val = (slotWidth > 0.0) ? (mouseX - SlotX) / slotWidth : 0.0;
            return Normalize(val);
        }

        double MouseXFromValue(double val) => SlotX + val * SlotWidth;

        double MouseX(MouseEventArgs e) => e.GetPosition(this).X;

        bool CheckOverThumb(double mouseX)
        {
            bool ret = false;
            if (m_Over)
            {
                bool overThumb = Math.Abs(mouseX - MouseXFromValue(base.Value)) < ThumbWidthHalf;
                if (overThumb != m_OverThumb)
                {
                    m_OverThumb = overThumb;
                    ret = true;
                }
            }
            return ret;
        }

        protected override void OnMouseEnter(MouseEventArgs e)
        {
            base.OnMouseEnter(e);

            if (m_ThumbIsVisible)
            {
                double mouseX = MouseX(e);
                m_MouseX = mouseX;
                m_Over = true;
                CheckOverThumb(mouseX);
                base.InvalidateVisual();
            }
        }

        protected override void OnMouseLeave(MouseEventArgs e)
        {
            base.OnMouseLeave(e);

            if (m_ThumbIsVisible)
            {
                m_MouseX = 0.0;
                m_Over = false;
                m_OverThumb = false;
                base.InvalidateVisual();
            }
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            base.OnMouseMove(e);

            if (m_ThumbIsVisible)
            {
                double mouseX = MouseX(e);
                m_MouseX = mouseX;
                if (base.IsMouseCaptured)
                {
                    m_DragVal = ValueFromMouseX(mouseX);
                    if (m_SyncValAndDragVal)
                    {
                        base.Value = m_DragVal;
                    }
                    base.InvalidateVisual();

                    ForcedPosition?.Invoke(PositionFromValue(m_DragVal), false);
                }
                else if (CheckOverThumb(mouseX))
                {
                    base.InvalidateVisual();
                }
            }
        }

        protected override void OnMouseLeftButtonDown(MouseButtonEventArgs e)
        {
            base.OnMouseLeftButtonDown(e);

            if (m_ThumbIsVisible)
            {
                double mouseX = MouseX(e);
                m_MouseX = mouseX;
                CheckOverThumb(mouseX);
                if (m_OverThumb)
                {
                    base.CaptureMouse();
                    m_DragVal = ValueFromMouseX(mouseX);
                    if (m_SyncValAndDragVal)
                    {
                        base.Value = m_DragVal;
                    }
                    base.InvalidateVisual();

                    ForcedPosition?.Invoke(PositionFromValue(m_DragVal), false);
                }
            }
        }

        protected override void OnMouseLeftButtonUp(MouseButtonEventArgs e)
        {
            base.OnMouseLeftButtonUp(e);

            if (m_ThumbIsVisible)
            {
                if (m_Over)
                {
                    double mouseX = MouseX(e);
                    m_MouseX = mouseX;
                    base.Value = ValueFromMouseX(mouseX);
                    if (base.IsMouseCaptured)
                    {
                        base.ReleaseMouseCapture();
                        m_DragVal = 0.0;
                    }
                    base.InvalidateVisual();

                    ForcedPosition?.Invoke(Position, true);
                }
            }
        }

        void DrawThumb(DrawingContext dc, double thumbX)
        {
            if (m_ThumbIsVisible)
            {
                var rect = new Rect(thumbX, 0.0, ThumbWidth, ThumbHeight);
                Brush br = base.IsMouseCaptured
                    ? m_ThumbBrushDrag
                    : (m_OverThumb ? m_ThumbBrushOver : m_ThumbBrush);
                Pen pn = base.IsMouseCaptured
                    ? m_ThumbPen
                    : (m_Over && !m_OverThumb) ? m_ThumbPenOver : m_ThumbPen;

                double cornRad = m_ThumbCornRad >= 0.0 ? m_ThumbCornRad : ThumbWidthHalf;
                dc.DrawRoundedRectangle(br, pn, rect, cornRad, cornRad);
            }
        }

        Rect GetSlotRect()
        {
            double h = base.ActualHeight * m_SlotRelHeight;
            double y = (base.ActualHeight - h) * 0.5;

            return new Rect(SlotX, y, SlotWidth, h);
        }

        Rect FullRect => new Rect(0.0, 0.0, base.ActualWidth, base.ActualHeight);

        protected override void OnRender(DrawingContext dc)
        {
            dc.DrawRectangle(base.Background, NullPen, FullRect);

            if (Activated)
            {
                var slotRect = GetSlotRect();
                dc.DrawRectangle(m_SlotBrush, NullPen, slotRect);

                double currWidth = base.Value * slotRect.Width;
                double thumbX = !base.IsMouseCaptured ? currWidth : m_DragVal * slotRect.Width;
                DrawThumb(dc, thumbX);
                slotRect.Width = currWidth;
                dc.DrawRectangle(m_CurrPosBrush, NullPen, slotRect);
            }
        }

    } // class PlayerSliderPos

    public class PlayerSliderSndVol : PlayerSliderPos
    {
        public PlayerSliderSndVol()
            : base(false)
        {
            Activate(1.0);
            base.Value = 0.75;
            ShowThumb(true);
            SyncValAndDragVal();
        }

        public void ChangeVolume(bool up)
        {
            double step = base.SmallChange;
            base.Value = base.Value + (up ? step : -step);
            base.InvalidateVisual();
        }

    } // class PlayerSliderSndVol

} // namespace FFmpegNetPlayer
