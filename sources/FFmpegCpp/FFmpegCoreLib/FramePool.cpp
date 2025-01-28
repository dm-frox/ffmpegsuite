#include "pch.h"

#include "FramePool.h"

#include "FramePoolItem.h"


namespace Fcore
{

    FramePool::FramePool(int len)
        : m_Pool(new FramePoolItem[len])
        , m_Length(len)
    {
    }

    FramePool::~FramePool()
    {
        delete [] m_Pool;
    }

    FramePoolItem* FramePool::FindFreeItem() const
    {
        FramePoolItem* ret = nullptr;
        for (FramePoolItem* itr = m_Pool, *end = m_Pool + m_Length; itr < end && !ret; ++itr)
        {
            if (itr->TakeFreeItemForQueue())
            {
                ret = itr;
            }
        }
        return ret;
    }

} // namespace Fcore