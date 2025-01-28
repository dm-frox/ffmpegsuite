using System;
using System.Windows;
using System.Windows.Controls;

namespace Player
{
    /// <summary>
    /// Interaction logic for FilterCmdWindow1.xaml
    /// </summary>
    public partial class FilterCmdWindow1 : Window
    {
        const string Filter = "eq";

        const string CommBrt = "brightness";
        const string CommCon = "contrast";
        const string CommSat = "saturation";

        EqParams m_EqParams;
        Action<bool, string, string, string> m_Command;

        static Window TheWindow;

        public FilterCmdWindow1()
        {
            InitializeComponent();
        }

        FilterCmdWindow1(EqParams prms, Action<bool, string, string, string> cmd)
        {
            InitializeComponent();

            m_EqParams = prms;

            LoadParams(m_EqParams);

            textSliderB.Text = GetValue(sliderB);
            textSliderC.Text = GetValue(sliderC);
            textSliderS.Text = GetValue(sliderS);

            m_Command = cmd;
        }

        internal static void Launch(Window owner, EqParams prms, Action<bool, string, string, string> cmd)
        {
            if (TheWindow == null)
            {
                var dlg = new FilterCmdWindow1(prms, cmd) { Owner = owner };
                TheWindow = dlg;
                dlg.Show();
            }
            else
            {
                TheWindow.Focus();
            }
        }

        internal static void CloseEx() => TheWindow?.Close();

        void Window_Closed(object sender, EventArgs e) => TheWindow = null;

        void buttonExit_Click(object sender, EventArgs e) => this.Close();


        void LoadParams(EqParams prms)
        {
            sliderB.Value = prms.Brightness;
            sliderC.Value = prms.Contrast;
            sliderS.Value = prms.Saturation;
        }

        void SaveParams(EqParams prms)
        {
            prms.Brightness = sliderB.Value;
            prms.Contrast = sliderC.Value;
            prms.Saturation = sliderS.Value;
        }

        static string GetValue(Slider s)
        {
            var a = s.Value.ToString("F1");
            return a.Replace(',', '.');
        }

        void sliderB_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            var v = GetValue(sliderB);
            if (textSliderB != null)
            {
                textSliderB.Text = v;
            }
            if (m_Command != null)
            {
                m_Command.Invoke(true, Filter, CommBrt, v);
            }
        }

        void sliderC_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            var v = GetValue(sliderC);
            if (textSliderC != null)
            {
                textSliderC.Text = v;
            }
            if (m_Command != null)
            {
                m_Command.Invoke(true, Filter, CommCon, v);
            }
        }

        void sliderS_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            var v = GetValue(sliderS);
            if (textSliderS != null)
            {
                textSliderS.Text = v;
            }
            if (m_Command != null)
            {
                m_Command.Invoke(true, Filter, CommSat, v);
            }
        }

        void buttonReset_Click(object sender, EventArgs e)
        {
            m_EqParams.Reset();
            LoadParams(m_EqParams);
        }


        void Window_Closing(object sender, EventArgs e)
        {
            SaveParams(m_EqParams);
        }

    } // partial class FilterCmdWindow1

} // namespace Player
