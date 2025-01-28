#include "pch.h"

#include "FrameConvVideo.h"

#include "Logger.h"
#include "FrameTools.h"
#include "Misc.h"
#include "CommonConsts.h"
#include "OptionTools.h"


namespace Fcore
{
    const int FrameConvVideo::SwsFlagsDef = SWS_FAST_BILINEAR;

    FrameConvVideo::FrameConvVideo() = default;
 
    FrameConvVideo::~FrameConvVideo()
    {
        sws_freeContext(m_SwsCtx);
    }

    int FrameConvVideo::Init(
        int inWidth, int inHeight, AVPixelFormat inPixFmt,
        int outWidth, int outHeight, AVPixelFormat outPixFmt)
    {
        int ret = -1;

        if (!AssertFirst(m_SwsCtx, __FUNCTION__))
        {
            return ErrorAlreadyInitialized;
        }

        if (!(AssertVideoParams(inWidth, inHeight, AVRational{ 1, 1 }, __FUNCTION__) &&
            AssertVideoFormat(inPixFmt, __FUNCTION__)))
        {
            return ErrorBadArgs;
        }

        int outW = (outWidth > 0) ? outWidth : inWidth;
        int outH = (outHeight > 0) ? outHeight : inHeight;
        AVPixelFormat outFmt = (outPixFmt != AV_PIX_FMT_NONE) ? outPixFmt : inPixFmt;
        m_Counter = 0;

        m_InHeight  = inHeight;
        m_OutWidth  = outW;
        m_OutHeight = outH;
        m_OutPixFmt = outFmt;

        if (outW != inWidth || outH != inHeight || outFmt != inPixFmt)
        {
            if (SwsContext* swsCtx = sws_getContext(
                inWidth, inHeight, inPixFmt,
                outW, outH, outPixFmt,
                m_SwsFlags, nullptr, nullptr, nullptr))
            {
                m_SwsCtx = swsCtx;
                ret = 0;

                LOG_INFO("%s, in: frame=%dx%d, pix.fmt=%s; out: frame=%dx%d, pix.fmt=%s",
                    __FUNCTION__,
                    inWidth, m_InHeight, PixFmtToStr(inPixFmt),
                    m_OutWidth, m_OutHeight, PixFmtToStr(m_OutPixFmt));
            }
            else
            {
                ret = AVERROR(ENOMEM);
                LOG_ERROR("%s, sws_getContext", __FUNCTION__);
            }
        }
        else
        {
            LOG_INFO("%s, in: frame=%dx%d, pix.fmt=%s; out: identical",
                __FUNCTION__,
                inWidth, inHeight, PixFmtToStr(inPixFmt));

            ret = 0;
        }
        return ret;
    }

    int FrameConvVideo::Convert(AVFrame* inFrame, AVFrame* outFrame)
    {
        int ret = -1;
        if (m_SwsCtx)
        {
            ret = FrameTools::AllocFrameBuffVideo(m_OutWidth, m_OutHeight, m_OutPixFmt, outFrame);
            if (ret >= 0)
            {
                ret = sws_scale(m_SwsCtx, inFrame->data, inFrame->linesize, 0, m_InHeight, outFrame->data, outFrame->linesize);
                if (ret >= 0)
                {
                    outFrame->pts = inFrame->pts;
                }
                else
                {
                    FrameTools::FreeFrameBuffer(outFrame);
                }
            }
            FrameTools::FreeFrameBuffer(inFrame);
        }
        else
        {
            ret = FrameTools::MoveFrameRef(outFrame, inFrame);
        }

        if (ret >= 0)
        {
            ++m_Counter;
        }
        return ret;
    }

} // namespace Fcore



//values for the flags, the stuff on the command line is different
//#define SWS_FAST_BILINEAR     1
//#define SWS_BILINEAR          2
//#define SWS_BICUBIC           4
//#define SWS_X                 8
//#define SWS_POINT          0x10
//#define SWS_AREA           0x20
//#define SWS_BICUBLIN       0x40
//#define SWS_GAUSS          0x80
//#define SWS_SINC          0x100
//#define SWS_LANCZOS       0x200
//#define SWS_SPLINE        0x400


// AV_PIX_FMT_RGB32 is handled in an endian - specific manner.
// An RGBA color is put together as : (A << 24) | (R << 16) | (G << 8) | B 
// This is stored as BGRA on little - endian CPU architectures and ARGB on big - endian CPUs.

// When the pixel format is palettized RGB32(AV_PIX_FMT_PAL8), the palettized image data is stored in AVFrame.data[0].
// The palette is transported in AVFrame.data[1], is 1024 bytes long(256 4 - byte entries) 
// and is formatted the same as in AV_PIX_FMT_RGB32 described above(i.e., it is also endian - specific).
// Note also that the individual RGB32 palette components stored in AVFrame.data[1] should be in the range 0..255.
// This is important as many custom PAL8 video codecs that were designed to run on the IBM VGA graphics adapter use 6 - bit palette components.

// For all the 8bit per pixel formats, an RGB32 palette is in data[1] like for pal8.
// This palette is filled in automatically by the function allocating the picture.
// static uint32_t Palette[256];

//static void GenPalette() // gray scale std
//{
//    for (int i = 0; i < 256; ++i)
//    Palette[i] = (0xFF << 24) | (i << 16) | (i << 8) | i;
//}

//static void SetPalette(uint8_t* palette)
//{
//  memcpy(palette, Palette, sizeof(Palette));
//}

//static void LogPalette(const uint8_t* palette)
//{
//  LOG_INFO("FrameConvVideo, palette:");
//  const uint32_t* plt = (const uint32_t*)palette;
//  for (int i = 0; i < 256; ++i)
//      LOG_INFO(" -- %x", plt[i]);
//}
