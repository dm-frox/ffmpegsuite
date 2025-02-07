using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

using FFmpegInterop;
using AppTools;

namespace Explorer
{
    /// <summary>
    /// Interaction logic for Codecs.xaml
    /// </summary>
    public partial class CodecWindow : Window
    {
        const string DocUrl = @"https://ffmpeg.org/ffmpeg-codecs.html";


        CodecList m_Codecs;

        public CodecWindow()
        {
            InitializeComponent();
        }

        internal static void Launch(Window owner)
        {
            var dlg = new CodecWindow() { Owner = owner };
            dlg.SetupList();
            dlg.ShowDialog();
        }

        void SetupList()
        {
            m_Codecs = new CodecList();
            SetupCodecTree();
        }

        void Window_Closed(object sender, EventArgs e) => m_Codecs.Dispose();

        void treeView_SelectedItemChanged(object sender, EventArgs e)
        {
            var item = (TreeViewItem)treeView.SelectedItem;
            var node = (CodecNode)item.Tag;
            var info = item.Header as CodecInfo;
            SetCodecInfo(node, info);
        }

        void buttonExpandAll_Click(object sender, EventArgs e)
        {
            treeViewRootV.ExpandSubtree();
            treeViewRootA.ExpandSubtree();
            treeViewRootS.ExpandSubtree();
            treeViewRootD.ExpandSubtree();
        }

        void buttonCollapseAll_Click(object sender, EventArgs e)
        {
            treeViewRootV.IsExpanded = false;
            treeViewRootA.IsExpanded = false;
            treeViewRootS.IsExpanded = false;
            treeViewRootD.IsExpanded = false;
        }

        void SetCodecInfo(CodecNode node, CodecInfo info)
        {
            string TextCond(bool? f, string x, string y)
            {
                return f.HasValue ? (f.Value ? x : y) : null;
            }

            string YesNo(bool? f)
            {
                return TextCond(f, "Yes", "No");
            }

            string YesNoVA(bool? f)
            {
                return (f.HasValue && (node.MediaType == (int)MediaType.Video || node.MediaType == (int)MediaType.Audio))
                ? f.Value ? "Yes" : "No"
                : null;
            }

            textMediaType.Text = node?.MediaTypeStr;
            textCodecIdName.Text = node?.Name;
            textCodecIdDescr.Text = node?.LongName;
            textLossless.Text = YesNoVA(node?.Lossless);
            textLossy.Text = YesNoVA(node?.Lossy);
            textIntraOnly.Text = YesNoVA(node?.IntraOnly);
            textDecEnc.Text = (node != null) 
                ? $"{node.DecoderCount}/{node.EncoderCount}" 
                : null;

            labelCodec.Content = (info != null) ? (info.Decoder ? "Decoder" : "Encoder") : "Codec";
            textCodecName.Text = info?.Name;
            textCodecDescr.Text = info?.LongName;
            textExperimental.Text = YesNo(info?.Experimental);
            textMediaFormats.Text = info?.Formats;
            textAccelerations.Text = info?.HWAccels;
            textPrivOpts.Text = info?.PrivOptions;
            buttonPrivOpts.IsEnabled = !ListItemOpt.IsEmptyEx(textPrivOpts.Text);
        }

        static void SetupCodecSubTree(TreeViewItem root, CodecList codecs, MediaType type, bool sort)
        {
            root.Items.Clear();
            root.Tag = null;
            root.Visibility = Visibility.Visible;
            List<CodecNode> nodes = new List<CodecNode>(200);
            for (int i = 0, n = codecs.Count; i < n; ++i)
            {
                CodecNode node = codecs[i];
                if (node.MediaType == (int)type)
                {
                    nodes.Add(node);
                }
            }
            if (sort)
            {
                nodes.Sort((x, y) => string.Compare(x.Name, y.Name));
            }
            int codecCount = 0, decCount = 0, encCount = 0;
            foreach (var node in nodes)
            {
                root.Items.Add(SetupCodecNode(node));
                codecCount += node.CodecCount;
                decCount += node.DecoderCount;
                encCount += node.EncoderCount;
            }
            root.Header = $"{type.ToString()} (IDs: {nodes.Count}; codecs: {codecCount}={decCount}+{encCount})";
        }

        void SetupCodecTree()
        {
            if (m_Codecs != null)
            {
                bool sort = (bool)radioSortByName.IsChecked;
                SetupCodecSubTree(treeViewRootV, m_Codecs, MediaType.Video, sort);
                SetupCodecSubTree(treeViewRootA, m_Codecs, MediaType.Audio, sort);
                SetupCodecSubTree(treeViewRootS, m_Codecs, MediaType.Subtitle, sort);
                SetupCodecSubTree(treeViewRootD, m_Codecs, MediaType.Data, sort);
            }
        }

        static TreeViewItem SetupCodecNode(CodecNode node)
        {
            var nodeItem = new TreeViewItem();
            nodeItem.Tag = node;

            TreeViewItem AddChildItem(TreeViewItem parItem, object hdr)
            {
                var chItem = new TreeViewItem();
                chItem.Header = hdr;
                chItem.Tag = node;
                parItem.Items.Add(chItem);
                return chItem;
            }

            void SetupDecoders()
            {
                int count = node.DecoderCount;
                if (count > 0)
                {
                    TreeViewItem root = AddChildItem(nodeItem, $"Decoders ({count})");
                    root.Tag = node;
                    root.Foreground = Brushes.DarkRed;
                    for (int i = 0; i < count; ++i)
                    {
                        AddChildItem(root, node.Decoder(i));
                    }
                }
            }

            void SetupEncoders()
            {
                int count = node.EncoderCount;
                if (count > 0)
                {
                    TreeViewItem root = AddChildItem(nodeItem, $"Encoders ({count})");
                    root.Tag = node;
                    root.Foreground = Brushes.DarkRed;
                    for (int i = 0; i < count; ++i)
                    {
                        AddChildItem(root, node.Encoder(i));
                    }
                }
            }

            nodeItem.Header = node;
            SetupDecoders();
            SetupEncoders();
            return nodeItem;
        }

        void UpdateTree(object sender, EventArgs e) => SetupCodecTree();

        void buttonPrivOpts_Click(object sender, EventArgs e)
        {
            var item = (TreeViewItem)treeView.SelectedItem;
            var info = item?.Header as CodecInfo;
            OptionsWindow.Launch(this, info?.PrivOptionsEx);
        }

        void buttonContext_Click(object sender, EventArgs e) => OptionsWindow.Launch(this, m_Codecs.ContextOptionsEx, true);

        void buttonDocs_Click(object sender, EventArgs e) => Misc.OpenDoc(DocUrl);

        void buttonSave_Click(object sender, EventArgs e) => FileDlg.SaveInTxt(this, (sw) => CodecWriter.Write(sw, m_Codecs));

    } // partial class CodecWindow

} // namespace Explorer
