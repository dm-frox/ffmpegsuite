#include "pch.h"

#include "BsfList.h"

#include "ffmpeg.h"
#include "Logger.h"
#include "OptionTools.h"
#include "Misc.h"
#include "StrBuilder.h"


namespace Flist
{
	BsfInfo::BsfInfo(const AVBitStreamFilter* bsf)
		: m_Bsf(bsf)
	{}

	BsfInfo::~BsfInfo() = default;

	const char* BsfInfo::Name() const
	{
		return m_Bsf->name;
	}

	const char* BsfInfo::CodecIds() const
	{
		if (m_CodecIds.empty())
		{
			m_CodecIds = GetCodecIds(m_Bsf);
			SetEmptyEx(m_CodecIds);
		}
		return m_CodecIds.c_str();
	}

	//static 
	std::string BsfInfo::GetCodecIds(const AVBitStreamFilter* bsf)
	{
		Fcore::StrBuilder str(32);
		for (const AVCodecID* cds = bsf->codec_ids; cds && *cds != AV_CODEC_ID_NONE; ++cds)
		{
			str.Append(Fcore::CodecIdToStr(*cds));
		}
		return str.Str();
	}

	const char* BsfInfo::PrivOptions() const
	{
		return GetOpts(true, false, m_PrivOpts);
	}

	const char* BsfInfo::PrivOptionsEx() const
	{
		return GetOpts(true, true, m_PrivOptsEx);
	}

	const char* BsfInfo::GetOpts(bool priv, bool ex, std::string& optStr) const
	{
		if (optStr.empty())
		{
			const AVClass* avc = nullptr;
			AVBSFContext* bsfCtx = nullptr;
			if (priv)
			{
				avc = m_Bsf->priv_class;
			}
			else
			{
				int res = av_bsf_alloc(m_Bsf, &bsfCtx);
				if (res >= 0)
				{
					avc = bsfCtx->av_class;
				}
				else
				{
					LOG_ERROR("%s, name=%s, %s", __FUNCTION__, m_Bsf->name, Fcore::FmtErr(res));
				}
			}
			if (avc)
			{

				optStr = ex
					? Fcore::OptionTools::OptionsToStrEx(
						avc,
						priv ? m_Bsf->name : "Bitstream filter context",
						priv ? "bitstream filter" : nullptr)
					: Fcore::OptionTools::OptionsToStr(avc);
			}
			av_bsf_free(&bsfCtx);
			SetEmptyEx(optStr);
		}

		return optStr.c_str();
	}

// ----------------------------------------------------------------------

	BsfList::BsfList() = default;
	BsfList::~BsfList() = default;

	// static
	size_t BsfList::GetCapacity()
	{
		int cntr = 0;
		void* ctx = nullptr;
		while (av_bsf_iterate(&ctx))
		{
			++cntr;
		}
		return cntr + 1;
	}

	void BsfList::Setup()
	{
		if (IsEmpty())
		{
			Reserve(GetCapacity());
			void* ctx = nullptr;
			while (const AVBitStreamFilter* bsf = av_bsf_iterate(&ctx))
			{
				List().emplace_back(bsf);
			}
		}
	}

	const char* BsfList::CtxOptsEx() const
	{
		return !IsEmpty() ? List()[0].GetOpts(false, true, m_CtxOptsEx) : "";
	}

} // namespace Flist