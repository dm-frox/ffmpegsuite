using System;
using System.IO;
using System.Globalization;
using System.Windows;
using System.Windows.Controls;

using FFmpegInterop;


namespace Explorer
{
    /// <summary>
    /// Interaction logic for VersWindow.xaml
    /// </summary>
    public partial class VersWindow : Window
    {
 
        const double MB = 1024 * 1024;

        public VersWindow()
        {
            InitializeComponent();
        }

        internal static void Launch(Window owner)
        {
            var dlg = new VersWindow() { Owner = owner };
            dlg.Setup();
            dlg.ShowDialog();
        }

        void Setup()
        {
            string[] libs = null;
            string[] vers = null;
            Core.GetVersions(ref libs, ref vers);

            var libN = new Label[] { labLibName0, labLibName1, labLibName2, labLibName3, labLibName4, labLibName5, labLibName6, labLibName7 };
            var libV = new Label[] { labLibVers0, labLibVers1, labLibVers2, labLibVers3, labLibVers4, labLibVers5, labLibVers6, labLibVers7 };
            var libS = new Label[] { labLibSize0, labLibSize1, labLibSize2, labLibSize3, labLibSize4, labLibSize5, labLibSize6, labLibSize7 };

            if (libs.Length >= libN.Length + 2)
            {
                var fis = (new DirectoryInfo(App.StartParams.FFmpegFolder)).GetFiles();

                labWrapperVersion.Content = vers[0];
                labVersionInfo.Content = vers[1];
                for (int i = 0; i < libN.Length; ++i)
                {
                    var lib = libs[i + 2];
                    libN[i].Content = lib;
                    libV[i].Content = vers[i + 2];
                    libS[i].Content = GetLibSize(lib, fis);
                }
            }
            else
            {
                labVersionInfo.Content = "error";
            }
        }

        static string GetFmt(double sz)
        {
            return (sz < 1.0) ? "F3" : ((sz < 10.0) ? "F2" : "F1");
        }

        static string GetLibSize(string libName, FileInfo[] fis)
        {
            string ff = libName.Substring(3);
            foreach (var fi in fis)
            {
                if (fi.Name.StartsWith(ff, true, CultureInfo.InvariantCulture))
                {
                    double sz = fi.Length / MB;
                    return sz.ToString(GetFmt(sz));
                }
            }
            return "0";
        }

    } // partial class VersionWindow

} // namespace Explorer
