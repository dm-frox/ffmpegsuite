#include "pch.h"

#include <sstream>

#include "StreamReader.h"

#include "Logger.h"
#include "Misc.h"
#include "DictTools.h"
#include "StreamInfo.h"
#include "Demuxer.h"


namespace Fcore
{
    const int StreamReader::FramePoolInterval = 20;
    const int StreamReader::NoPacketsInterval = 40;
    const bool StreamReader::UseCondVar       = true;
    const char StreamReader::ThreadTagSuff[]  = "dec";

    StreamReader::StreamReader(AVStream& strm, Demuxer& demuxer)
        : StreamBase(strm)
        , m_Demuxer(demuxer)
        , m_PacketQueue(m_Demuxer.Mutex(), m_Demuxer.PacketQueueMaxLength())
        , m_Decoder(Stream())
    {
        StreamBase::InitPump(static_cast<IProceed*>(this), UseCondVar, StreamBase::ThreadTag(ThreadTagSuff));
        m_PacketQueue.EnableReadPumpCtrl(PumpSwitchR());

        m_Framerate = m_Demuxer.GuessFrameRate(Stream());

        InitFrameTime();
        m_PacketQueue.EnableWritePumpCtrl(m_Demuxer.PumpSwitchW(), m_Demuxer.PacketQueueOffThreshold(), false);

        LogStreamInfo();
    }

    StreamReader::~StreamReader() = default;

    bool StreamReader::AssertDecoder(const char* msg) const
    {
        bool ret = m_Decoder.IsOpened();
        if (!ret)
        {
            LOG_ERROR("%s, %s, decoder is not opened", msg, MediaTypeStr());
        }
        return ret;
    }

    void StreamReader::LogStreamInfo() const
    {
        auto tb = Stream().time_base;
        LOG_INFO("%s, %s: ind=%d, id=%d, cod.id=%s, t.base=%d/%d, fr.rate=%d/%d, start=%lld, dur=%lld, fr.count=%lld, lang=%s",
            __FUNCTION__,
            MediaTypeStr(),
            Index(),
            Id(),
            CodecIdStr(),
            tb.num, tb.den,
            m_Framerate.num, m_Framerate.den,
            TsToInt64(Stream().start_time),
            TsToInt64(Stream().duration),
            TsToInt64(Stream().nb_frames),
            Language());

#ifdef LOG_METADATA_
        DictHolder::Log(Metadata(), __FUNCTION__, "stream metadata");
#endif
    }

    void StreamReader::Reset()
    {
        if (IsActivated())
        {
            m_Decoder.Reset();
            ClearQueue();
            m_CntrPkt = 0;
            m_CntrFrm = 0;
        }
    }

    void StreamReader::ClearQueue()
    {
        int c = m_PacketQueue.Clear();

        LOG_INFO("%s, %s, count=%d", __FUNCTION__, MediaTypeStr(), c);
    }

    const char* StreamReader::GetMetadata(int& len) const
    {
        if (m_Metadata.IsEmpty())
        {
            SetupMetadata(m_Metadata);
        }
        len = m_Metadata.Length();
        return m_Metadata.Str();
    }

    const char* StreamReader::GetInfo(int& len) const
    {
        if (m_Info.IsEmpty())
        {
            SetupInfo(m_Info);
        }
        len = m_Info.Length();
        return m_Info.Str();
    }

    void StreamReader::SetupMetadata(MultiString& metadata) const
    {
        metadata.SetDictionary(StreamBase::Metadata());
    }

    void StreamReader::SetupInfo(MultiString& info) const
    {
        info.Reset();
        info.Reserve(10);

        info.Add("Type", MediaTypeStr());
        info.Add("Index", Index());

        if (const AVCodecDescriptor* cdescr = avcodec_descriptor_get(CodecParams().codec_id))
        {
            info.Add("Codec ID (compression format)", cdescr->name);
            info.Add("Codec ID description", cdescr->long_name);
        }
        else
        {
            info.Add("Codec ID", "Unknown");
        }

        if (m_Decoder.IsOpened())
        {
            info.Add("Decoder", m_Decoder.Name());
            info.Add("Decoder description", m_Decoder.Description());
            if (IsVideo())
            {
                info.Add("Pixel format", PixFmtToStr(m_Decoder.PixelFormat()));
            }
            else if (IsAudio())
            {
                info.Add("Sample format", SampFmtToStr(m_Decoder.SampleFormat()));
            }
        }

        if (IsVideo())
        {
            info.Add("Scan type", FieldOrderToStr(CodecParams().field_order));

            char ratio[64] = { '\0' };
            AVRational sar = Sar();
            if (sar.num == 0)
            {
                sar.num = 1;
                sar.den = 1;
            }
            av_strlcatf(ratio, sizeof(ratio), "%d : %d", sar.num, sar.den);
            info.Add("Sample aspect ratio", ratio);

            int w = CodecParams().width, 
                h = CodecParams().height;
            info.Add("Width", w);
            info.Add("Height", h);

            int num = 0, 
                den = 0;
            int ww = w * sar.num, 
                hh = h * sar.den;
            av_reduce(&num, &den, ww, hh, Max_(ww, hh));
            ratio[0] = '\0';
            av_strlcatf(ratio, sizeof(ratio), "%d : %d", num, den);
            info.Add("Display aspect ratio", ratio);

            info.AddFramerate("Frame rate", m_Framerate);
        }
        else if (IsAudio())
        {
            ChannLayout channLayout;
            channLayout.Set(&CodecParams().ch_layout);

            info.Add("Sample rate", CodecParams().sample_rate);
            info.Add("Channels", channLayout.Chann());
            char layStr[128];
            channLayout.ToString(layStr, sizeof(layStr));
            av_strlcatf(layStr, sizeof(layStr), " (%lld)", channLayout.Mask());
            info.Add("Channel layout", layStr);
            info.Add("Frame size", CodecParams().frame_size);
        }

        info.Add("Bitrate", Bitrate());

        info.AddTime("Duration", Duration());
        info.Add("Language", Language());
        info.Add("Default", IsDefault());
        info.Add("Id", Id());

        info.SetEnd();
    }

    void StreamReader::GetStreamInfo(StreamInfo& strmInfo)
    {
        strmInfo.Reset();
        strmInfo.Type = CodecParams().codec_type;
        strmInfo.DurationS = Duration();
        strmInfo.Bitrate = Bitrate();

        if (IsVideo())
        {
            strmInfo.Width  = CodecParams().width;
            strmInfo.Height = CodecParams().height;
            strmInfo.Fps_N = m_Framerate.num;
            strmInfo.Fps_D = m_Framerate.den;
            strmInfo.PixFormat = static_cast<AVPixelFormat>((CodecParams().format));
            strmInfo.Sar = Sar();
        }

        if (IsAudio())
        {
            ChannLayout channLayout;
            channLayout.Set(&CodecParams().ch_layout);

            strmInfo.Chann = channLayout.Chann();
            strmInfo.SampRate = CodecParams().sample_rate;
            strmInfo.SampFormat = static_cast<AVSampleFormat>(CodecParams().format);
            strmInfo.ChannMask = channLayout.Mask();
            strmInfo.FrameSize = CodecParams().frame_size;
        }

        strmInfo.MediaType = MediaTypeStr();
        strmInfo.CodecId = avcodec_get_name(CodecParams().codec_id);
        strmInfo.IsDefault = IsDefault();
    }

} // namespace Fcore
