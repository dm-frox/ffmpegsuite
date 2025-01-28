#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)
#define WINDOWS_

#if defined(_MSC_VER)
#define VISUAL_STUDIO_
#endif // _MSC_VER

#endif // WIN32 _WIN32 _WIN64
