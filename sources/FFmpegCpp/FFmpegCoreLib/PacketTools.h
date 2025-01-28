#pragma once

// ---------------------------------------------------------------------
// File: PacketTools.h
// Classes: PacketTools, Packet
// Purpose: helpers to create, initialize and release AVPacket instances
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include "ffmpeg.h"
#include "NonCopyable.h"

namespace Fcore
{
    // static
    class PacketTools
    {
// ctor, dtor, copying
    public:
        PacketTools() = delete;
// operations
    public:
        static AVPacket* Alloc() { return av_packet_alloc(); }
        static void      Free(AVPacket* pkt) { av_packet_free(&pkt); }
        static void      FreeBuffer(AVPacket* pkt) { if (pkt)av_packet_unref(pkt); }

    }; // class PacketTools

    class Packet : NonCopyable
    {
// data
    private:
        AVPacket* m_Packet{ nullptr };

// ctor, dtor, copying
    public:
        Packet() = default;
        ~Packet() { av_packet_free(&m_Packet); }

// operations
    public:
        explicit operator bool() const { return m_Packet != nullptr; }

        operator const AVPacket* () const { return m_Packet; }

        operator AVPacket* () { return m_Packet; }

        int StreamIndex() const { return m_Packet ? m_Packet->stream_index : -1; }

        bool Alloc()
        {
            if (!m_Packet)
            {
                m_Packet = av_packet_alloc();
            }
            return m_Packet != nullptr;
        }

        AVPacket* MoveToNewPacket()
        {
            if (m_Packet)
            {
                if (AVPacket* ret = av_packet_alloc())
                {
                    av_packet_move_ref(ret, m_Packet);
                    return ret;
                }
            }
            return nullptr;
        }

        void FreeBuffer()
        {
            if (m_Packet)
            {
                av_packet_unref(m_Packet);
            }
        }

    }; // class Packet

} // namespace Fcore
