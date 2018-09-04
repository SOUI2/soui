/**
 * Copyright (C) 2014-2050 SOUI团队
 * All rights reserved.
 * 
 * @file       SListbox.h
 * @brief      
 * @version    v1.0      
 * @author     soui      
 * @date       2014-07-06
 * 
 * Describe     
 */
#pragma  once
#include "core/SPanel.h"

namespace SOUI
{
/** 
 * @struct    tagLBITEM
 * @brief     LBitem
 *
 * @Describe  LBitem结构体
 */
typedef struct tagLBITEM
{
    STrText     strText;  /**< 文本 */
    int         nImage;   /**< 图标 */
    LPARAM      lParam;   /**< 附加参数 */

	tagLBITEM(ITrCtxProvider * pTrCtx):strText(pTrCtx)
    {
        nImage = -1;
        lParam = NULL;
    }

} LBITEM, *LPLBITEM;

/** 
 * @class     SListBox
 * @brief     列表框
 *
 * Describe   列表框
 */
class SOUI_EXP SListBox :public SScrollView
{
public:

    SOUI_CLASS_NAME(SListBox, L"listbox")
    /**
     * SListBox::SListBox
     * @brief    构造函数
     *
     * Describe  构造函数  
     */
    SListBox();

    /**
     * SListBox::~SListBox
     * @brief    析构函数
     *
     * Describe  析构函数  
     */
    virtual ~SListBox();

    /**
     * SListBox::GetCount
     * @brief    获取项个数
     * @return   返回int
     *
     * Describe  获取项个数
     */
    int GetCount() const;

    /**
     * SListBox::GetCurSel
     * @brief    获取当前选中项索引
     * @return   返回int
     *
     * Describe  获取当前选中项索引
     */
    int GetCurSel() const;

    /**
     * SListBox::SetCurSel
     * @brief    设置选中项
     * @param    int nIndex -- 待选中索引
     *
     * Describe  设置选中项
     */
    BOOL SetCurSel(int nIndex);

    /**
     * SListBox::GetTopIndex
     * @brief    获取第一条可见记录索引
     * @return   返回int
     *
     * Describe  
     */
    int GetTopIndex() const;

    /**
     * SListBox::SetTopIndex
     * @brief    设置第一条可见记录索引
     * @param    int nIndex -- 第一条可见记录索引
     *
     * Describe  
     */
    BOOL SetTopIndex(int nIndex);

    /**
     * SListBox::GetItemHeight
     * @brief    获取高度
     * @return   返回int 
     *
     * Describe  获取高度
     */
    int GetItemHeight() const
    {
        return m_itemHeight.toPixelSize(GetScale());
    }

    /**
     * SListBox::GetItemData
     * @brief    获取附加数据
     * @param    int nIndex -- 选项索引
     * @return   返回LPARAM
     *
     * Describe  获取附加数据
     */
    LPARAM GetItemData(int nIndex) const;

    /**
     * SListBox::SetItemData
     * @brief    设置选项附加数据
     * @param    int nIndex -- 选项索引
     * @param    LPARAM lParam  -- 附加数据
     * @return   返回BOOL
     *
     * Describe  设置选项附加数据
     */
    BOOL SetItemData(int nIndex, LPARAM lParam);

    /**
     * SListBox::GetText
     * @brief    获取指定项文本
     * @param    int nIndex -- 选项索引
	 * @param    BOOL bRawText -- 原始数据标志
     * @return   SStringT,列表项的原始字符串
     *
     * Describe  获取指定项文本
     */
    SStringT GetText(int nIndex,BOOL bRawText=FALSE) const;

    /**
     * SListBox::GetItemHeight
     * @brief    获取指定项高度
     * @param    int nIndex -- 选项索引
     * @return   返回int 
     *
     * Describe  获取指定项高度
     */
    int GetItemHeight(int nIndex) const;

    /**
     * SListBox::SetItemHeight
     * @brief    设置指定项高度
     * @param    int nIndex -- 选项索引
     * @param    int cyItemHeight -- 高度
     * @return   返回int 
     *
     * Describe  设置指定项高度
     */
    BOOL SetItemHeight(int nIndex, int cyItemHeight);

    /**
     * SListBox::DeleteAll
     * @brief    删除所有
     *
     * Describe  删除所有
     */
    void DeleteAll();

    /**
     * SListBox::DeleteString
     * @brief    设置指定项文本
     * @param    int nIndex -- 选项索引
     * @return   返回BOOL
     *
     * Describe  设置指定项文本
     */
    BOOL DeleteString(int nIndex);

    /**
     * SListBox::AddString
     * @brief    添加文本
     * @param    LPCTSTR lpszItem -- 文本
     * @param    int nImage -- 图标
     * @param    LPARAM lParam -- 附加数据
     * @return   返回int 索引
     *
     * Describe  添加文本
     */
    int AddString(LPCTSTR lpszItem, int nImage = -1, LPARAM lParam = 0);

    /**
     * SListBox::InsertString
     * @brief    插入文本
     * @param    int nIndex  -- 索引
     * @param    LPCTSTR lpszItem -- 文本
     * @param    int nImage -- 图标
     * @param    LPARAM lParam -- 附加数据
     * @return   返回int 索引
     *
     * Describe  在某项插入文本
     */
    int InsertString(int nIndex, LPCTSTR lpszItem, int nImage = -1, LPARAM lParam = 0);

    /**
     * SListBox::EnsureVisible
     * @brief    设置某项显示
     * @param    int nIndex  -- 索引
     *
     * Describe  设置某项显示
     */
    void EnsureVisible(int nIndex);

    /**
     * SListBox::RedrawItem
     * @brief    重绘选项
     * @param    int iItem  -- 索引
     *
     * Describe  重绘选项
     */
    void RedrawItem(int iItem);

    /**
     * SListBox::HitTest
     * @brief    获取鼠标相关信息
     * @param    CPoint &pt -- 坐标
     *
     * Describe  自动修改pt的位置为相对当前项的偏移量
     */
    int HitTest(CPoint &pt);

	int FindString(int iFindAfter,LPCTSTR pszText) const;
protected:
	virtual HRESULT OnLanguageChanged();

    /**
     * SListBox::CreateChildren
     * @brief    创建新项
     * @param    pugi::xml_node xmlNode -- xml文件
     *
     * Describe  通过解析xml文件创建
     */
    virtual BOOL CreateChildren(pugi::xml_node xmlNode);

    /**
     * SListBox::LoadItemAttribute
     * @brief    加载属性
     * @param    pugi::xml_node xmlNode -- xml文件
     * @param    LPLBITEM pItem  -- 新项
     *
     * Describe
     */
    void LoadItemAttribute(pugi::xml_node xmlNode, LPLBITEM pItem);

    /**
     * SListBox::InsertItem
     * @brief    插入新项
     * @param    int nIndex -- 索引
     * @param    LPLBITEM pItem -- 待插入新项
     *
     * Describe  在某项之后插入
     */
    int InsertItem(int nIndex, LPLBITEM pItem);

    /**
     * SListBox::DrawItem
     * @brief    绘制
     * @param    IRenderTarget *pRT -- 绘制设备
     * @param    CRect &rc -- 目标区域
     * @param    int iItem -- 选项
     *
     * Describe  绘制
     */
    virtual void DrawItem(IRenderTarget *pRT, CRect &rc, int iItem);

    /**
     * SListBox::NotifySelChange
     * @brief    修改选中项
     * @param    int nOldSel -- 旧选中项
     * @param    int nNewSel -- 新选中项
     *
     * Describe  修改选中项
     */
    void NotifySelChange(int nOldSel,int nNewSel);

    /**
     * SListBox::OnGetDlgCode
     * @brief    获取窗口消息码
     * @param    返回UINT
     *
     * Describe  获取窗口消息码
     */
    UINT OnGetDlgCode();

protected:
    /**
     * SListBox::OnSize
     * @brief    消息响应函数
     * @param    UINT nType --
     * @param    CSize size -- 
     *
     * Describe  获取某项得索引  
     */
    void OnSize(UINT nType,CSize size);

    /**
     * SListBox::OnPaint
     * @brief    绘制
     * @param    IRenderTarget *pRT -- 绘制设备
     *
     * Describe  消息响应函数  
     */
    void OnPaint(IRenderTarget *pRT);

    /**
     * SListBox::OnLButtonDown
     * @brief    左键按下
     * @param    UINT nFlags -- 标志
     * @param    CPoint pt -- 坐标
     *
     * Describe  消息响应函数 
     */
    void OnLButtonDown(UINT nFlags,CPoint pt);

    /**
     * SListBox::OnLButtonDbClick
     * @brief    左键双击
     * @param    UINT nFlags -- 标志
     * @param    CPoint pt -- 坐标
     *
     * Describe  消息响应函数 
     */
    void OnLButtonDbClick(UINT nFlags,CPoint pt);

    /**
     * SListBox::OnLButtonUp
     * @brief    左键抬起
     * @param    UINT nFlags -- 标志
     * @param    CPoint pt -- 坐标
     *
     * Describe  消息响应函数 
     */
    void OnLButtonUp(UINT nFlags,CPoint pt);
    
    /**
     * SListBox::OnMouseMove
     * @brief    鼠标移动
     * @param    UINT nFlags -- 标志
     * @param    CPoint pt -- 坐标
     *
     * Describe  消息响应函数 
     */
    void OnMouseMove(UINT nFlags,CPoint pt);
    
    /**
     * SListBox::OnKeyDown
     * @brief    按键按下
     * @param    TCHAR nChar -- 键码值
     * @param    UINT nRepCnt -- 重复次数
     * @param    UINT nFlags -- 标志
     *
     * Describe  消息响应函数 
     */
    void OnKeyDown( TCHAR nChar, UINT nRepCnt, UINT nFlags );

    /**
     * SListBox::OnChar
     * @brief    按键按下
     * @param    TCHAR nChar -- 键码值
     * @param    UINT nRepCnt -- 重复次数
     * @param    UINT nFlags -- 标志
     *
     * Describe  消息响应函数 
     */
    void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

    /**
     * SListBox::OnDestroy
     * @brief    销毁
     *
     * Describe  销毁
     */
    void OnDestroy();

    /**
     * SListBox::OnChar
     * @brief    按键按下
     * @param    TCHAR nChar -- 键码值
     * @param    UINT nRepCnt -- 重复次数
     * @param    UINT nFlags -- 标志
     *
     * Describe  消息响应函数 
     */
    void OnShowWindow(BOOL bShow, UINT nStatus);


    void OnMouseLeave();
protected:

    SArray<LPLBITEM>    m_arrItems;  /**< 保存item */

    SLayoutSize   m_itemHeight;     /**< item高度 */
    int     m_iSelItem;     /**< 选中item */
    int     m_iHoverItem;   /**< Hover状态的item */
    int     m_iScrollSpeed; /**< 滚动速度 */
    BOOL    m_bHotTrack;    /**<  */

	SLayoutSize m_ptIcon[2]; /**< 图标坐标 */
    SLayoutSize m_ptText[2]; /**< 文本坐标 */

    COLORREF m_crItemBg;    /**< 背景色 */
    COLORREF m_crItemBg2;   /**< 背景色 */
    COLORREF m_crItemSelBg; /**< 选中背景色 */
	COLORREF m_crItemHotBg; /**< Hot背景色 */
    COLORREF m_crText;      /**< 背景色 */
    COLORREF m_crSelText;   /**< 选中背景色 */
    ISkinObj *m_pItemSkin, *m_pIconSkin;

public:

    SOUI_ATTRS_BEGIN()
        ATTR_LAYOUTSIZE(L"itemHeight", m_itemHeight, FALSE)
        ATTR_SKIN(L"itemSkin", m_pItemSkin, TRUE)
        ATTR_SKIN(L"iconSkin", m_pIconSkin, TRUE)
        ATTR_COLOR(L"colorItemBkgnd",m_crItemBg,FALSE)
        ATTR_COLOR(L"colorItemBkgnd2", m_crItemBg2, FALSE)
        ATTR_COLOR(L"colorItemSelBkgnd",m_crItemSelBg,FALSE)
		ATTR_COLOR(L"colorItemHotBkgnd",m_crItemHotBg,FALSE)
        ATTR_COLOR(L"colorText",m_crText,FALSE)
        ATTR_COLOR(L"colorSelText",m_crSelText,FALSE)
        ATTR_LAYOUTSIZE(L"icon-x", m_ptIcon[0], FALSE)
        ATTR_LAYOUTSIZE(L"icon-y", m_ptIcon[1], FALSE)
        ATTR_LAYOUTSIZE(L"text-x", m_ptText[0], FALSE)
        ATTR_LAYOUTSIZE(L"text-y", m_ptText[1], FALSE)
        ATTR_INT(L"hotTrack",m_bHotTrack,FALSE)
    SOUI_ATTRS_END()

    SOUI_MSG_MAP_BEGIN()
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_SIZE(OnSize)
        MSG_WM_PAINT_EX(OnPaint)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONDBLCLK(OnLButtonDbClick)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_MOUSEMOVE(OnMouseMove)
        MSG_WM_KEYDOWN(OnKeyDown)
        MSG_WM_CHAR(OnChar)
        MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
    SOUI_MSG_MAP_END()
};

}//namespace SOUI