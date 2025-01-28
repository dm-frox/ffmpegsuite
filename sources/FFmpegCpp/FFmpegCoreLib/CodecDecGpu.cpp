#include "pch.h"

#include "CodecDecGpu.h"

#include "Misc.h"


namespace Fcore
{
    // ffmpeg version 6.0

    // static
    const CodecGpu::GpuDecoderItem CodecGpu::GpuDecoderItemsQsv[] =
    {
        { AV_CODEC_ID_H264,       "h264_qsv" },
        { AV_CODEC_ID_HEVC,       "hevc_qsv" },
        { AV_CODEC_ID_VP8,        "vp8_qsv" },   // decoder error
        { AV_CODEC_ID_VP9,        "vp9_qsv" },
        { AV_CODEC_ID_AV1,        "av1_qsv" },   // decoder error
        { AV_CODEC_ID_MJPEG,      "mjpeg_qsv" },
        { AV_CODEC_ID_MPEG2VIDEO, "mpeg2_qsv" },
        { AV_CODEC_ID_VC1,        "vc1_qsv" },   // decoder error
    };

    // static
    const CodecGpu::GpuDecoderItem CodecGpu::GpuDecoderItemsCuvid[] =
    {
        { AV_CODEC_ID_H264,       "h264_cuvid" },
        { AV_CODEC_ID_HEVC,       "hevc_cuvid" },
        { AV_CODEC_ID_VP8,        "vp8_cuvid" },
        { AV_CODEC_ID_VP9,        "vp9_cuvid" },
        { AV_CODEC_ID_AV1,        "av1_cuvid" },
        { AV_CODEC_ID_MJPEG,      "mjpeg_cuvid" },
        { AV_CODEC_ID_MPEG1VIDEO, "mpeg1_cuvid" },
        { AV_CODEC_ID_MPEG2VIDEO, "mpeg2_cuvid" },
        { AV_CODEC_ID_MPEG4,      "mpeg4_cuvid" },
        { AV_CODEC_ID_VC1,        "vc1_cuvid" },
    };


    //static 
    const char CodecGpu::QsvSuff[]   = "_qsv";

    //static 
    const char CodecGpu::CuvidSuff[] = "_cuvid";


    // 	static 
    template <size_t N>
    const char* CodecGpu::FindName(const GpuDecoderItem(&items)[N], AVCodecID id)
    {
        for (const auto& item : items)
        {
            if (item.Id == id)
            {
                return item.Name;
            }
        }
        return nullptr;
    }

    //static 
    bool CodecGpu::IsGpuDecoder(const char* decName)
    {
        return std::strstr(decName, QsvSuff) || std::strstr(decName, CuvidSuff);
    }

    //static 
    const char* CodecGpu::GetName(const AVMediaType mt, AVCodecID cid, const char* gpuSuff)
    {
        const char* ret = nullptr;
        if (mt == AVMEDIA_TYPE_VIDEO)
        {
            if (StrAreEq(gpuSuff + 1, QsvSuff + 1))
            {
                ret = FindName(GpuDecoderItemsQsv, cid);
            }
            else if (StrAreEq(gpuSuff + 1, CuvidSuff + 1))
            {
                ret = FindName(GpuDecoderItemsCuvid, cid);
            }
        }
        return ret;
    }

} // namespace Fcore
