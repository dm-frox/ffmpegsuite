#include "pch.h"

#include "CodecBase.h"

#include "Logger.h"
#include "Misc.h"
#include "DictTools.h"
#include "CodecDecIface.h"

namespace Fcore
{

    const char CodecBase::HWSepr = '+';

    CodecBase::CodecBase(AVCodecContext* codecCtx)
        : MediaTypeHolder(codecCtx)
        , m_CodecCtx(codecCtx)
    {}

    CodecBase::CodecBase(AVStream& strm)
        : MediaTypeHolder(strm)
        , m_CodecCtx(nullptr)
    {}

    CodecBase::~CodecBase()
    {
        Close();
    }

    void CodecBase::Close()
    {
        avcodec_free_context(&m_CodecCtx);
    }

    AVRational CodecBase::Timebase() const
    { 
        return m_CodecCtx ? m_CodecCtx->time_base : AVRational{0, 1};
    }

    void CodecBase::CorrectChannLayout()
    {
        if (m_ChannLayout.Mask() == 0)
        {
            m_ChannLayout.SetFromChann(m_ChannLayout.Chann());
            LOG_INFO("%s, force ch.mask=%lld", __FUNCTION__, m_ChannLayout.Mask());
        }
    }

    void CodecBase::SetMediaParams()
    {
        if (m_CodecCtx)
        {
            if (IsVideo())
            {
                m_Width = m_CodecCtx->width;
                m_Height = m_CodecCtx->height;
                m_PixelFormat = m_CodecCtx->pix_fmt;
            }
            else if (IsAudio())
            {
                m_SampleRate = m_CodecCtx->sample_rate;
                m_SampleFormat = m_CodecCtx->sample_fmt;
                m_ChannLayout.Set(&m_CodecCtx->ch_layout);
                CorrectChannLayout();
            }
            m_Name = m_CodecCtx->codec->name;
            m_Description = m_CodecCtx->codec->long_name;
        }
    }

    void CodecBase::SetMediaParams(const IDecoder* decoder, const char* hwaccel)
    {
        if (decoder)
        {
            if (IsVideo())
            {
                m_Width = decoder->Width();
                m_Height = decoder->Height();
                m_PixelFormat = decoder->PixelFormat();
            }
            else if (IsAudio())
            {
                m_SampleRate = decoder->SampleRate();
                m_SampleFormat = decoder->SampleFormat();
                if (const AVChannelLayout* chl = decoder->ChannLauoyt())
                {
                    m_ChannLayout.Set(chl);
                    CorrectChannLayout();
                }
                else
                {
                    m_ChannLayout.SetFromChann(decoder->Chann());
                }
            }
            m_Name = decoder->Name();
            m_Description = decoder->Description();
            if (hwaccel && *hwaccel)
            {
                m_Name += HWSepr;
                m_Name += hwaccel;
            }
        }
    }

} // namespace Fcore
