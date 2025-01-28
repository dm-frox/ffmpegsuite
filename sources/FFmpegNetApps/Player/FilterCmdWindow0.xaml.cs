using System;
using System.Windows;

namespace Player
{
    /// <summary>
    /// Interaction logic for FilterCmdWindow0.xaml
    /// </summary>
    public partial class FilterCmdWindow0 : Window
    {
        readonly bool m_Video;

        Action<bool, string, string, string> m_Command;


        static Window TheWindow;

        public FilterCmdWindow0()
        {
            InitializeComponent();
        }

        FilterCmdWindow0(string filter, bool video, Action<bool, string, string, string> cmd)
        {
            InitializeComponent();

            m_Command = cmd;
            int eqPos = filter.IndexOf('=');
            textFlt.Text = eqPos < 0 ? filter : filter.Substring(0, eqPos);
            m_Video = video;
            if (!m_Video)
            {
                labelFlt.Content = "Audio filter:";
            }
        }

        internal static void Launch(Window owner, string filter, bool video, Action<bool, string, string, string> cmd)
        {
            if (TheWindow == null)
            {
                var dlg = new FilterCmdWindow0(filter, video, cmd) { Owner = owner };
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

        void buttonApply_Click(object sender, EventArgs e)
        {
            string flt = textFlt.Text;
            string cmd = textCmd.Text;
            string arg = textArg.Text;
            if (!string.IsNullOrEmpty(cmd) && !string.IsNullOrEmpty(arg))
            {
                m_Command.Invoke(m_Video, flt, cmd, arg);
            }
            else
            {
                App.ShowMessageErr(this, ResStr1.Msg7);
            }
        }

    } //  partial class FilterCmdWindow0

} // namespace Player

