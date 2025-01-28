using System;
using System.IO;

namespace Explorer
{
    abstract class ListItemBase
    {
        public int Index { get; set; }

        public string Name { get; set; }

        public string Descr { get; set; }

        internal abstract void Write(StreamWriter sw);

        internal static void WriteList(StreamWriter sw, ListItemBase[] itms, string txt, string txt2 = null)
        {
            sw.WriteLine(txt);
            if (!string.IsNullOrEmpty(txt2))
            {
                sw.WriteLine(txt);
            }
            foreach (var itm in itms)
            {
                itm.Write(sw);
            }
        }

    } // abstract class ListItemBase

    class ListItemFlt : ListItemBase
    {
        public string In { get; set; }
        public string Out { get; set; }
        public string Cmd { get; set; }

        internal override void Write(StreamWriter sw)
        {
            sw.WriteLine("{0,3} {1,-20} {2,3}  {3,3}  {4,3}  {5}",
                Index, Name, In, Out,
                !string.IsNullOrEmpty(Cmd) ? "cmd" : "",
                Descr);
        }

    } // class ListItemFlt

    class ListItemProto : ListItemBase
    {
        public string In { get; set; }

        public string Out { get; set; }

        internal override void Write(StreamWriter sw)
        {
            sw.WriteLine("{0,3} {1,-12} {2,3}  {3}",
                Index, Name,
                !string.IsNullOrEmpty(In) ? "in" : "",
                !string.IsNullOrEmpty(Out) ? "out" : "");
        }

    } // class ListItemProto

    class ListItemBsf : ListItemBase
    {
        public string CodecIds { get; set; }

        internal override void Write(StreamWriter sw)
        {
            sw.WriteLine("{0,3} {1,-12}, codecs: {2}", Index, Name, CodecIds);
        }

    } // class ListItemBsf

    class ListItemPix : ListItemBase
    {
        public int CompCount { get; set; }

        public int PlaneCount { get; set; }

        public int BitPerPix { get; set; }

        public string CompDepth { get; set; }

        public string Extra { get; set; }

        internal override void Write(StreamWriter sw)
        {
            sw.WriteLine("{0,3}  {1,-16} {2} {3} {4,2}  {5,-11}  {6}", 
                Index, Name, CompCount, PlaneCount, BitPerPix, CompDepth, Extra);
        }

    } // class ListItemPix

    class ListItemSamp : ListItemBase
    {
        public int BytesPerSamp { get; set; }

        internal override void Write(StreamWriter sw)
        {
            sw.WriteLine("{0,2}   {1,-4}   {2}   {3}", Index, Name, BytesPerSamp, Descr);
        }

    } // class ListItemSamp

    class ListItemCh : ListItemBase
    {
        public int Chann { get; set; }
        public string Mask { get; set; }
        internal override void Write(StreamWriter sw)
        {
            sw.WriteLine("{0,2}  {1,2} {2,15} {3,14:X}  {4}",
                 Index, Chann, Name, Mask, Descr);
        }

    } // class ListItemCh

    class ListItemOpt : ListItemBase
    {
        static readonly char[] SeprLine = { '\n' };
        static readonly char[] SeprItem = { '\t' };

        public string Type { get; set; }

        public string DefVal { get; set; }

        internal static bool IsEmptyEx(string str)
        {
            return str == null || str.Length == 0 || str == " ";
        }

        ListItemOpt(int ind, string line)
        {
            string[] items = line.Split(SeprItem, StringSplitOptions.RemoveEmptyEntries);
            if (items.Length < 4)
            {
                throw new ArgumentException("PrivOpts: bad size");
            }
            Index = ind; //$"{ind,3}";
            Name = items[0];
            Type = items[1];
            DefVal = items[2];
            Descr = items[3];
        }

        internal static ListItemOpt[] GetList(string privOpts, ref string prefix)
        {
            string[] lines = privOpts.Split(SeprLine, StringSplitOptions.RemoveEmptyEntries);
            int n = lines.Length;
            if (n > 0)
            {
                prefix = lines[0];
                int n1 = n - 1;
                var ret = new ListItemOpt[n1];
                for (int i = 0; i < n1; ++i)
                {
                    ret[i] = new ListItemOpt(i, lines[i + 1]);
                }
                return ret;
            }
            else
            {
                return new ListItemOpt[0];
            }
        }

        internal override void Write(StreamWriter sw)
        {
            sw.WriteLine("{0,5} {1,30} {2,15} {3,10}   {4}", Index, Name, Type, DefVal, Descr);
        }

    } // class ListItemOpt

} // namespace Explorer
