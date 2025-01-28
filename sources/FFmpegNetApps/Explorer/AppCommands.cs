using System;
using System.Windows.Input;

namespace Explorer
{
    public static class AppCommands
    {

        private static RoutedUICommand CmdHelp;
        private static RoutedUICommand CmdDocs;

        static InputGestureCollection GetKeyGesture(Key key, ModifierKeys mdfk)
        {
            InputGestureCollection g = new InputGestureCollection();
            g.Add(new KeyGesture(key, mdfk));
            return g;
        }

        static AppCommands()
        {

            CmdHelp = new RoutedUICommand("Help", "Help", typeof(AppCommands),
                GetKeyGesture(Key.F1, ModifierKeys.None));

            CmdDocs = new RoutedUICommand("Docs", "Docs", typeof(AppCommands),
                GetKeyGesture(Key.F1, ModifierKeys.Control));
        }

        public static RoutedUICommand Help => CmdHelp;
        public static RoutedUICommand Docs => CmdDocs;

    } // class AppCommands

} // namespace Explorer

