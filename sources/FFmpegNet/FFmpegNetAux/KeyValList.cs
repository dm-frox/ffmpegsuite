// ---------------------------------------------------------------------
// File: KeyValList.cs
// Classes: KeyValList
// Purpose: helper to support some ffmpeg data
// Module: FFmpegNetCore - .NET assembly to provide more comfortable interface to FFmpegInterop.dll
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2020
// ---------------------------------------------------------------------

using System;
using System.Text;
using System.Collections;

using KeyVal         = System.Collections.Generic.KeyValuePair<string, string>;
using KeyValListImpl = System.Collections.Generic.List<System.Collections.Generic.KeyValuePair<string, string>>;

namespace FFmpegNetAux
{
    public class KeyValList : IEnumerable
    {
        const int DefListLen = 8;
        const int DefPairLen = 64;

        const char KeyValDelim = '\n';
        const char PairDelim   = '\0';

        static readonly char[] PairSepar = { PairDelim };

        KeyValListImpl m_List;

        public KeyValList()
        { }

        internal void Clear() => m_List?.Clear();

        public int Length => m_List?.Count ?? 0;
            //(m_List != null) ? m_List.Count : 0;

        public KeyVal this[int ind] => m_List[ind];

        IEnumerator IEnumerable.GetEnumerator() => m_List.GetEnumerator();

        public override string ToString()
        {
            string ret = null;
            if (Length > 0)
            {
                var sb = new StringBuilder(m_List.Count * DefPairLen);
                foreach (var kv in m_List)
                {
                    sb.Append(kv.Key);
                    sb.Append(KeyValDelim);
                    sb.Append(kv.Value);
                    sb.Append(PairDelim);
                }
                sb.Append(PairDelim);
                ret = sb.ToString();
            }
            return ret;
        }

        void Prepare(int length)
        {
            if (m_List == null)
            {
                m_List = new KeyValListImpl(length);
            }
        }

        internal void Add(KeyVal kv)
        {
            Prepare(DefListLen);
            m_List.Add(kv);
        }

        public void Add(string key, string val) => Add(new KeyVal(key, val));

        public void AddNoEmpty(string key, string val)
        {
            if (!string.IsNullOrEmpty(val))
            {
                Add(new KeyVal(key, val));
            }
        }

        public string Find(string key)
        {
            string ret = null;
            if (m_List != null)
            {
                KeyVal res = m_List.Find(val => val.Key == key);
                if (!string.IsNullOrEmpty(res.Key))
                {
                    ret = res.Value;
                }
            }
            return ret;
        }

        public string FirstValue => (Length > 0) ? m_List[0].Value : "";

        public static KeyValList FromString(string kvStr)
        {
            KeyValList list = new KeyValList();
            if (!string.IsNullOrEmpty(kvStr))
            {
                string[] items = kvStr.Split(PairSepar, StringSplitOptions.RemoveEmptyEntries);
                if (items.Length > 0)
                {
                    list.Prepare(items.Length);
                    foreach (var s in items)
                    {
                        int sepPos = s.IndexOf(KeyValDelim);
                        if (sepPos > 0)
                        {
                            list.Add(s.Substring(0, sepPos), s.Substring(sepPos + 1));
                        }
                    }
                }
            }
            return list;
        }

    } // class KeyValList

} // namespace FFmpegNetAux
