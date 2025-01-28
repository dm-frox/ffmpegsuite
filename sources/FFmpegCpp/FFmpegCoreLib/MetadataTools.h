#pragma once

#include "DictTools.h"
#include "MultiString.h"
#include "Logger.h"

namespace Fcore
{
    template<class S> // S - AVStream, AVFormat
    void SetMetadata(S* s, const char* metadata, const char* msg)
    {
        if (metadata && *metadata)
        {
            if (AVDictionary* md = MultiString::CreateDictionary(metadata))
            {
                if (DictHolder::Count(md) > 0)
                {
                    s->metadata = md;
                    DictHolder::Log(md, msg, "metadata");
                }
            }
            else
            {
                LOG_ERROR("%s, failed to create dictionary, data: %s", msg, metadata);
            }
        }
    }

} // namespace Fcore

