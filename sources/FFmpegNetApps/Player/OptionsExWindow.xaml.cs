using System;
using System.Windows;

using FFmpegNetAux;
using FFmpegNetPlayer;

namespace Player
{
    /// <summary>
    /// Interaction logic for PlayerOptionsWindow.xaml
    /// </summary>
    public partial class OptionsExWindow : Window
    {
        static string[] AudioDevs;

        public OptionsExWindow()
        {
            InitializeComponent();
        }

        void SetupAudioDevs(int audioDevInd)
        {
            if (AudioDevs == null)
            {
                AudioDevs = MediaPlayerNet.GetAudioDeviceList();
                LogNet.PrintInfo("Audio devices:");
                foreach (var s in AudioDevs)
                {
                    LogNet.PrintInfo(" -- {0}", s);
                }
            }

            comboAudioDevs.Items.Add("Default");
            foreach (var s in AudioDevs)
            {
                comboAudioDevs.Items.Add(s);
            }
            comboAudioDevs.SelectedIndex = audioDevInd;
        }

        void Init(bool stopOnEof, OptionsX optx)
        {
            checkBoxStopOnEof.IsChecked = stopOnEof;
            checkBoxSyncByAudio.IsChecked          = optx.SyncByAudio;
            checkBoxUseFilterGraphAlways.IsChecked = optx.UseFilterGraphAlways; 
            checkBoxConvFrameSeq.IsChecked         = optx.ConvFrameSequentially;
            checkBoxSwapFields.IsChecked           = optx.SwapFields;

            SetupAudioDevs(optx.AudioDevId + 1);
        }

        internal static bool Launch(Window owner, ref bool stopOnEof, OptionsX optx)
        {
            bool ret = false;
            var dlg = new OptionsExWindow() { Owner = owner };
            dlg.Init(stopOnEof, optx);
            if ((bool)dlg.ShowDialog())
            {
                stopOnEof = (bool)dlg.checkBoxStopOnEof.IsChecked;
                optx.SyncByAudio           = (bool)dlg.checkBoxSyncByAudio.IsChecked;
                optx.UseFilterGraphAlways  = (bool)dlg.checkBoxUseFilterGraphAlways.IsChecked;
                optx.ConvFrameSequentially = (bool)dlg.checkBoxConvFrameSeq.IsChecked;
                optx.SwapFields            = (bool)dlg.checkBoxSwapFields.IsChecked;
                optx.AudioDevId            = dlg.comboAudioDevs.SelectedIndex - 1;

                ret = true;
            }
            return ret;
        }
        private void buttonOK_Click(object sender, EventArgs e)
        {
            DialogResult = true;
        }

        private void buttonReset_Click(object sender, EventArgs e)
        {
            checkBoxStopOnEof.IsChecked = true;
            checkBoxSyncByAudio.IsChecked = true;
            checkBoxUseFilterGraphAlways.IsChecked = false;
            checkBoxConvFrameSeq.IsChecked = false;
            checkBoxSwapFields.IsChecked = false;

            comboAudioDevs.SelectedIndex = 0;
        }

    } // partial class PlayerOptionsWindow

} // namespace Player
