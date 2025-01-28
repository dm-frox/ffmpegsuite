#include "pch.h"

#include "FrameConvAudio.h"

#include "Logger.h"
#include "Misc.h"
#include "FrameTools.h"
#include "CommonConsts.h"


namespace Fcore
{
    static AVChannelLayout* GetPtr_(const ChannLayout& channLayout)
    {
        return const_cast<ChannLayout&>(channLayout).ToPtr();
    }

    FrameConvAudio::FrameConvAudio() = default;

    FrameConvAudio::~FrameConvAudio()
    {
        swr_free(&m_SwrCtx);
    }

    int FrameConvAudio::Init(
        int inSampleRate, AVSampleFormat inSampleFmt, const ChannLayout& inChannLayout,
        int outSampleRate, AVSampleFormat outSampleFmt, const ChannLayout& outChannLayout)
    {
        int ret = -1;

        if (!AssertFirst(m_SwrCtx, __FUNCTION__))
        {
            return ErrorAlreadyInitialized;
        }

        if (!(AssertAudioParams(inChannLayout.Chann(), inSampleRate, __FUNCTION__) &&
            AssertAudioFormat(inSampleFmt, __FUNCTION__)))
        {
            return ErrorBadArgs;
        }

        if (outSampleRate > 0 && inSampleRate != outSampleRate)
        {
            LOG_ERROR("%s, resampling is not supported", __FUNCTION__);
            return ErrorBadArgs;
        }

        m_OutSampleRate  = inSampleRate;
        m_OutSampleFmt   = (outSampleFmt != AV_SAMPLE_FMT_NONE) ? outSampleFmt : inSampleFmt;
        m_OutChannLayout.Set(outChannLayout);

        if (m_OutSampleRate != inSampleRate || m_OutSampleFmt != inSampleFmt || m_OutChannLayout != inChannLayout)
        {
            SwrContext* swrCtx = nullptr;
            ret = swr_alloc_set_opts2(&swrCtx,
                m_OutChannLayout.ToPtr(), m_OutSampleFmt, m_OutSampleRate,
                GetPtr_(inChannLayout), inSampleFmt, inSampleRate,
                0, nullptr);
            if (ret >= 0)
            {
                ret = swr_init(swrCtx);
                if (ret >= 0)
                {
                    m_SwrCtx = swrCtx;

                    LOG_INFO(
                        "%s, in: chann=%d, samp.rate=%d, samp.fmt=%s, ch.mask=%lld;"
                        " out: chann=%d, samp.rate=%d, samp.fmt=%s, ch.mask=%lld",
                        __FUNCTION__,
                        inChannLayout.Chann(), inSampleRate, SampFmtToStr(inSampleFmt), inChannLayout.Mask(),
                        m_OutChannLayout.Chann(), m_OutSampleRate, SampFmtToStr(m_OutSampleFmt), m_OutChannLayout.Mask());
                }
                else
                {
                    LOG_ERROR("%s, swr_init, %s", __FUNCTION__, FmtErr(ret));
                }
            }
        }
        else
        {
            LOG_INFO("%s, OK, in: chann=%d, samp.rate=%d, samp.fmt=%s, ch.mask=%lld; out: identical",
                __FUNCTION__,
                inChannLayout.Chann(), inSampleRate, SampFmtToStr(inSampleFmt), inChannLayout.Mask());

            ret = 0;
        }

        m_Counter = ( ret >= 0) ? 0 : -1;

        return ret;
    }

    int  FrameConvAudio::Convert(AVFrame* inFrame, AVFrame* outFrame)
    {
        int ret = -1;
        if (m_Counter >= 0)
        {
            if (m_SwrCtx)
            {
                int frameSize = inFrame->nb_samples;
                ret = FrameTools::AllocFrameBuffAudio(
                    m_OutSampleRate, m_OutSampleFmt, m_OutChannLayout, frameSize,
                    outFrame);
                if (ret >= 0)
                {
                    ret = swr_convert(m_SwrCtx, outFrame->data, frameSize, (const uint8_t**)inFrame->data, frameSize);
                    if (ret >= 0)
                    {
                        outFrame->pts = inFrame->pts;
                    }
                    else
                    {
                        FrameTools::FreeFrameBuffer(outFrame);
                    }
                }
                FrameTools::FreeFrameBuffer(inFrame);
            }
            else
            {
                ret = FrameTools::MoveFrameRef(outFrame, inFrame);
            }

            if (ret >= 0)
            {
                ++m_Counter;
            }
        }
        else
        {
            LOG_ERROR("%s, is not ready", __FUNCTION__);
        }
        return ret;
    }

} // namespace Fcore



//struct SwrContext* swr_alloc_set_opts(
//  struct SwrContext* s,
//	int64_t  	        out_ch_layout,
//  enum AVSampleFormat out_sample_fmt,
//	int  	            out_sample_rate,
//
//	int64_t  	        in_ch_layout,
//  enum AVSampleFormat in_sample_fmt,
//	int  	            in_sample_rate,
//	int  	log_offset,
//	void*   log_ctx
//	)

////
//// ------------------------------------------------------------------------------------------
////
//
//struct SwrContext* swr_alloc(void)
//
//    Allocate SwrContext.
//
//    If you use this function you will need to set the parameters(manually or with swr_alloc_set_opts()) before calling swr_init().
//
//    See Also
//    swr_alloc_set_opts(), swr_init(), swr_free()
//
//    Returns
//    NULL on error, allocated context otherwise
//
//    Definition at line 177 of file swresample.c.
//
//    Referenced by init_dict(), main(), open_audio(), opt_default(), and swr_alloc_set_opts().
////
//// ------------------------------------------------------------------------------------------
////
//    int swr_init(struct SwrContext *  	s)
//
//    Initialize context after user parameters have been set.
//
//    Returns
//    AVERROR error code in case of failure.
//
//    Definition at line 242 of file swresample.c.
//
//    Referenced by audio_decode_frame(), config_output(), config_props(), main(), open_audio(), and swr_set_compensation().
////
//// ------------------------------------------------------------------------------------------
////
//struct SwrContext* swr_alloc_set_opts(
//struct SwrContext *  	s,
//    int64_t  	        out_ch_layout,
//enum AVSampleFormat  	out_sample_fmt,
//    int  	            out_sample_rate,
//
//    int64_t  	        in_ch_layout,
//enum AVSampleFormat  	in_sample_fmt,
//    int  	            in_sample_rate,
//    int  	            log_offset,
//    void *  	        log_ctx
//    )
//
//    Allocate SwrContext if needed and set / reset common parameters.
//
//    This function does not require s to be allocated with swr_alloc().
//    On the other hand, swr_alloc() can use swr_alloc_set_opts() to set the parameters on the allocated context.
//
//    Parameters
//    s	            Swr context, can be NULL
//    out_ch_layout	output channel layout(AV_CH_LAYOUT_*)
//    out_sample_fmt	output sample format(AV_SAMPLE_FMT_*).
//    out_sample_rate	output sample rate(frequency in Hz)
//    in_ch_layout	input channel layout(AV_CH_LAYOUT_*)
//    in_sample_fmt	input sample format(AV_SAMPLE_FMT_*).
//    in_sample_rate	input sample rate(frequency in Hz)
//    log_offset	    logging level offset
//    log_ctx	        parent  logging context, can be NULL
//
//    See Also
//    swr_init(), swr_free()
//
//    Returns
//    NULL on error, allocated context otherwise
//
//    Definition at line 186 of file swresample.c.
//
//    Referenced by audio_decode_frame(), config_output(), config_props(), and main().
////
//// ------------------------------------------------------------------------------------------
////	
//    void swr_free(struct SwrContext **  	s)
//
//    Free the given SwrContext and set the pointer to NULL.
//
//    Definition at line 220 of file swresample.c.
//
//    Referenced by audio_decode_frame(), main(), opt_default(), stream_component_close(), and uninit().
////
//// ------------------------------------------------------------------------------------------
////	
//    int swr_convert(struct SwrContext *  	s,
//    uint8_t **  	    out,
//    int  	            out_count,
//    const uint8_t **  	in,
//    int  	            in_count
//    )
//
//    Convert audio.
//
//    in and in_count can be set to 0 to flush the last few samples out at the end.
//
//    If more input is provided than output space then the input will be buffered.You can avoid this buffering by providing more output space than input.Convertion will run directly without copying whenever possible.
//
//    Parameters
//    s	          allocated Swr context, with parameters set
//    out	          output buffers, only the first one need be set in case of packed audio
//    out_count	  amount of space available for output in samples per channel
//    in	          input buffers, only the first one need to be set in case of packed audio
//    in_count	  number of input samples available in one channel
//
//    Returns
//    number of samples output per channel, negative value on error
//
//
//