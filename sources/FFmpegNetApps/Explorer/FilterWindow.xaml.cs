using System;
using System.Windows;
using System.IO;

using FFmpegInterop;
using AppTools;

namespace Explorer
{
    /// <summary>
    /// Interaction logic for FilterWindow.xaml
    /// </summary>
    public partial class FilterWindow : Window
    {

        const string DocUrl = @"https://ffmpeg.org/ffmpeg-filters.html";

        const string CmdStr = "+";

        static char Mt(bool isVideo) => isVideo ? 'V' : 'A';

        static string FormatInOut(int count, bool isVideo) => (count == 0) ? "0" : $"{count}-{Mt(isVideo)}";

        static readonly string[] BlackList = { "alatency", "dnn_detect", "latency" }; // v. 7.0

        ListItemFlt[] m_ListItems;

        FilterList    m_FilterList;

        public FilterWindow()
        {
            InitializeComponent();
        }

        internal static void Launch(Window owner)
        {
            var dlg = new FilterWindow() { Owner = owner };
            dlg.SetupList();
            dlg.ShowDialog();
        }

        static bool InBlackList(string name)
        {
            foreach (var item in BlackList)
            {
                if (item == name)
                {
                    return true;
                }
            }
            return false;
        }

        void Window_Closed(object sender, EventArgs e) => m_FilterList.Dispose();

        void SetupList()
        {
            if (m_FilterList == null)
            {
                m_FilterList = new FilterList();

                labelCount.Content = $"Filters: {m_FilterList.Count}";

                m_ListItems = new ListItemFlt[m_FilterList.Count];
                for (int i = 0; i < m_ListItems.Length; ++i)
                {
                    FilterInfo fi = m_FilterList[i];

                    var item = new ListItemFlt();
                    item.Index = i;
                    item.Name = fi.Name;
                    item.In = FormatInOut(fi.InCount, fi.InVideo);
                    item.Out = FormatInOut(fi.OutCount, fi.OutVideo);
                    item.Cmd = fi.Cmd ? CmdStr : string.Empty;
                    item.Descr = fi.Descr;

                    m_ListItems[i] = item;
                }
            }

            listView.ItemsSource = m_ListItems;
        }

        void DisplaySelected()
        {
            int sel = listView.SelectedIndex;
            if (sel >= 0)
            {
                var pi = m_FilterList[sel];
                if (pi != null)
                {
                    textName.Text = pi.Name;
                    textDescr.Text = pi.Descr;

                    textPrivOpts.Text = !InBlackList(pi.Name) ? pi.PrivOptions : string.Empty;
                    buttonPrivOpts.IsEnabled = !ListItemOpt.IsEmptyEx(textPrivOpts.Text);
                }
            }
        }

        void WriteList(StreamWriter sw) => ListItemFlt.WriteList(sw, m_ListItems, "FFmpeg filters");

        void buttonSave_Click(object sender, EventArgs e) => FileDlg.SaveInTxt(this, WriteList);

        void listView_SelectionChanged(object sender, EventArgs e) => DisplaySelected();

        void buttonContext_Click(object sender, EventArgs e) => OptionsWindow.Launch(this, m_FilterList.ContextOptionsEx, true);

        void buttonPrivOpts_Click(object sender, EventArgs e)
        {
            int sel = listView.SelectedIndex;
            if (sel >= 0)
            {
                OptionsWindow.Launch(this, m_FilterList[sel].PrivOptionsEx);
            }
        }

        void buttonDocs_Click(object sender, EventArgs e) => Misc.OpenDoc(DocUrl);

    } // partial class FilterWindow

} // namespace Explorer
