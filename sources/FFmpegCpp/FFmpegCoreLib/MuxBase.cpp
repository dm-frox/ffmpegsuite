#include "pch.h"

#include "MuxBase.h"

#include "Logger.h"
#include "CommonConsts.h"
#include "Misc.h"
#include "DictTools.h"
#include "CodecTools.h"
#include "MultiString.h"
#include "MediaTypeHolder.h"
#include "MetadataTools.h"
#include "ChannLayout.h"


namespace Fcore
{
    MuxBase::MuxBase() = default;

    MuxBase::~MuxBase()
    {
        if (m_FmtCtx)
        {
            CloseOutputFile();
        }
    }

    void MuxBase::SetMetadata(const char* metadata, const char* msg)
    {
        if (m_FmtCtx)
        {
            Fcore::SetMetadata(m_FmtCtx, metadata, msg);
        }
    }

    // static 
    bool MuxBase::CheckOutputFlag(const AVOutputFormat* outFmt, int flag)
    { 
        return (HasFlag(outFmt->flags, flag)); 
    }

    int MuxBase::Open(const char* url, const char* formatName)
    {
        int ret = -1;
        if (!AssertFirst(__FUNCTION__))
            return ErrorAlreadyInitialized;

        AVFormatContext* fmtCtx = nullptr;
        AVOutputFormat*  ofmt = nullptr;
        ret = avformat_alloc_output_context2(&fmtCtx, ofmt, EmptyToNull(formatName), url);
        if (ret >= 0)
        {
            if (const AVOutputFormat* outFmt = fmtCtx->oformat)
            {
                m_File      = !CheckOutputFlag(outFmt, AVFMT_NOFILE);
                m_GlobalHeader = CheckOutputFlag(outFmt, AVFMT_GLOBALHEADER);
                m_FmtCtx = fmtCtx;
                LOG_INFO("%s, url=%s, fmt.name=%s; out fmt: flags(file/glb.hdr)=%d/%d, name=%s, l.name=%s",
                    __FUNCTION__,
                    url, StrToLog(formatName),
                    m_File, m_GlobalHeader,
                    StrToLog(outFmt->name), StrToLog(outFmt->long_name));
            }
            else
            {
                ret = -1;
                LOG_ERROR("%s, no output format", __FUNCTION__);
            }
        }
        else
        {
            LOG_ERROR("%s, avformat_alloc_output_context2, %s", __FUNCTION__, FmtErr(ret));
        }
        return ret;
    }

    int MuxBase::OpenOutputFile()
    {
        int ret = 0;
        if (m_File)
        {
            const char* url = m_FmtCtx->url;
            ret = avio_open(&m_FmtCtx->pb, url, AVIO_FLAG_WRITE);
            if (ret >= 0)
            {
                LOG_INFO("%s, file=%s", __FUNCTION__, url);
            }
            else
            {
                LOG_ERROR("%s, avio_open, file=%s, %s", __FUNCTION__, url, FmtErr(ret));
            }
        }
        return ret;
    }

    int MuxBase::WriteHeader(const char* options)
    {
        AVDictionary* opts = MultiString::CreateDictionary(options);
        DictHolder::Log(opts, __FUNCTION__, "options");
        int ret = avformat_write_header(m_FmtCtx, &opts);
        if (ret >= 0)
        {
            LOG_INFO("%s,", __FUNCTION__);
            DictHolder::LogNotFound(opts, __FUNCTION__);
        }
        else
        {
            LOG_ERROR("%s, avformat_write_header, %s", __FUNCTION__, FmtErr(ret));
        }
        return ret;
    }

    int MuxBase::OpenWriting(const char* options)
    {
        int ret = -1;

        if (!AssertFormat(__FUNCTION__))
            return ErrorNoFormatCtx;

        if (!AssertStreams(__FUNCTION__))
            return ErrorNoStreams;

        //DumpFopmat(true);

        ret = OpenOutputFile();
        if (ret >= 0)
        {
            ret = WriteHeader(options);
        }

        if (ret >= 0)
        {
            LOG_INFO("%s", __FUNCTION__);
        }
        return ret;
    }

    int MuxBase::WriteTrailer()
    {
        int ret = av_write_trailer(m_FmtCtx);
        if (ret >= 0)
        {
            LOG_INFO("%s", __FUNCTION__);
        }
        else
        {
            LOG_ERROR("%s, avformat_write_trailer, %s", __FUNCTION__, FmtErr(ret));
        }
        return ret;
    }

    int MuxBase::CloseOutputFile()
    {
        int ret = 0;

        if (m_File)
        {
            if (auto ioCtx = m_FmtCtx->pb)
            {
                ret = avio_close(ioCtx);
                m_FmtCtx->pb = nullptr;

                if (ret >= 0)
                {
                    LOG_INFO("%s", __FUNCTION__);
                }
                else
                {
                    LOG_ERROR("%s, avio_close, %s", __FUNCTION__, FmtErr(ret));
                }
            }
        }

        return ret;
    }

    int MuxBase::Finalize()
    {
        if (!AssertFormat(__FUNCTION__))
            return ErrorNoFormatCtx;

        LOG_INFO("%s, begin...", __FUNCTION__);

        int ret = WriteTrailer();
        int rr = CloseOutputFile();

        if (ret >= 0)
        {
            if (rr >= 0)
            {
                LOG_INFO("%s", __FUNCTION__);
            }
            else
            {
                ret = rr;
            }
        }

        return ret;
    }

    //static
    const AVCodec* MuxBase::GetEncoderByName(const char* encoderName)
    {
        const AVCodec* ret = nullptr;

        if (encoderName && *encoderName)
        {
            ret = avcodec_find_encoder_by_name(encoderName);

            if (!ret)
            {
                LOG_ERROR("%s, failed to find encoder, name=%s", __FUNCTION__, encoderName);
            }
        }
        else
        {
            LOG_ERROR("%s, empty name", __FUNCTION__);
        }
        return ret;
    }

    int MuxBase::AddOutputStreamAudio(AVCodecID codecId, int chann, int sampRate, AVSampleFormat sampFmt, int frameSize)
    {
        int ret = -1;

        if (!AssertFormat(__FUNCTION__))
        {
            return ErrorNoFormatCtx;
        }

        if (AVStream* strm = avformat_new_stream(m_FmtCtx, nullptr))
        {
            if (auto codecPar = strm->codecpar)
            {
                codecPar->codec_type = AVMEDIA_TYPE_AUDIO;
                codecPar->codec_id = codecId;
                ChannLayout(chann).CopyTo(codecPar->ch_layout);
                codecPar->sample_rate = sampRate;
                codecPar->format = (int)sampFmt;
                codecPar->frame_size = frameSize;

                strm->time_base = AVRational{ frameSize, sampRate };
                ret = strm->index;

                LOG_INFO("%s, ind=%d, cod.id=%s, chann=%d, samp.rate=%d, samp.fmt=%s, frm.size=%d", __FUNCTION__,
                    ret,
                    CodecIdToStr(codecPar->codec_id),
                    codecPar->ch_layout.nb_channels,
                    codecPar->sample_rate,
                    SampFmtToStr((AVSampleFormat)codecPar->format),
                    codecPar->frame_size);
            }
            else
            {
                LOG_ERROR("%s, null codecpar", __FUNCTION__);
            }
        }
        else
        {
            LOG_ERROR("%s, avformat_new_stream() failed", __FUNCTION__);
            ret = AVERROR(ENOMEM);
        }
        return ret;
    }

    int MuxBase::AddOutputStream(const AVStream* srcStream, AVStream*& outStream)
    {
        int ret = -1;

        if (!AssertFormat(__FUNCTION__))
            return ErrorNoFormatCtx;

        if (AVStream* strm = avformat_new_stream(m_FmtCtx, nullptr))
        {
            if (strm->codecpar)
            {
                ret = avcodec_parameters_copy(strm->codecpar, srcStream->codecpar);
                if (ret >= 0)
                {
                    strm->time_base = srcStream->time_base;
                    strm->sample_aspect_ratio = srcStream->sample_aspect_ratio;
                    outStream = strm;
                    LOG_INFO("%s, %s", __FUNCTION__, MediaTypeHolder(strm).MediaTypeStr());
                }
                else
                {
                    LOG_ERROR("%s, avcodec_parameters_copy, %s", __FUNCTION__, FmtErr(ret));
                }
            }
            else
            {
                LOG_ERROR("%s, null codecpar", __FUNCTION__);
            }
        }
        else
        {
            LOG_ERROR("%s, avformat_new_stream", __FUNCTION__);
            ret = AVERROR(ENOMEM);
        }
        return ret;
    }

    int MuxBase::AddOutputStream(const AVCodecContext* codecCtx, AVStream*& outStream)
    {
        int ret = -1;

        if (!AssertFormat(__FUNCTION__))
            return ErrorNoFormatCtx;

        if (AVStream* strm = avformat_new_stream(m_FmtCtx, nullptr))
        {
            if (strm->codecpar)
            {
                ret = avcodec_parameters_from_context(strm->codecpar, codecCtx);
                if (ret >= 0)
                {
                    outStream = strm;
                    LOG_INFO("%s, %s", __FUNCTION__, MediaTypeHolder(strm).MediaTypeStr());
                }
                else
                {
                    LOG_ERROR("%s, avcodec_parameters_from_context, %s", __FUNCTION__, FmtErr(ret));
                }
            }
            else
            {
                LOG_ERROR("%s, null codecpar", __FUNCTION__);
            }
        }
        else
        {
            LOG_ERROR("%s, avformat_new_stream() failed", __FUNCTION__);
            ret = AVERROR(ENOMEM);
        }
        return ret;
    }

    int MuxBase::OpenEncoder(AVCodecContext* codecCtx, const AVCodec* encoder, const char* options) const 
    {
        if (m_GlobalHeader)
        {
            codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }

        int ret = CodecTools::OpenCodec(codecCtx, encoder, options, __FUNCTION__);
        if (ret >= 0)
        {
            LOG_INFO("%s, %s, name=%s", __FUNCTION__, MediaTypeHolder(encoder).MediaTypeStr(), encoder->name);
        }

        return ret;
    }

    int MuxBase::WritePacket(AVPacket* pkt)
    {
        int ret = -1;
        if (!AssertFormat(__FUNCTION__))
            return ErrorNoFormatCtx;

        {
            AutoLock al(m_Mutex);
            ret = av_interleaved_write_frame(m_FmtCtx, pkt);
            if (ret < 0)
            {
                LOG_ERROR("%s, av_interleaved_write_frame, %s", __FUNCTION__, FmtErr(ret));
            }
        }
        return ret;
    }

} // namespace Fcore

/*
int av_interleaved_write_frame (AVFormatContext *s, AVPacket *pkt);

Write a packet to an output media file ensuring correct interleaving.
This function will buffer the packets internally 
as needed to make sure the packets in the output file are properly 
interleaved in the order of increasing dts.
Callers doing their own interleaving should call av_write_frame() instead of this function.
Using this function instead of av_write_frame() can give muxers advance knowledge of future packets, 
improving e.g.the behaviour of the mp4 muxer for VFR content in fragmenting mode.
Parameters
s   - media file handle
pkt - the packet containing the data to be written.
If the packet is reference - counted, this function will take ownership of this reference 
and unreference it later when it sees fit.
The caller must not access the data through this reference after this function returns.
If the packet is not reference - counted, libavformat will make a copy.
This parameter can be NULL (at any time, not just at the end), to flush the interleaving queues.
Packet's stream_index field must be set to the index of the corresponding stream in s->streams. 
The timestamps (pts, dts) must be set to correct values in the stream's timebase 
(unless the output format is flagged with the AVFMT_NOTIMESTAMPS flag, 
then they can be set to AV_NOPTS_VALUE). 
The dts for subsequent packets in one stream must be strictly increasing 
(unless the output format is flagged with the AVFMT_TS_NONSTRICT, 
then they merely have to be nondecreasing). 
duration should also be set if known.
Returns
0 on success, a negative AVERROR on error.
Libavformat will always take care of freeing the packet, even if this function fails.
*/
