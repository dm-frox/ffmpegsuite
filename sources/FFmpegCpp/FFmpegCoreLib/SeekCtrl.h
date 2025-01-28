#pragma once

// ---------------------------------------------------------------------
// File: SeekCtrl.h
// Classes: SeekCtrl
// Purpose: a helper to seek position
// Module: FFmpegCoreLib - C++ wrappers for ffmpeg API
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2023
// ---------------------------------------------------------------------

#include <atomic>

#include "MediaTypeHolder.h"

namespace Ftool
{
	class ProceedPump;
}

namespace Fcore
{
	class SeekCtrl : MediaTypeHolder
	{
	// types
	private:		
		using PumpType = Ftool::ProceedPump;

	// data
	private:
		static const double Eps;

		std::atomic<bool>   m_InProgress{ false };
		std::atomic<double> m_PosToSeek{ -1.0 };
		bool                m_Paused{ false };
		PumpType*           m_RendPump{ nullptr };

	// ctor, dtor, copying
	public:
		SeekCtrl(AVMediaType mediaType);
		SeekCtrl(AVMediaType mediaType, PumpType& pump);

	// operations
	public:
		void SetPump(PumpType* pump);
		bool InProgress() const { return m_InProgress; }
		void SetPosition(double pos, bool pause);
		bool CheckSeekingEnd(double pos);
		void Reset();
		void Cancel();

	}; // class SeekCtrl

} // namespace Fcore

