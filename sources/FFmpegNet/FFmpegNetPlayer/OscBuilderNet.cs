// ---------------------------------------------------------------------
// File: OscBuilderNet.cs
// Classes: OscBuilderNet
// Purpose: wrapper for OscBuilder from FFmpegInterop.dll assembly
// Module: FFmpegNetPlayer.dll - .NET assembly which provides media player functionality
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

using System;

using FFmpegInterop;
using FFmpegNetAux;

namespace FFmpegNetPlayer
{
    class OscBuilderNet : IDisposable
    {

        OscBuilder    m_Buider;

        internal OscBuilderNet()
        {}

        void IDisposable.Dispose() => Close();

        internal void Close()
        {
            if (IsActive)
            {
                m_Buider.Dispose();
                m_Buider = null;
                LogNet.PrintInfo("OscBuilder.Close");
            }
        }

        internal bool EndOfSource => IsActive ? m_Buider.EndOfData : false; 

        internal bool IsActive => m_Buider != null;

        internal double Progress => IsActive ? m_Buider.Progress : 0.0;

        internal IntPtr Data => IsActive ? m_Buider.Data : IntPtr.Zero;

        internal int Length => IsActive ? m_Buider.Length : 0;

        internal double SourceDuration => m_Buider.SourceDuration;

        internal bool Build(DemuxerParams prms, int oscLen)
        {
            bool ret = false;
            try
            {
                Close();
                m_Buider = new OscBuilder();
                m_Buider.Build(prms.Url, prms.Format, prms.GetOptionString(), oscLen);
                ret = true;
                LogNet.PrintInfo("OscBuilder.Build, OK");
            }
            catch (Exception exc)
            {
                LogNet.PrintExc(exc, "OscBuilder.Build");
                Close();
            }
            return ret;
        }

    } // class OscBuilderEx

} // namespace FFmpegNetPlayer



