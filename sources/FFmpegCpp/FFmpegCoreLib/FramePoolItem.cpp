#include "pch.h"

#include "FramePoolItem.h"

#include "FrameTools.h"

namespace Fcore
{

    FramePoolItem::FramePoolItem() = default;

    FramePoolItem::~FramePoolItem()
    {
        FrameTools::FreeFrame(m_Frame);
    }

    bool FramePoolItem::TakeFreeItemForQueue()
    {
        bool ret = false;
        if (m_Free)
        {
            if (!m_Frame)
            {
                m_Frame = FrameTools::AllocFrame(); 
            }
            if (m_Frame)
            {
                m_Free = false;
                ret = true;
            }
        }
        return ret;
    }

    int64_t FramePoolItem::FramePts() const
    {
        return m_Frame ? m_Frame->pts : AV_NOPTS_VALUE;
    }

    void FramePoolItem::ReturnToPool()
    {
        FrameTools::FreeFrameBuffer(m_Frame);
        m_RendPosition = -1.0;
        m_Free = true;
    }

    //static 
    void FramePoolItem::ReturnToPool(FramePoolItem* item)
    {
        if (item)
        {
            item->ReturnToPool();
        }
    }

} // namespace Fcore


