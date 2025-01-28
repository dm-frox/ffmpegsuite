#pragma once

// ---------------------------------------------------------------------
// File: MediaTypeEnum.h
// Classes: enum class MediaType
// Purpose: FFmpegWrapper counterpart for ffmpeg enum AVMediaType
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

namespace Fwrap
{
    enum class MediaType // copy of enum AVMediaType, for .NET only
    { 
        Unknown = -1, 
        Video   = 0, 
        Audio, 
        Data, 
        Subtitle, 
        Attachment, 
        Nb,

    }; // enum class MediaType

} // namespace Fwrap

//enum AVMediaType {
//    AVMEDIA_TYPE_UNKNOWN = -1,  ///< Usually treated as AVMEDIA_TYPE_DATA
//    AVMEDIA_TYPE_VIDEO,
//    AVMEDIA_TYPE_AUDIO,
//    AVMEDIA_TYPE_DATA,          ///< Opaque data information usually continuous
//    AVMEDIA_TYPE_SUBTITLE,
//    AVMEDIA_TYPE_ATTACHMENT,    ///< Opaque data information usually sparse
//    AVMEDIA_TYPE_NB
//};
