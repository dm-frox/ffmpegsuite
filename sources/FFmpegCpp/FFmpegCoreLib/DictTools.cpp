#include "pch.h"

#include "DictTools.h"

#include "Logger.h"

#include "MultiString.h"


namespace Fcore
{
    const int  DictHolder::Flags   = 0;


    DictHolder::DictHolder(const char* keyVal)
        : m_Dict(MultiString::CreateDictionary(keyVal))
    {}

    DictHolder::~DictHolder()
    {
        av_dict_free(&m_Dict);
    }

    AVDictionary* DictHolder::Move()
    { 
        AVDictionary* ret = m_Dict;
        m_Dict = nullptr;
        return ret; 
    }

    // static
    inline int DictHolder::Count(const AVDictionary* dict)
    {
        return (dict != nullptr) ? av_dict_count(dict) : -1;
    }

    int  DictHolder::Count() const
    {
        return Count(m_Dict);
    }

    void DictHolder::Log() const
    {
        Log(__FUNCTION__, "dictionary");
    }

    void DictHolder::Log(const char* func, const char* descr) const
    {
        Log(m_Dict, func, descr);
    }

    // static
    void DictHolder::LogNotFound(AVDictionary* dict, const char* func)
    {
        if (dict)
        {
            if (av_dict_count(dict) > 0)
            {
                Log(dict, func, "not found options", (int)SLog::Level::Warning);
            }
            av_dict_free(&dict);
        }
    }

    // static
    void DictHolder::Log(const AVDictionary* dict, const char* func, const char* descr, int logLev)
    {
        SLog::Level lev = logLev >= 0 ? (SLog::Level)logLev : SLog::Level::Info;
        if (SLog::CanLog(lev))
        {
            if (dict)
            {
                DictIterator diter(dict);
                int count = diter.Count();
                SLog::Log(lev, "%s, %s, item count=%d, items:", func, descr, count);
                if (count > 0)
                {
                    std::string ss;
                    ss.reserve(256);
                    const char* key = nullptr;
                    const char* val = nullptr;
                    while (diter.Next(key, val))
                    {
                        ss += key;
                        ss += '=';
                        ss += val;
                        ss += ';';
                    }
                    SLog::Log(lev, " -- %s", ss.c_str());
                }
            }
            else
            {
                SLog::Log(lev, "%s, %s, null", func, descr);
            }
        }
    }

    const char* DictHolder::operator[](const char* key) const
    {
        const char* val = nullptr;
        if (key && *key)
        {
            FindKey(m_Dict, key, val);
        }
        return val;
    }

    // static
    bool DictHolder::FindKey(const AVDictionary* dict, const char* key, const char*& val)
    {
        bool ret = false;
        if (dict)
        {
            if (const AVDictionaryEntry* entry = av_dict_get(dict, key, nullptr, 0))
            {
                val = entry->value;
                ret = true;
            }
        }
        return ret;
    }

    //static 
    AVDictionary* DictHolder::Copy(const AVDictionary* dict)
    {
        AVDictionary* ret = nullptr;
        if (dict)
        {
            av_dict_copy(&ret, dict, 0);
        }
        return ret;
    }

// ---------------------------------------------------------------------

    const char DictIterator::NullKey[] = "";
    const int  DictIterator::Flags     = AV_DICT_IGNORE_SUFFIX;

    DictIterator::DictIterator(const AVDictionary* dict)
        : m_Dict(dict)
    {}

    DictIterator::~DictIterator() = default;

    int DictIterator::Count() const
    {
        return DictHolder::Count(m_Dict);
    }

    bool DictIterator::Next(const char*& key, const char*& val)
    {
        bool ret = false;
        if (m_Dict)
        {
            if (auto entry = av_dict_get(m_Dict, NullKey, m_Entry, Flags))
            {
                m_Entry = entry;
                key = m_Entry->key;
                val = m_Entry->value;
                ret = true;
            }
        }
        return ret;
    }

} // namespace Fcore



//#define AV_DICT_MATCH_CASE   1
//
//#define AV_DICT_IGNORE_SUFFIX   2
//
//#define AV_DICT_DONT_STRDUP_KEY   4
//Take ownership of a key that's been allocated with av_malloc() and children.
//
//#define AV_DICT_DONT_STRDUP_VAL   8
//Take ownership of a value that's been allocated with av_malloc() and chilren.
//
//#define AV_DICT_DONT_OVERWRITE   16
//Don't overwrite existing entries.
//
//#define AV_DICT_APPEND   32
//If the entry already exists, append to it.

