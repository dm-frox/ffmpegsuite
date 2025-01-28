using System;
using System.IO;

namespace CsLogger
{
    class BackupMan
    {
        const int Mbyte             = 1024 * 1024;
        const int BackupNumMax      = 99;
        const int DefBackupCount    = 20;
        const int DefFileSize       = Mbyte;
        const int FileSize0         = 512;
        const int FileSize1         = 1024 * Mbyte;


        static readonly char[] BackupSuffTempl = { '_', '0', '0' };

        int  m_BackupCount = DefBackupCount;
        bool m_BackupSize  = false;
        int  m_MaxFileSize = DefFileSize;

        string          m_FullPath;
        string          m_Folder;
        DirectoryInfo   m_DirInfo;
        string          m_Mask;
        int             m_DotPos;


        internal BackupMan()
        {
        }

        internal void Clear()
        {
            m_FullPath = null;
            m_Folder = null;
            m_DirInfo = null;
            m_Mask = null;
            m_DotPos = 0;
        }

        internal int BackupCount
        {
            get { return m_BackupCount; }
            set { m_BackupCount = Clamp(value, 1, BackupNumMax); }
        }

        internal bool BackupSize
        {
            get { return m_BackupSize; }
            set { m_BackupSize = value; }
        }

        internal int MaxFileSize
        {
            get { return m_MaxFileSize; }
            set { m_MaxFileSize = Clamp(value, FileSize0, FileSize1); }
        }

        internal string FullPath
        {
            get { return m_FullPath; }
        }

        internal string Folder
        {
            get { return m_Folder; }
        }

        internal void SetParamFromText(string name, string svalue) // from config fileName
        {
            switch (name)
            {
            case "BackupCount":
                ConfigFile.ParseInt(svalue, ref m_BackupCount);
                break;
            case "MaxFileSize":
                ConfigFile.ParseInt(svalue, ref m_MaxFileSize);
                break;
            case "BackupSize":
                ConfigFile.ParseBool(svalue, ref m_BackupSize);
                break;
            }
        }

        internal bool Init(string folder, string fileName)
        {
            bool ret = false;
            m_DirInfo  = new DirectoryInfo(folder);
            m_Folder   = m_DirInfo.FullName;
            m_FullPath = Path.Combine(folder, fileName);
            int n = fileName.LastIndexOf('.');
            if (n > 0)
            {
                m_Mask = fileName.Insert(n, "*");
                m_DotPos = m_FullPath.Length - (fileName.Length - n);
                ret = true;
            }
            return ret;
        }

        internal void Backup()
        {
            if (m_DirInfo != null)
            {
                m_DirInfo.Refresh();
                FileInfo[] fs = m_DirInfo.GetFiles(m_Mask);
                int n = fs.Length;
                if (n > 0)
                {
                    if (n > 1)
                        Array.Sort(fs, (f1, f2) => { return string.Compare(f1.Name, f2.Name); });
                    int maxBackupCount = m_BackupSize
                        ? EstimateCount(fs) 
                        : m_BackupCount;
                    DoBackup(fs, maxBackupCount);
                }
            }
        }

        int EstimateCount(FileInfo[] fs)
        {
            int totSize = m_MaxFileSize * m_BackupCount;
            int maxBackup = fs.Length;
            for (int i = 0, s = 0; i < fs.Length && i <= BackupNumMax + 1; ++i)
            {
                s += (int)fs[i].Length;
                if (s > totSize)
                {
                    maxBackup = i - 1;
                    break;
                }
            }
            maxBackup = Math.Max(1, maxBackup);
            return maxBackup;
        }

        void DoBackup(FileInfo[] fs, int maxBackupCount)
        {
            for (int i = fs.Length - 1; i >= 0; --i)
            {
                string file = fs[i].FullName,
                    newFile = null;
                bool del = i >= maxBackupCount;
                if (!del)
                    newFile = IncrementName(file, ref del);
                if (del)
                    File.Delete(file);
                else if (newFile != null)
                    File.Move(file, newFile);
            }
        }

        string IncrementName(string name, ref bool del)
        {
            string t = null;
            int n = name.Length;
            int logLen = m_FullPath.Length;
            if (n == logLen + 3)
            {
                int k = name.LastIndexOf('.') - 3; // underscore position
                if (k > 0 && (name[k] == BackupSuffTempl[0]))
                {
                    char d1 = name[k + 1], d0 = name[k + 2];
                    if (Char.IsDigit(d1) && Char.IsDigit(d0))
                    {
                        int zz = (int)'0';
                        int xx = ((int)d1 - zz) * 10 + ((int)d0 - zz);
                        if (xx < BackupNumMax)
                        {
                            ++d0;
                            if (d0 > '9')
                            {
                                ++d1;
                                d0 = '0';
                            }
                            BackupSuffTempl[1] = d1;
                            BackupSuffTempl[2] = d0;
                            t = new string(BackupSuffTempl);
                        }
                        else
                        {
                            del = true;
                        }
                    }
                }
            }
            else if (n == logLen)
            {
                BackupSuffTempl[1] = '0';
                BackupSuffTempl[2] = '1';
                t = new string(BackupSuffTempl);
            }
            return (t != null) ? m_FullPath.Insert(m_DotPos, t) : null;
        }

        static int Clamp(int val, int min, int max)
        {
            return Math.Min(Math.Max(min, val), max);
        }
    } // class BackupMan

} // namespace CsLogger

