#include "pch.h"

#include "StrBuilder.h"

namespace Fcore
{

    StrBuilder::StrBuilder(int capacity, const char* sepr)
        : m_Sepr(sepr)
    {
        m_Str.reserve(capacity);
    }

    StrBuilder::StrBuilder(int capacity)
        : StrBuilder(capacity, ", ")
    {}

    StrBuilder::~StrBuilder() = default;

    void StrBuilder::Append(const char* item)
    {
        if (item && *item)
        {
            if (!m_Str.empty())
            {
                m_Str += m_Sepr;
            }
            m_Str += item;
        }
    }

    std::string StrBuilder::Str() const
    {
        return m_Str;
    }

    std::string StrBuilder::FormatBytes(const void* bytes, int count)
    {
        using pcbyte_t = const unsigned char*;
        constexpr unsigned int msHalfMask = 0xf0;
        constexpr unsigned int lsHalfMask = 0x0f;
        constexpr unsigned int HalfByteSize = 4;

        std::string ret;
        ret.reserve(count * 3);
        for (pcbyte_t b = static_cast<pcbyte_t>(bytes), end = b + count; b < end; ++b)
        {
            unsigned int u = *b;
            ret += Hex((u & msHalfMask) >> HalfByteSize);
            ret += Hex(u & lsHalfMask);
            ret += ' ';
        }
        return ret;
    }

} // namespace Fcore