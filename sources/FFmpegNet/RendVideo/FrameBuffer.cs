// ---------------------------------------------------------------------
// File: FrameBuffer.cs
// Classes: FrameBuffer
// Purpose: frame buffer for video renderer
// Module: RendVideo.dll - .NET assembly which provides video rendering
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace RendVideo
{
    class FrameBuffer
    {
        const uint StopColor  = 0xFF_00_00_00;
        const double Dpi      = 96.0;

        static readonly PixelFormat PixFmt = PixelFormats.Bgr32;
        static readonly int BytesPerPixel  = PixFmt.BitsPerPixel / 8;

        const int LockTimeoutMs = 40;
        static readonly Duration LockDuration = new Duration(TimeSpan.FromMilliseconds(LockTimeoutMs));

        WriteableBitmap m_Bitmap;
        Int32Rect       m_SourceRect;

        internal FrameBuffer() => m_SourceRect = new Int32Rect(0, 0, 0, 0);

        internal string PixelFormatStr => IsValid ? "Bgr32" : "null";

        int Width => m_SourceRect.Width;

        int Height => m_SourceRect.Height;

        bool IsValid => m_Bitmap != null;

        internal ImageSource ImageSrc => m_Bitmap;

        internal BitmapSource BitmapSrc => m_Bitmap;

        internal void Close()
        {
            m_Bitmap = null;
            m_SourceRect = new Int32Rect(0, 0, 0, 0);
        }

        internal void Init(int width, int height)
        {
            if (width <= 0 || height <= 0)
            {
                throw new ArgumentException("FrameBuffer, Init");
            }
            if (!(width == Width && height == Height)) // new width/height
            {
                m_Bitmap = new WriteableBitmap(width, height, Dpi, Dpi, PixFmt, null);
                m_SourceRect = new Int32Rect(0, 0, width, height);
            }
        }

        internal bool CheckSize(int width, int height, int stride)
        {
            return (IsValid && (width == Width) && (height == Height) && (width * BytesPerPixel <= stride));
        }

        internal void WriteFrame(IntPtr srcData, int srcStride)
        {
            m_Bitmap.WritePixels(m_SourceRect, srcData, srcStride * Height, srcStride);
        }

        unsafe void ClearFrameBuffer()
        {
            byte* dstRow = (byte*)m_Bitmap.BackBuffer.ToPointer();
            int dstStride = m_Bitmap.BackBufferStride;
            for (int i = 0, h = Height; i < h; ++i, dstRow += dstStride)
            {
                uint* dst = (uint*)dstRow;
                for (int j = 0, w = Width; j < w; ++j, ++dst)
                {
                    *dst = StopColor;
                }
            }
        }

        internal void ResetPicture()
        {
            if (IsValid)
            {
                if (m_Bitmap.TryLock(LockDuration))
                {
                    ClearFrameBuffer();
                    m_Bitmap.AddDirtyRect(m_SourceRect);
                    m_Bitmap.Unlock();
                }
            }
        }

    } // class FrameBuffer

} // namespace RendVideo
