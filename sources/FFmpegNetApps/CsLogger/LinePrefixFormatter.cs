using System;
using System.IO;
using System.Threading;

namespace CsLogger
{
    class LinePrefixFormatter
    {
        const int ThreadLabLen     = 4;
        const char ThreadLabPrefix = '{';
        const string PrefixTempl   = "[0] 00 mes 00:00:00.000 ";
        const string PrefixEnd     = "| ";
        const string ThreadLabs    = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYXabcdefghijklmnopqrstuvwxyz@#$%&";

        static readonly string[] Month    = { "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec" };
        static readonly string[] PrefixEW = { "Error !! - ", "Warning! - " };

        bool    m_ThreadLabel = false;
        char[]  m_Prefix = null;
        char[]  m_ThreadLab = null;


        internal LinePrefixFormatter()
        {
            m_Prefix = new char[PrefixTempl.Length];
            m_ThreadLab = new char[ThreadLabLen];
            m_ThreadLab[0] = ThreadLabPrefix;
        }

        internal bool ThreadLabel
        {
            get { return m_ThreadLabel; }
            set { m_ThreadLabel = value; }
        }

        void Fd1(int offset, int r)
        {
            m_Prefix[offset] += (char)(r);
        }
        void Fd2(int offset, int r)
        {
            Fd1(offset, r / 10);
            Fd1(offset + 1, r % 10);
        }
        void Fd3(int offset, int r)
        {
            Fd1(offset, r / 100);
            Fd2(offset + 1, r % 100);
        }

        void WriteTreadLabel(StreamWriter writer)
        {
            int id = Thread.CurrentThread.ManagedThreadId;
            for (int i = ThreadLabLen - 1, s = 0; i > 0; --i, s += 6)
                m_ThreadLab[i] = ThreadLabs[(id >> s) & 0x3F];
            writer.Write(m_ThreadLab);
        }

        internal void Write(StreamWriter writer, Level level)
        {
            int lev = (int)level;
            DateTime dt = DateTime.Now;
            PrefixTempl.CopyTo(0, m_Prefix, 0, m_Prefix.Length);
            Fd1(1, lev);
            Fd2(4, dt.Day);
            Month[dt.Month - 1].CopyTo(0, m_Prefix, 7, 3);
            Fd2(11, dt.Hour);
            Fd2(14, dt.Minute);
            Fd2(17, dt.Second);
            Fd3(20, dt.Millisecond);
            writer.Write(m_Prefix);
            if (m_ThreadLabel)
                WriteTreadLabel(writer);
            writer.Write(PrefixEnd);
            if (0 < lev && lev <= PrefixEW.Length)
                writer.Write(PrefixEW[lev - 1]);
        }

    } // class LinePrefix

} // namespace CsLogger
