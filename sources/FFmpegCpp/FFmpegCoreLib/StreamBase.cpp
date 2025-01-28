#include "pch.h"

#include "StreamBase.h"

#include "Logger.h"
#include "Misc.h"
#include "DictTools.h"

namespace Fcore
{

    const char StreamBase::LangKey[] = "language";
    const char StreamBase::NoLang[]  = "no";
    char StreamBase::ThreadTagBuff[8];


    StreamBase::StreamBase(AVStream& strm) 
        : MediaTypeHolder(strm)
        , m_Stream(strm)
        , m_StartTime((m_Stream.start_time != AV_NOPTS_VALUE) ? m_Stream.start_time : 0)
    {
        SetLanguage();
    }

    StreamBase::~StreamBase() = default;

    const AVDictionary* StreamBase::Metadata() const
    {
        return m_Stream.metadata;
    }

    void StreamBase::AddDispositionFlag(int flag)
    {
        m_Stream.disposition |= flag;
    }

    bool StreamBase::CheckDispositionFlag(int flag) const
    {
        return HasFlag(m_Stream.disposition, flag);
    }

    bool StreamBase::IsDefault() const
    {
        return CheckDispositionFlag(AV_DISPOSITION_DEFAULT);
    }

    //static 
    const char* StreamBase::GetMetadataItem(const AVStream& strm, const char* key)
    {
        const char* ret = nullptr;
        if (key)
        {
            DictHolder::FindKey(strm.metadata, key, ret);
        }
        return ret;
    }

    const char* StreamBase::GetMetadataItem(const char* key) const
    {
        return GetMetadataItem(m_Stream, key);
    }

    void StreamBase::SetLanguage()
    {
        const char* lang = GetMetadataItem(LangKey);
        m_Lang = lang ? lang : NoLang;
    }

    int StreamBase::AudioChann() const
    {
        return IsAudio() ? CodecParams().ch_layout.nb_channels : 0;
    }

    int  StreamBase::AudioSampleRate() const
    {
        return IsAudio() ? CodecParams().sample_rate : 0;
    }

    const char* StreamBase::CodecIdStr() const
    {
        return CodecIdToStr(CodecParams().codec_id);
    }

    int64_t StreamBase::Bitrate() const
    {
        return CodecParams().bit_rate;
    }

    double StreamBase::Duration() const
    {
        return Fcore::PosTsToPosSecEx(m_Stream.duration, m_Stream.time_base);
    }

    AVRational StreamBase::Sar() const
    {
        AVRational ret = m_Stream.sample_aspect_ratio;
        if (ret.num == 0)
        {
            ret = CodecParams().sample_aspect_ratio;
        }
        return ret;
    }

    double StreamBase::PosTsToPosSec(int64_t ts) const
    {
        return Fcore::PosTsToPosSec(ts, m_Stream.time_base);
    }

    int64_t StreamBase::PosSecToPosTs(double posSec) const
    {
        return Fcore::PosSecToPosTs(posSec, m_Stream.time_base);
    }

    void StreamBase::InitPump(Ftool::IProceed* pr, bool useCondVar, const char* name, bool logEx)
    { 
        m_Pump.Init(pr, useCondVar, name, logEx);
    }

    bool StreamBase::CreatePump()
    {
        return m_Pump.CreatePump();
    }

    void StreamBase::ClosePump()
    {
        m_Pump.ClosePump();
    }

    void StreamBase::Run()
    {
        m_Pump.Run();
    }

    void StreamBase::Pause()
    {
        m_Pump.Pause();
    }

    void StreamBase::Stop(bool skipPause)
    {
        m_Pump.Stop(skipPause);
    }

    bool StreamBase::PumpError() const
    {
        return m_Pump.PumpError();
    }

    const char* StreamBase::ThreadTag(const char* suff) const
    {
        ThreadTagBuff[0] = '\0';
        av_strlcatf(ThreadTagBuff, sizeof(ThreadTagBuff), "%c-%s", *MediaTypeStr(), suff);
        return ThreadTagBuff;
    }

} // namespace Fcore

