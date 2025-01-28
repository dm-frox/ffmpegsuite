#include "pch.h"

#include "FrameHub.h"

#include "PumpSwitchIface.h"

#include "Logger.h"
#include "CommonConsts.h"
#include "Misc.h"
#include "FramePoolItem.h"
#include "FrameProcIface.h"
#include "StreamReader.h"
#include "FrameConvVideo.h"
#include "FrameConvAudio.h"

//#define LOG_PTS_VIDEO_
//#define LOG_PTS_AUDIO_

namespace Fcore
{
    const int FrameHub::MaxFramesInPacketVideo   = 32;
    const int FrameHub::MaxFramesInPacketAudio   = 128;
    const int FrameHub::FrameQueueThresholdVideo = 4;
    const int FrameHub::FrameQueueThresholdAudio = 16;

    //static 
    int FrameHub::FrameQueueThreshold(bool isVideo)
    {
        return isVideo ? FrameQueueThresholdVideo : FrameQueueThresholdAudio;
    }

    //static 
    int FrameHub::FrameQueueMaxLength(bool isVideo)
    {
        return FrameQueueThreshold(isVideo) + (isVideo ? MaxFramesInPacketVideo : MaxFramesInPacketAudio);
    }

    FrameHub::FrameHub(AVMediaType mt, bool frameConvSeq)
        : MediaTypeHolder(mt)
        , m_FrameQueueThreshold(FrameQueueThreshold(IsVideo()))
        , m_SkipConvFlt(!frameConvSeq && IsVideo())
        , m_FrameQueue(MediaType(), FrameQueueMaxLength(IsVideo()))
        , m_MultiInput(true)
        , m_SeekCtrl(mt)
    {
        LOG_INFO("%s, %s, skip conv/flt=%d, queue max len=%d", __FUNCTION__, 
            MediaTypeStr(), m_SkipConvFlt, m_FrameQueue.MaxLength());
    }

    void FrameHub::EnableWritePumpCtrl()
    {
        m_FrameQueue.EnableWritePumpCtrl(m_MultiInput.PumpSwitch(), m_FrameQueueThreshold);
    }

    FrameHub::~FrameHub() = default;

    void FrameHub::ClearQueue()
    {
        if (IsReady())
        {
            int c = m_FrameQueue.Clear();
            LOG_INFO("%s, %s, count=%d", __FUNCTION__, MediaTypeStr(), c);
        }
    }

    void FrameHub::ClearFilterGraph()
    {
        m_FilterGraph.Reset();
    }

    void FrameHub::Reset()
    {
        LOG_INFO("%s, %s, frames=%d", __FUNCTION__, MediaTypeStr(), m_FrameCntr);
        ClearQueue();
        ClearFilterGraph();
        m_FrameCntr = 0;
        m_SeekCtrl.Reset();
     }

    int FrameHub::CreateFilterGraph()
    {
        if (IsReady())
        {
            LOG_ERROR("%s, %s, already initialized", __FUNCTION__, MediaTypeStr());
            return ErrorAlreadyInitialized;
        }

        int ret = m_FilterGraph.Create(MediaType());
        if (ret >= 0)
        {
            if (m_FltFrame.Alloc())
            {
                if (m_SkipConvFlt)
                {
                    m_FilterGraph.DisableRebuldOnReset();
                }
                ret = 0;
                LOG_INFO("%s, %s, filter graph is set", __FUNCTION__, MediaTypeStr());
            }
            else
            {
                m_FilterGraph.Close();
                ret = AVERROR(ENOMEM);
                LOG_ERROR("%s, %s, failed to alloc frame", __FUNCTION__, MediaTypeStr());
            }
        }
        return ret;
    }

    bool FrameHub::SetFrameProc(IFrameProcessor* frameProc)
    {
        bool ret = false;
        if (m_FrameProc.Enable(frameProc))
        {
            LOG_INFO("%s, %s, FrameProc, tag=%s", __FUNCTION__, MediaTypeStr(), frameProc->Tag());
            ret = true;
        }
        else
        {
            LOG_ERROR("%s, failed to set FrameProc", __FUNCTION__, MediaTypeStr());
        }
        return ret;
    }

    void FrameHub::SetTimebase(AVRational tb)
    {
        m_Timebase_N = tb.num;
        m_Timebase_D = tb.den;
        LOG_INFO("%s, %s, t.base=%d/%d", __FUNCTION__, MediaTypeStr(), m_Timebase_N, m_Timebase_D);

        if (IsVideo())
        {
            if (m_FpsFactor != 0)
            {
                if (m_FpsFactor > 0)
                {
                    m_Timebase_N = 1;
                    m_Timebase_D = m_FpsFactor;
                }
                else
                {
                    m_Timebase_D *= (-m_FpsFactor);
                }
                LOG_INFO("%s, %s, fps fact=%d, new t.base=%d/%d", __FUNCTION__, MediaTypeStr(), m_FpsFactor, m_Timebase_N, m_Timebase_D);
            }
        }

        m_RendPosFactor = (m_Timebase_N > 0 && m_Timebase_D > 0)
            ? 1.0 / static_cast<double>(m_Timebase_D)
            : 0.0;
    }

    double FrameHub::CalcRendPosition(const AVFrame* frame) const
    {
        double ret = 0.0;
        if (m_RendPosFactor > 0.0)
        {
            int64_t pts = frame->pts;
            if (pts != AV_NOPTS_VALUE)
            {
                ret = static_cast<double>(pts * m_Timebase_N) * m_RendPosFactor;
            }
        }
        return ret;
    }

    void FrameHub::SetRendPosition(FramePoolItem* fpi)
    {
        if (IsVideo() && fpi)
        {
            double pos = CalcRendPosition(fpi->Frame());
            fpi->SetRendPosition(pos);

            LogPts(fpi->FramePts(), pos);
        }
    }

    int FrameHub::PushFramePoolItem(FramePoolItem* fpi)
    {
        SetRendPosition(fpi);

        int qlen =  m_FrameQueue.Push(fpi);
        if (qlen >= 0) // OK
        {
            if (fpi)
            {
                ++m_FrameCntr;
            }
            if (m_KeepAtThreshold)
            {
                qlen = m_FrameQueue.Shrink(m_FrameQueueThreshold);
            }
        }
        else // can't be so
        {
            fpi->ReturnToPool();
        }
        return qlen;
    }

    void FrameHub::SetEndOfData()
    {
        LOG_INFO("%s, %s, end of data", __FUNCTION__, MediaTypeStr());
        PushFramePoolItem(nullptr);
    }

    void FrameHub::SetPosition(double pos, bool paused)
    {
        m_SeekCtrl.SetPosition(pos, paused);
    }

    bool FrameHub::CheckSeekingEnd(AVFrame* frame)
    {
        bool ret = false;

        if (m_SeekCtrl.InProgress())
        {
            if (frame)
            {
                double pos = CalcRendPosition(frame);
                if (!m_SeekCtrl.CheckSeekingEnd(pos))
                {
                    FrameTools::FreeFrameBuffer(frame);
                    ret = true;
                }
            }
            else
            {
                m_SeekCtrl.Cancel();
            }
        }

        return ret;
    }

    int FrameHub::AddFrame(AVFrame* frame, int inputIdx)
    {
        int ret = -1;

        if (IsAudio())
        {
            if (CheckSeekingEnd(frame))
            {
                return 0;
            }
        }

        if (m_FrameConv.IsReady())
        {
            ret = AddFrameConv(frame);
        }
        else if (m_FilterGraph.IsReady())
        {
            ret = AddFrameFlt(frame, inputIdx);
        }
        else
        {
            LOG_ERROR("%s, %s, no frame converter or filter", __FUNCTION__, MediaTypeStr());
        }

        if (ret < 0)
        {
            LOG_ERROR("%s, %s, ret=%d", __FUNCTION__, MediaTypeStr(), ret);
            FrameTools::FreeFrameBuffer(frame);
        }

        return ret;
    }

    int FrameHub::PushFrame(AVFrame* frame, bool conv)
    {
        int ret = -1;

        if (frame)
        {
            if (FramePoolItem* fpi = m_FrameQueue.FindFreeFramePoolItem())
            {
                AVFrame* dstFrame = fpi->Frame();
                ret = !conv
                    ? FrameTools::MoveFrameRef(dstFrame, frame)
                    : m_FrameConv.Convert(frame, dstFrame);
                if (ret >= 0)
                {
                    ret = PushFramePoolItem(fpi);
                }
                else
                {
                    fpi->ForceFree();
                }
            }
            else
            {
                LOG_ERROR("%s, %s, frame queue is full, the frame has lost", __FUNCTION__, MediaTypeStr());
                FrameTools::FreeFrameBuffer(frame);
            }
        }
        else
        {
            SetEndOfData();
            ret = 0;
        }

        return ret;
    }

    int FrameHub::ForwardFrame(AVFrame* frame, int outputIdx) // ISinkFrameFlt impl
    {
        int ret = -1;

        if (outputIdx == 0)
        {
            ret = PushFrame(frame, false);
        }
        else
        {
            LOG_ERROR("%s, %s, multi out filters is not supported", __FUNCTION__, MediaTypeStr());
        }

        return ret;
    }

    int FrameHub::FilterFrame(AVFrame* frame, int inputIdx)
    {
        return m_FilterGraph.FilterFrame(frame, inputIdx, static_cast<ISinkFrameFlt*>(this));
    }

    int FrameHub::AddFrameFlt(AVFrame* frame, int inputIdx)
    {
        int ret = -1;

        if (!m_SkipConvFlt)
        {
            ret = FilterFrame(frame, inputIdx);
        }
        else
        {
            ret = PushFrame(frame, false);
        }

        return ret;
    }
 
    int FrameHub::AddFrameConv(AVFrame* frame) 
    {
        int ret = -1;

        if (!m_FrameProc.IsEnabled())
        {
            ret = PushFrame(frame, !m_SkipConvFlt);
        }
        else
        {
            ret = m_FrameProc.ProcFrame(frame, m_FrameConv);
        }
 
        return ret;
    }

    bool FrameHub::AssertNoFilterGraph(const char* msg) const
    {
        bool ret = !m_FilterGraph.IsReady();
        if (!ret)
        {
            LOG_ERROR("%s, %s, filter graph is attached", msg, MediaTypeStr());
        }
        return ret;
    }

    bool FrameHub::AssertNoConverter(const char* msg) const
    {
        bool ret = !m_FrameConv.IsReady();
        if (!ret)
        {
            LOG_ERROR("%s, %s, converter is set", msg, MediaTypeStr());
        }
        return ret;
    }

    bool FrameHub::AssertFilterGraph(const char* msg) const
    {
        bool ret = m_FilterGraph.IsReady();
        if (!ret)
        {
            LOG_ERROR("%s, %s, no filter graph", msg, MediaTypeStr());
        }
        return ret;
    }

    int FrameHub::SendFilterGraphCommand(const char* tar, const char* cmd, const char* arg)
    {
        if (!AssertFilterGraph(__FUNCTION__))
            return ErrorBadState;

        return m_FilterGraph.SendCommand(tar, cmd, arg);
    }

    int FrameHub::ConfigureFilterGraph(const char* filterString)
    {
        if (!AssertFilterGraph(__FUNCTION__))
            return ErrorBadState;

        if (m_FilterGraph.InputCount() == 0 || m_FilterGraph.OutputCount() == 0)
        {
            LOG_ERROR("%s, %s, no input or output filters", __FUNCTION__, MediaTypeStr());
            return ErrorFiltertGraphBuild;
        }

        return m_FilterGraph.ConfigureFilterGraph(filterString);
    }

    int FrameHub::ReconfigureFilterGraph(const char* filterString)
    {
        if (!AssertFilterGraph(__FUNCTION__))
            return ErrorBadState;

        if (m_FilterGraph.InputCount() == 0 || m_FilterGraph.OutputCount() == 0)
        {
            LOG_ERROR("%s, %s, no input or output filters", __FUNCTION__, MediaTypeStr());
            return ErrorFiltertGraphBuild;
        }

        return m_FilterGraph.ReconfigureFilterGraph(filterString);
    }

    int FrameHub::AddFilterGraphOutputVideo(AVPixelFormat pixFmt)
    {
        if (!AssertFilterGraph(__FUNCTION__))
            return ErrorBadState;
        if (!AssertVideo(__FUNCTION__))
            return ErrorWrongMediaType;

        if (m_FilterGraph.OutputCount() > 0)
        {
            LOG_ERROR("%s, %s, only one output filter supported", __FUNCTION__, MediaTypeStr());
            return ErrorNotImplemented;;
        }

        int ret = m_FilterGraph.AddOutputVideo(pixFmt);
        if (ret >= 0)
        {
            EnableWritePumpCtrl();
        }
        return ret;
    }

    int FrameHub::AddFilterGraphOutputAudio(AVSampleFormat sampFmt)
    {
        if (!AssertFilterGraph(__FUNCTION__))
            return ErrorBadState;
        if (!AssertAudio(__FUNCTION__))
            return ErrorWrongMediaType;

        if (m_FilterGraph.OutputCount() > 0)
        {
            LOG_ERROR("%s, %s, only one output filter supported", __FUNCTION__, MediaTypeStr());
            return ErrorNotImplemented;;
        }

        int ret = m_FilterGraph.AddOutputAudio(sampFmt);
        if (ret >= 0)
        {
            EnableWritePumpCtrl();
        }
        return ret;
    }

    int FrameHub::AddFilterGraphInputVideo(StreamReader* strm)
    {
        if (!AssertStreamReader(strm, __FUNCTION__))
            return ErrorBadArgs;
        if (!AssertFilterGraph(__FUNCTION__))
            return ErrorBadState;
        if (!AssertNoConverter(__FUNCTION__))
            return ErrorBadState;
        if (!AssertVideo(__FUNCTION__))
            return ErrorWrongMediaType;

        if (m_FilterGraph.OutputCount() > 0)
        {
            LOG_ERROR("%s, %s, output filter must be added after the input filters", __FUNCTION__, MediaTypeStr());
            return ErrorFiltertGraphBuild;
        }

        const Decoder& dec = strm->Decoder();
        int ret = m_FilterGraph.AddInputVideo(
            dec.Width(), dec.Height(), dec.PixelFormat(),
            strm->Timebase(), strm->Framerate());
        if (ret >= 0)
        {
            m_MultiInput.Add(strm);
        }
        return ret;
    }

    int FrameHub::AddFilterGraphInputAudio(StreamReader* strm)
    {
        if (!AssertStreamReader(strm, __FUNCTION__))
            return ErrorBadArgs;
        if (!AssertFilterGraph(__FUNCTION__))
            return ErrorBadState;
        if (!AssertNoConverter(__FUNCTION__))
            return ErrorBadState;
        if (!AssertAudio(__FUNCTION__))
            return ErrorWrongMediaType;

        if (m_FilterGraph.OutputCount() > 0)
        {
            LOG_ERROR("%s, %s, output filter must be added after the input filters", __FUNCTION__, MediaTypeStr());
            return ErrorFiltertGraphBuild;
        }

        const Decoder& dec = strm->Decoder();
        int ret = m_FilterGraph.AddInputAudio(
            dec.SampleRate(), dec.SampleFormat(), dec.ChannLayout(),
            strm->Timebase());
        if (ret >= 0)
        {
            m_MultiInput.Add(strm);
        }
        return ret;
    }

    bool FrameHub::AssertStreamReader(const StreamReader* strm, const char* msg) const
    {
        return AssertPtr(strm, msg) && strm->AssertDecoder(msg) && AssertConnect(strm, msg);
    }

    int FrameHub::SetupConverterVideo(StreamReader* strm, int outWidth, int outHeight, AVPixelFormat outPixFmt)
    {
        if (!AssertStreamReader(strm, __FUNCTION__))
            return ErrorBadArgs;
        if (!AssertNoFilterGraph(__FUNCTION__))
            return ErrorBadState;
        if (!AssertNoConverter(__FUNCTION__))
            return ErrorBadState;
        if (!AssertVideo(__FUNCTION__))
            return ErrorWrongMediaType;

        const Decoder& dec = strm->Decoder();
        int ret = CreateConverterVideo(
            dec.Width(), dec.Height(), dec.PixelFormat(),
            outWidth, outHeight, outPixFmt);
        if (ret >= 0)
        {
            m_MultiInput.Add(strm);
            EnableWritePumpCtrl();
        }
        return ret;
    }

    int  FrameHub::CreateConverterVideo(
        int inWidth, int inHeight, AVPixelFormat inPixFmt,
        int outWidth, int outHeight, AVPixelFormat outPixFmt)
    {
        return m_FrameConv.InitVideo(inWidth, inHeight, inPixFmt, outWidth, outHeight, outPixFmt);
    }

    int FrameHub::SetupConverterAudio(StreamReader* strm, int outSampleRate, AVSampleFormat outSampleFmt, const ChannLayout& outLayout)
    {
        if (!AssertStreamReader(strm, __FUNCTION__))
            return ErrorBadArgs;
        if (!AssertNoFilterGraph(__FUNCTION__))
            return ErrorBadState;
        if (!AssertNoConverter(__FUNCTION__))
            return ErrorBadState;
        if (!AssertAudio(__FUNCTION__))
            return ErrorWrongMediaType;

        const Decoder& dec = strm->Decoder();
        int ret = CreateConverterAudio(
            dec.SampleRate(), dec.SampleFormat(), dec.ChannLayout(),
            outSampleRate, outSampleFmt, outLayout);
        if (ret >= 0)
        {
            m_MultiInput.Add(strm);
            EnableWritePumpCtrl();
        }
        return ret;
    }

    int FrameHub::CreateConverterAudio(
        int inSampleRate, AVSampleFormat inSampleFmt, const ChannLayout& inLayout,
        int outSampleRate, AVSampleFormat outSampleFmt, const ChannLayout& outLayout)
    {
        return m_FrameConv.InitAudio(
            inSampleRate, inSampleFmt, inLayout,
            outSampleRate, outSampleFmt, outLayout);
    }

// IFrameConv impl, invoked from renderer

    int  FrameHub::Convert(AVFrame* inFrame, AVFrame* outFrame)
    {
        int ret = -1;

        if (m_SkipConvFlt)
        {
            if (m_FrameConv.IsReady())
            {
                ret = m_FrameConv.Convert(inFrame, outFrame);
            }
            else if (m_FilterGraph.IsReady())
            {
                ret = m_FilterGraph.FilterFrameOne2One(inFrame, outFrame);
            }
        }
        return ret;
    }

    bool FrameHub::FilterIsUsed() const
    {
        return m_SkipConvFlt && m_FilterGraph.IsReady();
    }

    IFrameConv* FrameHub::FrameConv()
    { 
        return (m_SkipConvFlt && (m_FrameConv.IsReady() || m_FilterGraph.IsReady()))
            ? static_cast<IFrameConv*>(this)
            : nullptr; 
    }

// ---------------------------------------------------------------------
// logging

    void FrameHub::LogPts_(int64_t pts, double pos)
    {
        LOG_TEMP("%s ^^^^^ %5d, pts=%5d, pos=%6.3lf", MediaTypeTag(), m_FrameCntr, TsToInt(pts), pos);
    }

#pragma warning(disable:4100)
    void FrameHub::LogPts(int64_t pts, double pos)
    {
#ifdef LOG_PTS_VIDEO_
        if (IsVideo())
        {
            LogPts_(pts, pos);
        }
#endif
#ifdef LOG_PTS_AUDIO_
        if (IsAudio())
        {
            LogPts_(pts, pos);
        }
#endif
#pragma warning(default:4100)
    }

} // namespace Fcore
