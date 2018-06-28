/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       SwndStyle.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/02
* 
* Describe    SOUI窗口风格管理
*/
#pragma once

#include "res.mgr/SDpiAwareFont.h"

namespace SOUI
{

	
class SWindow;
class SOUI_EXP SwndStyle: public SObject
{
    SOUI_CLASS_NAME(SwndStyle, L"style")

    enum
    {
        Align_Left               = 0x000UL, // valign = top
        Align_Center             = 0x100UL, // valign = middle
        Align_Right              = 0x200UL, // valign = bottom

        VAlign_Top               = 0x0000UL, // valign = top
        VAlign_Middle            = 0x1000UL, // valign = middle
        VAlign_Bottom            = 0x2000UL, // valign = bottom
    };
public:
    SwndStyle();


    COLORREF m_crBg;                /**<背景颜色 */
    COLORREF m_crBorder;            /**<边框颜色 */
    
    SStringT m_strCursor;           /**<光标NAME */

    BYTE     m_byAlpha;             /**<窗口透明度 */
    BYTE     m_bySepSpace;          /**<子窗口水平间隔 */
    DWORD    m_bDotted:1;           /**<支持省略号显示文本 */
    DWORD    m_bTrackMouseEvent:1;  /**<监视鼠标进入及移出消息 */
    DWORD    m_bBkgndBlend:1;       /**<渲染窗口内容和背景混合标志 */

    UINT GetTextAlign();
    int GetStates();
    COLORREF GetTextColor(int iState);
    IFontPtr GetTextFont(int iState);
	CRect GetMargin() const;
	CRect GetPadding() const;
    void SetTextColor(int iState,COLORREF cr){m_crText[iState]=cr;}

	void SetScale(int nScale);
protected:
	SLayoutSize    m_rcMargin[4];   /**< 4周非客户区大小 */
	SLayoutSize    m_rcInset[4];    /**< 文字区4个方向的内边距 */

    UINT m_nTextAlign;      /**<文本对齐 */
    UINT m_uAlign,m_uVAlign;/**<水平及垂直对齐 */
    COLORREF m_crText[4];   /**<文字4种状态下的颜色 */
    SDpiAwareFont m_ftText[4];  /**<文字4种状态下的字体 */

	int GetScale() const {return m_nScale;}

	int		m_nScale;

	HRESULT OnAttrPadding(const SStringW & strValue,BOOL bLoading);
    HRESULT OnAttrMargin(const SStringW &strValue,BOOL bLoading);
    HRESULT OnAttrMarginX(const SStringW &strValue,BOOL bLoading);
    HRESULT OnAttrMarginY(const SStringW &strValue,BOOL bLoading);
    
	void _ParseLayoutSize4(const SStringW & strValue, SLayoutSize layoutSizes[]);
    SOUI_ATTRS_BEGIN()
        ATTR_HEX(L"textMode", m_nTextAlign, TRUE)

        ATTR_ENUM_BEGIN(L"align", UINT, TRUE)
            ATTR_ENUM_VALUE(L"left", Align_Left)
            ATTR_ENUM_VALUE(L"center", Align_Center)
            ATTR_ENUM_VALUE(L"right", Align_Right)
        ATTR_ENUM_END(m_uAlign)
        ATTR_ENUM_BEGIN(L"valign", UINT, TRUE)
            ATTR_ENUM_VALUE(L"top", VAlign_Top)
            ATTR_ENUM_VALUE(L"middle", VAlign_Middle)
            ATTR_ENUM_VALUE(L"bottom", VAlign_Bottom)
        ATTR_ENUM_END(m_uVAlign)

        ATTR_COLOR(L"colorBkgnd", m_crBg, TRUE)
        ATTR_COLOR(L"colorBorder", m_crBorder, TRUE)

        ATTR_FONT(L"font", m_ftText[0], TRUE)
        ATTR_FONT(L"fontHover", m_ftText[1], TRUE)
        ATTR_FONT(L"fontPush", m_ftText[2], TRUE)
        ATTR_FONT(L"fontDisable", m_ftText[3], TRUE)

        ATTR_COLOR(L"colorText", m_crText[0], TRUE)
        ATTR_COLOR(L"colorTextHover", m_crText[1], TRUE)
        ATTR_COLOR(L"colorTextPush", m_crText[2], TRUE)
        ATTR_COLOR(L"colorTextDisable", m_crText[3], TRUE)

        ATTR_CUSTOM(L"margin-x", OnAttrMarginX)
        ATTR_CUSTOM(L"margin-y", OnAttrMarginY)
        ATTR_CUSTOM(L"margin", OnAttrMargin)
        ATTR_CUSTOM(L"inset",OnAttrPadding)
        ATTR_CUSTOM(L"padding",OnAttrPadding)
        ATTR_STRINGT(L"cursor",m_strCursor,FALSE)
        ATTR_INT(L"dotted",m_bDotted,FALSE)
        ATTR_INT(L"trackMouseEvent",m_bTrackMouseEvent,FALSE)
        ATTR_INT(L"alpha",m_byAlpha,TRUE)
        ATTR_INT(L"bkgndBlend",m_bBkgndBlend,TRUE)
        ATTR_INT(L"sepSpace",m_bySepSpace,TRUE)
    SOUI_ATTRS_BREAK()      //属性不交给SObject处理
};


}//namespace SOUI