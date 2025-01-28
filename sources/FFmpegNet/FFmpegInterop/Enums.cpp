#include "pch.h"

#include "Enums.h"

#include "include/MediaTypeEnum.h"
#include "include/LogLevelEnum.h"


namespace FFmpegInterop
{
    public enum struct MediaType
    {
        Unknown    = (int)Fwrap::MediaType::Unknown,
        Video      = (int)Fwrap::MediaType::Video,
        Audio      = (int)Fwrap::MediaType::Audio,
        Data       = (int)Fwrap::MediaType::Data,
        Subtitle   = (int)Fwrap::MediaType::Subtitle,
        Attachment = (int)Fwrap::MediaType::Attachment,
        Nb         = (int)Fwrap::MediaType::Nb,

    }; // enum struct MediaType

    public enum struct LogLevel
    {
        None    = (int)Fwrap::LogLevel::None,
        Error   = (int)Fwrap::LogLevel::Error,
        Warning = (int)Fwrap::LogLevel::Warning,
        Info    = (int)Fwrap::LogLevel::Info,
        Verbose = (int)Fwrap::LogLevel::Verbose,
        Debug   = (int)Fwrap::LogLevel::Debug,
        Trace   = (int)Fwrap::LogLevel::Trace,

    }; // enum struct LogLevel

    [System::Flags]
    public enum struct LogOptions
    {
        None        = (int)Fwrap::LogOptions::None,
        FlushLine   = (int)Fwrap::LogOptions::FlushLine,
        ThreadLabel = (int)Fwrap::LogOptions::ThreadLabel,
        All         = FlushLine | ThreadLabel,

    }; // enum struct LogOptions

} // namespace FFmpegInterop
