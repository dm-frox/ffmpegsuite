using System;
using System.Windows;
using System.Windows.Controls;

using FFmpegNetAux;

namespace AppTools
{
    public class PosTracker
    {
        Label m_Label;
        int m_CurrTimeSec; // sec

        public PosTracker(Label lab)
        {
            m_Label = lab;
            Disable();
        }

        public void ShowPosition(double pos)
        {
            if (pos >= 0.0)
            {
                int timeSec = (int)Math.Round(pos);

                if (timeSec != m_CurrTimeSec)
                {
                    if (m_CurrTimeSec < 0)
                    {
                        m_Label.Content = string.Empty;
                        m_Label.Visibility = Visibility.Visible;
                    }
                    m_CurrTimeSec = timeSec;
                    m_Label.Content = ParamsTools.FormatTimeSec(m_CurrTimeSec, false);
                }
            }
            else
            {
                m_Label.Content = string.Empty;
                m_CurrTimeSec = -1;
                m_Label.Visibility = Visibility.Hidden;
            }
        }

        public void Disable()
        {
            ShowPosition(-1.0);
        }

    } // class PosTracker

    public class CurrTimeMan
    {
        const string Null = "0:00:00";

        Label    m_Label;
        DateTime m_StartTime;
        double   m_CurrTime;    // sec
        int      m_CurrTimeSec; // sec

        bool     m_Paused;
        DateTime m_BeginPauseTime;
        int      m_TotPauseTime;   // msec

        public CurrTimeMan(Label lab)
        {
            m_Label = lab;
            m_Label.Content = Null;
        }

        public void Start()
        {
            m_Label.Content = Null;
            m_StartTime = DateTime.Now;
            m_CurrTimeSec = 0;
            m_CurrTime = 0.0;
            m_Paused = false;
            m_BeginPauseTime = new DateTime(0);
            m_TotPauseTime = 0;
        }

        public void Stop()
        {
            m_Label.Content = Null;
            m_StartTime = new DateTime(0);
            m_CurrTimeSec = 0;
            m_CurrTime = 0.0;
            m_Paused = false;
            m_BeginPauseTime = new DateTime(0);
            m_TotPauseTime = 0;
        }

        public double CurrentTime => m_CurrTime;

        public string CurrentTimeStr => m_Label.Content.ToString();

        void SetTimeSec(int timeSec)
        {
            if (timeSec != m_CurrTimeSec)
            {
                m_CurrTimeSec = timeSec;
                m_Label.Content = ParamsTools.FormatTimeSec(m_CurrTimeSec, false);
            }
        }

        public void Reset()
        {
            m_CurrTime = 0.0;
            m_CurrTimeSec = 0;
            m_Label.Content = Null;
        }

        public void Update(double pos) // by position
        {
            m_CurrTime = pos;
            SetTimeSec((int)Math.Round(pos));
        }

        public void Suspend()
        {
            m_BeginPauseTime = DateTime.Now;
            m_Paused = true;
        }

        int TimeFromStartMs(DateTime startTime)
        {
            return (int)((DateTime.Now - startTime).TotalMilliseconds);
        }

        public void Resume()
        {
            m_TotPauseTime += TimeFromStartMs(m_BeginPauseTime);
            m_Paused = false;
        }

        public void Update() // by current time
        {
            if (!m_Paused)
            {
                int currTimeMs = TimeFromStartMs(m_StartTime) - m_TotPauseTime;
                m_CurrTime = currTimeMs / 1000.0;
                SetTimeSec((currTimeMs + 500) / 1000);
            }
        }

    } // CurrTimeMan

} // namespace AppTools
