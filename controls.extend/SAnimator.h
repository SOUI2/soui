#pragma once

namespace SOUI
{
    class SAnimator
    {
    public:
        enum {
            ABORT   = -1,
            PER_START = 0,
            PER_END = 100,
        };

        typedef void (*AnimatorStateHandler)(int percent,LPARAM lp);

        SAnimator():m_uDuration(0),m_funAnimatorStateHandler(NULL),m_lpUserdata(0){}

        virtual ~SAnimator(){}

        void Start(UINT uDuration)
        {
            m_dwStart = GetTickCount();
            m_uDuration = uDuration;
            OnAnimatorState(PER_START);
        }
        
        void Stop()
        {
            if(!IsBusy()) return;
            m_uDuration =0;
            OnAnimatorState(ABORT);
        }

        void SetAnimatorHandler(AnimatorStateHandler funHandler,LPARAM lpUserData)
        {
            m_funAnimatorStateHandler = funHandler;
            m_lpUserdata =lpUserData;
        }

        BOOL IsBusy(){return m_uDuration != 0;}

        BOOL Update()
        {
            if(m_uDuration == 0) return FALSE;

            DWORD dwElapse = GetTickCount() - m_dwStart;
            if(dwElapse >= m_uDuration)
            {
                OnAnimatorState(100);
                m_uDuration = 0;
            }else
            {
                OnAnimatorState(dwElapse*100/m_uDuration);
            }
            return TRUE;
        }

        //¶¯»­×´Ì¬¸Ä±ä£¬percent in [0,100]
        virtual void OnAnimatorState(int percent)
        {
            if(m_funAnimatorStateHandler) m_funAnimatorStateHandler(percent,m_lpUserdata);
        }
    private:
        DWORD m_dwStart;
        DWORD m_uDuration;
        LPARAM m_lpUserdata;
        AnimatorStateHandler  m_funAnimatorStateHandler;
    };
}