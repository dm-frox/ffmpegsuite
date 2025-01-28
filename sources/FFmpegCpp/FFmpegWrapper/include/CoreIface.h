#pragma once

// ---------------------------------------------------------------------
// File: CoreIface.h
// Classes: ICore (abstract)
// Purpose: interface for the object which allows to setup core functionality of the module
// and create the other objects
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "BaseIface.h"

namespace Fwrap
{
    class IProbe;
    class IMediaPlayerStem;
    class IOscBuilder;
    class ITranscoder;
    class IHdrReader;
    class IAudioArrMuxer;
    class ICodecList;
    class IFormatList;
    class IProtocolList;
    class IFilterList;
    class IChannList;
    class IPixFmtList;
    class ISampFmtList;
    class IBsfList;

    enum class LibTag
    {
        Self, FFmpeg, AvCodec, AvDevice, AvFilter, AvFormat, AvUtil, SwResample, SwScale, PostProc, LibCount
    };

    class ICore : IBase0
    {
    protected:
        ICore() = default;
        ~ICore() = default;

    public:
        virtual void Initialize() = 0;
        virtual void EnableFFmpegLogging(int logLevel) = 0;
        virtual void Free() = 0;
        virtual const char* GetVersion(LibTag libTag) = 0;
        virtual void SetBaseFolder(const wchar_t* folder) = 0;
        virtual bool OpenLog(int lev, const wchar_t* path, int options, int maxFileSize, int backupCount) = 0;
        virtual bool OpenLog(int lev, const char* path, int options, int maxFileSize, int backupCount) = 0;
        virtual void LogString(int lev, const char* str) = 0;
        virtual void FlushLog() = 0;
        virtual void CloseLog() = 0;
        virtual int UpdateDShowDevList(int devType) = 0;
        virtual const wchar_t* GetDShowDev(int ind) const = 0;
        virtual const char* GetHWAccels() const = 0;
// factories
        virtual IProbe* CreateProbe() const = 0;
        virtual IMediaPlayerStem* CreateMediaPlayerStem(bool useFilterGraphAlways, bool convFrameSequentially) const = 0;
        virtual IOscBuilder* CreateOscBuilder()  const = 0;
        virtual ITranscoder* CreateTranscoder(bool useFilterGraph) const = 0;
        virtual IHdrReader* CreateHdrReader() const = 0;
        virtual IAudioArrMuxer* CreateDataMuxer() const = 0;
        virtual ICodecList* CreateCodecList() const = 0;
        virtual IFormatList* CreateFormatList() const = 0;
        virtual IProtocolList* CreateProtocolList() const = 0;
        virtual IFilterList* CreateFilterList() const = 0;
        virtual IChannList* CreateChannList() const = 0;
        virtual IPixFmtList* CreatePixFmtList() const = 0;
        virtual ISampFmtList* CreateSampFmtList() const = 0;
        virtual IBsfList* CreateBsfList() const = 0;

    }; // class ICore

} // namespace Fwrap
