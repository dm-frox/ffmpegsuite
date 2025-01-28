using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Effects;

namespace EffectsLibrary
{
    class BcsEffect : ShaderEffect
    {
        public static readonly DependencyProperty InputProperty;

        public static readonly DependencyProperty BrightnessProperty;
        public static readonly DependencyProperty ContrastProperty;
        public static readonly DependencyProperty SaturationProperty;

        static BcsEffect()
        {
            InputProperty = ShaderEffect.RegisterPixelShaderSamplerProperty("Input", typeof(BcsEffect), 0);

            BrightnessProperty = DependencyProperty.Register("Brightness",
                typeof(double), typeof(BcsEffect),
                new UIPropertyMetadata(0.0, PixelShaderConstantCallback(0)));
            ContrastProperty = DependencyProperty.Register("Contrast",
                typeof(double), typeof(BcsEffect),
                new UIPropertyMetadata(1.0, PixelShaderConstantCallback(1)));
            SaturationProperty = DependencyProperty.Register("Saturation",
                typeof(double), typeof(BcsEffect),
                new UIPropertyMetadata(1.0, PixelShaderConstantCallback(2)));
        }

        public BcsEffect(string psFile)
        {
// The inherited property PixelShader must be set
            this.PixelShader = new PixelShader();
            this.PixelShader.UriSource = new Uri(psFile, UriKind.Absolute);
            this.PixelShader.ShaderRenderMode = ShaderRenderMode.HardwareOnly;

// Force the pixel shader to update all input values
            UpdateShaderValue(InputProperty);

            UpdateShaderValue(BrightnessProperty);
            UpdateShaderValue(ContrastProperty);
            UpdateShaderValue(SaturationProperty);
        }


// Implicit input
        public Brush Input
        {
            get { return (Brush)GetValue(InputProperty); }
            set { SetValue(InputProperty, value); }
        }
// Custom shader parameter
        public double Brightness
        {
            get { return (double)GetValue(BrightnessProperty); }
            set { SetValue(BrightnessProperty, value); }
        }

        public double Contrast
        {
            get { return (double)GetValue(ContrastProperty); }
            set { SetValue(ContrastProperty, value); }
        }

        public double Saturation
        {
            get { return (double)GetValue(SaturationProperty); }
            set { SetValue(SaturationProperty, value); }
        }

    } // class BcsEffect

} // namespace EffectsLibrary
