// ---------------------------------------------------------------------
// File: VideoControl.cs
// Classes: VideoControl
// Purpose: control to display video
// Module: FFmpegNetPlayer.dll - .NET assembly which provides media player functionality
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Controls;

namespace FFmpegNetPlayer
{
    public class VideoControl : Control
    {
        const RectPos DefRectPos = RectPos.Cent;
        const double  DefScale   = 1.0;

        Control       m_ScrollView;

        ImageSource   m_VideoSource;
        ImageSource   m_StopImage;
        ImageSource   m_NoVideoImage;

        RectMan       m_RectMan;
        Rect          m_FrameRendRect;

        bool          m_Stopped;

        bool          m_Pix2Pix;
        double        m_PixelsPerDip;
        double        m_ImgWidthLogPix;
        double        m_ImgHeightLogPix;

        public VideoControl()
        {
            m_ScrollView = this;
            m_RectMan = new RectMan(DefRectPos, DefScale);
            ResetFrameRect();
            this.ClipToBounds = true;
            this.Tag = null;
        }

        public void Init(ScrollViewer sv)
        {
            if (sv != null)
            {
                m_ScrollView = sv;
                m_PixelsPerDip = VisualTreeHelper.GetDpi(this).PixelsPerDip;
                SetScrollBarVisibility(sv);
            }
            this.HorizontalAlignment = HorizontalAlignment.Stretch;
            this.VerticalAlignment   = VerticalAlignment.Stretch;
            ResetSize();
        }

        public void SetRectPos(RectPos rp) => m_RectMan.SetRectPos(rp);

        public void SetScale(double scale) => m_RectMan.SetScale(scale);

        void ResetFrameRect() => m_FrameRendRect = new Rect();

        public double PixelsPerDip => m_PixelsPerDip;

        internal void SetStopImage(ImageSource stopImage) => m_StopImage = stopImage;

        internal void SetNoVideoImage(ImageSource noVideoImage) => m_NoVideoImage = noVideoImage;

        internal void Stop(bool enable)
        {
            if (enable != m_Stopped)
            {
                m_Stopped = enable;
                if (m_Pix2Pix)
                {
                    if (m_Stopped)
                    {
                        ResetSize();
                    }
                    else
                    {
                        UpdateSize();
                        ResetScrollBarPosition(m_ScrollView as ScrollViewer);
                    }
                }
                InvalidateVisual();
            }
        }

        void SetScrollBarVisibility(ScrollViewer sv)
        {
            if (sv != null)
            {
                var vis = m_Pix2Pix ? ScrollBarVisibility.Auto : ScrollBarVisibility.Disabled;
                sv.HorizontalScrollBarVisibility = vis;
                sv.VerticalScrollBarVisibility = vis;
            }
        }

        void ResetScrollBarPosition(ScrollViewer sv)
        {
            if (m_Pix2Pix)
            {
                sv?.ScrollToTop();
                sv?.ScrollToLeftEnd();
            }
        }

        public void ResetSize()
        {
            ResetFrameRect();
            this.Width = double.NaN;
            this.Height = double.NaN;
        }

        void SetPix2PixRect()
        {
            if (m_ImgWidthLogPix > 0.0 && m_ImgHeightLogPix > 0.0)
            {
                m_FrameRendRect.X = 0.0;
                m_FrameRendRect.Y = 0.0;
                m_FrameRendRect.Width = m_ImgWidthLogPix;
                m_FrameRendRect.Height = m_ImgHeightLogPix;
                this.Width = m_ImgWidthLogPix;
                this.Height = m_ImgHeightLogPix;
            }
            else
            {
                ResetSize();
            }
        }

        void UpdateSize()
        {
            if (m_Pix2Pix)
            {
                SetPix2PixRect();
            }
            else
            {
                ResetSize();
            }
        }

        public void SetPix2Pix(bool set)
        {
            if (m_PixelsPerDip > 0.0)
            {
                m_Pix2Pix = set;

                var sv = m_ScrollView as ScrollViewer;
                SetScrollBarVisibility(sv);
                ResetScrollBarPosition(sv);
                UpdateSize();
                m_RectMan.SetUpdated();
                InvalidateVisual();
            }
        }

        void SetVideoSource(ImageSource img)
        {
            if (img != null)
            {
                m_VideoSource = img;
                double w = m_VideoSource.Width, 
                       h = m_VideoSource.Height;
                m_RectMan.SetImageRatio(w, h);

                if (m_PixelsPerDip > 0.0)
                {
                    m_ImgWidthLogPix  = w / m_PixelsPerDip;
                    m_ImgHeightLogPix = h / m_PixelsPerDip;
                    if (m_Pix2Pix)
                    {
                        SetPix2PixRect();
                    }
                }
            }
        }

        internal void Reset()
        {
            m_VideoSource = null;
            this.Tag = null;
            ResetSize();
            m_ImgWidthLogPix = 0.0;
            m_ImgHeightLogPix = 0.0;
            m_StopImage = null;
            m_NoVideoImage = null;
            m_Stopped = false;
            InvalidateVisual();
        }

        Size RendSize => m_ScrollView.RenderSize;

        static Rect GetImageRect(Size rendSize, ImageSource img)
        {
            double rr = (rendSize.Width - img.Width) * 0.5;
            double x = rr >= 0.0 ? rr : 0.0;
            double w = rr >= 0.0 ? img.Width : rendSize.Width;
            rr = (rendSize.Height - img.Height) * 0.5;
            double y = rr >= 0.0 ? rr : 0.0;
            double h = rr >= 0.0 ? img.Height : rendSize.Height;
            return new Rect(x, y, w, h);
        }

        void DrawImage(DrawingContext dc, ImageSource img)
        {
            if (img != null)
            {
                dc.DrawImage(img, GetImageRect(RendSize, img));
            }
        }
 
        protected override void OnRender(DrawingContext dc)
        {
            dc.DrawRectangle(Background, null, new Rect(RendSize));

            if (m_VideoSource == null)
            {
                SetVideoSource(this.Tag as ImageSource);
            }

            if (!m_Stopped)
            {
                if (m_VideoSource != null)
                {
                    if (!m_Pix2Pix)
                    {
                        m_RectMan.GetFrameRendRect(RendSize, ref m_FrameRendRect);
                    }
                    dc.DrawImage(m_VideoSource, m_FrameRendRect);
                }
                else
                {
                    DrawImage(dc, m_NoVideoImage);
                }
            }
            else
            {
                DrawImage(dc, m_StopImage);
            }
        }

    } // class VideoControl

} // namespace FFmpegNetPlayer

