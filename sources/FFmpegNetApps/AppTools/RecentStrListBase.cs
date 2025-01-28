using System;
using System.IO;

namespace AppTools
{
    public class RecentStrListBase
    {
        readonly bool m_IgnoreCase;
        string[]      m_Items;

        public RecentStrListBase(int n, bool ignoreCase)
        {
            if (n < 1)
            {
                throw new ArgumentException("RecentStrListBase: bad list size");
            }
            m_IgnoreCase = ignoreCase;
            m_Items = new string[n];
        }

        protected string[] Items => m_Items;

        int FindToRemove(string item)
        {
            int ret = -1;
            for (int i = 0, n = m_Items.Length; i < n && ret < 0; ++i)
            {
                string curr = m_Items[i];
                if (curr != null)
                {
                    if (string.Compare(curr, item, m_IgnoreCase) == 0)
                    {
                        ret = i;
                    }
                }
                else
                {
                    ret = i;
                }
            }
            return ret;
        }

        void ShiftAndAdd(int k, string item)
        {
            int n1 = m_Items.Length - 1;
            int rr = (0 <= k && k <= n1) ? k : n1;
            for (int i = rr; i >= 1; --i)
            {
                m_Items[i] = m_Items[i - 1];
            }
            m_Items[0] = item;
        }

        public void AddItem(string item)
        {
            if (item != null)
            {
                int k = FindToRemove(item);
                ShiftAndAdd(k, item);
            }
        }

        public string[] GetItems()
        {
            string[] items = null;
            for (int n1 = m_Items.Length - 1, i = n1; i >= 0; --i)
            {
                if (m_Items[i] != null)
                {
                    if (i == n1)
                    {
                        items = m_Items;
                    }
                    else
                    {
                        items = new string[i + 1];
                        Array.Copy(m_Items, items, items.Length);
                    }
                    break;
                }
            }
            return items ?? new string[0];
        }

    } // class RecentStrListBase

    public class RecentStrList : RecentStrListBase
    {
        readonly string m_FilePath; // to store list

        public RecentStrList(int n, string filePath, bool ignoreCase)
            : base(n, ignoreCase)
        {
            if (string.IsNullOrEmpty(filePath))
            {
                throw new ArgumentException("RecentStrList: bad file path");
            }

            m_FilePath = filePath;
        }

        static void Save(string path, string[] items)
        {
            File.WriteAllLines(path, items);
        }

        static void Load(string path, string[] items)
        {
            string[] lines = File.ReadAllLines(path);
            Array.Copy(lines, items, Math.Min(lines.Length, items.Length));
        }

        public void Save()
        {

            try
            {
                Save(m_FilePath, GetItems());
            }
            catch 
            { 
            }
        }

        public void Load()
        {

            try
            {
                Load(m_FilePath, Items);
            }
            catch 
            {
            }
        }

    } // class RecentStrList

} // namespace AppTools

