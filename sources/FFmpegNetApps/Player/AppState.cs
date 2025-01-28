using System;
using System.ComponentModel;


namespace Player
{
    class AppState : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;


        bool m_Unloaded;
        bool m_IsFile;

        public AppState()
        {
            m_Unloaded = true;
        }

        public bool Unloaded
        {
            get { return m_Unloaded; }
            set
            {
                if (value != m_Unloaded)
                {
                    m_Unloaded = value;
                    OnPropertyChanged("Unloaded");
                    OnPropertyChanged("UnloadedFile");
                }
            }
        }

        public bool IsFile
        {
            get { return m_IsFile; }
            set
            {
                if (value != m_IsFile)
                {
                    m_IsFile = value;
                    OnPropertyChanged("IsFile");
                    OnPropertyChanged("UnloadedFile");
                }
            }
        }

        public bool UnloadedFile
        {
            get { return m_Unloaded && m_IsFile; }
        }

        void OnPropertyChanged(string propName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propName));
            }
        }

    } // class AppState

} // namespace Player
