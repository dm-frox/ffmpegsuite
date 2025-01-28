using System;
using System.Windows;
using System.Text;

using FFmpegInterop;
using AppTools;


namespace Explorer
{
    /// <summary>
    /// Interaction logic for FixFmtWindow.xaml
    /// </summary>
    public partial class PixFmtWindow : Window
    {

        ListItemPix[] m_ListItems;

        public PixFmtWindow()
        {
            InitializeComponent();
        }

        internal static void Launch(Window owner)
        {
            var dlg = new PixFmtWindow() { Owner = owner };
            dlg.Setup();
            dlg.ShowDialog();
        }

        void Setup()
        {
            if (m_ListItems == null)
            {
                using (var pfList = new PixFmtList())
                {
                    labelCount.Content = $"Pixel formats ({pfList.Count})";

                    m_ListItems = new ListItemPix[pfList.Count];
                    for (int i = 0; i < m_ListItems.Length; ++i)
                    {
                        PixFmtInfo fi = pfList[i];

                        var item = new ListItemPix();
                        item.Index = i;
                        item.Name = fi.Name;
                        item.CompCount = fi.CompCount;
                        item.PlaneCount = fi.PlaneCount;
                        item.BitPerPix = fi.BitPerPix;
                        item.CompDepth = GetCompDepth(fi.CompDepth);
                        item.Extra = GetExtra(fi);
                        m_ListItems[i] = item;
                    }
                }

                listView.ItemsSource = m_ListItems;
            }
        }

        static string GetCompDepth(uint compDepth)
        {
            var sb = new StringBuilder(16);
            for (int i = 0; i < 4; ++i)
            {
                uint r = (compDepth >> i * 8) & 0xFF;
                if (r > 0)
                {
                    if (i > 0)
                    {
                        sb.Append('-');
                    }
                    sb.Append(r.ToString());
                }
                else
                {
                    break;
                }
            }
            return sb.ToString();
        }

        static string GetExtra(PixFmtInfo fi)
        {
            if (fi.Palette)
            {
                return "Palette";
            }
            if (fi.HWAccel)
            {
                return "HW acceleration";
            }
            if (fi.Bitstream)
            {
                return "Bitstream";
            }
            return string.Empty;
        }

 
        void buttonSave_Click(object sender, EventArgs e) => FileDlg.SaveInTxt(this, (sw) => ListItemSamp.WriteList(sw, m_ListItems, "Pixel formats"));

    } // partial class PixFmtWindow

} // namespace Explorer
