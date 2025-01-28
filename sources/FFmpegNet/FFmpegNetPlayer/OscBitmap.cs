// ---------------------------------------------------------------------
// File: OscBitmap.cs
// Classes: OscBitmap
// Purpose: helper for audio oscillogram vizualization
// Module: FFmpegNetPlayer.dll - .NET assembly which provides media player functionality
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

using FFmpegNetAux;

namespace FFmpegNetPlayer
{
    class OscBitmap
    {
        struct MaxMin
        {
            internal readonly float Max;
            internal readonly float Min;

            internal MaxMin(float max, float min)
            {
                Max = max;
                Min = min;
            }
        }

        const double DefDpi = 96.0;
        const int PaletteLen = 256;

        static readonly Color BkgnColor = Color.FromRgb(0x0, 0x0, 0x0);
        static readonly Color OscColor = Color.FromRgb(0x32, 0xCD, 0x32);
        static readonly Color TailColor = Color.FromRgb(0x48, 0x48, 0x48);

        static readonly PixelFormat PixFormat = PixelFormats.Indexed8;

        const byte BkgnColorInd = 0;
        const byte OscColorInd = 1;
        const byte TailColorInd = 2;

        const int LockTimeoutMs = 10;
        static readonly Duration LockDuration = new Duration(TimeSpan.FromMilliseconds(LockTimeoutMs));


        readonly double m_AmpFactor;

        int m_Half;
        int m_Last;

        BitmapSource m_Bitmap;

        internal OscBitmap(double ampFactor) => m_AmpFactor = ampFactor;

        internal void Close() => m_Bitmap = null;

        internal BitmapSource Bitmap => m_Bitmap;

        static WriteableBitmap CreateBitmap(int width, int height) =>
            new WriteableBitmap(width, height, DefDpi, DefDpi, PixFormat, BuildPalette());

        static bool BeginBuild(WriteableBitmap bitmap) => bitmap.TryLock(LockDuration);

        static BitmapSource EndBuild(WriteableBitmap bitmap, int width, int height)
        {
            bitmap.AddDirtyRect(new Int32Rect(0, 0, width, height));
            bitmap.Unlock();
            return Transform(bitmap);
        }

        static BitmapSource Transform(WriteableBitmap bitmap)
        {
            TransformGroup transGroup = new TransformGroup();
            transGroup.Children.Add(new RotateTransform(-90.0));
            return new TransformedBitmap(bitmap, transGroup);
        }

        static BitmapPalette BuildPalette()
        {
            var cols = new List<Color>(PaletteLen);
            cols.Add(BkgnColor);
            cols.Add(OscColor);
            cols.Add(TailColor);
            for (int i = 3; i < PaletteLen; ++i)
                cols.Add(OscColor);
            return new BitmapPalette(cols);
        }

        internal void Build(int oscHeight, OscBuilderNet builder)
        {
            LogNet.PrintInfo("OscHolder.Build, begin...");
            Build(oscHeight, builder.Length, builder.Data);
            LogNet.PrintInfo("OscHolder.Build, done");
        }

        int AmplToIndex(double ampl)
        {
            int ret = 0;
            double amp = m_AmpFactor * ampl;
            if (amp >= 0.0)
                ret = (m_Half + (int)(m_Half * amp));
            else
                ret = (m_Half - (int)(m_Half * (-amp)));

            return Math.Min(Math.Max(0, ret), m_Last);
        }

        unsafe void Build(int width, int height, IntPtr oscData)
        {
            WriteableBitmap bitmap = CreateBitmap(width, height);
            int w2 = width / 2;
            m_Half = w2;
            m_Last = m_Half + m_Half - 1;
            if (BeginBuild(bitmap))
            {
                MaxMin* maxMins = (MaxMin*)oscData.ToPointer();
                byte* rowBegin = (byte*)bitmap.BackBuffer.ToPointer();
                int dstStride = bitmap.BackBufferStride;
                for (int i = 0; i < height; ++i, rowBegin += dstStride)
                {
                    int maxInd = AmplToIndex(maxMins[i].Max);
                    int minInd = AmplToIndex(maxMins[i].Min);

                    byte* px = rowBegin + minInd;
                    for (int j = minInd; j <= maxInd; ++j, ++px)
                        *px = OscColorInd;
                }
                m_Bitmap = EndBuild(bitmap, width, height);
            }
        }

    } // class OscHolder

} // namespace FFmpegNetPlayer

