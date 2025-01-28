#include "pch.h"

#include "FilterGraph.h"

#include "Logger.h"
#include "CommonConsts.h"
#include "Misc.h"
#include "FrameHub.h"
#include "FrameTools.h"


namespace Fcore
{

    template<class T> // T - VectorPtrT<FilterInput>, VectorPtrT<FilterOutput>
    AVFilterInOut* SetupFilterListT(const T& flist)
    {
        AVFilterInOut* ret = nullptr;
 
        if (AVFilterInOut* head = avfilter_inout_alloc())
        {
            flist[0].InitInOutItem(head);
            AVFilterInOut* curr = head;
            for (int i = 1, count = flist.Count(); i < count && curr; ++i)
            {
                if (AVFilterInOut* next = avfilter_inout_alloc())
                {
                    flist[i].InitInOutItem(next);

                    curr->next = next;
                    curr = next;
                }
                else
                {
                    curr = nullptr;
                }
            }
            if (curr)
            {
                ret = head;
            }
            else
            {
                avfilter_inout_free(&head);
            }
        }
        if (!ret)
        {
            LOG_ERROR("%s, avfilter_inout_alloc", __FUNCTION__);
        }
        return ret;
    }

    FilterGraph::FilterGraph() = default;
   
    FilterGraph::~FilterGraph()
    {
        Close();
    }

    void FilterGraph::Close()
    {
        avfilter_graph_free(&m_Graph);
    }

    int FilterGraph::Create(AVMediaType mediaTypeInput, AVMediaType mediaTypeOutput, const char* name)
    {
        int ret = -1;
        if (auto fg = avfilter_graph_alloc())
        {
            m_Graph = fg;
            m_Inputs.Reserve(4);
            m_Outputs.Reserve(1);
            m_MediaTypeInput = mediaTypeInput;
            m_MediaTypeOutput = mediaTypeOutput;
            m_Name = name;
            ret = 0;
            LOG_INFO("%s, %s", __FUNCTION__, Name());
        }
        else
        {
            ret = AVERROR(ENOMEM);
            LOG_ERROR("%s, avfilter_graph_alloc", __FUNCTION__);
        }
        return ret;
    }

    int FilterGraph::Create(AVMediaType mediaType) 
    { 
        return Create(mediaType, mediaType, MediaTypeToStr(mediaType)); 
    }

    bool FilterGraph::AssertGraph(const char* msg) const
    {
        bool ret = m_Graph ? true : false;
        if (!ret)
        {
            LOG_ERROR("%s: no filter graph", msg);
        }
        return ret;
    }

    bool FilterGraph::AssertInput(const char* msg) const
    {
        bool ret = !m_Inputs.IsEmpty();
        if (!ret)
        {
            LOG_ERROR("%s: no input filters", msg);
        }
        return ret;
    }

    bool FilterGraph::AssertInputIndex(int inputIdx, const char* msg) const
    {
        bool ret = m_Inputs.IndexIsValid(inputIdx);
        if (!ret)
        {
            LOG_ERROR("%s, bad input index=%d", msg, inputIdx);
        }
        return ret;
    }

    bool FilterGraph::AssertOutput(const char* msg) const
    {
        bool ret = m_Outputs.Count() > 0;
        if (!ret)
        {
            LOG_ERROR("%s: no output filters", msg);
        }
        return ret;
    }

    bool FilterGraph::AssertOutputIndex(int outputIdx, const char* msg) const
    {
        bool ret = m_Outputs.IndexIsValid(outputIdx);
        if (!ret)
        {
            LOG_ERROR("%s, bad output index=%d", msg, outputIdx);
        }
        return ret;
    }

    int FilterGraph::AddInputVideo(int width, int height, AVPixelFormat pixFmt,AVRational timeBase, AVRational frameRate)
    {
        if (!AssertGraph(__FUNCTION__))
            return ErrorNoFiltertGraph;
        if (m_MediaTypeInput != AVMEDIA_TYPE_VIDEO)
            return ErrorWrongMediaType;

        auto flt = new FilterInput(m_MediaTypeInput, m_Inputs.Count(), *this);
        int ret = flt->CreateFilterVideo(width, height, pixFmt, timeBase, frameRate);
        if (ret >= 0)
        {
            m_Inputs.AppendItem(flt);
        }
        else
        {
            delete flt;
        }
        return ret;
    }

    int FilterGraph::AddInputAudio(int sampleRate, AVSampleFormat sampFmt, const ChannLayout& channLayout, AVRational timeBase)
    {
        if (!AssertGraph(__FUNCTION__))
            return ErrorNoFiltertGraph;
        if (m_MediaTypeInput != AVMEDIA_TYPE_AUDIO)
            return ErrorWrongMediaType;

        auto flt = new FilterInput(m_MediaTypeInput, m_Inputs.Count(), *this);
        int ret = flt->CreateFilterAudio(channLayout, sampleRate, sampFmt, timeBase);
        if (ret >= 0)
        {
            m_Inputs.AppendItem(flt);
        }
        else
        {
            delete flt;
        }
        return ret;
    }

    int FilterGraph::AddOutputVideo(AVPixelFormat pixFmt)
    {
        if (!AssertGraph(__FUNCTION__))
            return ErrorNoFiltertGraph;
        if (m_MediaTypeOutput != AVMEDIA_TYPE_VIDEO)
            return ErrorWrongMediaType;

        auto flt = new FilterOutput(m_MediaTypeInput, m_Outputs.Count(), *this);
        int ret = flt->CreateFilterVideo(pixFmt);
        if (ret >= 0)
        {
            m_Outputs.AppendItem(flt);
        }
        else
        {
            delete flt;
        }
        return ret;
    }

    int FilterGraph::AddOutputAudio(AVSampleFormat sampFmt)
    {
        if (!AssertGraph(__FUNCTION__))
            return ErrorNoFiltertGraph;
        if (m_MediaTypeOutput != AVMEDIA_TYPE_AUDIO)
            return ErrorWrongMediaType;

        auto flt = new FilterOutput(m_MediaTypeInput, m_Outputs.Count(), *this);
        int ret = flt->CreateFilterAudio(sampFmt);
        if (ret >= 0)
        {
            m_Outputs.AppendItem(flt);
        }
        else
        {
            delete flt;
        }
        return ret;
    }

    int FilterGraph::ConfigureFilterGraph(const char* filterString)
    {
        if (!AssertGraph(__FUNCTION__))
            return ErrorNoFiltertGraph;
        if (!AssertInput(__FUNCTION__))
            return ErrorNoFilters;
        if (!AssertOutput(__FUNCTION__))
            return ErrorNoFilters;

        int ret = -1;
        if (filterString && *filterString)
        {
            if (AVFilterInOut* inputs = SetupFilterListT(m_Inputs))
            {
                if (AVFilterInOut* outputs = SetupFilterListT(m_Outputs))
                {
                    ret = avfilter_graph_parse_ptr(m_Graph, filterString, &outputs, &inputs, nullptr);
                    if (ret < 0)
                    {
                        LOG_ERROR("%s, avfilter_graph_parse_ptr, filt.str=%s, %s", __FUNCTION__, StrToLog(filterString), FmtErr(ret));
                    }
                    avfilter_inout_free(&outputs);
                }
                else
                {
                    ret = AVERROR(ENOMEM);
                }
                avfilter_inout_free(&inputs);
            }
            else
            {
                ret = AVERROR(ENOMEM);
            }
        }
        else
        {
            ret = FilterBase::LinkDirect(m_Inputs[0], m_Outputs[0]);
        }

        if (ret >= 0)
        {
            ret = avfilter_graph_config(m_Graph, nullptr);
            if (ret < 0)
            {
                LOG_ERROR("%s, avfilter_graph_config, filt.str=%s, %s", __FUNCTION__, StrToLog(filterString), FmtErr(ret));
            }
        }

        if (ret >= 0)
        {
            m_FilterString = filterString ? filterString : "";
            LOG_INFO("%s, %s, filt.str=%s", __FUNCTION__, Name(), StrToLog(filterString));
            LogFilters();
        }

        return ret;
    }

    void FilterGraph::Reset()
    {
        if (IsReady())
        {
            if (m_RebuldOnReset || m_InputNullCntr > 0)
            {
                LOG_INFO("%s, %s, begin rebuild...", __FUNCTION__, Name());
                int ret = Rebuild(m_FilterString.c_str());
                if (ret >= 0)
                {
                    LOG_INFO("%s, %s, end rebuid", __FUNCTION__, Name());
                }
                else
                {
                    LOG_ERROR("%s, %s, rebuid error, ret=%d", __FUNCTION__, Name(), ret);
                }
            }
            else
            {
                int c = RemoveOutputFrames();
                LOG_INFO("%s, %s, removed output frames=%d", __FUNCTION__, Name(), c);
            }
            m_InputNullCntr = 0;
        }
    }

    int FilterGraph::ReconfigureFilterGraph(const char* filterString)
    {
        if (!AssertGraph(__FUNCTION__))
            return ErrorNoFiltertGraph;

        LOG_INFO("%s, %s, try rebuild...", __FUNCTION__, Name());

        {
            AutoLock al(m_Mutex);
            return Rebuild(filterString);
        }
    }

    int FilterGraph::Rebuild(const char* filterString)
    {
        avfilter_graph_free(&m_Graph);
        m_Graph = avfilter_graph_alloc();

        int rr = 0;
        for (int i = 0, n = m_Inputs.Count(); i < n && rr >= 0; ++i)
        {
            FilterInput& flt = m_Inputs[i];
            flt.FreeFilter();
            rr = flt.RecreateFilter();
        }
        for (int i = 0, n = m_Outputs.Count(); i < n && rr >= 0; ++i)
        {
            FilterOutput& flt = m_Outputs[i];
            flt.FreeFilter();
            rr = m_MediaTypeOutput == AVMEDIA_TYPE_VIDEO
                ? flt.RecreateFilterVideo() 
                : flt.RecreateFilterAudio();
        }
        if (rr >= 0)
        {
            rr = ConfigureFilterGraph(filterString);
        }

        int ret = rr;

        return ret;
    }

    void FilterGraph::LogFilters() const
    {
        if (!AssertGraph(__FUNCTION__))
            return;

        LOG_INFO("%s, %s, filters:", __FUNCTION__, Name());
        std::string sf;
        sf.reserve(128);
        for (int i = 0, n = (int)m_Graph->nb_filters; i < n; ++i)
        {
            sf += m_Graph->filters[i]->name;
            sf += ' ';
        }
        LOG_INFO(" -- %s", sf.c_str());
    }

    int FilterGraph::SendCommand(const char* target, const char* cmd, const char* arg)
    {
        int ret = -1;
        if (!AssertGraph(__FUNCTION__))
            return ErrorNoFiltertGraph;

        {
            AutoLock al(m_Mutex);
            ret = avfilter_graph_send_command(m_Graph, target, cmd, arg, nullptr, 0, 0);
        }

        if (ret >= 0)
        {
            LOG_INFO("%s, tar=%s, cmd=%s, arg=%s", __FUNCTION__, target, cmd, arg);
        }
        else
        {
            LOG_ERROR("%s, avfilter_graph_send_command, tar=%s, cmd=%s, arg=%s, %s", __FUNCTION__, target, cmd, arg, FmtErr(ret));
        }
        return ret;
    }

    int FilterGraph::FilterFrameOne2One(AVFrame* inFrame, AVFrame* outFrame)
    {
        int ret = -1;

        if (m_Inputs.Count() == 1 && m_Outputs.Count() == 1)
        {
            ret = m_Inputs[0].AddFrame(inFrame);
            if (ret >= 0)
            {
                ret = m_Outputs[0].GetFrame(outFrame);
                if (ret >= 0)
                {
                    if (!m_Outputs[0].NoOutputFrames())
                    {
                        ret = -1;
                    }
                }
            }
        }
        else
        {
            LOG_INFO("%s, inputs/outpus", __FUNCTION__);
        }

        if (ret < 0)
        {
            FrameTools::FreeFrameBuffer(inFrame);
            LOG_ERROR("%s, %s", __FUNCTION__, FmtErr(ret));
        }

        return ret;
    }

    int FilterGraph::FilterFrame(AVFrame* inFrame, int inputIdx, ISinkFrameFlt* sink)
    {
        if (!AssertInputIndex(inputIdx, __FUNCTION__))
            return ErrorBadFilterIndex;

        AutoLock al(m_Mutex);
        if (inFrame == nullptr)
        {
            ++m_InputNullCntr;
            LOG_INFO("%s, null input frame", __FUNCTION__);
        }
        int ret = m_Inputs[inputIdx].SendFrame(inFrame);
        if (ret >= 0)
        {
            for (int i = 0, n = OutputCount(); i < n && ret >= 0; ++i)
            {
                ret = m_Outputs[i].ReceiveFrame(sink);
            }
        }
        else
        {
            FrameTools::FreeFrameBuffer(inFrame);
        }
        return ret;
    }

    int FilterGraph::RemoveOutputFrames()
    {
        int ret = 0;
        AutoLock al(m_Mutex);
        for (int i = 0, n = OutputCount(); i < n; ++i)
        {
            ret += m_Outputs[i].RemoveFrames();
        }
        return ret;
    }

} // namespace Fcore 


//split, asplit
//Split input into several identical outputs.
//
//"asplit" works with audio input, "split" with video.
//
//The filter accepts a single parameter which specifies the number of
//outputs.If unspecified, it defaults to 2.
//
//Examples
//
//�   Create two separate outputs from the same input :
//
//[in] split[out0][out1]
//
//�   To create 3 or more outputs, you need to specify the number of
//outputs, like in :
//
//[in] asplit = 3[out0][out1][out2]
//
//�   Create two separate outputs from the same input, one cropped and
//one padded :
//
//[in] split[splitout1][splitout2];
//[splitout1] crop = 100:100 : 0 : 0[cropout];
//[splitout2] pad = 200:200 : 100 : 100[padout];
//
//�   Create 5 copies of the input audio with ffmpeg :
//
//ffmpeg - i INPUT - filter_complex asplit = 5 OUTPUT
//
