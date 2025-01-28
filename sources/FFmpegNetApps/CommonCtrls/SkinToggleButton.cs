using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Controls.Primitives;


namespace CommonCtrls
{
// Skin toggle button, four skins - normal, inactive, normal checked, inactive checked.
// States normal over, normal checked over, normal pressed, normal checked pressed due to background changes.

    public class SkinToggleButton4 : ToggleButton
    {
        public static readonly DependencyProperty BkgnCornRadiusProperty;

        public static readonly DependencyProperty SkinNormalProperty;
        public static readonly DependencyProperty SkinDisabledProperty;

        public static readonly DependencyProperty SkinNormalChkProperty;
        public static readonly DependencyProperty SkinDisabledChkProperty;

        public static readonly DependencyProperty OverBrushProperty;
        public static readonly DependencyProperty PressedBrushProperty;

        static SkinToggleButton4()
        {
            BkgnCornRadiusProperty = DependencyProperty.Register(
                "BkgnCornRadius", typeof(double), typeof(SkinToggleButton4),
                new UIPropertyMetadata(0.0));

            SkinNormalProperty = DependencyProperty.Register(
                "SkinNormal", typeof(ImageSource), typeof(SkinToggleButton4),
                new UIPropertyMetadata());

            SkinDisabledProperty = DependencyProperty.Register(
                "SkinDisabled", typeof(ImageSource), typeof(SkinToggleButton4),
                new UIPropertyMetadata());

            SkinNormalChkProperty = DependencyProperty.Register(
                "SkinNormalChk", typeof(ImageSource), typeof(SkinToggleButton4),
                new UIPropertyMetadata());

            SkinDisabledChkProperty = DependencyProperty.Register(
                "SkinDisabledChk", typeof(ImageSource), typeof(SkinToggleButton4),
                new UIPropertyMetadata());

            OverBrushProperty = DependencyProperty.Register(
                "OverBrush", typeof(Brush), typeof(SkinToggleButton4),
                new UIPropertyMetadata(Brushes.White));

            PressedBrushProperty = DependencyProperty.Register(
                "PressedBrush", typeof(Brush), typeof(SkinToggleButton4),
                new UIPropertyMetadata(Brushes.WhiteSmoke));

            DefaultStyleKeyProperty.OverrideMetadata(typeof(SkinToggleButton4),
                new FrameworkPropertyMetadata(typeof(SkinToggleButton4)));
        }

        public SkinToggleButton4()
        {}

        public double BkgnCornRadius
        {
            get { return (double)GetValue(BkgnCornRadiusProperty); }
            set { SetValue(BkgnCornRadiusProperty, value); }
        }

        public ImageSource SkinNormal
        {
            get { return (ImageSource)GetValue(SkinNormalProperty); }
            set { SetValue(SkinNormalProperty, value); }
        }

        public ImageSource SkinDisabled
        {
            get { return (ImageSource)GetValue(SkinDisabledProperty); }
            set { SetValue(SkinDisabledProperty, value); }
        }

        public ImageSource SkinNormalChk
        {
            get { return (ImageSource)GetValue(SkinNormalChkProperty); }
            set { SetValue(SkinNormalChkProperty, value); }
        }

        public ImageSource SkinDisabledChk
        {
            get { return (ImageSource)GetValue(SkinDisabledChkProperty); }
            set { SetValue(SkinDisabledChkProperty, value); }
        }

        public Brush OverBrush
        {
            get { return (Brush)GetValue(OverBrushProperty); }
            set { SetValue(OverBrushProperty, value); }
        }

        public Brush PressedBrush
        {
            get { return (Brush)GetValue(PressedBrushProperty); }
            set { SetValue(PressedBrushProperty, value); }
        }
    } // SkinToggleButton4

} // namespace CommonCtrls
