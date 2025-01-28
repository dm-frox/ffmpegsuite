#include "pch.h"

#include "Version.h"

#include "ffmpeg.h"


namespace
{
    char Buff[64];

    const char* GetVersion(int major, int minor, int micro)
    {
        Buff[0] = '\0';
        av_strlcatf(Buff, sizeof (Buff), "%2d.%2d.%3d", major, minor, micro);
        return Buff;
    }

} // namespace anon

namespace Fcore 
{

    const char* Version::AvUtil()
    {
        return GetVersion(LIBAVUTIL_VERSION_MAJOR, LIBAVUTIL_VERSION_MINOR, LIBAVUTIL_VERSION_MICRO);
    }

    const char* Version::AvCodec() 
    {
        return GetVersion(LIBAVCODEC_VERSION_MAJOR, LIBAVCODEC_VERSION_MINOR, LIBAVCODEC_VERSION_MICRO);
    }

    const char* Version::AvFormat()
    {
        return GetVersion(LIBAVFORMAT_VERSION_MAJOR, LIBAVFORMAT_VERSION_MINOR, LIBAVFORMAT_VERSION_MICRO);
    }

    const char* Version::AvDevice() 
    {
        return GetVersion(LIBAVDEVICE_VERSION_MAJOR, LIBAVDEVICE_VERSION_MINOR, LIBAVDEVICE_VERSION_MICRO);
    }

    const char* Version::AvFilter() 
    {
        return GetVersion(LIBAVFILTER_VERSION_MAJOR, LIBAVFILTER_VERSION_MINOR, LIBAVFILTER_VERSION_MICRO);
    }

    const char* Version::SwScale()
    {
        return GetVersion(LIBSWSCALE_VERSION_MAJOR, LIBSWSCALE_VERSION_MINOR, LIBSWSCALE_VERSION_MICRO);
    }

    const char* Version::SwResample()
    {
        return GetVersion(LIBSWRESAMPLE_VERSION_MAJOR, LIBSWRESAMPLE_VERSION_MINOR, LIBSWRESAMPLE_VERSION_MICRO);
    }

    const char* Version::PostProc()
    {
        return GetVersion(LIBPOSTPROC_VERSION_MAJOR, LIBPOSTPROC_VERSION_MINOR, LIBPOSTPROC_VERSION_MICRO);
    }

    static const char* WrapperVersion();

    const char* Version::Wrapper()
    {
        return WrapperVersion();
    }

    const char* Version::FFmpeg()
    {
        return av_version_info();
    }

    static const char* WrapperVersion()
    {
        return
            // FFmpeg version info: 6.0-full_build-www.gyan.dev
            "1.0.0" // 14.12.2022
            ;
    }

// ---------------------------------------------------------------------

    void InitializeFFmpeg()
    {
        avdevice_register_all();
    }

} // namespace Fcore 
