// ---------------------------------------------------------------------
// File: RectMan.cs
// Classes: enum RectPos, RectMan
// Purpose: helper to calculate display rectangle for video
// Module: FFmpegNetPlayer.dll - .NET assembly which provides media player functionality
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

using System;
using System.Windows;


namespace FFmpegNetPlayer
{
    public enum RectPos
    {
        Cent, LT, LB, RT, RB, End
    }

    class RectMan
    {
        const double Eps = 1.0 / 128.0;

        RectPos m_RectPos;
        double  m_Scale;

        double m_ImgRatio;
        double m_WndWidth;
        double m_WndHeight;

        bool   m_Updated;


        internal RectMan(RectPos rp, double scale)
        {
            m_RectPos = rp;
            m_Scale = scale;
            m_Updated = true;
        }

        internal void SetRectPos(RectPos rp)
        {
            if ((rp != m_RectPos) && (RectPos.Cent <= rp && rp < RectPos.End))
            {
                m_RectPos = rp;
                m_Updated = true;
            }
        }

        internal void SetScale(double scale)
        {
            if ((0.0 < scale && scale < 1.0) && (Math.Abs(scale - m_Scale) > Eps))
            {
                m_Scale = scale;
                m_Updated = true;
            }
        }

        internal void SetImageRatio(double w, double h)
        {
            if (w > 0.0 && h > 0.0)
            {
                m_ImgRatio = w / h;
                m_Updated = true;
            }
        }

        internal void SetUpdated()
        {
            if (!m_Updated)
            {
                m_Updated = true;
            }
        }

        void SetWindowSize(Size size)
        {
            if (Math.Abs(m_WndWidth - size.Width) > Eps || Math.Abs(m_WndHeight - size.Height) > Eps)
            {
                m_WndWidth = size.Width;
                m_WndHeight = size.Height;
                m_Updated = true;
            }
        }

        bool CheckUpdated()
        {
            bool ret = false;
            if (m_Updated)
            {
                m_Updated = false;
                ret = true;
            }
            return ret;
        }

        internal void GetFrameRendRect(Size size, ref Rect rect)
        {
            SetWindowSize(size);
            if (CheckUpdated())
            {
                CalcFrameRendRect(ref rect);
                if (m_Scale < 1.0)
                {
                    CalcScaledRendRect(ref rect);
                }
            }
        }

        void CalcFrameRendRect(ref Rect rect)
        {
            double x = 0.0,
                   y = 0.0,
                   w = 0.0,
                   h = 0.0;
            if (m_ImgRatio > 0.0 && m_WndWidth > 0.0 && m_WndHeight > 0.0)
            {
                double wndRatio = m_WndWidth / m_WndHeight;
                if (wndRatio > m_ImgRatio)  // the window is more elongated in width, reduce the image width 
                {
                    w = m_WndWidth * (m_ImgRatio / wndRatio);
                    h = m_WndHeight;
                    x = 0.0;
                    switch (m_RectPos)
                    {
                    case RectPos.Cent:
                        x = (m_WndWidth - w) * 0.5;
                        break;
                    case RectPos.RT:
                    case RectPos.RB:
                        x = (m_WndWidth - w);
                        break;
                    }
                }
                else // the window is more elongated in height, reduce the image height 
                {
                    w = m_WndWidth;
                    h = m_WndHeight * (wndRatio / m_ImgRatio);
                    y = 0.0;
                    switch (m_RectPos)
                    {
                    case RectPos.Cent:
                        y = (m_WndHeight - h) * 0.5;
                        break;
                    case RectPos.LB:
                    case RectPos.RB:
                        y = (m_WndHeight - h);
                        break;
                    }
                }
            }

            rect.X = x;
            rect.Y = y;
            rect.Width = w;
            rect.Height = h;
        }

        void CalcScaledRendRect(ref Rect rect)
        {
            double x = 0.0;
            double y = 0.0;
            double w = rect.Width * m_Scale;
            double h = rect.Height * m_Scale;
            switch (m_RectPos)
            {
            case RectPos.Cent:
                x = (m_WndWidth - w) * 0.5;
                y = (m_WndHeight - h) * 0.5;
                break;
            //case RectPos.LT: do nothing
            case RectPos.LB:
                y = m_WndHeight - h;
                break;
            case RectPos.RT:
                x = m_WndWidth - w;
                break;
            case RectPos.RB:
                x = m_WndWidth - w;
                y = m_WndHeight - h;
                break;
            }
            rect.X = x;
            rect.Y = y;
            rect.Width = w;
            rect.Height = h;
        }

    } // class RectMan

} // namespace FFmpegNetPlayer