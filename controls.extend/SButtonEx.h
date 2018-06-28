#pragma once

namespace SOUI
{
    class SButtonEx : public SButton
    {
    SOUI_CLASS_NAME(SButtonEx,L"buttonex")
    
    enum{
    FREE_DRAW,
    VERT_ICON_TEXT,
    VERT_TEXT_ICON,
    HORZ_ICON_TEXT,
    HORZ_TEXT_ICON,
    };
    
    public:
        SButtonEx(void);
        ~SButtonEx(void);
        
        HICON SetIcon(HICON hIcon,int nIconSize);
        void  SetIconVisible(bool bVisible);
    public:
        SOUI_ATTRS_BEGIN()
            ATTR_POINT(L"pushOffset",m_ptPushOffet,TRUE)//下压状态整体偏移
            ATTR_SKIN(L"icon",m_pIcon,TRUE) //图标SKIN
            ATTR_INT(L"iconIndex",m_iIcon,TRUE)//图标在Skin中的索引
            ATTR_ENUM_BEGIN(L"drawMode",int,TRUE)
                ATTR_ENUM_VALUE(L"freeDraw",FREE_DRAW)
                ATTR_ENUM_VALUE(L"vertIconText",VERT_ICON_TEXT)
                ATTR_ENUM_VALUE(L"vertTextIcon",VERT_TEXT_ICON)
                ATTR_ENUM_VALUE(L"horzIconText",HORZ_ICON_TEXT)
                ATTR_ENUM_VALUE(L"horzTextIcon",HORZ_TEXT_ICON)
            ATTR_ENUM_END(m_drawMode)
            ATTR_INT(L"sepSize",m_nSepSize,TRUE)    //FREE_DRAW时无效
            ATTR_POINT(L"iconPos",m_ptIcon,TRUE)//图标显示位置,FREE_DRAW时有效
            ATTR_POINT(L"textPos",m_ptText,TRUE)//文本显示位置,FREE_DRAW时有效
        SOUI_ATTRS_END()
        
    protected:
        virtual CSize GetDesiredSize(LPCRECT pRcContainer);
        
        CSize GetDesiredSize(IRenderTarget *pRT,LPCRECT pRcContainer);
        CSize GetIconSize();
        void DrawIcon(IRenderTarget *pRT,CRect rcIcon);
    protected:
        void OnPaint(IRenderTarget *pRT);
        
        SOUI_MSG_MAP_BEGIN()
            MSG_WM_PAINT_EX(OnPaint)
        SOUI_MSG_MAP_END()
        
    protected:
        HICON       m_hIcon;
        int         m_nIconSize;
        
        ISkinObj * m_pIcon;
        int        m_iIcon;//-1代表m_pIcon是一个独占的皮肤
        CPoint     m_ptIcon;
        CPoint     m_ptText;
        CPoint     m_ptPushOffet;
        int        m_nSepSize;
        int         m_drawMode;
        bool        m_bIconVisible;
    };

}
