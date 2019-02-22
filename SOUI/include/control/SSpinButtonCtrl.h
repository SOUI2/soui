#pragma once
#include "core/SWnd.h"

namespace SOUI
{
    class SOUI_EXP SSpinButtonCtrl : public SWindow
    {
    SOUI_CLASS_NAME(SSpinButtonCtrl,L"spinButton")
    public:
        SSpinButtonCtrl(void);
        ~SSpinButtonCtrl(void);
        
		void SetValue(int nValue);
		void SetRange(int nMin, int nMax);
		void SetStep(UINT nStep);

		SWindow * GetBuddy();

    protected:
        virtual CSize GetDesiredSize(LPCRECT pRcContainer);
		virtual BOOL NeedRedrawWhenStateChange() { return TRUE; }
        virtual void OnColorize(COLORREF cr);

    protected:
        void OnValueChanged(bool bInit=false);
    
		void OnLButtonDown(UINT nFlags, CPoint point);
		void OnLButtonUp(UINT nFlags, CPoint point);
		void OnMouseMove(UINT nFlags, CPoint point);
		void OnPaint(IRenderTarget *pRT);
		void OnTimer(char cTimerId);
		int OnCreate(void *);
		void OnClick();
		SOUI_MSG_MAP_BEGIN()
			MSG_WM_CREATE(OnCreate)
			MSG_WM_LBUTTONDOWN(OnLButtonDown)
			MSG_WM_LBUTTONDBLCLK(OnLButtonDown)
			MSG_WM_MOUSEMOVE(OnMouseMove)
			MSG_WM_LBUTTONUP(OnLButtonUp)
			MSG_WM_PAINT_EX(OnPaint)
			MSG_WM_TIMER_EX(OnTimer)
		SOUI_MSG_MAP_END()

    protected:
        HRESULT OnAttrValue(const SStringW& strValue, BOOL bLoading);
        
		SOUI_ATTRS_BEGIN()
			ATTR_INT(L"max", m_nMax, FALSE)
			ATTR_INT(L"min", m_nMin, FALSE)
			ATTR_CUSTOM(L"value", OnAttrValue)
			ATTR_UINT(L"step", m_uStep, FALSE)
			ATTR_INT(L"circle", m_bCircle, FALSE)
			ATTR_STRINGW(L"buddy", m_strBuddy, FALSE)
			ATTR_SKIN(L"upskin", m_pUpSkin, TRUE)
			ATTR_SKIN(L"downSkin",m_pDownSkin,TRUE)
        SOUI_ATTRS_END()
        
	protected:
        int m_nMax;
        int m_nMin;
        int m_nValue;
        UINT m_uStep;
        BOOL m_bCircle;
        SStringW m_strBuddy;
        
		CAutoRefPtr<ISkinObj> m_pUpSkin;
		CAutoRefPtr<ISkinObj> m_pDownSkin;

		enum ActionButton{
			ACTION_NULL=-1,
			ACTION_UP,
			ACTION_DOWN,
		};
		ActionButton			  m_iActionBtn;
    };
}
