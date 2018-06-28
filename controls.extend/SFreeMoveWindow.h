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
        void OnLButtonDown(UINT nFlags,CPoint pt);

        void OnLButtonUp(UINT nFlags,CPoint pt);

        void OnMouseMove(UINT nFlags,CPoint pt);

        SOUI_MSG_MAP_BEGIN()
            MSG_WM_LBUTTONDOWN(OnLButtonDown)
            MSG_WM_LBUTTONUP(OnLButtonUp)
            MSG_WM_MOUSEMOVE(OnMouseMove)
        SOUI_MSG_MAP_END()
        
    protected:
        enum {
        HALIGN_LEFT = 0x01,
        HALIGN_RIGHT = 0x02,
        VALIGN_TOP   = 0x10,
        VALIGN_BOTTOM = 0x20,
        };
        
        SOUI_ATTRS_BEGIN()
            ATTR_INT(L"vAlign",m_nFloatVAlign,FALSE)
            ATTR_INT(L"hAlign",m_nFloatHAlign,FALSE)
            ATTR_INT(L"distX",m_nDistX,FALSE)
            ATTR_INT(L"distY",m_nDistY,FALSE)
        SOUI_ATTRS_END()
        
        int m_nFloatVAlign;
        int m_nFloatHAlign;
        int m_nDistX,m_nDistY;
        
        BOOL m_bDraging;
        CPoint m_ptClick;
    };

}
