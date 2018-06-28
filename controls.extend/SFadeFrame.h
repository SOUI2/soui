#pragma once

#include "SAnimator.h"

namespace SOUI
{
    class SFadeFrame : public SWindow, public SAnimator, public ITimelineHandler
    {
        SOUI_CLASS_NAME(SFadeFrame,L"fadeframe")
    public:
        SFadeFrame(void);
        ~SFadeFrame(void);

    protected:        
        void SetChildrenAlpha(BYTE byAlpha);
        
        virtual void OnAnimatorState(int percent);
        virtual void OnNextFrame();

        SOUI_ATTRS_BEGIN()
            ATTR_UINT(L"AniTime",m_nAniTime,FALSE)
            ATTR_UINT(L"DelayTime",m_nDelayTime,FALSE)
            ATTR_STRINGW(L"targetName",m_strTargetName,FALSE)
            ATTR_UINT(L"showAlpha",m_byAlphaShow,FALSE)
            ATTR_INT(L"initShow",m_bInitShow,FALSE)
        SOUI_ATTRS_END()

        void OnMouseHover(WPARAM wParam, CPoint ptPos);
        void OnMouseLeave();
        void OnTimer(char cTimerID);
        int OnCreate(LPVOID);

        SOUI_MSG_MAP_BEGIN()
            MSG_WM_MOUSEHOVER(OnMouseHover)
            MSG_WM_MOUSELEAVE(OnMouseLeave)
            MSG_WM_TIMER_EX(OnTimer)
            MSG_WM_CREATE(OnCreate)
        SOUI_MSG_MAP_END()
    protected:
        DWORD  m_nAniTime;
        DWORD  m_nDelayTime;
        
        SStringW m_strTargetName;
        int     m_nAlphaBegin,m_nAlphaEnd;
        BYTE    m_byAlphaCurrent;
        BYTE    m_byAlphaShow;
        BOOL    m_bInitShow;
    };

}

