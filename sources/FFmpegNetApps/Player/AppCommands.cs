using System;
using System.Windows.Input;

namespace Player
{
    static class AppCommands
    {

        private static RoutedUICommand CmdHelp;
        private static RoutedUICommand CmdDocs;

        private static RoutedUICommand CmdMute;
        private static RoutedUICommand CmdVolDown;
        private static RoutedUICommand CmdVolUp;
        private static RoutedUICommand CmdNextFrame;
        private static RoutedUICommand CmdPrevFrame;
        private static RoutedUICommand CmdPlayPause;
        private static RoutedUICommand CmdStop;
        private static RoutedUICommand CmdSaveFrame;
        private static RoutedUICommand CmdRewind;
        private static RoutedUICommand CmdFps;
        private static RoutedUICommand CmdLoadUnload;
        private static RoutedUICommand CmdPix2Pix;

        static InputGestureCollection GetKeyGesture(Key key, ModifierKeys mdfk)
        {
            var g = new InputGestureCollection();
            g.Add(new KeyGesture(key, mdfk));
            return g;
        }

        static AppCommands()
        {

            CmdHelp = new RoutedUICommand("Help", "Help", typeof(AppCommands),
                GetKeyGesture(Key.F1, ModifierKeys.None));

            CmdDocs = new RoutedUICommand("Docs", "Docs", typeof(AppCommands),
                GetKeyGesture(Key.F1, ModifierKeys.Control));

            CmdMute = new RoutedUICommand("Mute", "Mute", typeof(AppCommands),
                GetKeyGesture(Key.F7, ModifierKeys.None));

            CmdVolDown = new RoutedUICommand("VolDown", "VolDown", typeof(AppCommands),
                GetKeyGesture(Key.F8, ModifierKeys.None));

            CmdVolUp = new RoutedUICommand("VolUp", "VolUp", typeof(AppCommands),
                GetKeyGesture(Key.F9, ModifierKeys.None));

            CmdNextFrame = new RoutedUICommand("NextFrame", "NextFrame", typeof(AppCommands),
                GetKeyGesture(Key.F12, ModifierKeys.None));

            CmdPrevFrame = new RoutedUICommand("PrevFrame", "PrevFrame", typeof(AppCommands),
                GetKeyGesture(Key.F11, ModifierKeys.None));

            CmdPlayPause = new RoutedUICommand("PlayPause", "PlayPause", typeof(AppCommands),
                GetKeyGesture(Key.P, ModifierKeys.Control));

            CmdStop = new RoutedUICommand("Stop", "Stop", typeof(AppCommands),
                GetKeyGesture(Key.S, ModifierKeys.Control));

            CmdSaveFrame = new RoutedUICommand("SaveFrame", "SaveFrame", typeof(AppCommands),
                GetKeyGesture(Key.F, ModifierKeys.Control));

            CmdRewind = new RoutedUICommand("Rewind", "Rewind", typeof(AppCommands),
                GetKeyGesture(Key.R, ModifierKeys.Control));

            CmdFps = new RoutedUICommand("Fps", "Fps", typeof(AppCommands),
                GetKeyGesture(Key.U, ModifierKeys.Control));

            CmdLoadUnload = new RoutedUICommand("LoadUnload", "LoadUnload", typeof(AppCommands),
                GetKeyGesture(Key.Enter, ModifierKeys.Control));

            CmdPix2Pix = new RoutedUICommand("Pix2Pix", "Pix2Pix", typeof(AppCommands),
                GetKeyGesture(Key.D2, ModifierKeys.Control));

        }

        public static RoutedUICommand Help => CmdHelp;

        public static RoutedUICommand Docs => CmdDocs;

        public static RoutedUICommand Mute => CmdMute;

        public static RoutedUICommand VolDown => CmdVolDown;

        public static RoutedUICommand VolUp => CmdVolUp;

        public static RoutedUICommand NextFrame => CmdNextFrame;

        public static RoutedUICommand PrevFrame => CmdPrevFrame;

        public static RoutedUICommand PlayPause => CmdPlayPause;

        public static RoutedUICommand Stop => CmdStop;

        public static RoutedUICommand SaveFrame => CmdSaveFrame;

        public static RoutedUICommand Rewind => CmdRewind;

        public static RoutedUICommand Fps => CmdFps;

        public static RoutedUICommand LoadUnload => CmdLoadUnload;

        public static RoutedUICommand Pix2Pix => CmdPix2Pix;

    } // class AppCommands

} // namespace Player

// Комбинации клавиш для быстрого управления проигрывателем Windows Media

// Открытие файла	Ctrl + O
// Воспроизведения или приостановки воспроизведения	Ctrl + P
// Остановка воспроизведения	Ctrl + S
// Отключение/включение звука	F7
// Уменьшения громкости	F8
// Увеличение громкости	F9

