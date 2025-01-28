#include "pch.h"

#include <mmsystem.h>

#include <string>

std:: wstring WaveDevList;

constexpr auto wocSize = sizeof WAVEOUTCAPS;


const wchar_t* GetWaveDevList()
{
    WaveDevList.clear();
    WAVEOUTCAPS woc;
    for (int i = 0, n = waveOutGetNumDevs(); i < n; ++i)
    {
        memset(&woc, 0, wocSize);
        MMRESULT res = waveOutGetDevCapsW(static_cast<UINT_PTR>(i), &woc, wocSize);
        if (res == MMSYSERR_NOERROR)
        {
            WaveDevList += woc.szPname;
            WaveDevList += '\n';
        }
    }
    return WaveDevList.c_str();
}

//typedef struct { 
//    WORD      wMid; 
//    WORD      wPid; 
//    MMVERSION vDriverVersion; 
//    TCHAR     szPname[MAXPNAMELEN]; 
//    DWORD     dwFormats; 
//    WORD      wChannels; 
//    WORD      wReserved1; 
//    DWORD     dwSupport; 
//} WAVEOUTCAPS; 
