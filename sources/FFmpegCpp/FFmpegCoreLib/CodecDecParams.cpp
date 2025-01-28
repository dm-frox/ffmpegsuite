#include "pch.h"

#include "CodecDecParams.h"

#include "Misc.h"
#include "MultiString.h"
#include "DictTools.h"
#include "Logger.h"
#include "CodecDecGpu.h"

namespace Fcore
{

    ParamsEx::ParamsEx(const char* decParamsEx, AVMediaType mediaType, AVCodecID codecId)
        : MediaTypeHolder(mediaType)
    {
        Parse(decParamsEx, codecId);
    }

    ParamsEx::~ParamsEx() = default;

    void ParamsEx::SetName(const char* name, AVCodecID codecId)
    {
        if (name && *name)
        {
            if (IsVideo())
            {
                if (HasGpuPrefix(name))
                {
                    if (const char* gpuDecName = CodecGpu::GetName(MediaType(), codecId, name))
                    {
                        m_Name = gpuDecName;
                        m_IsGpu = true;
                    }
                    else
                    {
                        LOG_WARNING("%s, %s, cannot find gpu decoder, type=%s", __FUNCTION__, MediaTypeStr(), name);
                    }
                }
                else
                {
                    m_Name = name;
                    if (CodecGpu::IsGpuDecoder(m_Name.c_str()))
                    {
                        m_IsGpu = true;
                    }
                }
            }
            else
            {
                m_Name = name;
            }
            if (!m_Name.empty())
            {
                LOG_INFO("%s, %s, dec.name=%s, gpu=%d", __FUNCTION__, MediaTypeStr(), m_Name.c_str(), m_IsGpu);
            }
        }
    }

    void ParamsEx::SetFormat(const char* fmt)
    {
        if (fmt && *fmt)
        {
            if (IsVideo())
            {
                AVPixelFormat pf = PixFmtFromStr(fmt);
                if (pf != AV_PIX_FMT_NONE)
                {
                    m_PrefPixFmt = pf;
                    LOG_INFO("%s, %s, pix.fmt=%s", __FUNCTION__, MediaTypeStr(), PixFmtToStr(m_PrefPixFmt));
                }
                else
                {
                    LOG_WARNING("%s, %s, cannot get pix.fmt=%s", __FUNCTION__, MediaTypeStr(), fmt);
                }
            }
            if (IsAudio())
            {
                AVSampleFormat sf = SampFmtFromStr(fmt);
                if (sf != AV_SAMPLE_FMT_NONE)
                {
                    m_PrefSampFmt = sf;
                    LOG_INFO("%s, %s, samp.fmt=%s", __FUNCTION__, MediaTypeStr(), SampFmtToStr(m_PrefSampFmt));
                }
                else
                {
                    LOG_WARNING("%s, %s, cannot get samp.fmt=%s", __FUNCTION__, MediaTypeStr(), fmt);
                }
            }
        }
    }

    void ParamsEx::SetOptions(const char* opts)
    {
        if (opts && *opts)
        {
            m_OptsIni = opts;
        }
    }

    void ParamsEx::SetHWAccel(const char* hw)
    {
        if (hw && *hw && IsVideo())
        {
            m_HWAccel = hw;
            LOG_INFO("%s, %s, hwaccel=%s", __FUNCTION__, MediaTypeStr(), m_HWAccel.c_str());
        }
    }

    void ParamsEx::SetThreads(const char* th)
    {
        if (th && *th && IsVideo())
        {
            m_Threads = th;
            LOG_INFO("%s, %s, threads=%s", __FUNCTION__, MediaTypeStr(), m_Threads.c_str());
        }
    }

    void ParamsEx::SetupOptions()
    {
        if (!m_Threads.empty())
        {
            if (!m_IsGpu && m_HWAccel.empty())
            {
                m_OptsIni += "threads=";
                m_OptsIni += m_Threads;
                m_OptsIni += '\n';
            }
            else
            {
                LOG_WARNING("%s, %s, ignore threads because of gpu/hwaccel", __FUNCTION__, MediaTypeStr());
            }
        }
        if (!m_OptsIni.empty())
        {
            m_Options = MultiString::FromIniFormat(m_OptsIni.c_str());
            if (!m_Options.empty())
            {
                std::string ss = MultiString::ToString(m_Options);
                LOG_INFO("%s, %s, options:", __FUNCTION__, MediaTypeStr());
                LOG_INFO(" -- %s", ss.c_str());
            }
        }
    }

    void ParamsEx::Parse(const char* decParamsEx, AVCodecID codecId)
    {
         if (decParamsEx && *decParamsEx)
         {
            DictHolder dict(decParamsEx);
            if (dict.Count() > 0)
            {
                const char* mt = dict["mediatype"];
                if ((IsVideo() && StrAreEq(mt, "video")) || (IsAudio() && StrAreEq(mt, "audio")))
                {
                    SetName(dict["name"], codecId);
                    SetFormat(dict["format"]);
                    SetOptions(dict["options"]);
                    SetHWAccel(dict["hwaccel"]);
                    SetThreads(dict["threads"]);
                    SetupOptions();
                }
                else
                {
                    LOG_WARNING("%s, %s, wrong media type=%s", __FUNCTION__, MediaTypeStr(), StrToLog(mt));
                }
            }
            // else empty dictionary
         }
         // else empty string
    }

} // namespace Fcore