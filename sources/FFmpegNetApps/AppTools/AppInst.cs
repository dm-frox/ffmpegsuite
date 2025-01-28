using System;
using System.Threading;

namespace AppTools
{
    public class AppInst
    {
        const string EventName = @"_Inst_Event_9A89D711_DDE1_41E7_9D16_1B0C2926B5AD";

        readonly bool   m_FirstInst;
        EventWaitHandle m_OneInstEvent;

        public AppInst(string appPrefix)
        {
            string eventName = appPrefix + EventName;
            m_OneInstEvent = new EventWaitHandle(false, EventResetMode.AutoReset, eventName, out m_FirstInst);
        }

        public void Close() => m_OneInstEvent?.Close();

        public bool IsFirstAppInst => m_FirstInst;

    } // class AppInst

} // namespace AppTools
