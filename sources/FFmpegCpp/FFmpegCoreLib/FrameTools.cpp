#include "pch.h"

#include "FrameTools.h"

#include <cmath>

#include "Logger.h"
#include "Misc.h"
#include "ChannLayout.h"

namespace Fcore
{

    using byte_t  = uint8_t;
    using cbyte_t = uint8_t const;

    using pix32_t  = uint32_t;
    using cpix32_t = uint32_t const;

    const int FrameTools::BytesPerPixel32bpp = 4;

// ---------------------------------------------------------------------
// ref counting allocation scheme

    //static 
    int FrameTools::AllocFrameBuffVideo(int width, int height, AVPixelFormat pixFmt, AVFrame* frame)
    {
        if (!AssertArgs(frame, __FUNCTION__))
            return ErrorBadArgs;

        frame->width = width;
        frame->height = height;
        frame->format = (int)pixFmt;
        int ret = av_frame_get_buffer(frame, 0);
        if (ret < 0)
        {
            LOG_ERROR("%s, av_frame_get_buffer, %s", __FUNCTION__, FmtErr(ret));
        }

        return ret;
    }

    //static 
    int FrameTools::AllocFrameBuffAudio(int sampleRate, AVSampleFormat sampFmt, const ChannLayout& channLayout, int frameSize, AVFrame* frame)
    {
        if (!AssertArgs(frame, __FUNCTION__))
            return ErrorBadArgs;

        frame->sample_rate = sampleRate;
        channLayout.CopyTo(frame->ch_layout);
        frame->nb_samples = frameSize;
        frame->format = (int)sampFmt;
        int ret = av_frame_get_buffer(frame, 0);
        if (ret < 0)
        {
            LOG_ERROR("%s, av_frame_get_buffer, %s", __FUNCTION__, FmtErr(ret));
        }

        return ret;
    }

// ----------------------------------------------------------------------

    // static
    bool FrameTools::GetVideoData32bpp(const AVFrame* frame, void* buff, int width, int height, int stride)
    {
        bool ret = false;

        if (!frame || !buff || width <= 0 || height <= 0 || stride <= 0)
        {
            LOG_ERROR("%s, bad args", __FUNCTION__);
            return ret;
        }

        int srcWidth = frame->width,
            srcHeight = frame->height,
            srcStride = frame->linesize[0],
            srcRowSize = srcWidth * BytesPerPixel32bpp;
        const byte_t* srcData = frame->data[0];

        if (srcWidth <= 0 || srcHeight <= 0)
        {
            LOG_ERROR("%s, bad frame size", __FUNCTION__);
            return ret;
        }
        if (!srcData || srcStride <= 0)
        {
            LOG_ERROR("%s, bad frame data/stride", __FUNCTION__);
            return ret;
        }
        if (srcStride < srcRowSize)
        {
            LOG_ERROR("%s, bad frame stride: stride=%d, rowSize=%d", __FUNCTION__, srcStride, srcRowSize);
            return ret;
        }
        if (stride < srcRowSize)
        {
            LOG_ERROR("%s, bad buffer params: stride=%d, rowSize=%d", __FUNCTION__, stride, srcRowSize);
            return ret;
        }
        if (width != srcWidth || height != srcHeight)
        {
            LOG_ERROR("%s, bad buffer params: width/height=%d/%d, frame=%d/%d", __FUNCTION__, width, height, srcWidth, srcHeight);
            return ret;
        }

        FillBitmapBuffer(srcData, srcStride, buff, width, height, stride, BytesPerPixel32bpp);
        ret = true;
        return ret;
    }

    // static
    void FrameTools::FillBitmapBuffer(const void* srcData, int srcStride,
        void* dstBuff, int dstWidth, int dstHeight, int dstStride, int bytesPerPixel)
    {
        size_t dstRowSize = dstWidth * static_cast<size_t>(bytesPerPixel);
        auto dstRow = static_cast<byte_t*>(dstBuff);
        auto srcRow = static_cast<cbyte_t*>(srcData);
        for (int i = 0; i < dstHeight; ++i, dstRow += dstStride, srcRow += srcStride)
        {
            memcpy(dstRow, srcRow, dstRowSize);
        }
    }

    // pixel by pixel copying, about 30% slower
    // static
    void FrameTools::FillBitmapBufferAlt(const void* srcData, int srcStride,
        void* dstBuff, int dstWidth, int dstHeight, int dstStride, int bytesPerPixel)
    {
        if (bytesPerPixel == BytesPerPixel32bpp)
        {
            auto dstRow = static_cast<byte_t*>(dstBuff);
            auto srcRow = static_cast<cbyte_t*>(srcData);
            for (int i = 0; i < dstHeight; ++i, dstRow += dstStride, srcRow += srcStride)
            {
                auto dst = reinterpret_cast<pix32_t*>(dstRow);
                auto src = reinterpret_cast<cpix32_t*>(srcRow);
                for (int j = 0; j < dstWidth; ++j, ++dst, ++src)
                {
                    *dst = *src;
                }
            }
        }
    }

    // static
    void FrameTools::FillBitmapBuffer32bppEmul(int cntr, void* buff, int width, int height, int stride)
    {
        static cpix32_t Col1 = 0xFFE0A070;
        static cpix32_t Col2 = 0xFF407030;

        int k = cntr % height;
        auto row = static_cast<byte_t*>(buff);
        for (int i = 0; i < height; ++i, row += stride)
        {
            cpix32_t val = (i < k) ? Col1 : Col2;
            auto pp = reinterpret_cast<pix32_t*>(row);
            for (int j = 0; j < width; ++j, ++pp)
            {
                *pp = val;
            }
        }
    }

    // static
    void FrameTools::FillFrameBuffer32bppEmul(int cntr, AVFrame* frame)
    {
        FillBitmapBuffer32bppEmul(cntr, frame->data[0], frame->width, frame->height, frame->linesize[0]);
    }

    //static 
    void FrameTools::FillFrameBuffer0(const void* srcData, int srcSize, AVFrame* frame)
    {
        memcpy(frame->data[0], srcData, srcSize);
    }

    // static
    void FrameTools::FillBitmapBufferInterlaced(const void* srcData, int srcStride, bool even, bool evenOnly, void* dstBuff, int dstHeight, int dstStride)
    {
        auto srcBegin = static_cast<cbyte_t*>(srcData);
        auto dstBegin = static_cast<byte_t*>(dstBuff);

        int dstStride2 = dstStride * 2;
        int h2 = dstHeight / 2;
        auto dstRow = dstBegin;
        auto srcRow = srcBegin;
        if (even)
        {
            for (int i = 0; i < h2; ++i, dstRow += dstStride2, srcRow += srcStride) // even lines
            {
                memcpy(dstRow, srcRow, srcStride);
            }
        }

        if (evenOnly || !even)
        {
            dstRow = dstBegin + dstStride;
            srcRow = srcBegin;
            for (int i = 0; i < h2; ++i, dstRow += dstStride2, srcRow += srcStride) // odd lines
            {
                memcpy(dstRow, srcRow, srcStride);
            }
        }
    }

    // static
    void FrameTools::FillFrameBufferInterlaced(const void* srcData, int srcStride, bool even, bool evenOnly, AVFrame* frame)
    {
        FillBitmapBufferInterlaced(srcData, srcStride, even, evenOnly, frame->data[0], frame->height, frame->linesize[0]);
    }

    // static
    int FrameTools::CalcAudioDataSize(const AVFrame* frame)
    {
        return frame
            ? frame->ch_layout.nb_channels * frame->nb_samples * BytesPerSample((AVSampleFormat)frame->format)
            : 0;
    }

    // static
    void FrameTools::LogVideoFrame(const AVFrame* frame)
    {
        LOG_INFO("-- frame: %dx%d, strides: %d/%d/%d/%d",
            frame->width, frame->height,
            frame->linesize[0], frame->linesize[1], frame->linesize[2], frame->linesize[3]);
    }

    // static
    void FrameTools::LogVideoFrame2(const AVFrame* frame)
    {
        LOG_INFO("-- frame: %dx%d, interlacing=%d/%d",
            frame->width, frame->height,
            HasFlag(frame, AV_FRAME_FLAG_INTERLACED),
            HasFlag(frame, AV_FRAME_FLAG_TOP_FIELD_FIRST));
    }

    // static
    void FrameTools::LogVideoFrame3(const AVFrame* frame, const char* msg)
    {
        if (frame)
        {
            LOG_INFO("%s, frame(data/w/h/stride): %d/%d/%d/%d", msg, frame->data[0] ? 1 : 0, frame->width, frame->height, frame->linesize[0]);
        }
        else
        {
            LOG_INFO("%s, null frame", msg);
        }
    }

    void LineSwap::SwapEvenOddLines(AVFrame* frame, int planeCount)
    {
        if (planeCount == 1)
        {
            size_t sz = std::abs(frame->linesize[0]);
            if (sz > m_BuffSize)
            {
                ::free(m_Buff);
                m_Buff = ::malloc(sz);
                m_BuffSize = m_Buff ? sz : 0;
            }
            if (m_Buff)
            {
                size_t sz2 = sz + sz;
                uint8_t* evnRow = frame->data[0];
                for (int i = 0; i < frame->height; i += 2, evnRow += sz2)
                {
                    uint8_t* oddRow = evnRow + sz;
                    ::memcpy(m_Buff, evnRow, sz);
                    ::memcpy(evnRow, oddRow, sz);
                    ::memcpy(oddRow, m_Buff, sz);
                }
            }
        }
    }

} // namespace Fcore

