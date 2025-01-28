#include "pch.h"

#include "CoreImpl.h"

#include "DShowDevList.h"

#include "ffmpeg.h"
#include "Logger.h"
#include "Version.h"
#include "OptionTools.h"
#include "CodecDecHW.h"

#include "include/ProbeIface.h"
#include "include/MediaPlayerStemIface.h"
#include "include/OscBuilderIface.h"
#include "include/TranscoderIface.h"
#include "include/HdrReaderIface.h"
#include "include/AudioArrMuxerIface.h"
#include "include/CodecListIface.h"
#include "include/FormatListIface.h"
#include "include/ProtocolListIface.h"
#include "include/FilterListIface.h"
#include "include/ChannListIface.h"
#include "include/PixFmtListIface.h"
#include "include/SampFmtListIface.h"
#include "include/BsfListIface.h"


namespace Fwrap
{
    //static 
    CoreImpl CoreImpl::TheCore;

    //static 
    ICore* CoreImpl::GetIface() { return static_cast<Fwrap::ICore*>(&TheCore); }

    CoreImpl::CoreImpl() = default;

    void CoreImpl::EnableFFmpegLogging(int logLevel)
    {
        Fcore::Logger::EnableFFmpegLogging(logLevel);
    }

    void CoreImpl::Initialize()
    {
        Fcore::InitializeFFmpeg();
    }

    void CoreImpl::Free()
    {}

    const char* CoreImpl::GetVersion(LibTag libTag)
    {
        const char* ret = nullptr;
        switch (libTag)
        {
        case LibTag::Self:
            ret = Fcore::Version::Wrapper();
            break;
        case LibTag::FFmpeg:
            ret = Fcore::Version::FFmpeg();
            break;

        case LibTag::AvCodec:
            ret = Fcore::Version::AvCodec();
            break;
        case LibTag::AvDevice:
            ret = Fcore::Version::AvDevice();
            break;
        case LibTag::AvFilter:
            ret = Fcore::Version::AvFilter();
            break;
        case LibTag::AvFormat:
            ret = Fcore::Version::AvFormat();
            break;
        case LibTag::AvUtil:
            ret = Fcore::Version::AvUtil();
            break;
        case LibTag::SwResample:
            ret = Fcore::Version::SwResample();
            break;
        case LibTag::SwScale:
            ret = Fcore::Version::SwScale();
            break;
        case LibTag::PostProc:
            ret = Fcore::Version::PostProc();
            break;
        default:
            ret = "<bad tag>";
        }
        return ret;
    }

    void CoreImpl::SetBaseFolder(const wchar_t*)
    {}

    template<typename T>
    bool OpenLog_(int lev, const T* path, int options, int maxFileSize, int maxBackupCount)
    {
        bool ret = false;

        if (path && *path)
        {
            if (Fcore::Logger::Open(lev, path, options))
            {
                Fcore::Logger::SetMaxFileSize(maxFileSize);
                Fcore::Logger::SetMaxBackupCount(maxBackupCount);
                Fcore::Logger::LogFFmpegVersionInfo();
                ret = true;
            }
        }
        else
        {
            Fcore::Logger::Close();
            ret = true;
        }

        return ret;
    }

    bool CoreImpl::OpenLog(int lev, const wchar_t* path, int options, int maxFileSize, int maxBackupCount)
    {
        return OpenLog_(lev, path, options, maxFileSize, maxBackupCount);
    }

    bool CoreImpl::OpenLog(int lev, const char* path, int options, int maxFileSize, int maxBackupCount)
    {
        return OpenLog_(lev, path, options, maxFileSize, maxBackupCount);
    }

    void CoreImpl::LogString(int lev, const char* str)
    {
        Fcore::Logger::LogString(lev, str);
    }

    void CoreImpl::FlushLog()
    {
        Fcore::Logger::Flush();
    }
    
    void CoreImpl::CloseLog()
    {
        Fcore::Logger::Close();
    }

    int CoreImpl::UpdateDShowDevList(int devType)
    {
        m_DShowDevList = Ftool::GetDShowDevList(devType);
        return (int)m_DShowDevList.size();
    }

    const wchar_t* CoreImpl::GetDShowDev(int ind) const
    {
        return (0 <= ind && ind < (int)m_DShowDevList.size()) ? m_DShowDevList[ind].c_str() : nullptr;
    }

    const char* CoreImpl::GetHWAccels() const
    {
        if (m_HWAccels.empty())
        {
            m_HWAccels = Fcore::CodecHW::GetHWAccels();
        }
        return m_HWAccels.c_str();
    }

// factories

    IProbe* CoreImpl::CreateProbe() const
    {
        return IProbe::CreateInstance();
    }

    IMediaPlayerStem* CoreImpl::CreateMediaPlayerStem(bool useFilterGraphAlways, bool convFrameSequentially) const
    {
        return IMediaPlayerStem::CreateInstance(useFilterGraphAlways, convFrameSequentially);
    }

    IOscBuilder* CoreImpl::CreateOscBuilder() const
    {
        return IOscBuilder::CreateInstance();
    }

    ITranscoder*  CoreImpl::CreateTranscoder(bool useFilterGraph) const
    {
        return ITranscoder::CreateInstance(useFilterGraph);
    }

    IHdrReader* CoreImpl::CreateHdrReader() const
    {
        return IHdrReader::CreateInstance();
    }

    IAudioArrMuxer* CoreImpl::CreateDataMuxer() const
    {
        return IAudioArrMuxer::CreateInstance();
    }

    ICodecList* CoreImpl::CreateCodecList() const
    {
        return ICodecList::CreateInstance();
    }

    IFormatList* CoreImpl::CreateFormatList() const
    {
        return IFormatList::CreateInstance();
    }

    IProtocolList* CoreImpl::CreateProtocolList() const
    {
        return IProtocolList::CreateInstance();
    }

    IFilterList* CoreImpl::CreateFilterList() const
    {
        return IFilterList::CreateInstance();
    }

    IChannList* CoreImpl::CreateChannList() const
    {
        return IChannList::CreateInstance();
    }

    IPixFmtList* CoreImpl::CreatePixFmtList() const
    {
        return IPixFmtList::CreateInstance();
    }

    ISampFmtList* CoreImpl::CreateSampFmtList() const
    {
        return ISampFmtList::CreateInstance();
    }

    IBsfList* CoreImpl::CreateBsfList() const
    {
        return IBsfList::CreateInstance();
    }

} // namespace Fwrap

