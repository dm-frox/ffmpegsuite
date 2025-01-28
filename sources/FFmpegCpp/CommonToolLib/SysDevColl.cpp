#include "pch.h"

#include "windefs.h"

#ifdef VISUAL_STUDIO_

#include "SysDevColl.h"

#include <DShow.h>

#include "SLogger.h"

namespace // COM stuff
{
    void OnError(int32_t hr, const char* msg)
    {
        LOG_ERROR("COM error: hr=0x%X, msg=%s", hr, msg);
    }

    ICreateDevEnum* CreateDevEnum()
    {
        ICreateDevEnum* ret = nullptr;
        void* p = nullptr;
        HRESULT hr = ::CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC_SERVER, __uuidof(ICreateDevEnum), &p);
        if (SUCCEEDED(hr))
        {
            ret = static_cast<ICreateDevEnum*>(p);
        }
        else
        {
            OnError(hr, "CreateDevEnum");
        }
        return ret;
    }

    IEnumMoniker* CreateClassEnumerator(ICreateDevEnum* devEnum, const void* catId)
    {
        IEnumMoniker* ret = nullptr;
        DWORD flags = 0;
        HRESULT hr = devEnum->CreateClassEnumerator(*static_cast<const IID*>(catId), &ret, flags);
        if (!SUCCEEDED(hr))
        {
            OnError(hr, "CreateClassEnumerator");
        }
        return ret;
    }

    IMoniker* GetNextMoniker(IEnumMoniker* enumMon)
    {
        IMoniker* ret = nullptr;
        HRESULT hr = enumMon->Next(1, &ret, nullptr);
        if (hr == S_OK)
        {
            // OK, ret != nullptr
        }
        else
        {
            if (hr != S_FALSE)
            {
                OnError(hr, "GetNextMoniker");
            }
            ret = nullptr;
        }
        return ret;
    }

    IPropertyBag* BindToStorage(IMoniker* moniker)
    {
        IPropertyBag* ret = nullptr;
        void* p = nullptr;
        HRESULT hr = moniker->BindToStorage(nullptr, nullptr, IID_IPropertyBag, &p);
        if (SUCCEEDED(hr))
        {
            ret = static_cast<IPropertyBag*>(p);
        }
        else
        {
            OnError(hr, "BindToStorage");
        }
        return ret;
    }

    IBaseFilter* BindToObject(IMoniker* moniker)
    {
        IBaseFilter* ret = nullptr;
        void* p = nullptr;
        HRESULT hr = moniker->BindToObject(nullptr, nullptr, IID_IBaseFilter, &p);
        if (SUCCEEDED(hr))
        {
            ret = static_cast<IBaseFilter*>(p);
        }
        else
        {
            OnError(hr, "BindToObject");
        }
        return ret;
    }

    bool GetStringFromPropertyBag(IPropertyBag* propBag, const wchar_t* propName, std::wstring& val)
    {
        bool ret = false;
        VARIANT var;
        ::VariantInit(&var);
        var.vt = VT_BSTR;
        HRESULT hr = propBag->Read(propName, &var, nullptr);
        if (SUCCEEDED(hr))
        {
            ret = true;
            val = var.bstrVal;
            ::VariantClear(&var);
        }
        else
        {
            OnError(hr, "GetStringFromPropertyBag");
        }
        return ret;
    }

    bool QueryFilterName(IBaseFilter* filter, std::wstring& val)
    {
        bool ret = false;
        FILTER_INFO fi;
        memset(&fi, 0, sizeof(fi));
        HRESULT hr = filter->QueryFilterInfo(&fi);
        if (SUCCEEDED(hr))
        {
            ret = true;
            val = fi.achName;
        }
        else
        {
            OnError(hr, "QueryFilterName");
        }
        return ret;
    }

} // namespace anon

// ---------------------------------------------------------------------

namespace Ftool
{

    SysDevColl::SysDevColl(const void* catId, bool fetchFilter)
        : m_CatId(catId)
        , m_FetchFilter(fetchFilter)
    {}

    SysDevColl::~SysDevColl()
    {
        Close();
    }

    void SysDevColl::Close()
    {
        m_Current.Release();
        if (m_EnumMon)
        {
            m_EnumMon->Release();
            m_EnumMon = nullptr;
        }
    }

    bool SysDevColl::Init()
    {
        bool ret = false;
        Close();
        if (ICreateDevEnum* devEnum = CreateDevEnum())
        {
            if (IEnumMoniker* enumMon = CreateClassEnumerator(devEnum, m_CatId))
            {
                m_EnumMon = enumMon;
                ret = true;
            }
            devEnum->Release();
        }
        return ret;
    }

    bool SysDevColl::Next()
    {
        bool ret = false;
        m_Current.Release();
        if (m_EnumMon)
        {
            if (IMoniker* moniker = GetNextMoniker(m_EnumMon))
            {
                if (IPropertyBag* propBag = BindToStorage(moniker))
                {
                    m_Current.SetPropBag(propBag);
                    ret = true;
                    if (m_FetchFilter)
                    {
                        if (IBaseFilter* filter = BindToObject(moniker))
                        {
                            m_Current.SetFilter(filter);
                        }
                    }
                }
                moniker->Release();
            }
        }
        else
        {
            OnError(-1, "SysDevColl::Next, not initialized");
        }
        return ret;
    }

    SysDevIter SysDevColl::begin()
    {
        bool rr = false;
        if (Init())
        {
            if (Next())
                rr = true;
        }
        return SysDevIter(rr ? this : nullptr);
    }

    const SysDevIter SysDevColl::end() const
    {
        return SysDevIter();
    }

// ---------------------------------------------------------------------

    const wchar_t SysDevItem::PropName[] = L"FriendlyName";
    const wchar_t SysDevItem::PropGuid[] = L"CLSID";
    const wchar_t SysDevItem::PropFcc[]  = L"FccHandler";
    const wchar_t SysDevItem::Error[]    = L"<error>";

    SysDevItem::SysDevItem() = default;

    SysDevItem::~SysDevItem()
    {
        Release();
    }

    void SysDevItem::Release()
    {
        if (m_PropBag)
        {
            m_PropBag->Release();
            m_PropBag = nullptr;
        }
        if (m_Filter)
        {
            m_Filter->Release();
            m_Filter = nullptr;
        }
    }

    bool SysDevItem::GetString(const wchar_t* propName, std::wstring& val) const
    {
        bool ret = false;
        if (m_PropBag)
        {
            if (propName && *propName)
            {
                ret = GetStringFromPropertyBag(m_PropBag, propName, val);
            }
            else
            {
                OnError(-1, "SysDevItem::GetString, bad prop name");
            }
        }
        else
        {
            OnError(-1, "SysDevItem::GetString, no prop bag");
        }
        if (!ret)
            val = Error;
        return ret;
    }

    bool SysDevItem::GetFilterName(std::wstring& val) const
    {
        bool ret = false;
        if (m_Filter)
        {
            ret = QueryFilterName(m_Filter, val);
        }
        else
        {
            OnError(-1, "SysDevItem::GetString, no filter");
        }
        if (!ret)
            val = Error;
        return ret;
    }

} // namespace Ftool

#endif // VISUAL_STUDIO_



