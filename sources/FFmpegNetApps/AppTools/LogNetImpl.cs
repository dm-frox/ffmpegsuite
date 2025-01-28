using System;

using FFmpegNetAux;
using CsLogger;

namespace AppTools
{
    public class LogNetImpl : ILogNet
    {
        static LogNetImpl LogNet_;

        static LogNetImpl()
        {
            LogNet_ = new LogNetImpl();
        }

        public static ILogNet Default
        {
            get { return LogNet_ as ILogNet; }
        }

        bool ILogNet.Open(string logFile, string cfgFile)
        {
            return SLog.Open(logFile, cfgFile);
        }
        void ILogNet.Close()
        {
            SLog.Close();
        }

        void ILogNet.SkipLine()
        {
            SLog.SkipLine();
        }

        void ILogNet.Flush()
        {
            SLog.Flush();
        }

        void ILogNet.PrintExc(string msg, Exception exc)
        {
            SLog.Exc(msg, exc);
        }

        void ILogNet.PrintError(string msg)
        {
            SLog.E(msg);
        }

        void ILogNet.PrintWarning(string msg)
        {
            SLog.W(msg);
        }

        void ILogNet.PrintInfo(string msg)
        {
            SLog.I(msg);
        }

        void ILogNet.PrintDebug(string msg)
        {
            SLog.D(msg);
        }

        void ILogNet.PrintTrace(string msg)
        {
            SLog.D(msg);
        }

    } // class LogNet

} // namespace AppTools

