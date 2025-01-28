#pragma once

// ---------------------------------------------------------------------
// File: ArrSize.h
// Functions: Size() (template)
// Purpose: array size
// Module: FFmpegToolsLib - auxiliary classes, functions
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

namespace Ftool
{
	template<typename T, size_t N>
	int Size(T (&)[N])
	{
		return static_cast<int>(N);
	}

} // namespace Ftool
