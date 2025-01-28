#include "pch.h"

#include "SubtitDump.h"

#include "Misc.h"
#include "Logger.h"


namespace Fcore
{

    SubtitDump::SubtitDump()
        : m_Muxer(false)
    {}

    SubtitDump::~SubtitDump() = default;

    void SubtitDump::Dump(const AVSubtitle& sub)
    {
        int n = (int)sub.num_rects;
        for (int i = 0; i < n; ++i)
        {
            if (AVSubtitleRect* rect = sub.rects[i])
            {
                AVSubtitleType t = rect->type;
                LOG_INFO(" -- AVSubtitleRect, ind=%d, type=%s", i, SubtitTypeToStr(t));
                if (t == SUBTITLE_TEXT)
                {
                    LOG_INFO(" -- txt: %s", StrToLog(rect->text));
                }
                else if (t == SUBTITLE_ASS)
                {
                    LOG_INFO(" -- ass: %s", StrToLog(rect->ass));
                }
            }
            else
            {
                break;
            }
        }
    }

    void SubtitDump::Dump(StreamReader* strm, int count)
    {
        if (strm->OpenDecoderNoHub() >= 0)
        {
            for (int i = 0, rr = 0; i < count && rr == 0; ++i)
            {
                rr = m_Muxer.ProceedEx();
                if (rr == 0)
                {
                    if (AVPacket* pkt = strm->PopPacket())
                    {
                        AVSubtitle sub;
                        int got = 0;
                        int r = strm->GetDecoder().DecodeSubtitle(pkt, &sub, &got);
                        LOG_INFO("DecodeSubtitle, i=%2d, res=%d, got=%d, format=%d, num.rects=%d", i,
                            r, got, got > 0 ? sub.format : 0, got > 0 ? sub.num_rects : 0);
                        if (got)
                        {
                            Dump(sub);
                            Decoder::FreeSubtitle(&sub);
                        }
                        PacketTools::Free(pkt);
                    }
                    else
                    {
                        LOG_ERROR("%s, no packet", __FUNCTION__);
                    }
                }
                if (rr != 0)
                {
                    if (rr == 1)
                    {
                        LOG_INFO("%s, end of data ::::::::::", __FUNCTION__);
                    }
                    else
                    {
                        LOG_ERROR("%s, proceed res=%d, ind=%d", __FUNCTION__, rr, i);
                    }
                }
            }
        }
    }

    void SubtitDump::Build(const char* path, const char* lang, int count)
    {
        if (m_Muxer.Open(path, nullptr, nullptr) >= 0)
        {
            int ind = m_Muxer.FindSubtitStreamByLang(lang);
            if (ind >= 0)
            {
                StreamReader* strm = m_Muxer.GetStreamReader(ind);
                LOG_INFO("%s, index=%d, req.lang=%s, act.lang=%s", __FUNCTION__, ind, lang, strm->Language());
                Dump(strm, count);
            }
            else
            {
                LOG_ERROR("%s, no subtit streams", __FUNCTION__);
            }
        }
        else
        {
            LOG_ERROR("%s, failed to open url reader", __FUNCTION__);
        }
    }

} // namespace Fcore

    //enum AVSubtitleType {
    //  SUBTITLE_NONE,
    //  SUBTITLE_BITMAP,                ///< A bitmap, pict will be set
    // 
    //   * Plain text, the text field must be set by the decoder and is
    //   * authoritative. ass and pict fields may contain approximations.
    //   SUBTITLE_TEXT,
    // 
    //    * Formatted text, the ass field must be set by the decoder and is
    //    * authoritative. pict and text fields may contain approximations.
    //    SUBTITLE_ASS,
    //};


