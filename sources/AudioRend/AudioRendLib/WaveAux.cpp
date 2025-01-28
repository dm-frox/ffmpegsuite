#include "pch.h"

#include <mmsystem.h>

#include <string>

#define ERR_SYMB(symb)\
    case symb: s = #symb; break;

const char* ErrCodeToStr(MMRESULT res)
{
    const char* s = "?";
    switch (res)
    {
        ERR_SYMB(MMSYSERR_NOERROR)
        ERR_SYMB(MMSYSERR_ERROR)
        ERR_SYMB(MMSYSERR_ALLOCATED)
        ERR_SYMB(MMSYSERR_BADDEVICEID)
        ERR_SYMB(MMSYSERR_NODRIVER)
        ERR_SYMB(MMSYSERR_NOMEM)
        ERR_SYMB(WAVERR_BADFORMAT)
        ERR_SYMB(WAVERR_SYNC)
        ERR_SYMB(MMSYSERR_NOTENABLED)
        ERR_SYMB(MMSYSERR_INVALHANDLE)
        ERR_SYMB(MMSYSERR_NOTSUPPORTED)
        ERR_SYMB(MMSYSERR_INVALFLAG)
        ERR_SYMB(MMSYSERR_INVALPARAM)
        ERR_SYMB(MMSYSERR_HANDLEBUSY)
        ERR_SYMB(MMSYSERR_INVALIDALIAS)
        ERR_SYMB(MMSYSERR_BADDB)
        ERR_SYMB(MMSYSERR_KEYNOTFOUND)
        ERR_SYMB(MMSYSERR_READERROR)
        ERR_SYMB(MMSYSERR_WRITEERROR)
        ERR_SYMB(MMSYSERR_DELETEERROR)
        ERR_SYMB(MMSYSERR_VALNOTFOUND)
        ERR_SYMB(MMSYSERR_NODRIVERCB)
        ERR_SYMB(MMSYSERR_MOREDATA)
    }
    return s;
}

#undef ERR_SYMB

std::string ErrCodeToStr(MMRESULT res, const char* msg)
{

    std::string str;
    str.reserve(64);
    str += "ERROR: ";
    str += msg;
    str += ", ";
    str += ErrCodeToStr(res);
    return str;
}

// ---------------------------------------------------------------------

//bool MMR2Bool_(MMRESULT res, const char* /*msg*/)
//{
//    bool ret = (res == MMSYSERR_NOERROR);
//    if (!ret)
//    {
//        wchar_t text[64]{};
//        waveOutGetErrorTextW(res, text, sizeof(text) / sizeof(wchar_t));
//    }
//    return ret;
//}

//#define MMSYSERR_NOERROR      0                    /* no error */
//#define MMSYSERR_ERROR        (MMSYSERR_BASE + 1)  /* unspecified error */
//#define MMSYSERR_BADDEVICEID  (MMSYSERR_BASE + 2)  /* device ID out of range */
//#define MMSYSERR_NOTENABLED   (MMSYSERR_BASE + 3)  /* driver failed enable */
//#define MMSYSERR_ALLOCATED    (MMSYSERR_BASE + 4)  /* device already allocated */
//#define MMSYSERR_INVALHANDLE  (MMSYSERR_BASE + 5)  /* device handle is invalid */
//#define MMSYSERR_NODRIVER     (MMSYSERR_BASE + 6)  /* no device driver present */
//#define MMSYSERR_NOMEM        (MMSYSERR_BASE + 7)  /* memory allocation error */
//#define MMSYSERR_NOTSUPPORTED (MMSYSERR_BASE + 8)  /* function isn't supported */
//#define MMSYSERR_BADERRNUM    (MMSYSERR_BASE + 9)  /* error value out of range */
//#define MMSYSERR_INVALFLAG    (MMSYSERR_BASE + 10) /* invalid flag passed */
//#define MMSYSERR_INVALPARAM   (MMSYSERR_BASE + 11) /* invalid parameter passed */
//#define MMSYSERR_HANDLEBUSY   (MMSYSERR_BASE + 12) /* handle being used */

//#define MMSYSERR_INVALIDALIAS (MMSYSERR_BASE + 13) /* specified alias not found */
//#define MMSYSERR_BADDB        (MMSYSERR_BASE + 14) /* bad registry database */
//#define MMSYSERR_KEYNOTFOUND  (MMSYSERR_BASE + 15) /* registry key not found */
//#define MMSYSERR_READERROR    (MMSYSERR_BASE + 16) /* registry read error */
//#define MMSYSERR_WRITEERROR   (MMSYSERR_BASE + 17) /* registry write error */
//#define MMSYSERR_DELETEERROR  (MMSYSERR_BASE + 18) /* registry delete error */
//#define MMSYSERR_VALNOTFOUND  (MMSYSERR_BASE + 19) /* registry value not found */
//#define MMSYSERR_NODRIVERCB   (MMSYSERR_BASE + 20) /* driver does not call DriverCallback */
//#define MMSYSERR_MOREDATA     (MMSYSERR_BASE + 21) /* more data to be returned */
//#define MMSYSERR_LASTERROR    (MMSYSERR_BASE + 21) /* last error in range */


//MMSYSERR_ALLOCATED Specified resource is already allocated. 
//MMSYSERR_BADDEVICEID Specified device identifier is out of range. 
//MMSYSERR_NODRIVER No device driver is present. 
//MMSYSERR_NOMEM Unable to allocate or lock memory. 
//WAVERR_BADFORMAT Attempted to open with an unsupported waveform-audio format. 
//WAVERR_SYNC The device is synchronous but waveOutOpen was called without using the WAVE_ALLOWSYNC flag.

