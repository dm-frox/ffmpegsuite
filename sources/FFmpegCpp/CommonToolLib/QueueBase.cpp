#include "pch.h"

#include "QueueBase.h"

#include "SLogger.h"

namespace Ftool
{

    QueueBase::QueueBase(Mutex& mutex, int buffLen)
        : m_Mutex(mutex)
        , m_Queue(buffLen)
    {}

    QueueBase::~QueueBase() = default;

    int QueueBase::Clear(ClearFunc clearFunc)
    {
        AutoLock al(m_Mutex);
        int ret = m_Queue.Clear(clearFunc);
        // OnPop(0); !! 13.03.2022
        return ret;
    }

    int QueueBase::Shrink(int newLen, ClearFunc clearFunc)
    {
        AutoLock al(m_Mutex);
        int len = -1;
        if (newLen >= 0)
        {
            len = m_Queue.DataLength();
            if (len > newLen)
            {
                len = m_Queue.Shrink(newLen, clearFunc);
                OnPop(len);
            }
        }
        return len;
    }

    int QueueBase::DataLength() const
    {
        AutoLock al(m_Mutex);
        return m_Queue.DataLength();
    }

    bool QueueBase::Peek(ElemConstType& item) const
    {
        AutoLock al(m_Mutex);
        return m_Queue.Peek(item);
    }

    bool QueueBase::Push(ElemType item)
    {
        AutoLock al(m_Mutex);

        return OnPush(m_Queue.Push(item));
    }

    bool QueueBase::Pop(ElemType& item)
    {
        AutoLock al(m_Mutex);

        return OnPop(m_Queue.Pop(item));
    }

    void QueueBase::Drop(ClearFunc clearFunc)
    {
        AutoLock al(m_Mutex);
        OnPop(m_Queue.Drop(clearFunc));
    }

    bool QueueBase::OnPush(int len)
    {
        bool ret = false;

        if (len > 0)
        {
            m_ReadPumpCtrl.OnPush(len);
            m_WritePumpCtrl.OnPush(len);
            ret = true;
        }

        return ret;
    }

    bool QueueBase::OnPop(int len)
    {
        bool ret = false;

        if (len >= 0)
        {
            m_ReadPumpCtrl.OnPop(len);
            m_WritePumpCtrl.OnPop(len);
            ret = true;
        }
        else // empty queue
        {
            m_ReadPumpCtrl.Disable();
        }

        return ret;
    }

    void QueueBase::EnableReadPumpCtrl(IPumpSwitchR* pumpSwitch)
    {
        m_ReadPumpCtrl = ReadPumpCtrl(pumpSwitch);
    }

    void QueueBase::EnableWritePumpCtrl(IPumpSwitchW* pumpSwitch, int thresholdOff, bool strict) 
    {
        m_WritePumpCtrl = WritePumpCtrl(pumpSwitch, thresholdOff, strict);
    }

} // namespace Ftool
