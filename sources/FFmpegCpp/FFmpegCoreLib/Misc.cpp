#include "pch.h"

#include "Misc.h"

#include "Logger.h"


namespace Fcore
{

    bool AssertPtr(const void* ptr, const char* msg)
    {
        bool ret = ptr ? true : false;
        if (!ret)
        {
            LOG_ERROR("%s, null ptr", msg);
        }
        return ret;
    }

    bool AssertArgs(bool a, const char* msg)
    {
        bool ret = a;
        if (!ret)
        {
            LOG_ERROR("%s, bad args", msg);
        }
        return ret;
    }

    bool AssertArgs(const void* ptr, const char* msg)
    {
        bool ret = ptr ? true : false;
        if (!ret)
        {
            LOG_ERROR("%s, bad args (null)", msg);
        }
        return ret;
    }

    bool AssertFirst(const void* p, const char* msg)
    {
        bool ret = (p == nullptr);
        if (!ret)
        {
            LOG_ERROR("%s, already initialized", msg);
        }
        return ret;
    }

    bool AssertFirst(bool initialized, const char* msg)
    {
        bool ret = (initialized == false);
        if (!ret)
        {
            LOG_ERROR("%s, already initialized", msg);
        }
        return ret;
    }

    bool AssertAudioParams(int chann, int sampleRate, const char* msg)
    {
        bool ret = (chann > 0) && (sampleRate > 0);
        if (!ret)
        {
            LOG_ERROR("%s, bad audio params, chann=%d, samp.rate=%d", msg, chann, sampleRate);
        }
        return ret;
    }

    bool AssertAudioFormat(AVSampleFormat sampFmt, const char* msg)
    {
        bool ret = (sampFmt != AV_SAMPLE_FMT_NONE);
        if (!ret)
        {
            LOG_ERROR("%s, bad audio format", msg);
        }
        return ret;
    }

    bool AssertVideoParams(int w, int h, AVRational frameRate, const char* msg)
    {
        bool ret = (w > 0) && (h > 0) && (frameRate.num > 0) && (frameRate.den > 0);
        if (!ret)
        {
            LOG_ERROR("%s, bad  video params, frame=%dx%d, fr.rate=%d/%d", msg, w, h, frameRate.num, frameRate.den);
        }
        return ret;
    }

    bool AssertVideoFormat(AVPixelFormat pixFmt, const char* msg)
    {
        bool ret = (pixFmt != AV_PIX_FMT_NONE);
        if (!ret)
        {
            LOG_ERROR("%s, bad  video format", msg);
        }
        return ret;
    }

    bool StrAreEqIgnoreCase(const char* x, const char* y)
    {
        while (true)
        {
            int xx = *x;
            int yy = *y;
            if (xx && yy)
            {
                if (av_tolower(xx) != av_tolower(yy))
                {
                    return false;
                }
            }
            else
            {
                if (xx || yy)
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
            ++x;
            ++y;
        }
    }

    const char* FourccToStr(uint32_t val)
    {
        static char ret[2 * sizeof(val)];

        memcpy(ret, &val, sizeof(val));
        ret[sizeof(val)] = '\0';
        return StrToLog(ret);
    }

    const char* SubtitTypeToStr(AVSubtitleType stype)
    {
        switch (stype)
        {
        case SUBTITLE_NONE:   return "none";
        case SUBTITLE_BITMAP: return "bitmap";
        case SUBTITLE_TEXT:   return "text";
        case SUBTITLE_ASS:    return "ass";
        }
        return "unknown";
    }

    const char* FieldOrderToStr(AVFieldOrder fieldOrder)
    {
        switch (fieldOrder)
        {
        case AV_FIELD_UNKNOWN:     return "Unknown";
        case AV_FIELD_PROGRESSIVE: return "Progressive";
        }
        return "Interlaced";
    }

} // namespace Fcore

