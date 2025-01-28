#include "pch.h"

#include "PixFmtList.h"

#include "Misc.h"


namespace Flist
{
    char Buff[128];

    // static
    uint32_t PixFmtInfo::PackDepth(const AVPixFmtDescriptor* pfd)
    {
        uint32_t ret = 0;
        for (int i = 0; i < 4; ++i)
        {
            ret |= pfd->comp[i].depth << i * 8;
        }
        return ret;
    }

    PixFmtInfo::PixFmtInfo(const AVPixFmtDescriptor* pfd)
        : m_PixFmt(av_pix_fmt_desc_get_id(pfd))
        , m_Name(pfd->name)
        , m_Descr(av_get_pix_fmt_string(Buff, sizeof(Buff), m_PixFmt))
        , m_CompCount(pfd->nb_components)
        , m_PlaneCount(av_pix_fmt_count_planes(m_PixFmt))
        , m_BitPerPix(av_get_bits_per_pixel(pfd))
        , m_HWAccel(Fcore::HasFlag(pfd->flags, AV_PIX_FMT_FLAG_HWACCEL))
        , m_Palette(Fcore::HasFlag(pfd->flags, AV_PIX_FMT_FLAG_PAL))
        , m_Bitstream(Fcore::HasFlag(pfd->flags, AV_PIX_FMT_FLAG_BITSTREAM))
        , m_CompDepth(PackDepth(pfd))
    {}

    PixFmtInfo::~PixFmtInfo() = default;

    const char* PixFmtInfo::Name() const { return m_Name; }
    const char* PixFmtInfo::Descr() const { return m_Descr.c_str(); }
    int  PixFmtInfo::CompCount() const { return m_CompCount; }
    int  PixFmtInfo::PlaneCount() const { return m_PlaneCount; }
    int  PixFmtInfo::BitPerPix() const { return m_BitPerPix; }
    bool PixFmtInfo::HWAccel() const { return m_HWAccel; }
    bool PixFmtInfo::Palette() const { return m_Palette; }
    bool PixFmtInfo::Bitstream() const { return m_Bitstream; }
    uint32_t PixFmtInfo::CompDepth() const { return m_CompDepth; }

// -----------------------------------------------------------------

    PixFmtList::PixFmtList() = default;
    PixFmtList::~PixFmtList() = default;

    // static
    size_t PixFmtList::GetCapacity()
    {
        int cntr = 0;
        const AVPixFmtDescriptor* pfd = nullptr;
        while (pfd = av_pix_fmt_desc_next(pfd))
        {
            ++cntr;
        }
        return cntr + 1;
    }

#pragma warning(disable:4706)
    void PixFmtList::Setup()
    {
        if (IsEmpty())
        {
            Reserve(GetCapacity());
            const AVPixFmtDescriptor* pfd = nullptr;
            while (pfd = av_pix_fmt_desc_next(pfd))
            {
                List().emplace_back(pfd);
            }
        }
#pragma warning(default:4706)
    }

} // namespace Flist


// AV_PIX_FMT_FLAG_PAL
// Pixel format has a palette in data[1], values are indexes in this palette.
//
// AV_PIX_FMT_FLAG_PLANAR
// At least one pixel component is not in the first data plane.
//
// AV_PIX_FMT_FLAG_BITSTREAM
// All values of a component are bit-wise packed end to end.