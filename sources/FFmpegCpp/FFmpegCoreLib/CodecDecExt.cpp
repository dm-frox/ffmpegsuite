#include "pch.h"

#include "CodecDecIface.h"

namespace Fcore
{
    // static
    IDecoder* IDecoder::FindExternalDecoder(const AVStream*) // stub
    {
        return nullptr;
    }

} // namespace Fcore

