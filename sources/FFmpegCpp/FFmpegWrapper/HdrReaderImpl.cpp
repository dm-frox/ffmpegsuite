#include "pch.h"

#include "HdrReaderImpl.h"


namespace Fwrap
{

    HdrReaderImpl::HdrReaderImpl()
        : m_Demuxer(false)
    {}

    HdrReaderImpl::~HdrReaderImpl() = default;

    int HdrReaderImpl::Open(const char* url, const char* fmt, const char* ops)
    {
        return m_Demuxer.Open(url, fmt, ops);
    }

    int HdrReaderImpl::StreamCount() const
    {
        return m_Demuxer.StreamCount();
    }

    CStrEx HdrReaderImpl::GetInfo() const
    {
        int len = 0;
        const char* str = m_Demuxer.GetInfo(len);
        return { str, len };
    }

    CStrEx HdrReaderImpl::GetMetadata() const
    {
        int len = 0;
        const char* str = m_Demuxer.GetMetadata(len);
        return { str, len };
    }

    CStrEx HdrReaderImpl::GetChapters() const
    {
        int len = 0;
        const char* str = m_Demuxer.GetChapters(len);
        return { str, len };
    }

    CStrEx HdrReaderImpl::GetStreamMetadata(int strmIdx) const
    {
        int len = 0;
        const char* str = m_Demuxer.GetStreamMetadata(strmIdx, len);
        return { str, len };
    }

    CStrEx HdrReaderImpl::GetStreamInfo(int strmIdx) const
    {
        int len = 0;
        const char* str = m_Demuxer.GetStreamInfo(strmIdx, len);
        return { str, len };
    }

// --------------------------------------------------------------------
// factory 

    IHdrReader* IHdrReader::CreateInstance()
    {
        return static_cast<IHdrReader*>(new HdrReaderImpl());
    }

} // namespace Fwrap

