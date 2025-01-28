#include "pch.h"

#include "Probe.h"

#include <algorithm>

#include "ProtocolList.h"

#include "ffmpeg.h"
#include "Logger.h"
#include "DictTools.h"
#include "DeviceList.h"
#include "MultiString.h"
#include "Misc.h"
#include "SubtitDump.h"

namespace Fcore
{
    int LogHWDeviceList();
}

namespace Flist
{
    int LogCodecList();
    int LogFormatList();
    void LogFilterList();
    void LogProtocolList();
}

namespace Fapp
{

    class DemoContext
    {
        AVClass* m_DemoClass;

    public:
        DemoContext();
        ~DemoContext();
    };

    DemoContext::DemoContext()
    {
        m_DemoClass = new AVClass();

        m_DemoClass->class_name = "Demo context";
        m_DemoClass->item_name = av_default_item_name;
        m_DemoClass->version = LIBAVUTIL_VERSION_INT;
    }

    DemoContext::~DemoContext()
    {
        delete m_DemoClass;
    }

    void TestDemoContext()
    {
        DemoContext ctx;
        av_log(&ctx, AV_LOG_INFO, "FROX encode init, OK, frame size=%d, packet size=%d\n", 512, 1024);
    }


    Probe::Probe()
    {
        LOG_INFO("%s, {<++++++|++++++>}", __FUNCTION__);
    }

    Probe::~Probe()
    {
        LOG_INFO("%s, {<~~~~~~|~~~~~~>}", __FUNCTION__);
    }

    void Probe::TestMultiString(const char* ms)
    {
        Fcore::MultiString::Dump(ms);
    }

    const char* Probe::Version() const
    {
        const char* ret = av_version_info();
        LOG_INFO("%s, %s", __FUNCTION__, ret);
        return ret;
    }

    int Probe::Do(int /*par1*/, const char* /*par2*/)
    {
        //LOG_INFO("%s, param1=%d, param2=%s", __FUNCTION__, par1, par2);
        //DictHolder dw(
        //    "channels\n2\0"
        //    "sample_rate\n11025\0"
        //    "video_size\n320x240\0\0");
        //dw.Log(__FUNCTION__, "test dictionary");

        //TestDemoContext();

        //LogProtocols(0);
        //LogProtocols(1);

        //AVCodecID id = AV_CODEC_ID_BIN_DATA;
        //const AVCodecDescriptor* cdescr = avcodec_descriptor_get(id);
        //LOG_INFO("%s, cod.id=%s, med.type=%s", __FUNCTION__, CodecIdToStr(id), MediaTypeToStr(cdescr->type));

        Flist::LogFilterList();

        return 9999;
    }

    void Probe::SortInt(int* arr, int n)
    {
        LOG_INFO("%s, len=%d", __FUNCTION__, n);
        std::sort(arr, arr + n);
    }

    void Probe::SortInt2(const int* inArr, int len, int* outArr)
    {
        LOG_INFO("%s, len=%d", __FUNCTION__, len);
        memcpy(outArr, inArr, len * sizeof(int));
        std::sort(outArr, outArr + len);
    }

    int Probe::LogDevList()
    {
       //return Fcore::DeviceList::LogListInput();
       //return Fcore::LogHWDeviceList();
       //return Fcore::TestCodecList();
       return Flist::LogFormatList();
    }

    void LogProtocols(int /*output*/)
    {
        //const char* avio_enum_protocols(void** opaque, int output);
        //const AVClass* avio_protocol_get_class(const char* name);
        //LOG_INFO("%s, %s", __FUNCTION__, output ? "output" : "input");
        //void* opaque = nullptr;
        //int c = 0;
        //while (const char* pname = avio_enum_protocols(&opaque, output))
        //{
        //    LOG_INFO("-- %2d   %s", c++, pname);

        //    const AVClass* ctx = avio_protocol_get_class(pname);
        //    const AVOption* opt = nullptr;
        //    while (opt = av_opt_next(&ctx, opt))
        //    {
        //        LOG_INFO("-- -- %s", opt->name);
        //    }
        //}
        Flist::LogProtocolList();
    }

    void Probe::TestSubtit(const char* path, const char* lang, int count)
    {
        Fcore::SubtitDump sd;
        sd.Build(path, lang, count);
    }

} // namespace Fapp
