#pragma once

#include "interface/stooltip-i.h"
#include "core/SimpleWnd.h"

namespace SOUI
{
    class STipCtrl : public IToolTip
                   , public CSimpleWnd 
    {
    public:
        STipCtrl(void);
        virtual ~STipCtrl(void);

        BOOL Create();

    public://IToolTip
        /*virtual*/ void RelayEvent(const MSG *pMsg);

        /*virtual*/ void UpdateTip(const TIPID &id, CRect rc,LPCTSTR pszTip,int nScale);

        /*virtual*/ void ClearTip();

        /*virtual*/ BOOL PreTranslateMessage(MSG* pMsg);
    protected:
        virtual void OnFinalMessage(HWND hWnd);

        void OnTimer(UINT_PTR idEvent);
        void OnPaint(HDC dc);
        void ShowTip(BOOL bShow);

        BEGIN_MSG_MAP_EX(STipCtrl)
            MSG_WM_PAINT(OnPaint)
            MSG_WM_TIMER(OnTimer)
            REFLECT_NOTIFICATIONS_EX()
        END_MSG_MAP()

    protected:
        int            m_nDelay;
        int            m_nShowSpan;
        SStringT       m_strTip;
        int            m_nScale;
        CRect          m_rcTarget;
        HFONT          m_font;

        TIPID          m_id;
    };
}

