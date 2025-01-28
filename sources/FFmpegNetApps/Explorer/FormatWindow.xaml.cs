using System;
using System.IO;
using System.Windows;
using System.Windows.Controls;

using FFmpegInterop;
using AppTools;

namespace Explorer
{
    /// <summary>
    /// Interaction logic for FormatWindow.xaml
    /// </summary>
    public partial class FormatWindow : Window
    {
        const string DocUrl = @"https://ffmpeg.org/ffmpeg-formats.html";

        FormatList m_Formats;

        public FormatWindow()
        {
            InitializeComponent();
        }

        internal static void Launch(Window owner)
        {
            var dlg = new FormatWindow() { Owner = owner };
            dlg.SetupList();
            dlg.ShowDialog();
        }

        void SetupList()
        {
            m_Formats = new FormatList();
            SetupFormatTree(treeViewRoot, m_Formats, labelInfo);
            labelDemuxMux.Content = " ";
        }

        void Window_Closed(object sender, EventArgs e) => m_Formats.Dispose();

        void treeView_SelectedItemChanged(object sender, EventArgs e) => SetFormatInfo(GetInfo());

        FormatInfo GetInfo()
        {
            var item = (TreeViewItem)treeView.SelectedItem;
            return item?.Tag as FormatInfo;
        }

        void SetFormatInfo(FormatInfo info)
        {
            textFormatName.Text = info?.Name;
            labelDemuxMux.Content = info != null ? (info.Demuxer ? "Demuxer" : "Muxer") : " ";
            textFormatLongName.Text = info?.LongName;
            textFormatExt.Text = info?.Extentions;
            textFormatMime.Text = info?.Mime;
            textPrivOptions.Text = info?.PrivOptions;
            buttonPrivOpts.IsEnabled = !ListItemOpt.IsEmptyEx(textPrivOptions.Text);
        }

        static TreeViewItem AddChildItem(TreeViewItem parItem, object hdr, object tag)
        {
            var chItem = new TreeViewItem();
            chItem.Header = hdr;
            chItem.Tag = tag;
            parItem.Items.Add(chItem);
            return chItem;
        }

        static void SetupFormatTree(TreeViewItem root, FormatList formats, Label labInfo)
        {
            root.Items.Clear();
            root.Tag = null;
            int fmtCount = 0, demuxCount = 0, muxCount = 0;
            string name = string.Empty;
            TreeViewItem curr = null;
            for (int i = 0, n = formats.Count; i < n; ++i)
            {
                FormatInfo info = formats[i];
                string nn = info.Name;
                if (nn != name) // new format
                {
                    name = nn;
                    curr = AddChildItem(root, name, null);
                    ++fmtCount;
                }
                AddChildItem(curr, info.Demuxer ? "Demuxer" : "Muxer", info);
                var ff = info.Demuxer ? ++demuxCount : ++muxCount;
            }
            labInfo.Content = $"Formats: {fmtCount}; Demuxers: {demuxCount}; Muxers: {muxCount}";
            root.IsExpanded = true;
        }

        void Write(StreamWriter sw) => FormatWriter.Write(sw, m_Formats, labelInfo.Content.ToString());

        void buttonSave_Click(object sender, EventArgs e) => FileDlg.SaveInTxt(this, Write);


        void buttonExpandAll_Click(object sender, EventArgs e) => treeViewRoot.ExpandSubtree();

        void buttonCollapseAll_Click(object sender, EventArgs e)
        {
            foreach (var itm in treeViewRoot.Items)
            {
                ((TreeViewItem)itm).IsExpanded = false;
            }
        }

        void buttonPrivOpts_Click(object sender, EventArgs e) => OptionsWindow.Launch(this, GetInfo()?.PrivOptionsEx);

        void buttonContext_Click(object sender, EventArgs e) => OptionsWindow.Launch(this, m_Formats.ContextOptionsEx, true);

        void buttonDocs_Click(object sender, EventArgs e) => Misc.OpenDoc(DocUrl);


    } // partial class FormatWindow

} // namespace Explorer
