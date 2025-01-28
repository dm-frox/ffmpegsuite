#pragma once

// ---------------------------------------------------------------------
// File: ffmpeg.h
// Purpose: includs for ffmpeg API
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

extern "C"
{

#include <libavutil/avutil.h>
#include <libavutil/opt.h>
#include <libavutil/log.h>
#include <libavutil/buffer.h>
#include <libavutil/time.h>
#include <libavutil/avstring.h>
#include <libavutil/error.h>
#include <libavutil/channel_layout.h>
#include <libavutil/pixdesc.h>
#include <libavutil/imgutils.h>
#include <libavutil/hwcontext.h>
#include <libavutil/version.h>

#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavformat/version.h>

#include <libavcodec/avcodec.h>
#include <libavcodec/bsf.h>
#include <libavcodec/version.h>

#include <libavdevice/avdevice.h>
#include <libavdevice/version.h>

#include <libswresample/swresample.h>
#include <libswresample/version.h>

#include <libswscale/swscale.h>
#include <libswscale/version.h>

#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavfilter/version.h>

#include <libpostproc/version.h>

} // extern "C" 


//#ifdef WIN32
//#define inline _inline
//#endif

