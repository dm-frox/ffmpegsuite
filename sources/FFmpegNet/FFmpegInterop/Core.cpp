#include "pch.h"

#include "Core.h"

#include "FFmpegWrapper.h"

#include "DllLoader.h"
#include "Marshal.h"
#include "Exceptions.h"
#include "ImplPtr.h"
#include "CommTypes.h"


namespace FFmpegInterop
{

    Core::Core()
        : m_DllLoader(nullptr)
        , m_Core(nullptr)
    {}

    void Core::Close_()
    {
        if (m_Core)
        {
            m_Core->CloseLog();
            m_Core->Free();
            m_Core = nullptr;
        }

        if (m_DllLoader)
        {
            DllFlag::SetLoaded(false);
            m_DllLoader->Unload();
            delete m_DllLoader;
            m_DllLoader = nullptr;
        }
    }

    void Core::Initialize_(StrType^ wrapFolderPath, StrType^ ffmpegFolderPath)
    {
        ASSERT_GEN(m_Core == nullptr, "already initialized");

        DllLoaderPtr loader;

        StrType^ dllPath = System::IO::Path::Combine(wrapFolderPath, gcnew StrType(FFmpegWrapperModuleName));
        ToWChars ffDir(ffmpegFolderPath);
        if (loader->Load(ToWChars(dllPath), ffDir))
        {
            if (void* p = loader->GetIface(FFmpegWrapperEntryPoint))
            {
                m_Core = static_cast<Fwrap::ICore*>(p);
                m_Core->SetBaseFolder(ffDir);
                m_Core->Initialize();
                m_DllLoader = loader.Move();
                DllFlag::SetLoaded(true);
            }
            else
            {
                RAISE_EXC("no DLL entry point");
            }
        }
        else
        {
            RAISE_EXC2("cannot load DLL", loader->GetErrCode());
        }
    }

    StrType^ Core::GetVersion_(Fwrap::LibTag libTag)
    {
        return gcnew StrType(m_Core->GetVersion(libTag));
    }

    void Core::GetVersions_(array<StrType^>^% libNames, array<StrType^>^% versions)
    {
        ASSERT_PTR(m_Core);
 
        int n = (int)Fwrap::LibTag::LibCount;
        array<StrType^>^ libs = gcnew array<StrType^>(n);
        array<StrType^>^ vers = gcnew array<StrType^>(n);
        int k = 0;
        libs[k] = "Wrapper";
        vers[k++] = GetVersion_(Fwrap::LibTag::Self);
        libs[k] = "FFmpeg";
        vers[k++] = GetVersion_(Fwrap::LibTag::FFmpeg);

        libs[k] = "libavutil";
        vers[k++] = GetVersion_(Fwrap::LibTag::AvUtil);
        libs[k] = "libavcodec";
        vers[k++] = GetVersion_(Fwrap::LibTag::AvCodec);
        libs[k] = "libavformat";
        vers[k++] = GetVersion_(Fwrap::LibTag::AvFormat);
        libs[k] = "libavdevice";
        vers[k++] = GetVersion_(Fwrap::LibTag::AvDevice);
        libs[k] = "libavfilter";
        vers[k++] = GetVersion_(Fwrap::LibTag::AvFilter);
        libs[k] = "libswscale";
        vers[k++] = GetVersion_(Fwrap::LibTag::SwScale);
        libs[k] = "libswresample";
        vers[k++] = GetVersion_(Fwrap::LibTag::SwResample);
        libs[k] = "libpostproc";
        vers[k++] = GetVersion_(Fwrap::LibTag::PostProc);

        libNames = libs;
        versions = vers;
    }

    array<StrType^>^ Core::GetDShowDevList_(int devType)
    {
        ASSERT_PTR(m_Core);

        int n = m_Core->UpdateDShowDevList(devType);

        array<StrType^>^ devList = gcnew array<StrType^>(n);

        for (int i = 0; i < n; ++i)
        {
            const wchar_t* str = m_Core->GetDShowDev(i);
            devList[i] = gcnew StrType((str && *str) ? str : L"");
        }

        return devList;
    }

// ---------------------------------------------------------------------------
// static

    static Core::Core()
    {
        TheInstance = gcnew Core();
    }

    Fwrap::ICore* Core::Ptr()
    {
        Fwrap::ICore* ret = TheInstance->m_Core;
        ASSERT_PTR(ret);
        return ret;
    }

    void Core::Initialize(StrType^ wrapFolderPath, StrType^ ffmpegFolderPath)
    {
        TheInstance->Initialize_(wrapFolderPath, ffmpegFolderPath);
    }

    void Core::Close()
    {
        TheInstance->Close_();
    }

    void Core::GetVersions(array<StrType^>^% libs, array<StrType^>^% vers)
    {
        TheInstance->GetVersions_(libs, vers);
    }

    StrType^ Core::PlatformTag()
    {
        const wchar_t *platf =
#ifdef _WIN64
        L"x64";
#else 
        L"x86";
#endif
        return gcnew StrType(platf);
    }

    void Core::EnableFFmpegLogging(LogLevel lev)
    {
        Ptr()->EnableFFmpegLogging((int)lev);
    }

    bool Core::OpenLog(LogLevel lev, StrType^ path, LogOptions options, int maxFileSize, int backupCount)
    {
        return Ptr()->OpenLog((int)lev, ToWChars(path), (int)options, maxFileSize, backupCount);;
    }

    StrType^ Core::GetHWAccels()
    {
        return gcnew StrType(Ptr()->GetHWAccels());
    }

    void Core::LogString(LogLevel lev, StrType^ str)
    {
        Ptr()->LogString((int)lev, TO_CHARS(str));
    }
    
    void Core::FlushLog()
    {
        Ptr()->FlushLog();
    }

    array<StrType^>^ Core::GetDShowDevList(int devType)
    {
        return TheInstance->GetDShowDevList_(devType);
    }

} // namespace FFmpegInterop

