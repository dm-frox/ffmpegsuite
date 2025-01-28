using System;
using System.Text;
using System.Windows;
using System.Windows.Controls;


namespace Player
{
    /// <summary>
    /// Interaction logic for FilterCmdWindow2.xaml
    /// </summary>
    public partial class FilterCmdWindow2 : Window
    {
        const double MinFontSize = 10.0;
        const double MaxFontSize = 200.0;

        const string Filter = "drawtext";

        const string Command = "reinit";

        static readonly string[] Colors = 
        { 
            "Black",
            "Gray",
            "Silver",
            "White",
            "Fuchsia",
            "Purple",
            "Red",
            "Maroon",
            "Yellow",
            "Olive",
            "Lime",
            "Green",
            "Aqua",
            "Teal",
            "Blue",
            "Navy"
        };

        Action<bool, string, string, string> m_Command;
        DrawTextParams                       m_DrawTextParams;
        bool                                 m_Initialized;

        static Window TheWindow;

        public FilterCmdWindow2()
        {
            InitializeComponent();
        }

        FilterCmdWindow2(int width, int height, DrawTextParams prms, Action<bool, string, string, string> cmd)
        {
            InitializeComponent();

            foreach (var s in Colors)
            {
                comboColor.Items.Add(s);
            }

            m_Initialized = true;

            m_Command = cmd;

            sliderSize.Maximum = MaxFontSize;
            sliderSize.Minimum = MinFontSize;
            sliderSize.SmallChange = MinFontSize;
            sliderSize.LargeChange = sliderSize.SmallChange * 10;

            sliderX.Maximum = width;
            sliderX.SmallChange = 10;
            sliderX.LargeChange = 50;

            sliderY.Maximum = height;
            sliderX.SmallChange = 10;
            sliderX.LargeChange = 50;

            m_DrawTextParams = prms;

            LoadParams(m_DrawTextParams);
        }

        internal static void Launch(Window owner, int width, int height, DrawTextParams prms, Action<bool, string, string, string> cmd)
        {
            if (TheWindow == null)
            {
                var dlg = new FilterCmdWindow2(width, height, prms, cmd) { Owner = owner };
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

        double Clamp(double val, double min, double max)
        {
            return Math.Max(Math.Min(val, max), min);
        }

        int Clamp(int val, int min, int max)
        {
            return Math.Max(Math.Min(val, max), min);
        }

        void SetValue(Slider slider, double val)
        {
            slider.Value = Clamp(val, slider.Minimum, slider.Maximum);
        }

        void LoadParams(DrawTextParams prms)
        {
            textText.Text = prms.Text != null ? prms.Text : string.Empty;
            SetValue(sliderSize, prms.Fontsize);
            comboColor.SelectedIndex = (int)Clamp(prms.Fontcolor, 0, Colors.Length - 1);
            SetValue(sliderX, prms.X);
            SetValue(sliderY, prms.Y);
        }

        void SaveParams(DrawTextParams prms)
        {
            prms.Text = textText.Text;
            prms.Fontsize = ValToInt(sliderSize);
            prms.Fontcolor = comboColor.SelectedIndex;
            prms.X = ValToInt(sliderX);
            prms.Y = ValToInt(sliderY);
        }

        void buttonApply_Click(object sender, EventArgs e)
        {
            var arg = new StringBuilder(64);
            arg.Append("text=");
            arg.Append(textText.Text);

            arg.Append(':');
            arg.Append("fontsize=");
            arg.Append(textSize.Text);

            arg.Append(':');
            arg.Append("fontcolor=");
            arg.Append(Colors[comboColor.SelectedIndex]);

            arg.Append(':');
            arg.Append("x=");
            arg.Append(textX.Text);

            arg.Append(':');
            arg.Append("y=");
            arg.Append(textY.Text);

            m_Command.Invoke(true, Filter, Command, arg.ToString());

            SaveParams(m_DrawTextParams);
        }

        static int ValToInt(Slider s)
        {
            return (int)Math.Round(s.Value);
        }

        static string ValToStr(Slider s )
        {
            return ValToInt(s).ToString();
        }

        void sliderSize_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (m_Initialized)
            {
                textSize.Text = ValToStr(sliderSize);
            }
        }

        void sliderX_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (m_Initialized)
            {
                textX.Text = ValToStr(sliderX);
            }
        }

        void sliderY_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (m_Initialized)
            {
                textY.Text = ValToStr(sliderY);
            }
        }

    } // partial class FilterCmdWindow2

} // namespace Player

// filter: drawtext
// command: reinit
// Alter existing filter parameters.
// Syntax for the argument is the same as for filter invocation, e.g.
// fontsize=56:fontcolor=green:text='Hello World'

