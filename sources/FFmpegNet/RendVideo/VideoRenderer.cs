// ---------------------------------------------------------------------
// File: VideoRenderer.cs
// Classes: VideoRenderer
// Purpose: .NET (WPF) video renderer
// Module: RendVideo.dll - .NET assembly which provides video rendering
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

using System;
using System.Threading;
using System.Windows.Threading;
using System.Windows.Controls;
using System.Windows.Media.Imaging;

using Status = System.Windows.Threading.DispatcherOperationStatus;

namespace RendVideo
{
    public class VideoRenderer
    {
        static readonly DispatcherPriority DispPrior = DispatcherPriority.Render;

        public const string PixelFormat = "bgra\0\0";

        Control     m_VideoControl;
        FrameBuffer m_FrameBuffer;
        Delegate    m_WriteFrame;

        IntPtr      m_FrameData;
        int         m_Width;
        int         m_Height;
        int         m_Stride;
        bool        m_Res;
        bool        m_PictureIsSet;

        public VideoRenderer(Control videoControl)
        {
            m_VideoControl = videoControl;
            m_FrameBuffer = new FrameBuffer();
            m_WriteFrame = new Action(WriteFrameGui);
        }

        public static unsafe bool CheckPixelFormat(IntPtr pixFmt)
        {
            bool ret = true;
            byte* pf = (byte*)pixFmt.ToPointer();
            foreach (char cc in PixelFormat)
            {
                if (cc != '\0')
                {
                    int c = *pf;
                    if (c == 0 || c != cc)
                    {
                        ret = false;
                        break;
                    }
                    ++pf;
                }
                else
                {
                    break;
                }
            }
            if (ret && *pf != 0)
            {
                ret = false;
            }
            return ret;
        }

        public BitmapSource BitmapSrc => m_FrameBuffer.BitmapSrc;

        public bool PictureIsSet => m_PictureIsSet;

        public int Width => m_Width;

        public int Height => m_Height;

        public string PixelFormatStrT => PixelFormat.TrimEnd('\0');

        public void Close()
        {
            m_FrameBuffer.Close();
            m_FrameData = IntPtr.Zero;
            m_Width = 0;
            m_Height = 0;
            m_Stride = 0;
            m_PictureIsSet = false;
            if (m_VideoControl != null)
            {
                m_VideoControl.Tag = null;
            }
        }

        public void ResetPicture()
        {
            m_FrameBuffer.ResetPicture();
            m_PictureIsSet = false;
        }

    // work thread, starts async operation for rendering and waits it completed
        public bool WriteFrame(IntPtr data, int width, int height, int stride)
        {
            bool ret = false;
            if (m_VideoControl != null)
            { 
                m_FrameData = data;
                if (width != m_Width)
                {
                    Interlocked.Exchange(ref m_Width, width);
                }
                if (height != m_Height)
                {
                    Interlocked.Exchange(ref m_Height, height);
                }
                if (stride != m_Stride)
                {
                    m_Stride = stride;
                }
                m_Res = false;
                DispatcherOperation dispOper = m_VideoControl.Dispatcher.BeginInvoke(DispPrior, m_WriteFrame);
                Status stat = dispOper.Wait(); // use timeout ?
                if (stat == Status.Completed)
                {
                    ret = m_Res;
                }
            }
            return ret;
        }

        // GUI thread, rendering itself
        void WriteFrameGui()
        {
            bool rr = m_FrameBuffer.CheckSize(m_Width, m_Height, m_Stride);
            if (!rr)
            {
                m_FrameBuffer.Init(m_Width, m_Height);
                rr = m_FrameBuffer.CheckSize(m_Width, m_Height, m_Stride);
                if (rr)
                {
                    m_VideoControl.Tag = m_FrameBuffer.ImageSrc;
                }
            }
            if (rr)
            {
                m_FrameBuffer.WriteFrame(m_FrameData, m_Stride);
                m_PictureIsSet = true;
                m_VideoControl.InvalidateVisual();
                m_Res = true;
            }
        }

    } // class RendVideo

} // namespace RendVideo
