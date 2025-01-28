using System;
using System.IO;
using System.Windows;

using FFmpegInterop;
using AppTools;

namespace Explorer
{
    /// <summary>
    /// Interaction logic for BsfWindow.xaml
    /// </summary>
    public partial class BsfWindow : Window
    {
        const string DocUrl = @"https://ffmpeg.org/ffmpeg-bitstream-filters.html";

        ListItemBsf[] m_ListItems;
        BsfList m_BsfList;

        public BsfWindow()
        {
            InitializeComponent();
        }

        internal static void Launch(Window owner)
        {
            var dlg = new BsfWindow() { Owner = owner };
            dlg.Setup();
            dlg.ShowDialog();
        }

        void Setup()
        {
            if (m_BsfList == null)
            {
                m_BsfList = new BsfList();

                int n = m_BsfList.Count;
                m_ListItems = new ListItemBsf[n];
                for (int i = 0; i < n; ++i)
                {
                    BsfInfo pi = m_BsfList[i];

                    var item = new ListItemBsf();
                    item.Index = i;
                    item.Name = pi.Name;
                    item.CodecIds = pi.CodecIds;

                    m_ListItems[i] = item;
                }
                labelInfo.Content = $"Bitstream filters: {n}";
                listView.ItemsSource = m_ListItems;
            }
        }

        void DisplaySelected()
        {
            int sel = listView.SelectedIndex;
            if (sel >= 0)
            {
                var pi = m_BsfList[sel];
                if (pi != null)
                {
                    textName.Text = pi.Name;
                    textCodecs.Text = pi.CodecIds;

                    textPrivOptions.Text = pi.PrivOptions;
                    buttonPrivOpts.IsEnabled = !ListItemOpt.IsEmptyEx(textPrivOptions.Text);
                }
            }
        }

        void Window_Closed(object sender, EventArgs e) => m_BsfList.Dispose();

        void Write(StreamWriter sw) => ListItemFlt.WriteList(sw, m_ListItems, labelInfo.Content.ToString());

        void buttonSave_Click(object sender, EventArgs e) => FileDlg.SaveInTxt(this, Write);

        void listView_SelectionChanged(object sender, EventArgs e) => DisplaySelected();

        void buttonPrivOpts_Click(object sender, EventArgs e)
        {
            int sel = listView.SelectedIndex;
            if (sel >= 0)
            {
                OptionsWindow.Launch(this, m_BsfList[sel].PrivOptionsEx);
            }
        }

        void buttonDocs_Click(object sender, RoutedEventArgs e) => Misc.OpenDoc(DocUrl);

        //void buttonOpts_Click(object sender, EventArgs e)
        //{
        //    if (m_BsfList != null)
        //    {
        //        int ind = listView.SelectedIndex;
        //        if (ind >= 0)
        //        {
        //            BsfInfo pi = m_BsfList[ind];
        //            FilterOptsWindow.Launch(this, pi.GetOptions, pi.Name, "Bitstream filter options");
        //        }
        //    }
        //}

    } // public partial class BsfWindow

} // namespace Explorer
