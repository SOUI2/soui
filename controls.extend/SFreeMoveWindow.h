#pragma once

namespace SOUI
{
    class SFreeMoveWindow : public SWindow
    {
    SOUI_CLASS_NAME(SFreeMoveWindow,L"freeMoveWindow")
    public:
        SFreeMoveWindow(void);
        ~SFreeMoveWindow(void);
        
    protected:
        virtual void OnUpdateFloatPosition(const CRect & rcParent);
    protected:
		void OnNcLButtonDown(UINT nFlags, CPoint pt);
		void OnNcLButtonUp(UINT nFlags, CPoint pt);
		void OnNcMouseMove(UINT nFlags, CPoint pt);
		void OnNcPaint(IRenderTarget * pRT);
		DWORD HitTest(CPoint pt);

        SOUI_MSG_MAP_BEGIN()
			MSG_WM_NCLBUTTONDOWN(OnNcLButtonDown)
			MSG_WM_NCLBUTTONUP(OnNcLButtonUp)
			MSG_WM_NCMOUSEMOVE(OnNcMouseMove)
			MSG_WM_NCPAINT_EX(OnNcPaint)
        SOUI_MSG_MAP_END()
        
    protected:
        enum {
        HALIGN_LEFT = 0x01,
        HALIGN_RIGHT = 0x02,
        VALIGN_TOP   = 0x10,
        VALIGN_BOTTOM = 0x20,
        };
		enum {
			HT_CAPTION=0,
			HT_LEFT=1,
			HT_RIGHT = 2,
			HT_TOP=1<<16,
			HT_BOTTOM=2<<16,
		};
        SOUI_ATTRS_BEGIN()
            ATTR_INT(L"vAlign",m_nFloatVAlign,FALSE)
            ATTR_INT(L"hAlign",m_nFloatHAlign,FALSE)
            ATTR_INT(L"distX",m_nDistX,FALSE)
            ATTR_INT(L"distY",m_nDistY,FALSE)
			ATTR_BOOL(L"resizable", m_bResizable,FALSE)
			ATTR_INT(L"captionHeight",m_nCaptionHeight,TRUE)
			ATTR_SKIN(L"captionSkin",m_pCaptionSkin,TRUE)
        SOUI_ATTRS_END()
        
        int m_nFloatVAlign;
        int m_nFloatHAlign;
        int m_nDistX,m_nDistY;
        
		int m_nCaptionHeight;

		BOOL m_bResizable;

        BOOL m_bDraging;
        CPoint m_ptClick;
		CRect  m_rcClickWndPos;

		DWORD m_dwHit;
		CAutoRefPtr<ISkinObj> m_pCaptionSkin;
    };

}
