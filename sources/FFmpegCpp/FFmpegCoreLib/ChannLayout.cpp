#include "pch.h"

#include "ChannLayout.h"

namespace Fcore
{
    //static 
    const ChannLayout ChannLayout::Null{};


    ChannLayout::ChannLayout(int chann)
    {
        av_channel_layout_default(&m_ChannLayout, chann);
    }

    ChannLayout::~ChannLayout()
    {
        Clear();
    }

    void ChannLayout::Clear()
    {
        av_channel_layout_uninit(&m_ChannLayout);
    }

    int ChannLayout::Set(const AVChannelLayout* src)
    {
        int ret = av_channel_layout_copy(&m_ChannLayout, src);
        if (ret < 0)
        {
            Clear();
        }
        return ret;
    }

    int ChannLayout::Set(const ChannLayout& src)
    {
        return Set(src.ToPtr());
    }

    int ChannLayout::SetFromString(const char* str)
    {
        int ret = av_channel_layout_from_string(&m_ChannLayout, str);
        if (ret < 0)
        {
            Clear();
        }
        return ret;
    }

    int ChannLayout::SetFromChann(int chann)
    {
        av_channel_layout_default(&m_ChannLayout, chann);
        return 0;
    }

    int ChannLayout::ToString(char* buff, int bsize) const
    {
        return av_channel_layout_describe(&m_ChannLayout, buff, bsize);
    }

    bool ChannLayout::operator==(const ChannLayout& other) const
    {
        return av_channel_layout_compare(&m_ChannLayout, other.ToPtr()) == 0;
    }

    int ChannLayout::CopyTo(AVChannelLayout& channLayout) const
    {
        return av_channel_layout_copy(&channLayout, &m_ChannLayout);
    }

    //static 
    const AVChannelLayout* ChannLayout::Find(const AVChannelLayout* vals, const ChannLayout& prefVal)
    {
        if (!vals)
        {
            return Null.ToPtr();
        }
        for (const AVChannelLayout* pval = vals; ; ++pval)
        {
            ChannLayout val;
            val.Set(pval);
            if (val.IsNull())
            {
                return vals;
            }
            else if (val == prefVal)
            {
                return prefVal.ToPtr();
            }
        }
    }

} // namespace Fcore