
//#define TEST_BUTTON
//#define TEST_AUDIO_ARRAY

using System;
using System.Windows;
using System.Windows.Threading;

using FFmpegNetAux;
using AppTools;
using FFmpegInterop;

namespace Explorer
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        const string DocUrl = @"https://ffmpeg.org/documentation.html";
        const string HelpFile = "Explorer.html";

        const int TimerIntervalMs = 200;

        DispatcherTimer m_Timer;

        int m_FFmpegStatus;
#if TEST_AUDIO_ARRAY
        AudioArrMuxer m_Muxer;
#endif
        public MainWindow()
        {
            InitializeComponent();

            this.Title = StartParams.GetAppTitle(App.AppName, App.IsFirstAppInst);

            m_Timer = new DispatcherTimer() { Interval = TimeSpan.FromMilliseconds(TimerIntervalMs) };
            m_Timer.Tick += delegate { TimerTick(); };

            panelButtons.IsEnabled = false;
#if !TEST_BUTTON
            panelTest.Visibility = Visibility.Collapsed;
#endif
        }

        void TimerTick()
        {
            if (m_FFmpegStatus == 0)
            {
                InitFFmpeg();
            }
#if TEST_AUDIO_ARRAY
            else if (m_FFmpegStatus > 0)
            {
                AudioArrMuxerTick();
            }
#endif
        }

        void InitFFmpeg()
        {
            m_Timer.Stop();
            if (FFmpegConfig.Init(App.StartParams, "LogParams.xml", App.LogFileNameWrap, App.IsFirstAppInst))
            {
                labelFFmpeg.Content = ResStr1.Lab1;
                m_FFmpegStatus = 1;
                panelButtons.IsEnabled = true;
#if TEST_AUDIO_ARRAY
                StartDataMuxer();
#endif
            }
            else
            {
                labelFFmpeg.Content = ResStr1.Lab2;
                m_FFmpegStatus = -1;
                App.ShowMessage(ResStr1.Lab2);
            }
        }

        static void CloseAll()
        {
            LogNet.PrintInfo("Window_Closed, begin...");
            Core.Close();
            LogNet.PrintInfo("Window_Closed, end");
        }

        void Window_Loaded(object sender, EventArgs e) => m_Timer.Start();

        void Window_Closed(object sender, EventArgs e) => CloseAll();

        void buttonVersions_Click(object sender, EventArgs e) => VersWindow.Launch(this);

        void buttonCodecs_Click(object sender, EventArgs e) => CodecWindow.Launch(this);

        void buttonFormats_Click(object sender, EventArgs e) => FormatWindow.Launch(this);

        void buttonProtocols_Click(object sender, EventArgs e) => ProtocolWindow.Launch(this);

        void buttonFilters_Click(object sender, EventArgs e) => FilterWindow.Launch(this);

        void buttonFixFmts_Click(object sender, EventArgs e) => PixFmtWindow.Launch(this);

        void buttonSampFmts_Click(object sender, EventArgs e) => SampFmtWindow.Launch(this);

        void buttonChanns_Click(object sender, EventArgs e) => ChannWindow.Launch(this);

        void buttonBsf_Click(object sender, EventArgs e) => BsfWindow.Launch(this);

        void cmdHelp_Executed(object sender, EventArgs e) => Misc.OpenDoc(App.StartParams.GetHelpFilePath(HelpFile));

        void cmdDocs_Executed(object sender, EventArgs e) => Misc.OpenDoc(DocUrl);

// --------------------------------------------------------------------
// tests

#if TEST_AUDIO_ARRAY
        void StartDataMuxer()
        {
            if (m_Muxer == null)
            {
                m_Muxer = new AudioArrMuxer();
                if (Tests.BuildAudioArrMuxer(m_Muxer))
                {
                    m_Timer.Start();
                    labelTest.Content = "***";
                }
                else
                {
                    LogNet.PrintError("DataMuxer.Build()");
                    labelTest.Content = "XXX";
                }
            }
        }

        void AudioArrMuxerTick()
        {
            if (m_Muxer != null)
            {
                bool err = m_Muxer.Error;
                if (m_Muxer.EndOfData || err)
                {
                    m_Timer.Stop();
                    m_Muxer.Close();
                    m_Muxer.Dispose();
                    m_Muxer = null;
                    labelTest.Content = err ? "Error" : "Ok";
                }
            }
        }
#endif
        void btnTest_Click(object sender, EventArgs e)
        {
            //Tests.TestCodecList();
            //Tests.TestDataMuxer();
            //StartDataMuxer();
            //Tests.TestSubtit();
            //Tests.TestProbe2();
            //Tests.TestChannList();
            
        }

    } // class MainWindow

} // namespace Explorer
