#include "pch.h"

#include "SeekCtrl.h"

#include "ProceedPump.h"

#include "Logger.h"

namespace Fcore
{
	const double SeekCtrl::Eps = -1.0 / 64.0;


	SeekCtrl::SeekCtrl(AVMediaType mediaType)
		: MediaTypeHolder(mediaType)
	{}

	SeekCtrl::SeekCtrl(AVMediaType mediaType, PumpType& pump)
		: MediaTypeHolder(mediaType)
		, m_RendPump(&pump)
	{}

	void SeekCtrl::SetPump(PumpType* pump)
	{ 
		m_RendPump = pump;
		if (m_RendPump)
		{
			LOG_INFO("%s, %s, tag=%s", __FUNCTION__, MediaTypeStr(), m_RendPump->Tag());
		}
		else
		{
			LOG_ERROR("%s, %s, null pump", __FUNCTION__, MediaTypeStr());
		}
	}

	void SeekCtrl::SetPosition(double pos, bool paused)
	{
		double p = (pos >= 0.0) ? pos : 0.0;
		m_InProgress = true;
		m_PosToSeek = p;
		m_Paused = paused;
		LOG_INFO("%s, %s, pos=%5.3lf, pause=%d", __FUNCTION__, MediaTypeStr(), p, m_Paused);
	}

	bool SeekCtrl::CheckSeekingEnd(double pos)
	{
		bool ret = false;
		if (m_InProgress)
		{
			double posToSeek = m_PosToSeek;
			double delt = pos - posToSeek;
			if (delt >= Eps)
			{
				LOG_INFO("%s, %s, pos(seek/frame)=%5.3lf/%5.3lf, delt=%5.3lf, paused=%d", __FUNCTION__,
					MediaTypeStr(), posToSeek, pos, delt, m_Paused);

				if (m_Paused && m_RendPump)
				{
					m_RendPump->Pause();
				}
				Reset();
				ret = true;
			}
		}
		return ret;
	}

	void SeekCtrl::Reset()
	{
		m_InProgress = false;
		m_PosToSeek = -1.0;
		m_Paused = false;
	}

	void SeekCtrl::Cancel()
	{
		if (m_InProgress)
		{
			double ps = m_PosToSeek;
			LOG_INFO("%s, %s, pos=%5.3lf, paused=%d", __FUNCTION__, MediaTypeStr(), ps, m_Paused);
			Reset();
		}
	}

} // namespace Fcore