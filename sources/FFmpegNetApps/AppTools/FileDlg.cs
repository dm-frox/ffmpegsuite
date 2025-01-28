using System;
using System.Windows;
using System.IO;

using Microsoft.Win32;


namespace AppTools
{
    public static class FileDlg
    {
        const string VideoAudioFilesFilter =
            "Video files (*.avi;*.mp4;*.mkv;*.ogg;*.wmv;*.asf;*.mov;*.flv;*.3gp;*.mpg;*.webm;*.m4v;*.ogv;*.swf;*.vob;*.m2ts;*.ts;*.rm)|" +
                         "*.avi;*.mp4;*.mkv;*.ogg;*.wmv;*.asf;*.mov;*.flv;*.3gp;*.mpg;*.webm;*.m4v;*.ogv;*.swf;*.vob;*.m2ts;*.ts;*.rm|" +
            "Audio files (*.wav;*.mp3;*.wma;*.oga;*.opus;*.m4a;*.aif;*.aac;*.mp2;*.ac3;*.amr;*.flac)|" +
                         "*.wav;*.mp3;*.wma;*.oga;*.opus;*.m4a;*.aif;*.aac;*.mp2;*.ac3;*.amr;*.flac|";

        const string AllFilesFilter = "All files (*.*)|*.*";

        const string MediaFilesFilter =
             VideoAudioFilesFilter +
             "Text files (*.txt)|*.txt|" +
             AllFilesFilter;

        const string MediaFilesFilterTrans =
             VideoAudioFilesFilter +
             AllFilesFilter;

        const string SdpFilesFilter =
            "Ses. Descr. Prot. files (*.sdp)|*.sdp|" +
            AllFilesFilter;

        const string TextFilesFilter =
            "Text files (*.txt)|*.txt|" +
            AllFilesFilter;

        const string FrameFilesFilter =
            "PNG image files (*.png)|*.png|Jpeg image files (*.jpg)|*.jpg";

        readonly static string[] FrameFileTypes = {"", "png", "jpg" };

// --------------------------------------------------------------------
// open

        static bool LaunchOpenFileDialog(Window owner, string filter, int filtIdx, string iniFolder, ref string path)
        {
            bool ret = false;
            var dlg = new OpenFileDialog();
            dlg.Filter = filter;
            dlg.FilterIndex = filtIdx;
            dlg.CheckFileExists = true;
            if (!string.IsNullOrEmpty(iniFolder))
            {
                dlg.InitialDirectory = iniFolder;
            }
            if ((bool)dlg.ShowDialog(owner))
            {
                path = dlg.FileName;
                ret = true;
            }
            return ret;
        }

        public static bool LaunchOpenFileDialog(Window owner, ref string path)
        {
            return LaunchOpenFileDialog(owner, MediaFilesFilter, 0, null, ref path);
        }

        public static bool LaunchOpenFileDialogRtp(Window owner, ref string path)
        {
            return LaunchOpenFileDialog(owner, SdpFilesFilter, 0, null, ref path);
        }

        public static bool LaunchOpenFileDialog(Window owner, int filtIdx, ref string path)
        {
            return LaunchOpenFileDialog(owner, MediaFilesFilter, filtIdx, null, ref path);
        }

// --------------------------------------------------------------------
// save

        static bool LaunchSaveFileDialog(Window owner, string filter, string iniFolder, bool overwritePrompt, ref string path, ref int filtIndex)
        {
            bool ret = false;
            var dlg = new SaveFileDialog();
            dlg.Filter = filter;
            dlg.OverwritePrompt = overwritePrompt;
            if (!string.IsNullOrEmpty(iniFolder))
            {
                dlg.InitialDirectory = iniFolder;
            }
            if ((bool)dlg.ShowDialog(owner))
            {
                path = dlg.FileName;
                filtIndex = dlg.FilterIndex; // one based !!
                ret = true;
            }
            return ret;
        }

        public static bool LaunchSaveFileDialog(Window owner, string filter, bool overwritePrompt, ref string path)
        {
            int filtIndex = -1;
            return LaunchSaveFileDialog(owner, filter, string.Empty, overwritePrompt, ref path, ref filtIndex);
        }

        public static bool LaunchSaveFileDialogTrans(Window owner, bool overwritePrompt, ref string path)
        {
            return LaunchSaveFileDialog(owner, MediaFilesFilterTrans, overwritePrompt, ref path);
        }

        public static bool LaunchSaveFileDialogFrame(Window owner, ref string path, ref string fileType)
        {
            bool ret = false;
            int filtIndex = 0;
            if (LaunchSaveFileDialog(owner, FrameFilesFilter, string.Empty, true, ref path, ref filtIndex))
            {
                fileType = FrameFileTypes[(0 <= filtIndex && filtIndex < FrameFileTypes.Length) ? filtIndex : 0];
                ret = true;
            }
            return ret;
        }

        public static bool LaunchSaveFileDialogRtp(Window owner, ref string path)
        {
            return LaunchSaveFileDialog(owner, SdpFilesFilter, false, ref path);
        }

        public static bool LaunchSaveFileDialogTxt(Window owner, ref string path)
        {
            return LaunchSaveFileDialog(owner, TextFilesFilter, true, ref path);
        }

        public static void SaveInTxt(Window owner, Action<StreamWriter> save)
        {
            string path = null;
            if (LaunchSaveFileDialogTxt(owner, ref path))
            {
                using (StreamWriter sw = File.CreateText(path))
                {
                    save.Invoke(sw);
                }
            }
        }

    } // static class FileDlg

} // namespace AppTools

