using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Threading;

namespace AppTools
{
    /// <summary>
    /// Логика взаимодействия для DShowDevListWindow.xaml
    /// </summary>
    public partial class DShowDevListWindow : Window
    {
        const int TimerIntervalMs = 200;

        DispatcherTimer m_Timer;

        Func<int, string[]>          m_ListGen;
        Func<string, string, string> m_UrlBuilder;

        public DShowDevListWindow()
        {
            InitializeComponent();
        }

        DShowDevListWindow(Func<int, string[]> listGen, Func<string, string, string> urlBuilder)
        {
            InitializeComponent();

            m_ListGen = listGen;
            m_UrlBuilder = urlBuilder;

            m_Timer = new DispatcherTimer() { Interval = TimeSpan.FromMilliseconds(TimerIntervalMs) };
            m_Timer.Tick += delegate { SetupDevList(); };
        }

        public static bool Launch(Window owner, 
            Func<int, string[]> listGen, Func<string, string, string> urlBuilder, 
            ref string url)
        {
            bool ret = false;
            var dlg = new DShowDevListWindow(listGen,  urlBuilder) { Owner = owner };
            if ((bool)dlg.ShowDialog())
            {
                url = dlg.BuildUrl();
                ret = true;
            }
            return ret;
        }

        private void Window_Loaded(object sender, RoutedEventArgs e) => m_Timer.IsEnabled = true;

        private void btnOk_Click(object sender, RoutedEventArgs e)
        {
            if (Verify())
            {
                DialogResult = true;
            }
        }

        void SetupDevList()
        {
            m_Timer.IsEnabled = false;

            FillDevList(0, listBoxVideo, checkBoxVideo);
            FillDevList(1, listBoxAudio, checkBoxAudio);

            labelInProgress.Visibility = Visibility.Hidden;
        }

        void FillDevList(int devType, ListBox listBox, CheckBox checkBox)
        {
            string[] items = m_ListGen(devType);
            if (items.Length > 0)
            {
                foreach (string dev in items)
                    listBox.Items.Add(dev);
            }
            else
            {
                checkBox.IsChecked = false;
                checkBox.IsEnabled = false;
            }
        }

        bool Video => (bool)checkBoxVideo.IsChecked;

        bool Audio => (bool)checkBoxAudio.IsChecked;

        bool Verify()
        {
            bool ret = true;
            string msg = null;
            if (Video || Audio)
            {
                if (Video && listBoxVideo.SelectedIndex < 0)
                {
                    ret = false;
                    msg = "You should select a video device or disable list.";
                }
                if (ret && Audio && listBoxAudio.SelectedIndex < 0)
                {
                    ret = false;
                    msg = "You should select an audio device or disable list.";
                }
            }
            else
            {
                ret = false;
                msg = "You should enable at least one not empty device lists.";
            }
            if (msg != null)
            {
                MessageBox.Show(msg);
            }
            return ret;
        }

        string BuildUrl()
        {
            return m_UrlBuilder(
                Video ? (string)listBoxVideo.SelectedItem : null,
                Audio ? (string)listBoxAudio.SelectedItem : null);
        }

    } // class DShowDevListWindow

} // namespace AppTools




