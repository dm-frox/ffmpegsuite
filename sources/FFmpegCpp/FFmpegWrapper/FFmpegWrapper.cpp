// FFmpegWrapper.cpp : Defines the exported functions for the DLL application.
//

#include "pch.h"

#include "CoreImpl.h"

#include "osdefs.h"

#if defined(WINDOWS_)

#define DLL_EXPORT_  __declspec(dllexport)

#else

#define DLL_EXPORT_

#endif // WINDOWS_

extern "C"
{
    DLL_EXPORT_
    void* GetFFmpegWrapper()
    {
        return Fwrap::CoreImpl::GetIface();
    }

} // extern "C"

