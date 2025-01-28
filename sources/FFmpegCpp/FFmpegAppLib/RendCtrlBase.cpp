#include "pch.h"

#include "RendCtrlBase.h"

#include "Logger.h"

namespace Fapp
{

    const int RendCtrlBase::MaxSleepInterval = 120;


    RendCtrlBase::RendCtrlBase(AVMediaType mediaType)
        : Fcore::MediaTypeHolder(mediaType)
    {}

    RendCtrlBase::~RendCtrlBase() = default;

    void RendCtrlBase::AttachFrameHub(IFrameData* frameHub)
    {
        m_FrameHub = frameHub;
    }

    void RendCtrlBase::InitPump(IProceed* pr, bool useCondVar, const char* tag, bool logEx)
    {
        m_Pump.Init(pr, useCondVar, tag, logEx);
    }

    bool RendCtrlBase::CreatePump()
    {
        return m_Pump.CreatePump();
    }

    void RendCtrlBase::ClosePump()
    {
        m_Pump.ClosePump();
    }

    bool RendCtrlBase::PumpError() const
    {
        return m_Pump.PumpError();
    }

    bool RendCtrlBase::EndOfData() const
    {
        return m_Pump.EndOfData();
    }

} // namespace Fapp
