#pragma once

namespace Fcore
{
    enum CommonConsts
    {
        ErrorBadArgs            = -1000,
        ErrorBadState           = -1001,
        ErrorWrongMediaType     = -1002,
        ErrorAlreadyInitialized = -1003,
        ErrorBadStreamIndex     = -1004,
        ErrorNoStreams          = -1005,
        ErrorNullCodecpar       = -1005,
        ErrorNullStream         = -1007,
        ErrorNoDecoder          = -1008,
        ErrorNoFormatCtx        = -1009,
        ErrorWrongMediaTypeConn = -1010,
        ErrorNoRequiredStreams  = -1011,
        ErrorNotImplemented     = -1012,
        ErrorPacketQueue        = -1013,

        ErrorBadFilterIndex     = -1014,
        ErrorNoFilters          = -1015,
        ErrorNoFiltertCtx       = -1016,
        ErrorNoFiltertGraph     = -1017,
        ErrorFiltertGraphBuild  = -1018,

        ErrorRendFrameSink      = -1019,

        DecoderNotFound         = -9990111,
        ErrorAudioRend          = -1020,

    }; // enum CommonConsts

} // namespace Fcore
