using System;
using System.Windows;
using System.IO;

using AppTools;

namespace Explorer
{
    /// <summary>
    /// Interaction logic for OptionsWindow.xaml
    /// </summary>
    public partial class OptionsWindow : Window
    {

        ListItemOpt[] m_ListItems;
        string        m_Name;

        public OptionsWindow()
        {
            InitializeComponent();
        }

        internal static void Launch(Window owner, string privOpts, bool ctx = false)
        {
            if (!string.IsNullOrEmpty(privOpts))
            {
                var dlg = new OptionsWindow() { Owner = owner };
                dlg.Setup(privOpts, ctx);
                dlg.ShowDialog();
            }
        }

        void Setup(string opts, bool ctx)
        {
            m_ListItems = ListItemOpt.GetList(opts, ref m_Name);
            listView.ItemsSource = m_ListItems;
            textName.Text = $"{m_Name} ({m_ListItems.Length})";
            if (ctx)
            {
                this.Title = "Context options";
            }
        }

        void Write(StreamWriter sw) => ListItemOpt.WriteList(sw, m_ListItems, this.Title, textName.Text.ToString());

        void buttonSave_Click(object sender, EventArgs e) => FileDlg.SaveInTxt(this, Write);

    } //  partial class PrivOptsWindow

} // namespace Explorer
