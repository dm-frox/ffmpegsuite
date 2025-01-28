#include "pch.h"

#include "AudioArrMuxer.h"

#include "ThreadTools.h"


namespace Fapp
{
    const double AudioArrMuxer::WriteDataThreshold      = 0.01; // 10 ms
    const int    AudioArrMuxer::WaitWriteDataIntervalMs = 20;


    AudioArrMuxer::AudioArrMuxer()
        : m_AudioArray(AV_CODEC_ID_PCM_S16LE, 66, 16000, AV_SAMPLE_FMT_S16, 512)
        , m_PacketTime(m_AudioArray.Timebase())
        , m_DataPacket(av_packet_alloc())
    {
        m_AudioArray.Log(__FUNCTION__);
    }

    AudioArrMuxer::~AudioArrMuxer()
    {
        Close();
        av_packet_free(&m_DataPacket);
    }

    const void* AudioArrMuxer::GetData(int )
    {
        return nullptr;
    }

    double AudioArrMuxer::SetTimestamps(int i)
    {
        m_DataPacket->pts = i;
        m_DataPacket->dts = i;
        m_DataPacket->duration = 1;

        double t = m_PacketTime.RecalcPacketTimestamps(m_DataPacket);

        if (i == 0)
        {
            LOG_TEMP("%s, OK, pack.dur=%lld", __FUNCTION__, m_DataPacket->duration);
        }

        return t;
    }

    bool AudioArrMuxer::SetData(int i)
    {
        bool ret = false;
        int rr = av_new_packet(m_DataPacket, m_AudioArray.BuffSize());
        if (rr >= 0)
        {
            const void* data = GetData(i);
            data ? ::memcpy(m_DataPacket->data, data, m_DataPacket->size)
                 : ::memset(m_DataPacket->data, 0, m_DataPacket->size);

            m_DataPacket->stream_index = m_DataStrmIndex;

            ret = true;
        }
        else
        {
            LOG_ERROR("%s, av_new_packet", __FUNCTION__);
        }
        return ret;
    }

    void AudioArrMuxer::WriteData(int n)
    {
        LOG_INFO("%s, enter, packCount=%d {{", __FUNCTION__, n);

        m_DataEnd = false;
        m_DataErr = false;
        bool res = true;

        if (!m_DataPacket)
        {
            LOG_ERROR("%s, null packet", __FUNCTION__);
            res = false;
        }
        else if (m_DataStrmIndex < 0)
        {
            LOG_ERROR("%s, no data strm", __FUNCTION__);
            res = false;
        }

        for (int i = 0; i < n && res; ++i)
        {
            if (SetData(i))
            {
                double t = SetTimestamps(i) - WriteDataThreshold;

                while (t > m_Muxer.Position() && !m_Muxer.EndOfData())
                {
                    Ftool::ThreadTools::Sleep(WaitWriteDataIntervalMs);
                }

                if (m_Muxer.WritePacket(m_DataPacket) < 0)
                {
                    res = false;
                    LOG_ERROR("%s, WritePacket, i=%d", __FUNCTION__, i);
                }
                //else
                //{
                //  LOG_TEMP("^^^^^, %d", i);
                //}
            }
            else
            {
                res = false;
            }
        }
        if (!res)
        {
            m_DataErr = true;
        }
        m_DataEnd = true;
        LOG_INFO("%s, exit }}", __FUNCTION__);
    }

    int AudioArrMuxer::Build(const char* urlVideo, const char* urlAudio, const char* urlDst)
    {
        int ret = -1;
        LOG_INFO("%s, begin...", __FUNCTION__);
        int rr = m_FrameSourceV.OpenDemuxer(urlVideo, nullptr, nullptr, true, false);
        if (rr >= 0)
        {
            rr = m_FrameSourceA.OpenDemuxer(urlAudio, nullptr, nullptr, false, true);
            if (rr >= 0)
            {
                rr = m_Muxer.Open(urlDst, nullptr);
                if (rr >= 0)
                {
                    rr = m_FrameSourceV.ConnectToMuxerVideo(&m_Muxer);
                    if (rr >= 0)
                    {
                        LOG_INFO("%s, added video, ind=%d", __FUNCTION__, rr);
                        m_Muxer.SetLastStreamDefault();
                        rr = m_FrameSourceA.ConnectToMuxerAudio(&m_Muxer);
                        if (rr >= 0)
                        {
                            LOG_INFO("%s, added audio, ind=%d", __FUNCTION__, rr);
                            m_Muxer.SetLastStreamDefault();
                            rr = m_Muxer.SetupOutputStreamAudioEx(
                                m_AudioArray.CodecId, 
                                m_AudioArray.Chann, 
                                m_AudioArray.SampleRate,
                                m_AudioArray.SampFmt,
                                m_AudioArray.FrameSize);
                            if (rr >= 0)
                            {
                                LOG_INFO("%s, added audio, ind=%d", __FUNCTION__, rr);
                                m_DataStrmIndex = rr;

                                rr = m_Muxer.OpenWriting(nullptr, nullptr, nullptr, nullptr);
                                if (rr >= 0)
                                {
                                    m_PacketTime.InitDst(m_Muxer.GetTimebase(m_DataStrmIndex));
                                    int dataPackCount = m_PacketTime.GetPacketCount(m_FrameSourceA.MediaSourceInfo()->Duration());
                                    LOG_INFO("%s, OK, data stream: ind=%d, t.base=%d/%d, pack.count=%d", __FUNCTION__, 
                                        m_DataStrmIndex, m_PacketTime.DstTb_N(), m_PacketTime.DstTb_D(), dataPackCount);
                                    SetupPump();
                                    Run();
                                    //WriteData(1); // TEMP
                                    m_DataThread = std::thread([this](int n) { WriteData(n); }, dataPackCount);
                                    ret = rr;
                                    m_Ready = true;
                                }
                            }
                            else
                            {
                                LOG_ERROR("%s, data stream", __FUNCTION__);
                            }
                        }
                    }
                }
            }
        }
        return ret;
    }

    void AudioArrMuxer::SetupPump()
    {
        m_Muxer.CreatePump();
        m_FrameSourceV.CreatePump();
        m_FrameSourceA.CreatePump();
    }

    void AudioArrMuxer::Run()
    {
        m_Muxer.Run();
        m_FrameSourceA.Run();
        m_FrameSourceV.Run();
    }

    void AudioArrMuxer::Pause()
    {
        m_Muxer.Pause();
        m_FrameSourceV.Pause();
        m_FrameSourceA.Pause();
    }

    bool AudioArrMuxer::EndOfData() const
    {
        return m_Muxer.EndOfData() && m_DataEnd;
    }

    bool AudioArrMuxer::Error() const
    {
        return m_Muxer.PumpError() || m_FrameSourceV.PumpError() || m_FrameSourceA.PumpError() || m_DataErr;
    }

    double AudioArrMuxer::Position() const
    {
        return m_Ready ? m_Muxer.Position() : 0.0;
    }

    void AudioArrMuxer::Close()
    {
        if (!m_Closed && m_Ready)
        {
            LOG_INFO("%s, begin...", __FUNCTION__);
            m_Muxer.CloseWriting();
            m_FrameSourceV.ClosePump();
            m_FrameSourceA.ClosePump();
            m_Muxer.ClosePump();
            m_DataThread.join();
            LOG_INFO("%s, end", __FUNCTION__);
            m_Closed = true;
            m_Ready = false;
        }
    }

} // namespace Fapp
