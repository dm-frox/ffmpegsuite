using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;


namespace AppTools
{
// скиновая кнопка, две картинки - нормальная, неактивная, состояния over, pressed за счет изменения фона
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
        {
        }

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
    }

// скиновая кнопка, 4 картинки
    public class SkinButton : Button
    {
        public static readonly DependencyProperty SkinNormalProperty;
        public static readonly DependencyProperty SkinDisabledProperty;

        public static readonly DependencyProperty SkinOverProperty;
        public static readonly DependencyProperty SkinPressedProperty;

        static SkinButton()
        {
            SkinNormalProperty = DependencyProperty.Register(
                "SkinNormal", typeof(ImageSource), typeof(SkinButton),
                new UIPropertyMetadata());
            
            SkinDisabledProperty = DependencyProperty.Register(
                "SkinDisabled", typeof(ImageSource), typeof(SkinButton),
                new UIPropertyMetadata());

            SkinOverProperty = DependencyProperty.Register(
                "SkinOver", typeof(ImageSource), typeof(SkinButton),
                new UIPropertyMetadata());
            
            SkinPressedProperty = DependencyProperty.Register(
                "SkinPressed", typeof(ImageSource), typeof(SkinButton),
                new UIPropertyMetadata());

            DefaultStyleKeyProperty.OverrideMetadata(typeof(SkinButton), 
                new FrameworkPropertyMetadata(typeof(SkinButton)));
        }

        public SkinButton()
        {
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
    }

    public class FlatButton : Button
    {
        public static readonly DependencyProperty BorderCornRadiusProperty;

        public static readonly DependencyProperty OverBrushProperty;
        public static readonly DependencyProperty PressedBrushProperty;
        public static readonly DependencyProperty DisabledBrushProperty;

        static FlatButton()
        {
            BorderCornRadiusProperty = DependencyProperty.Register(
                "BorderCornRadius", typeof(double), typeof(FlatButton),
                new UIPropertyMetadata(0.0));

            OverBrushProperty = DependencyProperty.Register(
                "OverBrush", typeof(Brush), typeof(FlatButton),
                new UIPropertyMetadata(Brushes.Gold));

            PressedBrushProperty = DependencyProperty.Register(
                "PressedBrush", typeof(Brush), typeof(FlatButton),
                new UIPropertyMetadata(Brushes.Orange));

            DisabledBrushProperty = DependencyProperty.Register(
                "DisabledBrush", typeof(Brush), typeof(FlatButton),
                new UIPropertyMetadata(Brushes.DarkGray));

            DefaultStyleKeyProperty.OverrideMetadata(typeof(FlatButton),
                new FrameworkPropertyMetadata(typeof(FlatButton)));
        }

        public FlatButton()
        {
        }

        public double BorderCornRadius
        {
            get { return (double)GetValue(BorderCornRadiusProperty); }
            set { SetValue(BorderCornRadiusProperty, value); }
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

        public Brush DisabledBrush
        {
            get { return (Brush)GetValue(DisabledBrushProperty); }
            set { SetValue(DisabledBrushProperty, value); }
        }
    }

} // namespace AppTools


// xmlns:comctrls="clr-namespace:CommonCtrls;assembly=CommonCtrls"

//< comctrls:SkinButton2 Name = "buttonStop" 
//    Canvas.Left="24"  Canvas.Top="3"
//    Width="20" Height="20"
//    SkinNormal="Images2/stop20.png" SkinDisabled="Images2/stop20_D.png"                 
//    IsEnabled="{Binding Path=CanStop}" ToolTip="{x:Static loc:ResStr.TpStop}"
//    Click="buttonStop_Click" />
