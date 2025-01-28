using System;
using System.Text;
using System.Xml;

namespace CsLogger
{
    static class ConfigFile
    {
        internal delegate void ParseParam(string name, string value);

        internal static bool GetParams(string file, string rootName, string nodeName, ParseParam parser)
        {
            bool ret = false;
            try
            {
                XmlNode root = GetRoot(file, rootName);
                ret = (GetParams(root, nodeName, parser) >= 0);
            }
            catch
            {
            }
            return ret;
        }

        internal static bool ParseBool(string sval, ref bool val)
        {
            bool ret = !string.IsNullOrEmpty(sval);
            if (ret)
            {
                switch (sval[0])
                {
                case '1':
                case 't':
                case 'T':
                    val = true;
                    break;
                case '0':
                case 'f':
                case 'F':
                    val = false;
                    break;
                default:
                    ret = false;
                    break;
                }
            }
            return ret;
        }

        internal static bool ParseInt(string sval, ref int val)
        {
            bool ret = false;
            if (int.TryParse(sval, out int ival))
            {
                val = ival;
                ret = true;
            }
            return ret;
        }

        internal static void ParseFlags(string sval, ref long val)
        {
            int n = (sval != null) ? sval.Length : 0;
            n = Math.Min(n, sizeof(long) * 8);
            long flags = 0;
            for (int i = n - 1, k = 0; i >= 0; --i)
            {
                char c = sval[i];
                if (c == '1')
                    flags += (long)(1 << k);
                else if (c == '0')
                    ++k;
            }
            val = flags;
        }

        internal static void ParseCodePage(string sval, ref Encoding val)
        {
            if (int.TryParse(sval, out int ival))
                val = Encoding.GetEncoding(ival);
            else if (string.Compare(sval, "unicode", true) == 0)
                val = Encoding.Unicode;
        }

        static XmlNode GetRoot(string file, string rootName)
        {
            XmlNode ret = null;
            using (var xtr = new XmlTextReader(file))
            {
                var xdc = new XmlDocument();
                xtr.WhitespaceHandling = WhitespaceHandling.None;
                xdc.Load(xtr);
                XmlNode root = xdc.DocumentElement;
                if (CheckNode(root, rootName) && root.HasChildNodes)
                    ret = root;
            }
            return ret;
        }

        static int GetParams(XmlNode root, string nodeName, ParseParam parser)
        {
            int ret = -1;
            XmlNode pars = root.FirstChild;
            if (CheckNode(pars, nodeName))
            {
                ret = 0;
                foreach (XmlNode att in pars.Attributes)
                {
                    parser(att.Name, att.Value);
                    ++ret;
                }
            }
            return ret;
        }

        static bool CheckNode(XmlNode node, string name)
        {
            return ((node.NodeType == XmlNodeType.Element) && (node.Name == name));
        }

    } // class ConfigFile

} // namespace CsLogger