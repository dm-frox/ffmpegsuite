#include "pch.h"

#include "MultiString.h"

#include <sstream>
#include <iomanip>
#include <cmath>

#include "DictTools.h"
#include "Logger.h"


namespace
{
    void FormatTime(std::ostringstream& ss, double val)
    {
        if (val > 0.0)
        {
            double ds = std::floor(val);
            int msecs = (int)std::round((val - ds) * 1000);
            int secs = (int)ds;
            int mins = secs / 60;
            int hours = secs / 3600;
            ss << hours << std::setfill('0')
                << ':' << std::setw(2) << mins % 60
                << ':' << std::setw(2) << secs % 60
                << '.' << std::setw(3) << msecs;
        }
        else
        {
            ss << "0";
        }
    }
}

namespace Fcore
{
    const char   MultiString::PairDelim    = '\0';
    const char   MultiString::KeyValDelim  = '\n';
    const char   MultiString::KeyValDelimIni = '=';
    const char   MultiString::PairDelimIni[] = "\n\r";

    const size_t MultiString::DefItemLen   = 64;
    const int    MultiString::DictFlags    = 0;
    const int    MultiString::DefPrecision = 3;


    MultiString::MultiString() = default;

    MultiString::~MultiString() = default;

    void MultiString::Reserve(int itemCount)
    {
        m_MultiString.reserve(itemCount * DefItemLen);
    }

    void MultiString::Reset()
    {
        m_MultiString.clear();
        m_Count = 0;
    }

    void MultiString::SetEnd()
    {
        m_MultiString += PairDelim;
    }

    void MultiString::Add(const char* key, const char* val)
    {
        if (key && *key)
        {
            m_MultiString += key;
            m_MultiString += KeyValDelim;
            m_MultiString += val;
            m_MultiString += PairDelim;
            ++m_Count;
        }
        else
        {
            LOG_ERROR("%s, empty key", __FUNCTION__);
        }
    }

    int MultiString::AddDictionary(const AVDictionary* dict)
    {
        int ret = 0;
        if (dict)
        {
            DictIterator diter(dict);
            int n = diter.Count();
            if (n > 0)
            {
                Reserve(n);
                const char* key = nullptr;
                const char* val = nullptr;
                while (diter.Next(key, val))
                {
                    Add(key, val);
                    ++ret;
                }
            }
        }
        return ret;
    }

    void MultiString::SetDictionary(const AVDictionary* dict)
    {
        Reset();
        if (AddDictionary(dict) > 0)
        {
            SetEnd();
        }
    }

    template<typename T>
    void MultiString::Add_(const char* key, T val)
    {
        std::ostringstream ss;
        ss << val;
        Add(key, ss.str().c_str());
    }

    void MultiString::Add(const char* key, int32_t val)
    {
        Add_(key, val);
    }

    void MultiString::Add(const char* key, uint32_t val)
    {
        Add_(key, val);
    }

    void MultiString::Add(const char* key, int64_t val)
    {
        Add_(key, val);
    }

    void MultiString::Add(const char* key, uint64_t val)
    {
        Add_(key, val);
    }

    void MultiString::Add(const char* key, bool val)
    {
        Add_(key, val ? "yes" : "no");
    }

    void MultiString::AddTime(const char* key, double val, double val2)
    {
        std::ostringstream ss;
        FormatTime(ss, val);
        if (val2 > 0.0)
        {
            ss << " - ";
            FormatTime(ss, val2);
        }
        Add(key, ss.str().c_str());
    }

    void MultiString::AddFramerate(const char* key, AVRational val)
    {
        double fr = val.den > 0 ? (double)val.num / val.den : 0.0;
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(3) << fr << " (" << val.num << "/" << val.den << ")";
        Add(key, ss.str().c_str());
    }

    // static 
    void MultiString::Dump(const char* keyVals)
    {
        MultiStringIterator itr(keyVals);
        for (const char* str = itr.Curr(); *str; str = itr.Next())
        {
            LOG_INFO("MultiString: len=%d, str=%s", std::strlen(str), str);
        }
    }

    //static 
    AVDictionary* MultiString::CreateDictionary(const char* keyVals)
    {
        AVDictionary* dict = nullptr;

        MultiStringIterator itr(keyVals);
        for (const char* pair = itr.Curr(); *pair; pair = itr.Next())
        {
            if (char* pcopy = av_strdup(pair))
            {
                if (char* keyEnd = std::strchr(pcopy, KeyValDelim))
                {
                    if (keyEnd > pcopy)
                    {
                        *keyEnd = '\0';
                        int res = av_dict_set(&dict, pcopy, keyEnd + 1, DictFlags);
                        if (res < 0)
                        {
                            LOG_ERROR("%s, av_dict_set, %s", __FUNCTION__, FmtErr(res));
                        }
                    }
                    else
                    {
                        LOG_ERROR("%s, empty key", __FUNCTION__);
                    }
                }
                else
                {
                    LOG_ERROR("%s, key marker", __FUNCTION__);
                }
                av_free(pcopy);
            }
            else
            {
                LOG_ERROR("%s, failed to duplicate string", __FUNCTION__);
            }
        }

        return dict;
    }

    std::string MultiString::PackDictionary(const AVDictionary* pDict, int& count)
    {
        int ret = 0;
        std::string md;
        DictIterator diter(pDict);
        int n = diter.Count();
        if (n > 0)
        {
            md.reserve(n * DefItemLen);
            const char* key = nullptr;
            const char* val = nullptr;
            while (diter.Next(key, val))
            {
                md += key;
                md += KeyValDelim;
                md += val;
                md += PairDelim;
                ++ret;
            }
            md += PairDelim;
        }
        count = ret;
        return md;
    }

    // static
    int MultiString::FindLength(const char* str)
    {
        if (str && *str)
        {
            for (int i = 0, prev = 1; ; ++i, ++str)
            {
                int curr = *str;
                if (curr == 0 && prev == 0)
                {
                    return (i + 1);
                }
                prev = curr;
            }
        }
        return 0;
    }

    //static 
    void MultiString::Assign(std::string& dst, const char* src)
    {
        if (src && *src)
        {
            int len = FindLength(src);
            dst.assign(src, len);
        }
        else
        {
            dst.clear();
        }
    }

    //static 
    std::string MultiString::FromIniFormat(const char* strIni) // from key=value\n...
    {
        std::string ret;
        if (strIni && *strIni)
        {
            ret.reserve(std::strlen(strIni) + 2);
            bool pdFlag = false;
            for (const char* p = strIni; *p; ++p)
            {
                char cc = *p;
                if (cc == KeyValDelimIni)
                {
                    ret += KeyValDelim;
                    pdFlag = false;
                }
                else if (std::strchr(PairDelimIni, cc))
                {
                    if (!pdFlag)
                    {
                        ret += PairDelim;
                        pdFlag = true;
                    }
                    // else skip multi delims
                }
                else
                {
                    ret += cc;
                    pdFlag = false;
                }
            }
            ret += PairDelim;
            int d = (int)ret.length() - 2;
            if (d >= 0 && ret[d] != PairDelim)
            {
                ret += PairDelim;
            }
        }
        return ret;
    }

    //static 
    std::string MultiString::ToString(const std::string& multiStr) // to key=value;...
    {
        std::string ret;
        auto len = multiStr.length();
        if (len > 0)
        {
            ret.reserve(len);
            for (char c : multiStr)
            {
                ret += ((c == KeyValDelim) ? '=' : ((c == PairDelim) ? ';' : c));
            }
            ret[len - 1] = '\0';
        }
        return ret;
    }


} // namespace Fcore

