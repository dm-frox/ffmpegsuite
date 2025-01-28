#include "pch.h"

#include "DllLoader.h"

#include <cstring>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>


namespace FFmpegInterop
{

    void* const DllLoaderBase::InvalidValue = nullptr;

    const int DllLoaderBase::NoError = ERROR_SUCCESS;


    DllLoaderBase::DllLoaderBase()
        : m_hModule(InvalidValue)
        , m_ErrCode(NoError)
    {
    }

    DllLoaderBase::~DllLoaderBase()
    {
        Unload();
    }

    bool DllLoaderBase::Load(const wchar_t* dllPath)
    {
        Unload();
        m_hModule = (void*)::LoadLibraryW(dllPath);
        m_ErrCode = ::GetLastError();
        return IsLoaded();
    }

    void DllLoaderBase::Unload()
    {
        if (m_hModule != InvalidValue)
        {
//            ::FreeLibrary((HMODULE)m_hModule);
            m_hModule = InvalidValue;
            m_ErrCode = NoError;
        }
    }

    bool DllLoaderBase::IsLoaded() const
    {
        return m_hModule != InvalidValue;
    }

    void* DllLoaderBase::Get()
    {
        return m_hModule;
    }

    int DllLoaderBase::GetErrCode() const
    {
        return m_ErrCode;
    }

// ---------------------------------------------------------------------

    DllLoader::DllLoader()
    {
    }

    //static
    void DllLoader::RemoveBackSeparator(wchar_t* str)
    {
        if (str && *str)
        {
            wchar_t* t = str + wcslen(str) - 1;
            if (*t == L'\\' || *t == L'/')
                *t = L'\0';
        }
    }

    bool DllLoader::Load(const wchar_t* dllPath, const wchar_t* dllFolderEx)
    {
        bool ret = false;
        Unload();
        if (dllPath && *dllPath)
        {
            wchar_t dllPath2[MAX_PATH] = L"";
            wchar_t* pF2 = nullptr;
            int rr = ::GetFullPathNameW(dllPath, MAX_PATH, dllPath2, &pF2);
            if (0 < rr && rr < MAX_PATH)
            {
                wchar_t dllFolderEx2[MAX_PATH] = L"";
                if (dllFolderEx && *dllFolderEx)
                {
                    wcscpy_s(dllFolderEx2, dllFolderEx);
                    RemoveBackSeparator(dllFolderEx2);
                }
                else if (pF2)
                {
                    wcsncpy_s(dllFolderEx2, dllPath2, pF2 - dllPath2 - 1);
                }

                if (*dllFolderEx2)
                    ::SetDllDirectoryW(dllFolderEx2);
                ret = m_Lib.Load(dllPath2);
                if (*dllFolderEx2)
                    ::SetDllDirectoryW(nullptr);
            }
        }
        return ret;
    }

    void* DllLoader::GetIface(const char* entryPointName)
    {
        void* ret = nullptr;
        if (m_Lib.IsLoaded() && entryPointName && *entryPointName)
        {
            P_DLL_ENTRY_POINT pEntryPoint = (P_DLL_ENTRY_POINT)::GetProcAddress((HMODULE)m_Lib.Get(), entryPointName);
            if (pEntryPoint)
                ret = (*pEntryPoint)();
        }
        return ret;
    }

} // namespace FFmpegInterop

