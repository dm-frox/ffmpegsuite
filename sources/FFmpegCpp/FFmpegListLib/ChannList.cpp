#include "pch.h"

#include "ChannList.h"


namespace Flist
{
    ChannInfo::ChannInfo(const AVChannelLayout* channLayout)
        : m_Chann(channLayout->nb_channels)
        , m_Mask(channLayout->u.mask)
    {
        m_Name.reserve(32);
        m_Descr.reserve(32);
        char buff[64];
        av_channel_layout_describe(channLayout, buff, sizeof(buff));
        m_Name = buff;
        const uint64_t one = 1;
        for (int i = 0, k = 63; i < 64; ++i, --k)
        {
            if ((one << k) & m_Mask)
            {
                if (av_channel_name(buff, sizeof(buff), (AVChannel)k) >= 0)
                {
                    if (!m_Descr.empty())
                    {
                        m_Descr += ',';
                    }
                    m_Descr += buff;
                }
            }
        }
    }

    ChannInfo::~ChannInfo() = default;

    const char* ChannInfo::Name() const { return m_Name.c_str(); }
    const char* ChannInfo::Descr() const { return m_Descr.c_str(); }
    int ChannInfo::Chann() const { return m_Chann; }
    uint64_t    ChannInfo::Mask() const { return m_Mask; }

// ----------------------------------------------------------------------

    ChannList::ChannList() = default;
    ChannList::~ChannList() = default;

    // static
    size_t ChannList::GetCapacity()
    {
        int cntr = 0;
        void* ctx = nullptr;
        while (av_channel_layout_standard(&ctx))
        {
            ++cntr;
        }
        return cntr + 1;
    }

    void ChannList::Setup()
    {
        if (IsEmpty())
        {
            Reserve(GetCapacity());
            void* ctx = nullptr;
            while (const AVChannelLayout* channLayout = av_channel_layout_standard(&ctx))
            {
                List().emplace_back(channLayout);
            }
        }
    }

} // namespace Flist