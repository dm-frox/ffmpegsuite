using System;
using System.Windows.Controls;

using EffectsLibrary;

namespace Player
{
    class BcsMan
    {
        class SliderVal
        {
            readonly double m_Eps;
            double          m_Value;

            internal SliderVal(double iniVal, double eps)
            {
                m_Value = iniVal;
                m_Eps = eps;
            }

            internal bool SetValue(double val)
            {
                bool ret = false;
                double v = Math.Floor(val / m_Eps) * m_Eps;
                if (Math.Abs(v - m_Value) >= m_Eps)
                {
                    m_Value = v;
                    ret = true;
                }
                return ret;
            }

            internal double Value => m_Value;

            internal string ToStr() => string.Format("{0:F0}", m_Value * 100.0);

        } // nested class SliderVal

        const double DefVal = 0.0;
        const double Eps    = 0.05;

        Control   m_Control;

        Slider    m_SliderB;
        Slider    m_SliderC;
        Slider    m_SliderS;

        SliderVal m_SliderValB;
        SliderVal m_SliderValC;
        SliderVal m_SliderValS;

        BcsEffect m_BcsEff;
        BcsConv   m_BcsConv;

        internal BcsMan(Control control, Slider sliderB, Slider sliderC, Slider sliderS, string psFile)
        {
            m_Control = control;

            m_SliderB = sliderB;
            m_SliderC = sliderC;
            m_SliderS = sliderS;

            m_SliderValB = new SliderVal(DefVal, Eps);
            m_SliderValC = new SliderVal(DefVal, Eps);
            m_SliderValS = new SliderVal(DefVal, Eps);

            m_BcsEff  = new BcsEffect(psFile);
            m_BcsConv = new BcsConv();
        }

        internal void UpdateB(TextBlock text)
        {
            if (m_SliderValB.SetValue(m_SliderB.Value))
            {
                m_BcsEff.Brightness = m_BcsConv.ToBrightness(m_SliderValB.Value);
                text.Text = m_SliderValB.ToStr();
            }
        }

        internal void UpdateC(TextBlock text)
        {
            if (m_SliderValC.SetValue(m_SliderC.Value))
            {
                m_BcsEff.Contrast = m_BcsConv.ToContrast(m_SliderValC.Value);
                text.Text = m_SliderValC.ToStr();
            }
        }

        internal void UpdateS(TextBlock text)
        {
            if (m_SliderValS.SetValue(m_SliderS.Value))
            {
                m_BcsEff.Saturation = m_BcsConv.ToSaturation(m_SliderValS.Value);
                text.Text = m_SliderValS.ToStr();
            }
        }

        internal void Enable(bool enable)
        {
            var eff = m_Control.Effect != null;
            if (enable)
            {
                if (!eff)
                {
                    m_Control.Effect = m_BcsEff;
                }
            }
            else if (eff)
            {
                m_Control.Effect = null;
            }
        }

        internal void Reset()
        {
            Reset(m_SliderB);
            Reset(m_SliderC);
            Reset(m_SliderS);
        }

        internal void Reset(CheckBox checkBox)
        {
            Reset();
            if ((bool)checkBox.IsChecked)
            {
                checkBox.IsChecked = false;
            }
        }

        static void Reset(Slider slider)
        {
            if (slider.Value != DefVal)
            {
                slider.Value = DefVal;
            }
        }

    } // class BcsMan

} // namespace Player
