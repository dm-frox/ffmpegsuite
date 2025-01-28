using System;
using System.Windows.Data;

using Culture = System.Globalization.CultureInfo;

namespace EffectsLibrary
{
// converts values from [-1.0,  1.0] to target ones. 0.0 means no effect
    public class BcsConv
    {
        internal const double DefBrightnessCoeff = 0.5;
        internal const double DefContrastCoeff   = 2.5;
        internal const double DefSaturationCoeff = 2.5;

        double m_BrightnessCoeff;
        double m_ContrastCoeff;
        double m_SaturationCoeff;

        static double ExpParam(double val, double coeff)
        {
            return Math.Exp(val * coeff);
        }
        
        public BcsConv()
        {
            m_BrightnessCoeff = DefBrightnessCoeff;
            m_ContrastCoeff   = DefContrastCoeff;
            m_SaturationCoeff = DefSaturationCoeff;
        }

        public double ToBrightness(double val)
        {
            return val * m_BrightnessCoeff;
        }

        public double ToContrast(double val)
        {
            return ExpParam(val, m_ContrastCoeff);
        }

        public double ToSaturation(double val)
        {
            return ExpParam(val, m_SaturationCoeff);
        }

        public double BrightnessCoeff
        {
            get { return m_BrightnessCoeff; }
            set { m_BrightnessCoeff = value; }
        }

        public double ContrastCoeff
        {
            get { return m_ContrastCoeff; }
            set { m_ContrastCoeff = value; }
        }

        public double SaturationCoeff
        {
            get { return m_SaturationCoeff; }
            set { m_SaturationCoeff = value; }
        }

    } //  class BcsConv

// for XAML binding, converts values from [-1.0,  1.0] to target ones. 0.0 means no effect
    [ValueConversion(typeof(double), typeof(double))]
    public class BrightnessConverter : IValueConverter
    {
        double m_BrightnessCoeff = BcsConv.DefBrightnessCoeff;

        public double BrightnessCoeff
        {
            get { return m_BrightnessCoeff; }
            set { m_BrightnessCoeff = value; }
        }

        public object Convert(object value, Type targetType, object parameter, Culture culture)
        {
            return (double)value * m_BrightnessCoeff;
        }

        public object ConvertBack(object value, Type targetType, object parameter, Culture culture)
        {
            return null;
        }

    } // class BrightnessConverter

    [ValueConversion(typeof(double), typeof(double))]
    public class ContrastConverter : IValueConverter
    {
        double m_ContrastCoeff = BcsConv.DefContrastCoeff;

        public double ContrastCoeff
        {
            get { return m_ContrastCoeff; }
            set { m_ContrastCoeff = value; }
        }

        public object Convert(object value, Type targetType, object parameter, Culture culture)
        {
            return Math.Exp((double)value * m_ContrastCoeff);
        }

        public object ConvertBack(object value, Type targetType, object parameter, Culture culture)
        {
            return null;
        }

    } // class ContrastConverter

    [ValueConversion(typeof(double), typeof(double))]
    public class SaturationConverter : IValueConverter
    {
        double m_SaturationCoeff = BcsConv.DefSaturationCoeff;

        public double SaturationCoeff
        {
            get { return m_SaturationCoeff; }
            set { m_SaturationCoeff = value; }
        }

        public object Convert(object value, Type targetType, object parameter, Culture culture)
        {
            return Math.Exp((double)value * m_SaturationCoeff);
        }

        public object ConvertBack(object value, Type targetType, object parameter, Culture culture)
        {
            return null;
        }

    } // class SaturationConverter

} // namespace EffectsLibrary
