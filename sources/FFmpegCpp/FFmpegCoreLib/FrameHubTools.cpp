#include "pch.h"

#include "FrameHubTools.h"

#include "FrameTools.h"
#include "FrameProcIface.h"
#include "FrameConv.h"
#include "StreamReader.h"

namespace Fcore
{
    FrameProc::FrameProc() = default;

    FrameProc::~FrameProc()
    {
        FrameTools::FreeFrame(m_Frame);
    }

    bool FrameProc::Enable(IFrameProcessor* frameProc)
    {
        if (!m_FrameProc && frameProc)
        {
            if (auto frame = FrameTools::AllocFrame())
            {
                m_FrameProc = frameProc;
                m_Frame = frame;
            }
        }
        return IsEnabled();
    }

    int FrameProc::ProcFrame(AVFrame* frame, FrameConv& frameConv)
    {
        int ret = -1;
        if (IsEnabled() && frameConv.IsReady())
        {
            if (frame)
            {
                ret = frameConv.Convert(frame, m_Frame);
                if (ret >= 0)
                {
                    m_FrameProc->ProcFrame(m_Frame);
                    FrameTools::FreeFrameBuffer(m_Frame);
                    ret = true;
                }
                else
                {
                    LOG_ERROR("%s, failed to convert, code=%d", __FUNCTION__, ret);
                }
            }
            else
            {
                m_FrameProc->ProcFrame(nullptr);
                ret = 0;
            }
        }
        return ret;
    }

// ---------------------------------------------------------------------

    MultiInput::MultiInput(bool player)
    {
        m_Ptrs.reserve(player ? 1 : 4);
    }

    MultiInput::~MultiInput() = default;

    void MultiInput::Add(StreamReader* strm)
    {
        m_Ptrs.push_back(strm->PumpSwitchW());
    }

    Ftool::IPumpSwitchW* MultiInput::PumpSwitch()
    { 
        return (m_Ptrs.size() == 1) ? m_Ptrs[0] : (!m_Ptrs.empty() ? static_cast<Ftool::IPumpSwitchW*>(this) : nullptr);
    }

    void MultiInput::EnablePumpWrite(bool enable)
    {
        for (auto ptr : m_Ptrs)
        {
            ptr->EnablePumpWrite(enable);
        }
    }

} // namespace Fcore
