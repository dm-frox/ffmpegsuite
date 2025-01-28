using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;


namespace CommonCtrls
{
// Skin button, two skins - normal, inactive.
// States normal over, normal pressed due to background changes.

    public class SkinButton2 : Button
    {
        public static readonly DependencyProperty BkgnCornRadiusProperty;
 
        public static readonly DependencyProperty SkinNormalProperty;
        public static readonly DependencyProperty SkinDisabledProperty;

        public static readonly DependencyProperty OverBrushProperty;
        public static readonly DependencyProperty PressedBrushProperty;

        static SkinButton2()
        {
            BkgnCornRadiusProperty = DependencyProperty.Register(
                "BkgnCornRadius", typeof(double), typeof(SkinButton2), 
                new UIPropertyMetadata(0.0));

            SkinNormalProperty = DependencyProperty.Register(
                "SkinNormal", typeof(ImageSource), typeof(SkinButton2),
                new UIPropertyMetadata());

            SkinDisabledProperty = DependencyProperty.Register(
                "SkinDisabled", typeof(ImageSource), typeof(SkinButton2),
                new UIPropertyMetadata());

            OverBrushProperty = DependencyProperty.Register(
                "OverBrush", typeof(Brush), typeof(SkinButton2),
                new UIPropertyMetadata(Brushes.White));
           
            PressedBrushProperty = DependencyProperty.Register(
                "PressedBrush", typeof(Brush), typeof(SkinButton2),
                new UIPropertyMetadata(Brushes.WhiteSmoke));

            DefaultStyleKeyProperty.OverrideMetadata(typeof(SkinButton2), 
                new FrameworkPropertyMetadata(typeof(SkinButton2)));
        }

        public SkinButton2()
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

    } // class SkinButton2

} // namespace CommonCtrls



