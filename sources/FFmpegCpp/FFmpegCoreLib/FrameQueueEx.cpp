#include "pch.h"

#include "FrameQueueEx.h"

#include "FrameTools.h"
#include "Logger.h"

namespace Fcore
{
    FrameQueueEx::FrameQueueEx(AVMediaType mediaType, int maxLen)
        : MediaTypeHolder(mediaType)
        , m_FrameQueue(m_Mutex, maxLen)
        , m_FramePool(maxLen)
    {}

    FrameQueueEx::~FrameQueueEx() = default;


 // IFrameData impl, invoked from renderer

    void FrameQueueEx::SetReadFramePump(Ftool::IPumpSwitchR* readPump)
    {
        m_FrameQueue.EnableReadPumpCtrl(readPump);
    }

    double FrameQueueEx::VideoFramePos() const
    {
        double ret = -1000.0;
        if (IsVideo())
        {
            if (auto rr = m_FrameQueue.Peek())
            {
                if (rr.Ptr)
                {
                    ret = rr.Ptr->RendPosition();
                    if (ret < 0.0)
                    {
                        ret = 0.0;
                    }
                }
                else
                {
                    LOG_INFO("%s, end of data", __FUNCTION__);
                    ret = EndOfVideoMarker();
                }
            }
            // else no frames in the queue
        }
        return ret;
    }

    const void* FrameQueueEx::PeekVideoFrame(int& width, int& height, int& stride, double& pos) const
    {
        const void* ret = nullptr;
        if (IsVideo())
        {
            if (auto rr = m_FrameQueue.Peek())
            {
                if (rr.Ptr)
                {
                    if (const AVFrame* frame = rr.Ptr->Frame())
                    {
                        width = frame->width;
                        height = frame->height;
                        stride = frame->linesize[0];
                        pos = rr.Ptr->RendPosition();
                        ret = frame->data[0];
                    }
                    // else can't be so
                }
                // else eof, can't be so
            }
            // else no frames in the queue
        }
        return ret;
    }

    AVFrame* FrameQueueEx::PeekVideoFrame(double& pos)
    {
        AVFrame* ret = nullptr;
        if (IsVideo())
        {
            if (auto rr = m_FrameQueue.Peek())
            {
                if (rr.Ptr)
                {
                    if (const AVFrame* frame = rr.Ptr->Frame())
                    {
                        pos = rr.Ptr->RendPosition();
                        ret = const_cast<AVFrame*>(frame);
                    }
                    // else can't be so
                }
                // else eof, can't be so
            }
            // else no frames in the queue
        }
        return ret;
    }

    const void* FrameQueueEx::PeekAudioFrame(int& size, int& chann, int& sampleRate, double& pos) const
    {
        const void* ret = nullptr;
        int sz = 0;
        if (IsAudio())
        {
            if (auto rr = m_FrameQueue.Peek())
            {
                if (rr.Ptr)
                {
                    if (const AVFrame* frame = rr.Ptr->Frame())
                    {
                        ret = frame->data[0];
                        sz = FrameTools::CalcAudioDataSize(frame);
                        chann = frame->ch_layout.nb_channels;
                        sampleRate = frame->sample_rate;
                    } // else can't be so
                    pos = rr.Ptr->RendPosition();
                }
                else
                {
                    LOG_INFO("%s, end of data", __FUNCTION__);
                    sz = EndOfAudioMarker();
                }
            }
            else // no frames in the queue
            {
                sz = 0;
            }
        }
        size = sz;
        return ret;
    }

    void FrameQueueEx::DropFrame()
    {
        m_FrameQueue.Drop();
    }

    int FrameQueueEx::FrameCount() const
    {
        return m_FrameQueue.DataLength();
    }

} // namespace Fcore


