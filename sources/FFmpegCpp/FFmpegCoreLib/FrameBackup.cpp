#include "pch.h"

#include "FrameBackup.h"

#include "FrameTools.h"

#include "Logger.h"

namespace Fcore
{
    FrameBackup::FrameBackup() = default;

    FrameBackup::~FrameBackup()
    {
        FrameTools::FreeFrame(m_Frame);
        FrameTools::FreeFrame(m_FrameOut);
    }

    void FrameBackup::Clear()
    {
        if (m_IsSet)
        {
            FrameTools::FreeFrameBuffer(m_Frame);
            m_IsSet = false;
            LOG_INFO("%s -----", __FUNCTION__);
        }
    }

    bool FrameBackup::Set(AVFrame* frame)
    {
        if (!m_Frame)
        {
            m_Frame = FrameTools::AllocFrame();
        }
        if (m_Frame)
        {
            Clear();
            if (FrameTools::CopyFrameRef(m_Frame, frame) >= 0)
            {
                m_IsSet = true;
                LOG_INFO("%s +++++", __FUNCTION__);
            }
        }
        return m_IsSet;
    }

    AVFrame* FrameBackup::Get()
    {
        AVFrame* ret = nullptr;
        if (m_IsSet)
        {
            if (!m_FrameOut)
            {
                m_FrameOut = FrameTools::AllocFrame();
            }
            if (m_FrameOut)
            {
                if (FrameTools::CopyFrameRef(m_FrameOut, m_Frame) >= 0)
                {
                    ret = m_FrameOut;
                }
            }
        }
        return ret;
    }

} // namespace Fcore

