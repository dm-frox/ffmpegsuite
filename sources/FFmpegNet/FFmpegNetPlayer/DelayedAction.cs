// ---------------------------------------------------------------------
// File: DelayedAction.cs
// Classes: DelayedActionBase DelayedAction1, DelayedAction (generic)
// Purpose: helpers for MediaPlayerNet
// Module: FFmpegNetPlayer.dll - .NET assembly which provides media player functionality
// Copyright (c) 2023 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2023
// ---------------------------------------------------------------------

using FFmpegNetAux;
using System;
using System.Windows.Threading;

namespace FFmpegNetPlayer
{
    public abstract class DelayedActionBase
    {
        DispatcherTimer m_Timer;

        protected DelayedActionBase(int intervalMs)
        {
            m_Timer = new DispatcherTimer();
            SetInterval(intervalMs);
            m_Timer.Tick += delegate { TickAction(); };
        }

        void TickAction()
        {
            if (!CheckState())
            {
                m_Timer.IsEnabled = false;
                DoAction();
            }
        }

        protected void SetInterval(int intervalMs)
        {
            if (intervalMs > 0)
            { 
                m_Timer.Interval = TimeSpan.FromMilliseconds(intervalMs);
            }
        }

        protected void Begin() => m_Timer.IsEnabled = true;

        public bool IsEnabled => m_Timer.IsEnabled;

        protected virtual bool CheckState() => false; // one tick

        protected abstract void DoAction();

        protected void CheckDelayedActionInProgress()
        {
            if (m_Timer.IsEnabled)
            {
                throw new Exception("DelayedActionBase: delayed action in progress");
            }
        }

        protected void CheckInterval()
        {
            if (m_Timer.Interval.Ticks <= 0)
            {
                throw new Exception("DelayedActionBase: timer interval is not set");
            }
        }

    } // class DelayedActionBase

    public class DelayedAction1 : DelayedActionBase
    {
        Action m_Action;

        public DelayedAction1(int intervalMs = 0)
            : base(intervalMs)
        {}

        public void Begin(Action act)
        {
            CheckDelayedActionInProgress();
            CheckInterval();
            m_Action = act;
            base.Begin();
        }

        public void Begin(Action act, int intervalMs)
        {
            CheckDelayedActionInProgress();
            SetInterval(intervalMs);
            CheckInterval();
            m_Action = act;
            base.Begin();
        }

        protected override void DoAction() => m_Action();

    } // class DelayedAction1

    public class DelayedAction<T> : DelayedActionBase
    {
        T m_Param;

        Func<bool> m_StatePredicate;
        Action<T>  m_Action;

        public DelayedAction(int intervalMs)
            : base(intervalMs)
        {}

        public void Begin(T param, Func<bool> stp, Action<T> act)
        {
            CheckDelayedActionInProgress();
            CheckInterval();
            m_Param = param;
            m_StatePredicate = stp;
            m_Action = act;
            base.Begin();
        }

        public T Param => m_Param;

        protected override bool CheckState() => m_StatePredicate();

        protected override void DoAction() => m_Action(m_Param);

    } // class DelayedAction<>

} // namespace FFmpegNetPlayer
