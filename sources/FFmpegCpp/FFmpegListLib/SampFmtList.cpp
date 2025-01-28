#include "pch.h"

#include "SampFmtList.h"

#include "ArrSize.h"


namespace Flist
{
	SampFmtInfo::SampFmtInfo(AVSampleFormat sampFmt, const char* decr)
		: m_Name(av_get_sample_fmt_name(sampFmt))
        , m_Descr(decr)
        , m_BytesPerSamp(av_get_bytes_per_sample(sampFmt))
		, m_IsPlanar(av_sample_fmt_is_planar(sampFmt))
    {}

	SampFmtInfo::~SampFmtInfo() = default;

    const char* SampFmtInfo::Name() const { return m_Name; }
    const char* SampFmtInfo::Descr() const { return m_Descr; }
    int  SampFmtInfo::BytesPerSamp() const { return m_BytesPerSamp; }
    bool SampFmtInfo::IsPlanar() const { return m_IsPlanar; }

    const SampFmtInfo Fmts[] =
    {
        {AV_SAMPLE_FMT_U8,   "unsigned 8 bits" },
        {AV_SAMPLE_FMT_S16,  "signed 16 bits" },
        {AV_SAMPLE_FMT_S32,  "signed 32 bits" },
        {AV_SAMPLE_FMT_FLT,  "float" },
        {AV_SAMPLE_FMT_DBL,  "double" },

        {AV_SAMPLE_FMT_U8P,  "unsigned 8 bits, planar" },
        {AV_SAMPLE_FMT_S16P, "signed 16 bits, planar" },
        {AV_SAMPLE_FMT_S32P, "signed 32 bits, planar" },
        {AV_SAMPLE_FMT_FLTP, "float, planar" },
        {AV_SAMPLE_FMT_DBLP, "double, planar" },

        {AV_SAMPLE_FMT_S64,  "signed 64 bits" },
        {AV_SAMPLE_FMT_S64P, "signed 64 bits, planar" },
    };

// ----------------------------------------------------------------------

    SampFmtList::SampFmtList() = default;

    SampFmtList::~SampFmtList() = default;

    void SampFmtList::Setup()
    {
        if (IsEmpty())
        {
            Reserve(Ftool::Size(Fmts));
            for (const SampFmtInfo& sfi : Fmts)
            {
                List().push_back(sfi);
            }
        }
    }

} // namespace Flist 

/*
    libavutil/samplefmt.h v. 7.0

    enum AVSampleFormat {
    AV_SAMPLE_FMT_NONE = -1,
    AV_SAMPLE_FMT_U8,          ///< unsigned 8 bits
    AV_SAMPLE_FMT_S16,         ///< signed 16 bits
    AV_SAMPLE_FMT_S32,         ///< signed 32 bits
    AV_SAMPLE_FMT_FLT,         ///< float
    AV_SAMPLE_FMT_DBL,         ///< double

    AV_SAMPLE_FMT_U8P,         ///< unsigned 8 bits, planar
    AV_SAMPLE_FMT_S16P,        ///< signed 16 bits, planar
    AV_SAMPLE_FMT_S32P,        ///< signed 32 bits, planar
    AV_SAMPLE_FMT_FLTP,        ///< float, planar
    AV_SAMPLE_FMT_DBLP,        ///< double, planar
    AV_SAMPLE_FMT_S64,         ///< signed 64 bits
    AV_SAMPLE_FMT_S64P,        ///< signed 64 bits, planar

    AV_SAMPLE_FMT_NB           ///< Number of sample formats. DO NOT USE if linking dynamically
};

*/
