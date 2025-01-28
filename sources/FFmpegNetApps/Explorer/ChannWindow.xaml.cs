using System;
using System.IO;
using System.Windows;

using FFmpegInterop;
using AppTools;

namespace Explorer
{
    /// <summary>
    /// Interaction logic for ChannWindow.xaml
    /// </summary>
    public partial class ChannWindow : Window
    {

        ListItemCh[] m_ListItems;

        public ChannWindow()
        {
            InitializeComponent();
        }

        internal static void Launch(Window owner)
        {
            var dlg = new ChannWindow() { Owner = owner };
            dlg.SetupList();
            dlg.ShowDialog();
        }

        void SetupList()
        {
            using (var chList = new ChannList())
            {
                labelCount.Content = $"Standard audio channel layouts ({chList.Count})";

                m_ListItems = new ListItemCh[chList.Count];
                for (int i = 0; i < m_ListItems.Length; ++i)
                {
                    ChannInfo fi = chList[i];

                    var item = new ListItemCh();
                    item.Index = i;
                    item.Chann = fi.Chann;
                    item.Name = fi.Name;
                    item.Mask  = $"{fi.Mask:X}";
                    item.Descr = fi.Descr;
                    m_ListItems[i] = item;
                }
            }

            listView.ItemsSource = m_ListItems;
        }

        void Write(StreamWriter sw) => ListItemCh.WriteList(sw, m_ListItems, "Standard channel layouts");

        void buttonSave_Click(object sender, EventArgs e) => FileDlg.SaveInTxt(this, Write);

    } // partial class ChannWindow

} // namespace Explorer
