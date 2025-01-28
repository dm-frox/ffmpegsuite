using System;
using System.Windows;


namespace Player
{
    /// <summary>
    /// Interaction logic for JpegQLevWindow.xaml
    /// </summary>
    public partial class JpegQLevWindow : Window
    {
        public JpegQLevWindow()
        {
            InitializeComponent();
        }

        internal static int Launch(Window owner, int qLev)
        {
            var dlg = new JpegQLevWindow(qLev) { Owner = owner };
            bool? res = dlg.ShowDialog();
            return (bool)res ? (int)dlg.sliderQLev.Value : -1;
        }

        JpegQLevWindow(int qLev)
        {
            InitializeComponent();

            sliderQLev.Value = qLev;
            textSlider.Text = qLev.ToString();
        }

        private void sliderQLev_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (textSlider != null && sliderQLev != null)
            {
                textSlider.Text = ((int)sliderQLev.Value).ToString();
            }
        }

        private void buttonOK_Click(object sender, EventArgs e)
        {
            DialogResult = true;
        }

    } // partial class JpegQLevWindow

} // namespace Player
