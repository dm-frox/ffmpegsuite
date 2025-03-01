// ---------------------------------------------------------------------
// File: FrameSinkVideo.cpp
// Classes: FrameSinkVideoImpl, FrameSinkVideoNet
// Purpose: native wrapper for .NET video renderer
// Module: RendVideoInterop.dll - .NET assembly which provides native interface for .NET video rendering
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "pch.h"

#using "RendVideo.dll"

#include <msclr/gcroot.h>

#include "../../FFmpegCpp/FFmpegWrapper/include/FrameSinkVideoIface.h"

namespace RendVideoInterop
{
    using Ptr = System::IntPtr;
    using VideoRend = RendVideo::VideoRenderer;

    class FrameSinkVideoImpl : public Fwrap::IFrameSinkVideo
    {
        msclr::gcroot<VideoRend^> m_VideoRend;

    public:
        FrameSinkVideoImpl(VideoRend^ rendVideo);
    private:
        ~FrameSinkVideoImpl();
// IFrameSinkVideo impl
    public:
        bool CheckPixelFormat(const char* pixFmt) override;
        bool WriteFrame(int width, int height, const void* data, int stride) override;
        bool WriteFrameYUV(int width, int height,
            const void* ydata, int ystride,
            const void* udata, int ustride,
            const void* vdata, int vstride) override;
        bool WriteFrameNV(int width, int height,
            const void* ydata, int ystride,
            const void* uvdata, int uvstride) override;
        int FrameWidth() const override;
        int FrameHeight() const override;
        void Detach() override;

    }; // class FrameSinkVideoImpl

    FrameSinkVideoImpl::FrameSinkVideoImpl(VideoRend^ videoRend)
        : m_VideoRend(videoRend)
    {}

    FrameSinkVideoImpl::~FrameSinkVideoImpl() = default;

    void FrameSinkVideoImpl::Detach()
    {
        m_VideoRend->Close();
        delete this;
    }

    bool FrameSinkVideoImpl::CheckPixelFormat(const char* pixFmt)
    {
        m_VideoRend->Close();
        return VideoRend::CheckPixelFormat(Ptr(const_cast<char*>(pixFmt)));
    }

    bool FrameSinkVideoImpl::WriteFrame(int width, int height, const void* data, int stride)
    {
        return m_VideoRend->WriteFrame(Ptr(const_cast<void*>(data)), width, height, stride);
    }

    bool FrameSinkVideoImpl::WriteFrameYUV(int width, int height,
        const void* ydata, int ystride,
        const void* udata, int ustride,
        const void* vdata, int vstride)
    {
        return false;
    }

    bool FrameSinkVideoImpl::WriteFrameNV(int width, int height,
        const void* ydata, int ystride,
        const void* uvdata, int uvstride)
    {
        return false;
    }

    int FrameSinkVideoImpl::FrameWidth() const
    {
        return m_VideoRend->Width;
    }

    int FrameSinkVideoImpl::FrameHeight() const
    {
        return m_VideoRend->Height;
    }

// ------------------------------------------------------------------------------------------

    public ref class FrameSinkVideoNet sealed
    {
    public:
        static Ptr CreateInstanse(VideoRend^ videoRend);

    }; // ref class FrameSinkVideoNet

    Ptr FrameSinkVideoNet::CreateInstanse(VideoRend^ videoRend)
    {
        auto ptr = static_cast<Fwrap::IFrameSinkVideo*>(new FrameSinkVideoImpl(videoRend));
        return Ptr(ptr);
    }

} // namespace RendVideoInterop

