using System;
using System.Windows;
using System.Windows.Controls;

using FFmpegNetAux;
using FFmpegNetMediaSource;
using AppTools;


namespace Player
{
    /// <summary>
    /// Interaction logic for MediaSourceWindow.xaml
    /// </summary>
    public partial class MediaSourceWindow : Window
    {
        const string RootTxt = "Media source";
        const string MetaTxt = "Metadata";
        const string StrsTxt = "Streams";
        const string ChpsTxt = "Chapters";

        MediaSourceHdr m_MediaSourceHdr;

        public MediaSourceWindow()
        {
            InitializeComponent();
        }

        MediaSourceWindow(MediaSourceHdr hdr)
        {
            InitializeComponent();

            m_MediaSourceHdr = hdr;

            SetupTree(treeViewRoot, m_MediaSourceHdr);
            treeView.Focus();
        }

        internal static void Launch(Window owner, MediaSourceHdr hdr)
        {
            if (hdr != null)
            {
                var dlg = new MediaSourceWindow(hdr) { Owner = owner };
                dlg.ShowDialog();
            }
        }

        static TreeViewItem AddNode(TreeViewItem parent, string header, KeyValList tag)
        {
            var item = new TreeViewItem();
            item.Header = header;
            item.Tag = tag;
            parent.Items.Add(item);
            return item;
        }

        static void SetupInfoNode(TreeViewItem parent, HdrItem hdrItem)
        {
            parent.Tag = hdrItem.Info;
            if (hdrItem.Metadata.Length > 0)
            {
                AddNode(parent, MetaTxt, hdrItem.Metadata);
            }
            if (hdrItem.Chapters != null && hdrItem.Chapters.Length > 0)
            {
                AddNode(parent, ChpsTxt, hdrItem.Chapters);
            }
        }

        static void SetupTree(TreeViewItem root, MediaSourceHdr hdr)
        {
            root.Header = RootTxt;
            SetupInfoNode(root, hdr.Root);
            TreeViewItem strms = AddNode(root, StrsTxt, null);
            foreach (var hdrItem in hdr.Streams)
            {
                TreeViewItem strm = AddNode(strms, hdrItem.Info.FirstValue, hdrItem.Info);
                SetupInfoNode(strm, hdrItem);
            }
            root.IsExpanded = true;
            strms.IsExpanded = true;
            root.IsSelected = true;
        }

        void SetList()
        {
            var item = (TreeViewItem)treeView.SelectedItem;
            listView.ItemsSource = (KeyValList)item.Tag;
        }

        void treeView_SelectedItemChanged(object sender, EventArgs e) => SetList();

        void buttonExpandAll_Click(object sender, EventArgs e) => treeViewRoot.ExpandSubtree();

        void buttonCollapseAll_Click(object sender, EventArgs e) => treeViewRoot.IsExpanded = false;

        void buttonPrint_Click(object sender, EventArgs e) => FileDlg.SaveInTxt(this, (sw) => MediaSourceRep.Write(m_MediaSourceHdr, sw));

    } // partial class MetadataWindow

} // namespace Player
