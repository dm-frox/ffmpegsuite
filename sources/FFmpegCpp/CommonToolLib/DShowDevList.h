#pragma once

// ---------------------------------------------------------------------
// File: DShowDevList.h
// Function: GetDShowDevList
// Purpose: fetch DirectShow device list (Windows only)
// Module: FFmpegToolsLib - auxiliary classes, functions
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

#include <vector>
#include <string>

namespace Ftool
{
    using DevList = std::vector<std::wstring>;

    DevList GetDShowDevList(int devType);

} // namespace Ftool
