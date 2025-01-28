using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Controls.Primitives;


namespace AppTools
{
    // скиновая кнопка, две картинки - нормальная, неактивная, состояния over, pressed за счет изменения фона
    public class SkinToggleButton4 : ToggleButton
    {
        public static readonly DependencyProperty BkgnCornRadiusProperty;

        public static readonly DependencyProperty SkinNormal1Property;
        public static readonly DependencyProperty SkinDisabled1Property;

        public static readonly DependencyProperty SkinNormal2Property;
        public static readonly DependencyProperty SkinDisabled2Property;

        public static readonly DependencyProperty OverBrushProperty;
        public static readonly DependencyProperty PressedBrushProperty;

        static SkinToggleButton4()
        {
            BkgnCornRadiusProperty = DependencyProperty.Register(
                "BkgnCornRadius", typeof(double), typeof(SkinToggleButton4),
                new UIPropertyMetadata(0.0));

            SkinNormal1Property = DependencyProperty.Register(
                "SkinNormal1", typeof(ImageSource), typeof(SkinToggleButton4),
                new UIPropertyMetadata());

            SkinDisabled1Property = DependencyProperty.Register(
                "SkinDisabled1", typeof(ImageSource), typeof(SkinToggleButton4),
                new UIPropertyMetadata());

            SkinNormal2Property = DependencyProperty.Register(
                "SkinNormal2", typeof(ImageSource), typeof(SkinToggleButton4),
                new UIPropertyMetadata());

            SkinDisabled2Property = DependencyProperty.Register(
                "SkinDisabled2", typeof(ImageSource), typeof(SkinToggleButton4),
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
        {
        }

        public double BkgnCornRadius
        {
            get { return (double)GetValue(BkgnCornRadiusProperty); }
            set { SetValue(BkgnCornRadiusProperty, value); }
        }

        public ImageSource SkinNormal1
        {
            get { return (ImageSource)GetValue(SkinNormal1Property); }
            set { SetValue(SkinNormal1Property, value); }
        }

        public ImageSource SkinDisabled1
        {
            get { return (ImageSource)GetValue(SkinDisabled1Property); }
            set { SetValue(SkinDisabled1Property, value); }
        }

        public ImageSource SkinNormal2
        {
            get { return (ImageSource)GetValue(SkinNormal2Property); }
            set { SetValue(SkinNormal2Property, value); }
        }

        public ImageSource SkinDisabled2
        {
            get { return (ImageSource)GetValue(SkinDisabled2Property); }
            set { SetValue(SkinDisabled2Property, value); }
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
    }

    public class SkinToggleButton : ToggleButton
    {
        public static readonly DependencyProperty SkinNormalProperty;
        public static readonly DependencyProperty SkinDisabledProperty;
        public static readonly DependencyProperty SkinOverProperty;
        public static readonly DependencyProperty SkinPressedProperty;

        public static readonly DependencyProperty SkinNormalChkProperty;
        public static readonly DependencyProperty SkinDisabledChkProperty;
        public static readonly DependencyProperty SkinOverChkProperty;
        public static readonly DependencyProperty SkinPressedChkProperty;

        static SkinToggleButton()
        {
            SkinNormalProperty = DependencyProperty.Register(
                "SkinNormal", typeof(ImageSource), typeof(SkinToggleButton),
                new UIPropertyMetadata());
           
            SkinDisabledProperty = DependencyProperty.Register(
                "SkinDisabled", typeof(ImageSource), typeof(SkinToggleButton),
                new UIPropertyMetadata());
           
            SkinOverProperty = DependencyProperty.Register(
                "SkinOver", typeof(ImageSource), typeof(SkinToggleButton),
                new UIPropertyMetadata());
            
            SkinPressedProperty = DependencyProperty.Register(
                "SkinPressed", typeof(ImageSource), typeof(SkinToggleButton),
                new UIPropertyMetadata());

            SkinNormalChkProperty = DependencyProperty.Register(
                "SkinNormalChk", typeof(ImageSource), typeof(SkinToggleButton),
                new UIPropertyMetadata());

            SkinDisabledChkProperty = DependencyProperty.Register(
                "SkinDisabledChk", typeof(ImageSource), typeof(SkinToggleButton),
                new UIPropertyMetadata());
           
            SkinOverChkProperty = DependencyProperty.Register(
                "SkinOverChk", typeof(ImageSource), typeof(SkinToggleButton),
                new UIPropertyMetadata());
            
            SkinPressedChkProperty = DependencyProperty.Register(
                "SkinPressedChk", typeof(ImageSource), typeof(SkinToggleButton),
                new UIPropertyMetadata());

            DefaultStyleKeyProperty.OverrideMetadata(typeof(SkinToggleButton), new FrameworkPropertyMetadata(typeof(SkinToggleButton)));
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

        public ImageSource SkinOver
        {
            get { return (ImageSource)GetValue(SkinOverProperty); }
            set { SetValue(SkinOverProperty, value); }
        }

        public ImageSource SkinPressed
        {
            get { return (ImageSource)GetValue(SkinPressedProperty); }
            set { SetValue(SkinPressedProperty, value); }
        }

        //

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

        public ImageSource SkinOverChk
        {
            get { return (ImageSource)GetValue(SkinOverChkProperty); }
            set { SetValue(SkinOverChkProperty, value); }
        }

        public ImageSource SkinPressedChk
        {
            get { return (ImageSource)GetValue(SkinPressedChkProperty); }
            set { SetValue(SkinPressedChkProperty, value); }
        }

        public void Toggle()
        {
            IsChecked = !((bool)IsChecked);
        }
    }

} // namespace AppTools
