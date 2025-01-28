#pragma once

// ---------------------------------------------------------------------
// File: CoretImpl.h
// Classes: CoretImpl
// Purpose: ICore implementation
// Module: FFmpegWrapper - shared module which exports C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <vector>
#include <string>

#include "include/CoreIface.h"

namespace Fwrap
{
    class CoreImpl : ICore
    {
        std::vector<std::wstring> m_DShowDevList;
        mutable std::string       m_HWAccels;

        static CoreImpl TheCore;

        CoreImpl();

// ICore impl
        void Initialize() override final;
        void EnableFFmpegLogging(int logLevel) override final;
        void Free() override final;
        const char* GetVersion(LibTag libTag) override final;
        void SetBaseFolder(const wchar_t* folder) override final;
        bool OpenLog(int lev, const wchar_t* path, int options, int maxFileSize, int backupCount) override final;
        bool OpenLog(int lev, const char* path, int options, int maxFileSize, int backupCount) override final;
        void LogString(int lev, const char* str) override final;
        void FlushLog() override final;
        void CloseLog() override final;
        int UpdateDShowDevList(int devType) override final;
        const wchar_t* GetDShowDev(int ind) const override final;
        const char* GetHWAccels() const override final;

        IProbe* CreateProbe() const override final;
        IMediaPlayerStem* CreateMediaPlayerStem(bool useFilterGraphAlways, bool convFrameSequentially) const override final;
        IOscBuilder* CreateOscBuilder() const override final;
        ITranscoder* CreateTranscoder(bool useFilterGraph) const override final;
        IHdrReader* CreateHdrReader() const override final;
        IAudioArrMuxer* CreateDataMuxer() const override final;
        ICodecList* CreateCodecList() const override final;
        IFormatList* CreateFormatList() const override final;
        IProtocolList* CreateProtocolList() const override final;
        IFilterList* CreateFilterList() const override final;
        IChannList* CreateChannList() const override final;
        IPixFmtList* CreatePixFmtList() const override final;
        ISampFmtList* CreateSampFmtList() const override final;
        IBsfList* CreateBsfList() const override final;

// access to the singleton
    public:
        static ICore* GetIface();

    }; // class CoreImpl

} // namespace Fwrap


