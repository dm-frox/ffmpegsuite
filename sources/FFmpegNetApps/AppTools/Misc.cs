using System;
using System.Windows;
using System.Diagnostics;
using System.Windows.Controls;

namespace AppTools
{
    [Flags] public enum HeadItems { Null = 0, None = 1, Default = 2 };

    public static class Misc
    {
        public static void OpenDoc(string docUrl)
        {
            try
            {
                Process.Start(docUrl);
            }
            catch
            {
                MessageBox.Show($"Failed to load the documentation file:\n{docUrl}");
            }
        }

        static void AddHeadItem(ComboBox combo, HeadItems headItems, HeadItems x)
        {
            if ((headItems & x) != 0)
            {
                combo.Items.Add(x.ToString());
            }
        }

        public static void Fill(this ComboBox combo, object[] items, HeadItems headItems = 0, int selInd = 0)
        {
            if (headItems != 0)
            {
                AddHeadItem(combo, headItems, HeadItems.None);
                AddHeadItem(combo, headItems, HeadItems.Default);
            }
            foreach (var item in items)
            {
                combo.Items.Add(item);
            }
            combo.SelectedIndex = selInd;
        }

    } // static class Misc

} // namespace AppTools
