using System;
using System.Windows;

using AppTools;
using FFmpegInterop;


namespace Explorer
{
    /// <summary>
    /// Interaction logic for SampFmtWindow.xaml
    /// </summary>
    public partial class SampFmtWindow : Window
    {

        ListItemSamp[] m_ListItems;

        public SampFmtWindow()
        {
            InitializeComponent();
        }

        internal static void Launch(Window owner)
        {
            var dlg = new SampFmtWindow() { Owner = owner };
            dlg.Setup();
            dlg.ShowDialog();
        }

        void Setup()
        {
            if (m_ListItems == null)
            {
                using (var pfList = new SampFmtList())
                {
                    labelCount.Content = $"Sample formats ({pfList.Count})";

                    m_ListItems = new ListItemSamp[pfList.Count];
                    for (int i = 0; i < m_ListItems.Length; ++i)
                    {
                        SampFmtInfo fi = pfList[i];

                        var item = new ListItemSamp();
                        item.Index = i;
                        item.Name = fi.Name;
                        item.BytesPerSamp = fi.BytesPerSamp;
                        item.Descr = fi.Descr;
                        m_ListItems[i] = item;
                    }
                }

                listView.ItemsSource = m_ListItems;
            }
            listView.ItemsSource = m_ListItems;
        }

        void buttonSave_Click(object sender, EventArgs e) => FileDlg.SaveInTxt(this, (sw) 
            => ListItemSamp.WriteList(sw, m_ListItems, "Sample formats"));

    } // partial class SampFmtWindow

} // namespace Explorer
