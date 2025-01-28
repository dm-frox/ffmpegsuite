#include "pch.h"

#include "FrameSourceEx.h"

#include "CommonConsts.h"
#include "FrameHub.h"


using namespace Fcore;

namespace Fapp
{

    FrameSourceEx::FrameSourceEx(bool useFilterGraphAlways, bool convFrameSequentially, bool player)
        : FrameSource(useFilterGraphAlways, convFrameSequentially, player)
    {}

    FrameSourceEx::FrameSourceEx() = default;

    FrameSourceEx::~FrameSourceEx() = default;

    void FrameSourceEx::SetFrameProcVideo(Fcore::IFrameProcessor* frameProc)
    {
        m_FrameHubVideo.SetFrameProc(frameProc);
    }

    void FrameSourceEx::SetFrameProcAudio(Fcore::IFrameProcessor* frameProc)
    {
        m_FrameHubAudio.SetFrameProc(frameProc);
    }

    Fcore::IFrameData* FrameSourceEx::FrameDataVideo()
    {
        return m_FrameHubVideo.FrameData();
    }

    Fcore::IFrameData* FrameSourceEx::FrameDataAudio()
    {
        return m_FrameHubAudio.FrameData();
    }

    Fcore::IFrameConv* FrameSourceEx::FrameConvVideo()
    {
        return m_FrameHubVideo.FrameConv();
    }

    int FrameSourceEx::SendFilterCommandVideo(const char* tar, const char* cmd, const char* arg)
    {
        return m_FrameHubVideo.SendFilterGraphCommand(tar, cmd, arg);
    }

    int FrameSourceEx::ReconfigureFilterGraphVideo(const char* filterString)
    {
        return m_FrameHubVideo.ReconfigureFilterGraph(filterString);
    }

    int FrameSourceEx::SendFilterCommandAudio(const char* tar, const char* cmd, const char* arg)
    {
        return m_FrameHubAudio.SendFilterGraphCommand(tar, cmd, arg);
    }

    int FrameSourceEx::ReconfigureFilterGraphAudio(const char* filterString)
    {
        return m_FrameHubAudio.ReconfigureFilterGraph(filterString);
    }

    void FrameSourceEx::SetPositionAudio(double pos, bool paused)
    {
        if (HasAudio())
        {
            m_FrameHubAudio.SetPosition(pos, paused);
        }
    }

    bool FrameSourceEx::SeekingInProgressAudio() const
    {
        return m_FrameHubAudio.SeekingInProgress();
    }

    //void FrameSourceEx::SetRendPumpAudio(Ftool::ProceedPump* rendPump)
    //{
    //    m_FrameHubAudio.SetRendPump(rendPump);
    //}

    void FrameSourceEx::SetSeekPumpAudio()
    {
        SetRendPumpAudio_();
    }

    //void FrameSourceEx::ResetAudio()
    //{
    //    m_FrameHubAudio.ClearQueue();
    //}

} // namespace Fapp