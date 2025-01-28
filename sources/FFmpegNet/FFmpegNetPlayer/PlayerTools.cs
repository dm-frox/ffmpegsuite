// ---------------------------------------------------------------------
// File: PlayerTools.cs
// Classes: PlayerTools (static)
// Purpose: helpers to save frame
// Module: FFmpegNetPlayer.dll - .NET assembly which provides media player functionality
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------


using System;
using System.IO;
using System.Windows.Media.Imaging;


namespace FFmpegNetPlayer
{

    public static class PlayerTools
    {
        static void SaveBitmap(BitmapEncoder enc, BitmapSource src, string path)
        {
            using (var str = new FileStream(path, FileMode.Create, FileAccess.Write, FileShare.None))
            {
                enc.Frames.Add(BitmapFrame.Create(src));
                enc.Save(str);
                str.Close();
            }
        }

        internal static void SaveBitmapInJpegFile(BitmapSource imageSrc, string path, int qualityLevel) // qualityLevel from 1 to 100
        {
            SaveBitmap(new JpegBitmapEncoder() { QualityLevel = qualityLevel }, imageSrc, path);
        }

        internal static void SaveBitmapInPngFile(BitmapSource imageSrc, string path)
        {
            SaveBitmap(new PngBitmapEncoder(), imageSrc, path);
        }

    } // static class PlayerTools

} // namespace FFmpegNetPlayer

