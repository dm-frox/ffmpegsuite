#include "pch.h"

#include "FilterTools.h"

#include "Misc.h"
#include "ChannLayout.h"


namespace
{
    template <size_t N>
    class CharBuff
    {
        char m_Buff[N]{ '\0' };

    protected:
        static const size_t BuffSize = N;

        CharBuff() = default;
        ~CharBuff() = default;

        char* Buff() { return m_Buff; }

    public:
        operator const char*() const { return m_Buff; }
        const char* Chars() const { return m_Buff; }
    };

    class IntStr : public CharBuff<32>
    {
    public:
        explicit IntStr(int val)
        {
            av_strlcatf(Buff(), BuffSize, "%d", val);
        }
    };

    class ChannLayoutStr : public CharBuff<64>
    {
    public:
        //explicit ChannLayoutStr(int64_t layout)
        //{
        //    av_get_channel_layout_string(Buff(), BuffSize, -1, layout);
        //}

        explicit ChannLayoutStr(const Fcore::ChannLayout& channLayout)
        {
            //av_channel_layout_describe(chLayt, Buff(), BuffSize);
            channLayout.ToString(Buff(), BuffSize);
        }
    };

} // namespace anon

namespace Fcore
{
    // static 
    const char FilterTools::FiltSepr = ',';
    const char FilterTools::ParmSepr = ':';

 
    //static 
    void FilterTools::AppendFiltSepr(std::string& str)
    {
        if (!str.empty())
        {
            str += FiltSepr;
        }
    }

    //static 
    void FilterTools::AppendFilter(std::string& str, const std::string& flt)
    {
        if (!flt.empty())
        {
            AppendFiltSepr(str);
            str += flt;
        }
    }

    //static 
    void FilterTools::AppendFps(std::string& str, int fps)
    {
        if (fps > 0)
        {
            AppendFiltSepr(str);
            str += "fps=fps=";
            str += IntStr(fps);
        }
    }

    //static 
    void FilterTools::AppendScale(std::string& str, int width, int height)
    {
        if (width > 0 && height > 0)
        {
            AppendFiltSepr(str);
            str += "scale=";
            str += "w=";
            str += IntStr(width);
            str += ParmSepr;
            str += "h=";
            str += IntStr(height);
        }
    }

    //static 
    std::string FilterTools::GetFilterStringVideo(int width, int height, const std::string& flt)
    {
        std::string ret;
        ret.reserve(64);

        AppendScale(ret, width, height);
        AppendFilter(ret, flt);

        return ret;
    }

    //static 
    //void FilterTools::AppendAformat(std::string& str, int sampleRate, int64_t channLayout)
    void FilterTools::AppendAformat(std::string& str, int sampleRate, const ChannLayout& channLayout)
    {
        bool s = (sampleRate > 0),
             y = !channLayout.IsNull();

        if (s || y)
        {
            AppendFiltSepr(str);
            str += "aformat=";
            if (s)
            {
                str += "sample_rates=";
                str += IntStr(sampleRate);
            }
            if (y)
            {
                if (s)
                {
                    str += ParmSepr;
                }
                str += "channel_layouts=";
                str += ChannLayoutStr(channLayout);
            }
        }
    }

    //static 
    void FilterTools::AppendFrameSize(std::string& str, int frameSize)
    {
        if (frameSize > 0)
        {
            AppendFiltSepr(str);
            str += "asetnsamples=n=";
            str += IntStr(frameSize);
            str += ":p=0";
        }
    }

    // static 
    std::string FilterTools::GetFilterStringAudio(int sampleRate, const ChannLayout& channLayout, const std::string& flt, int frameSize)
    {
        std::string ret;
        ret.reserve(64);

        AppendAformat(ret, sampleRate, channLayout);
        AppendFilter(ret, flt);
        AppendFrameSize(ret, frameSize);

        return ret;
    }

    // static 
    void FilterTools::FillInFilterArgsVideo(int width, int height, AVPixelFormat pixFmt, AVRational timeBase, AVRational frameRate,
        char* buff, int buffSize)
    {
        buff[0] = 0;
        av_strlcatf(buff, buffSize,
            "width=%d:height=%d:pix_fmt=%s:time_base=%d/%d",
            width, height, PixFmtToStr(pixFmt),
            timeBase.num, timeBase.den
        );

        if (frameRate.num && frameRate.den)
        {
            av_strlcatf(buff, buffSize,
                ":frame_rate=%d/%d",
                frameRate.num, frameRate.den);
        }
    }

    // static 
    void  FilterTools::FillInFilterArgsAudio(const ChannLayout& channLayout, int sampleRate, AVSampleFormat sampFmt,
        char* buff, int buffSize)
    {
        ChannLayoutStr chs(channLayout);
        buff[0] = 0;
        av_strlcatf(buff, buffSize,
            "channels=%d:sample_rate=%d:sample_fmt=%s:channel_layout=%s",
            channLayout.Chann(),
            sampleRate,
            SampFmtToStr(sampFmt),
            chs.Chars()
        );
    }
    
} // namespace Fcore

