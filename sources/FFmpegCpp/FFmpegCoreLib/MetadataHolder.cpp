#include "pch.h"

#include "MetadataHolder.h"

#include "ffmpeg.h"
#include "DictTools.h"
#include "Logger.h"
#include "MultiString.h"

namespace Fcore
{

    MetadataHolder::MetadataHolder() = default;

    MetadataHolder::~MetadataHolder() = default;

    int MetadataHolder::ExtractMetadata(const AVDictionary* pDict)
    {
        int ret = 0;
        m_Metadata.clear();
        if (pDict)
        {
            try
            {
                m_Metadata = MultiString::PackDictionary(pDict, ret);
            }
            catch (...)
            {
                m_Metadata.clear();
                LOG_ERROR("%s, exception while extracting metadata", __FUNCTION__);
            }
        }
        return ret;
    }

    const char* MetadataHolder::GetMetadata(int& len) const
    { 
        len = (int)m_Metadata.length();
        return m_Metadata.c_str(); 
    }

} // namespace Fcore
