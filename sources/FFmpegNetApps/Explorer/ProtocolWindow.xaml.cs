using System;
using System.IO;
using System.Windows;

using FFmpegInterop;
using AppTools;

namespace Explorer
{
    /// <summary>
    /// Interaction logic for ProtocolWindow.xaml
    /// </summary>
    public partial class ProtocolWindow : Window
    {
        const string DocUrl = @"https://ffmpeg.org/ffmpeg-protocols.html";

        const string DirStr = "+";

        ListItemProto[] m_ListItems;
        ProtocolList    m_ProtocolList;

        public ProtocolWindow()
        {
            InitializeComponent();
        }

        internal static void Launch(Window owner)
        {
            var dlg = new ProtocolWindow() { Owner = owner };
            dlg.Setup();
            dlg.ShowDialog();
        }

        void Setup()
        {
            if (m_ProtocolList == null)
            {
                m_ProtocolList = new ProtocolList();

                int n = m_ProtocolList.Count, inc = 0, outc = 0;
                m_ListItems = new ListItemProto[n];
                for (int i = 0; i < n; ++i)
                {
                    ProtocolInfo pi = m_ProtocolList[i];

                    var item = new ListItemProto();
                    item.Index = i;
                    item.Name = pi.Name;

                    if (pi.IsInput)
                    {
                        ++inc;
                        item.In = DirStr;
                    }
                    if (pi.IsOutput)
                    {
                        ++outc;
                        item.Out = DirStr;
                    }
                    m_ListItems[i] = item;
                }
                labelInfo.Content = $"Protocols: {n}; Input: {inc}; Output: {outc}";
                listView.ItemsSource = m_ListItems;
            }
        }

        void DisplaySelected()
        {
            int sel = listView.SelectedIndex;
            if (sel >= 0)
            {
                var pi = m_ProtocolList[sel];
                if (pi != null)
                {
                    textName.Text = pi.Name;

                    textPrivOpts.Text = pi.PrivOptions;
                    buttonPrivOpts.IsEnabled = !ListItemOpt.IsEmptyEx(textPrivOpts.Text);

                    textDirs.Text = (pi.IsInput && pi.IsOutput)
                        ? "input, output"
                        : pi.IsInput ? "input" : "output";
                }
            }
        }

        void Window_Closed(object sender, EventArgs e) => m_ProtocolList.Dispose();

        void Write(StreamWriter sw) => ListItemFlt.WriteList(sw, m_ListItems, labelInfo.Content.ToString());

        void buttonSave_Click(object sender, EventArgs e) => FileDlg.SaveInTxt(this, Write);

        void listView_SelectionChanged(object sender, EventArgs e) => DisplaySelected();

        void buttonPrivOpts_Click(object sender, EventArgs e)
        {
            int sel = listView.SelectedIndex;
            if (sel >= 0)
            {
                OptionsWindow.Launch(this, m_ProtocolList[sel].PrivOptionsEx);
            }
        }

        void buttonDocs_Click(object sender, RoutedEventArgs e) => Misc.OpenDoc(DocUrl);

    } // public partial class ProtocolWindow

} // namespace Explorer
