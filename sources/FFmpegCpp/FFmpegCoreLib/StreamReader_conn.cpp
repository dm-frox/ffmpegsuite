#include "pch.h"

#include "StreamReader.h"

#include "Demuxer.h"
#include "Logger.h"
#include "CommonConsts.h"
#include "Misc.h"
#include "FilterGraph.h"
#include "FrameHub.h"
#include "Muxer.h"


namespace Fcore
{

// video

    int StreamReader::ConnectToFrameHubVideo(FrameHub* frameHub, int outWidth, int outHeight, AVPixelFormat outPixFmt)
    {
        int ret = -1;

        if (!AssertVideo(__FUNCTION__))
            return ErrorWrongMediaType;
        if (!AssertArgs(frameHub, __FUNCTION__))
            return ErrorBadArgs;
        if (!AssertDecoder(__FUNCTION__))
            return ErrorNoDecoder;

        ret = frameHub->SetupConverterVideo(this, outWidth, outHeight, outPixFmt);

        if (ret >= 0)
        {
            m_FrameHub = frameHub;
            SetActivated();

            LOG_INFO("%s, %s, strm.idx=%d", __FUNCTION__, MediaTypeStr(), Index());
        }
 
        return ret;
    }

    int StreamReader::ConnectToFilterGraphVideo(FrameHub* frameHub)
    {
        int ret = -1;

        if (!AssertVideo(__FUNCTION__))
            return ErrorWrongMediaType;
        if (!AssertArgs(frameHub, __FUNCTION__))
            return ErrorBadArgs;
        if (!AssertDecoder(__FUNCTION__))
            return ErrorNoDecoder;

        ret = frameHub->AddFilterGraphInputVideo(this);

        if (ret >= 0)
        {
            m_FrameHub = frameHub;
            m_FilterInputIndex = ret;
            SetActivated();

            LOG_INFO("%s, %s, strm.idx=%d, filt.idx=%d", __FUNCTION__, MediaTypeStr(), Index(), m_FilterInputIndex);
        }
 

        return ret;        
    }

    int StreamReader::ConnectToMuxerVideo(Muxer* muxer)
    {
        int ret = -1;

        if (!AssertFirst(m_PacketQueue.BufferLength() > 0, __FUNCTION__))
            return ErrorAlreadyInitialized;
        if (!AssertVideo(__FUNCTION__))
            return ErrorWrongMediaType;
        if (!AssertArgs(muxer, __FUNCTION__))
            return ErrorBadArgs;

        if (m_PacketQueue.AllocBuffer())
        {
            int wrtIdx = muxer->AddOutputStream(this);

            if (wrtIdx >= 0)
            {
                SetActivated();
                ret = wrtIdx;

                LOG_INFO("%s, %s, strm.idx=%d, writer idx=%d, fr.rate=%d/%d", __FUNCTION__, MediaTypeStr(), Index(), wrtIdx,
                    m_Framerate.num, m_Framerate.den);
            }
            else
            {
                LOG_ERROR("%s, failed to connect", __FUNCTION__);
            }
        }
        else
        {
            LOG_ERROR("%s, %s, failed to allocate queue", __FUNCTION__, MediaTypeStr());
        }
        return ret;
    }

// audio

    int StreamReader::ConnectToFrameHubAudio(FrameHub* frameHub, const ChannLayout& outLayout, int outSampleRate, AVSampleFormat outSampFmt)
    {
        int ret = -1;

        if (!AssertAudio(__FUNCTION__))
            return ErrorWrongMediaType;
        if (!AssertArgs(frameHub, __FUNCTION__))
            return ErrorBadArgs;
        if (!AssertDecoder(__FUNCTION__))
            return ErrorNoDecoder;

        ret = frameHub->SetupConverterAudio(this, outSampleRate, outSampFmt, outLayout);

        if (ret >= 0)
        {
            m_FrameHub = frameHub;
            SetActivated();

            LOG_INFO("%s, %s, strm.idx=%d", __FUNCTION__, MediaTypeStr(), Index());
        }
 
        return ret;
    }

    int StreamReader::ConnectToFilterGraphAudio(FrameHub* frameHub)
    {
        int ret = -1;

        if (!AssertAudio(__FUNCTION__))
            return ErrorWrongMediaType;
        if (!AssertArgs(frameHub, __FUNCTION__))
            return ErrorBadArgs;
        if (!AssertDecoder(__FUNCTION__))
            return ErrorNoDecoder;

        ret = frameHub->AddFilterGraphInputAudio(this);

        if (ret >= 0)
        {
            m_FrameHub = frameHub;
            m_FilterInputIndex = ret;
            SetActivated();

            LOG_INFO("%s, %s, strm.idx=%d, filt.idx=%d", __FUNCTION__, MediaTypeStr(), Index(), m_FilterInputIndex);
        }
 
        return ret;
    }

    int StreamReader::ConnectToMuxerAudio(Muxer* muxer)
    {
        int ret = -1;

        if (!AssertFirst(m_PacketQueue.BufferLength() > 0, __FUNCTION__))
            return ErrorAlreadyInitialized;
        if (!AssertAudio(__FUNCTION__))
            return ErrorWrongMediaType;
        if (!AssertArgs(muxer, __FUNCTION__))
            return ErrorBadArgs;

        if (m_PacketQueue.AllocBuffer())
        {
            int wrtIdx = muxer->AddOutputStream(this); 

            if (wrtIdx >= 0)
            {
                SetActivated();
                ret = wrtIdx;

                LOG_INFO("%s, %s, strm.idx=%d, writer idx=%d", __FUNCTION__, MediaTypeStr(), Index(), wrtIdx);
            }
            else
            {
                LOG_ERROR("%s, failed to connect", __FUNCTION__);
            }
        }
        else
        {
            LOG_ERROR("%s, %s, failed to allocate queue", __FUNCTION__, MediaTypeStr());
        }
        return ret;
    }

} // namespace Fcore

