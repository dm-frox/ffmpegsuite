#include "pch.h"

#include "FrameConv.h"

#include "Misc.h"
#include "CommonConsts.h"


namespace Fcore
{
    FrameConv::FrameConv() = default;

    FrameConv::~FrameConv() = default;

    int FrameConv::InitVideo(
        int inWidth, int inHeight, AVPixelFormat inPixFmt,
        int outWidth, int outHeight, AVPixelFormat outPixFmt)
    {
        if (!AssertFirst(IsReady(), __FUNCTION__))
            return ErrorAlreadyInitialized;

        int ret = FrameConvVideo::Init(inWidth, inHeight, inPixFmt, 
            outWidth, outHeight, outPixFmt);
        if (ret >= 0)
        {
            m_MediaType = AVMEDIA_TYPE_VIDEO;
        }
        return ret;
    }

    int FrameConv::InitAudio(
        int inSampleRate, AVSampleFormat inSampFmt, const ChannLayout& inLayout,
        int outSampleRate, AVSampleFormat outSampleFmt, const ChannLayout& outLayout)
    {
        if (!AssertFirst(IsReady(), __FUNCTION__))
            return ErrorAlreadyInitialized;

        int ret = FrameConvAudio::Init(inSampleRate, inSampFmt, inLayout,
            outSampleRate, outSampleFmt, outLayout);
        if (ret >= 0)
        {
            m_MediaType = AVMEDIA_TYPE_AUDIO;
        }
        return ret;

    }

    int FrameConv::Convert(AVFrame* inFrame, AVFrame* outFrame)
    {
        switch (m_MediaType)
        {
        case AVMEDIA_TYPE_VIDEO:
            return FrameConvVideo::Convert(inFrame, outFrame);
        case AVMEDIA_TYPE_AUDIO:
            return FrameConvAudio::Convert(inFrame, outFrame);
        }
        return ErrorBadState;
    }

} // namespace Fcore
