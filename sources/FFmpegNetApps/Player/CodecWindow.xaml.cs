using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Diagnostics;

using FFmpegInterop;

namespace Explorer
{
    /// <summary>
    /// Interaction logic for Codecs.xaml
    /// </summary>
    public partial class CodecWindow : Window
    {
        const string DocUrl = @"http://ffmpeg.org/ffmpeg-codecs.html";

        private struct MediaTypeItem
        {
            internal readonly MediaType Type;
            internal readonly string    Name;

            internal MediaTypeItem(MediaType type, string name)
            {
                Type = type;
                Name = name;
            }
        }

        static readonly MediaTypeItem[] MediaTypeItems = 
        {
            new MediaTypeItem(MediaType.Unknown, "Any"),
            new MediaTypeItem(MediaType.Video, "Video"),
            new MediaTypeItem(MediaType.Audio, "Audio"),  
            new MediaTypeItem(MediaType.Subtitle, "Subtitle"),
        };

        CodecList m_Codecs;

        public CodecWindow()
        {
            InitializeComponent();
        }

        internal static void Launch(Window owner)
        {
            var dlg = new CodecWindow() { Owner = owner };
            dlg.ShowDialog();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            m_Codecs = new CodecList();
            m_Codecs.Init();
            SetupCodecTree();
        }

        private void Window_Closed(object sender, EventArgs e)
        {
            m_Codecs.Dispose();
        }

        private void treeView_SelectedItemChanged(object sender, RoutedEventArgs e)
        {
            var item = (TreeViewItem)treeView.SelectedItem;
            var node = (CodecNode)item.Tag;
            var info = item.Header as CodecInfo;
            SetCodecInfo(node, info);
        }

        private void buttonExpandAll_Click(object sender, RoutedEventArgs e)
        {
            treeViewRootV.ExpandSubtree();
            treeViewRootA.ExpandSubtree();
            treeViewRootS.ExpandSubtree();
        }

        private void buttonCollapseAll_Click(object sender, RoutedEventArgs e)
        {
            treeViewRootV.IsExpanded = false;
            treeViewRootA.IsExpanded = false;
            treeViewRootS.IsExpanded = false;
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
            textCodecId.Text = node?.LongName;
            textLossless.Text = YesNoVA(node?.Lossless);
            textLossy.Text = YesNoVA(node?.Lossy);
            textIntraOnly.Text = YesNoVA(node?.IntraOnly);
            textDecEnc.Text = (node != null) 
                ? $"{node.DecoderCount}/{node.EncoderCount}" 
                : null;

            labelCodecName.Content = (info != null) ? (info.Decoder ? "Decoder:" : "Encoder:") : null;
            textCodecName.Text = info?.Name;
            textCodecLongName.Text = info?.LongName;
            textExperimental.Text = YesNo(info?.Experimental);
            textMediaFormats.Text = info?.Formats;
            textAccelerations.Text = info?.Accelerations;
            textPrivOptions.Text = info?.PrivOptions;
            buttonPrivOpts.IsEnabled = !string.IsNullOrEmpty(textPrivOptions.Text);
        }

        static void SetupCodecSubTree(TreeViewItem root, CodecList codecs, MediaType type, bool sort, bool show)
        {
            root.Items.Clear();
            root.Tag = null;
            if (show)
            {
                root.Visibility = Visibility.Visible;
                List<CodecNode> nodes = new List<CodecNode>(200);
                for (int i = 0, n = codecs.NodeCount; i < n; ++i)
                {
                    CodecNode node = codecs.Node(i);
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
            else
            {
                root.Visibility = Visibility.Collapsed;
            }
        }

        void SetupCodecTree()
        {
            if (m_Codecs != null)
            {
                bool sort = (bool)radioSortByName.IsChecked;
                SetupCodecSubTree(treeViewRootV, m_Codecs, MediaType.Video, sort, (bool)checkBoxV.IsChecked);
                SetupCodecSubTree(treeViewRootA, m_Codecs, MediaType.Audio, sort, (bool)checkBoxA.IsChecked);
                SetupCodecSubTree(treeViewRootS, m_Codecs, MediaType.Subtitle, sort, (bool)checkBoxS.IsChecked);
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

        void UpdateList(object sender, RoutedEventArgs e)
        {
            SetupCodecTree();
        }

        private void buttonPrivOpts_Click(object sender, RoutedEventArgs e)
        {
            var item = (TreeViewItem)treeView.SelectedItem;
            var info = item?.Header as CodecInfo;
            OptionsWindow.Launch(this, info?.PrivOptionsEx);
        }

        private void buttonContext_Click(object sender, RoutedEventArgs e)
        {
            OptionsWindow.Launch(this, m_Codecs.ContextOptions, "Codec context options");
        }

        private void buttonDocs_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                Process.Start(DocUrl);
            }
            catch
            {
                MessageBox.Show("Failed to load the documentation file.");
            }
        }

    } // partial class CodecWindow

} // namespace PlayerProbe
