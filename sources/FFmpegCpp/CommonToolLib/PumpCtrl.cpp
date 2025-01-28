#include "pch.h"

#include "PumpSwitchIface.h"

#include "PumpCtrl.h"

namespace Ftool
{

    PumpCtrlBase::PumpCtrlBase(int thresholdOff, int thresholdOn)
        : m_ThresholdOff(thresholdOff)
        , m_ThresholdOn(thresholdOn)
    {}

// ----------------------------------------------------------------------

    ReadPumpCtrl::ReadPumpCtrl(IPumpSwitchR* pumpSwitch)
        : PumpCtrlBase(0, 1)
        , m_PumpSwitch(pumpSwitch)
    {}

    void ReadPumpCtrl::OnPush(int len)
    {
        if (m_PumpSwitch && (len == m_ThresholdOn))
        {
            m_PumpSwitch->EnablePumpRead(true);
        }
    }

    void ReadPumpCtrl::OnPop(int len)
    {
        if (m_PumpSwitch && (len == m_ThresholdOff))
        {
            m_PumpSwitch->EnablePumpRead(false);
        }
    }

    void ReadPumpCtrl::Disable()
    {
        if (m_PumpSwitch)
        {
            m_PumpSwitch->EnablePumpRead(false);
        }
    }

// ----------------------------------------------------------------------

    WritePumpCtrl::WritePumpCtrl(IPumpSwitchW* pumpSwitch, int thresholdOff, bool strict)
        : PumpCtrlBase(thresholdOff, thresholdOff - 1)
        , m_PumpSwitch(pumpSwitch)
        , m_Strict(strict)
    {}

    void WritePumpCtrl::OnPush(int len)
    {
        if (m_PumpSwitch && (len == m_ThresholdOff))
        {
            m_PumpSwitch->EnablePumpWrite(false);
        }
    }

    void WritePumpCtrl::OnPop(int len)
    {
        if (m_PumpSwitch && (m_Strict ? (len == m_ThresholdOn) : (len <= m_ThresholdOn)))
        {
            m_PumpSwitch->EnablePumpWrite(true);
        }
    }

} // namespace Ftool
