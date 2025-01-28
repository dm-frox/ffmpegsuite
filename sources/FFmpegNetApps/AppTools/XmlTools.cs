using System;
using System.Xml;
using System.Globalization;

namespace AppTools
{
    public class PrmBase
    {
        public readonly string DisplayName;

        protected PrmBase(string displayName)
        {
            DisplayName = displayName;
        }

        protected PrmBase()
        {
            DisplayName = string.Empty;
        }

        public override string ToString()
        {
            return DisplayName;
        }

        protected static string[] GetKeyValList(XmlNode node)
        {
            string[] opts = null;
            var subnodes = node.ChildNodes;
            int m = subnodes.Count;
            if (m > 0)
            {
                opts = new string[2 * m];
                for (int j = 0, jj = 0; j < m; ++j)
                {
                    var optattr = subnodes[j].Attributes;
                    opts[jj++] = optattr["Key"].Value;
                    opts[jj++] = optattr["Value"].Value;
                }
            }
            return opts;
        }

        internal static string GetAttrStr(XmlAttributeCollection attrs, string name, string defVal = null)
        {
            string ret = string.Empty;
            try
            {
                var attr = attrs[name];
                if (attr != null)
                {
                    ret = attr.Value;
                }
            }
            catch
            {
            }
            if (ret.Length == 0 && !string.IsNullOrEmpty(defVal))
            {
                ret = defVal;
            }
            return ret;
        }

        internal static string GetDisplayName(XmlAttributeCollection attr)
        {
            return GetAttrStr(attr, "DisplayName", "Undefined");
        }

        protected static int GetAttrInt(XmlAttributeCollection attrs, string name, int defVal = 0)
        {
            int ret = defVal;
            try
            {
                var attr = attrs[name];
                if (attr != null)
                {
                    if (int.TryParse(attr.Value, out int val))
                    {
                        ret = val;
                    }
                }
            }
            catch 
            {
            }
            return ret;
        }

        protected static long GetAttrLong(XmlAttributeCollection attrs, string name, long defVal = 0)
        {
            long ret = defVal;
            try
            {
                var attr = attrs[name];
                if (attr != null)
                {
                    if (long.TryParse(attr.Value, out long val))
                    {
                        ret = val;
                    }
                }
            }
            catch
            {
            }
            return ret;
        }

        protected static double GetAttrDbl(XmlAttributeCollection attrs, string name, double defVal = 0.0)
        {
            double ret = defVal;
            try
            {
                var attr = attrs[name];
                if (attr != null)
                {
                    if (double.TryParse(attr.Value, NumberStyles.Float, CultureInfo.InvariantCulture, out double val))
                    {
                        ret = val;
                    }
                }
            }
            catch
            {
            }
            return ret;
        }

        protected static XmlNodeList GetNodes(string path)
        {
            try
            {
                var doc = new XmlDocument();
                doc.Load(path);
                return doc.DocumentElement.ChildNodes;
            }
            catch
            {
                return null;
            }
        }

        protected static int Count(XmlNodeList nodes)
        {
            return nodes != null ? nodes.Count : 0;
        }

        protected static string GetNodeText(XmlNode node)
        {
            return node.InnerText.Trim();
        }

    } // class PrmBase


    public class PrmNmOpts : PrmBase
    {
        public readonly string Name;
        public readonly string Format;
        public readonly string[] Options;

        public PrmNmOpts(string displayName, string name, string format, string[] opts)
            : base(displayName)
        {
            Name = name;
            Format = format;
            if (opts != null && opts.Length > 0)
            {
                if ((opts.Length & 1) != 0)
                {
                    throw new Exception("FormatItem.SetOptions, options list length must be even");
                }
                Options = opts;
            }
        }

        public bool HasOptions => Options != null && Options.Length > 1;

        public static PrmNmOpts[] Read(string path)
        {
            var nodes = GetNodes(path);
            int n = Count(nodes);
            var items = new PrmNmOpts[n];
            for (int i = 0; i < n; ++i)
            {
                var node = nodes[i];
                var attr = node.Attributes;
                var opts = GetKeyValList(node);
                items[i] = new PrmNmOpts(
                    GetDisplayName(attr),
                    GetAttrStr(attr, "Name"),
                    GetAttrStr(attr, "Format"),
                    opts
                    );
            }
            return items;
        }

    } // class PrmNmOpts

    public class PrmFilter : PrmBase
    {
        public readonly string Filter;
        public readonly int Commands;
        public readonly int FpsFactor;
        public readonly int Seq;

        public PrmFilter(string displayName, string filter, int comm, int fpsFactor, int seq)
            : base(displayName)
        {
            Filter = filter;
            Commands = comm;
            FpsFactor = fpsFactor;
            Seq = seq;
    }

        public static PrmFilter[] Read(string path)
        {
            var nodes = GetNodes(path);
            int n = Count(nodes);
            var items = new PrmFilter[n];
            for (int i = 0; i < n; ++i)
            {
                var node = nodes[i];
                var attr = node.Attributes;
                items[i] = new PrmFilter(
                    GetDisplayName(attr),
                    GetNodeText(node),
                    GetAttrInt(attr, "Commands"),
                    GetAttrInt(attr, "Fps"),
                    GetAttrInt(attr, "Seq")
                    );
            }
            return items;
        }

    } // class PrmFilter

    public class PrmLog : PrmBase
    {
        const int DefValue = 3;

        public static bool Read(string path, out int wrapper, out int ffmpeg, out int options)
        {
            bool ret = false;
            try
            {
                var doc = new XmlDocument();
                doc.Load(path);
                var attr = doc.DocumentElement.Attributes;
                wrapper = GetAttrInt(attr, "wrapper", DefValue);
                ffmpeg  = GetAttrInt(attr, "ffmpeg",  DefValue);
                options = GetAttrInt(attr, "options");
                ret = true;
            }
            catch
            {
                wrapper = DefValue;
                ffmpeg = DefValue;
                options = 0;
            }
            return ret;
        }

    } //  class PrmLog

} // namespace AppTools

