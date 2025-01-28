#pragma once

#include "windefs.h"

#ifdef WINDOWS_

using pchar_t = wchar_t;

#else

using pchar_t = char;

#endif