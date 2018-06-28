/**
* Copyright (C) 2014-2050 SOUI团队
* All rights reserved.
* 
* @file       SListCtrl.h
* @brief      
* @version    v1.0      
* @author     soui      
* @date       2014-07-06
* 
* Describe    ListCtrl控件 
*/
#pragma once
#include "core/SPanel.h"
#include "SHeaderCtrl.h"

namespace SOUI
{
    /** 
    * @enum      
    * @brief     类型
    *
    * Describe   类型
    */
    enum{
        S_LVIF_TEXT=0x01,
        S_LVIF_IMAGE=0x02,
        S_LVIF_INDENT=0x04,
    };

    /** 
    * qsort_s    
    * @brief     比较函数--函数指针
    *
    * Describe   函数指针
    */
    typedef int (__cdecl  *PFNLVCOMPAREEX)(void *, const void *, const void *);//使用快速排序算法中的比较函数,参考qsort_s

    /** 
    * @struct    _DXLVSUBITEM
    * @brief     子项结构
    *
    * @Describe  子项结构
    */
    typedef struct _DXLVSUBITEM
    {
        /** 
        * _DXLVSUBITEM   
        * @brief     构造函数
        *
        * Describe   构造函数
        */
        _DXLVSUBITEM()
        {
            mask=0;
            nImage = 0;
            strText=NULL;
            cchTextMax=0;
            nIndent=0;
        }

        UINT    mask;         /**<  */
        LPTSTR  strText;      /**< 文本 */
        int     cchTextMax;   /**< 文本最大数 */
        UINT    nImage;       /**< 图标 */
        int     nIndent;      /**< 缩进 */
    } DXLVSUBITEM;

    typedef SArray<DXLVSUBITEM>   ArrSubItem; /**< 保存子项数组	 */

    /** 
    * @struct    _DXLVITEM
    * @brief     条目结构
    *
    * @Describe  条目结构
    */
    typedef struct _DXLVITEM
    {
        /** 
        * _DXLVITEM   
        * @brief     构造函数
        *
        * Describe   构造函数
        */
        _DXLVITEM()
        {
            dwData = 0;
            arSubItems=NULL;
            checked = FALSE;
        }

        ArrSubItem  *arSubItems;
		LPARAM       dwData;
        BOOL    checked;
    } DXLVITEM;

    //////////////////////////////////////////////////////////////////////////
    //  SListCtrl
    class SOUI_EXP SListCtrl : public SPanel
    {
        SOUI_CLASS_NAME(SListCtrl, L"listctrl")

    public:
        /**
        * SListCtrl::SListCtrl
        * @brief    构造函数
        *
        * Describe  构造函数  
        */
        SListCtrl();

        /**
        * SListCtrl::~SListCtrl
        * @brief    析构函数
        *
        * Describe  析构函数  
        */
        virtual ~SListCtrl();
        /**
        * SListCtrl::InsertColumn
        * @brief    插入一列
        * @param    int nIndex -- 索引
        * @param    LPCTSTR pszText -- 标题
        * @param    int nWidth -- 宽度
        * @param    LPARAM lParam -- 附加参数
        *
        * Describe  插入一列
        */
        int             InsertColumn(int nIndex, LPCTSTR pszText, int nWidth, LPARAM lParam=0);
        /**
        * SListCtrl::InsertItem
        * @brief    插入条目
        * @param    int nIndex -- 索引
        * @param    LPCTSTR pszText -- 标题
        * @param    int nImage -- 图标
        *
        * Describe  插入条目
        */
        int             InsertItem(int nItem, LPCTSTR pszText, int nImage=-1);

		/**
        * SListCtrl::SetItemData
        * @brief    设置附加数据
        * @param    int nItem -- 索引
        * @param    LPARAM dwData-- 附加数据
        *
        * Describe  设置附加数据
        */
        BOOL            SetItemData(int nItem, LPARAM dwData);
        /**
        * SListCtrl::GetItemData
        * @brief    获取附加数据
        * @param    int nItem -- 索引
        * @return   返回DWORD
        *
        * Describe  获取附加数据
        */
		LPARAM           GetItemData(int nItem);
        /**
        * SListCtrl::SetSubItem
        * @brief    设置子项
        * @param    int nItem -- 索引
        * @param    int nSubItem -- 子项索引
        * @param    const DXLVSUBITEM* plv -- 
        * @return   返回BOOL
        *
        * Describe  设置子项
        */
        BOOL            SetSubItem(int nItem, int nSubItem,const DXLVSUBITEM* plv);
        /**
        * SListCtrl::GetSubItem
        * @brief    获取子项
        * @param    int nItem -- 索引
        * @param    int nSubItem -- 子项索引
        * @param    DXLVSUBITEM* plv -- 
        * @return   返回BOOL
        *
        * Describe  获取附加数据
        */
        BOOL            GetSubItem(int nItem, int nSubItem, DXLVSUBITEM* plv) const;
        /**
        * SListCtrl::SetSubItemText
        * @brief    设置子项文本
        * @param    int nItem -- 索引
        * @param    int nSubItem -- 子项索引
        * @param    LPCTSTR pszText -- 文本
        * @return   返回BOOL
        *
        * Describe  设置子项文本
        */
        BOOL            SetSubItemText(int nItem, int nSubItem, LPCTSTR pszText);

        /**
        * GetSubItemText
        * @brief    获取子项文本
        * @param    int nItem -- 索引
        * @param    int nSubItem -- 子项索引
        * @return   SOUI::SStringT -- 结果字符串
        * Describe  
        */    
        SStringT        GetSubItemText(int nItem, int nSubItem) const;

        /**
        * SListCtrl::GetSelectedItem
        * @brief    获取选中项
        * @return   返回int
        *
        * Describe  设置选中项
        */
        int             GetSelectedItem();
        /**
        * SListCtrl::SetSelectedItem
        * @brief    设置选中项
        * @param    int nItem -- 索引
        *
        * Describe  设置选中项
        */
        void            SetSelectedItem(int nItem);
        /**
        * SListCtrl::GetItemCount
        * @brief    获取条目总数
        * @return   返回int
        *
        * Describe  获取条目总数
        */
        int             GetItemCount() const;
        /**
        * SListCtrl::SetItemCount
        * @brief    设置条目数
        * @param    int nItems -- 索引
        * @param    int nGrowBy -- 
        * @return   返回BOOL
        *
        * Describe  设置条目数
        */
        BOOL            SetItemCount( int nItems ,int nGrowBy);
        /**
        * SListCtrl::GetColumnCount
        * @brief    获取列总数
        * @return   返回int
        *
        * Describe  获取列总数
        */
        int             GetColumnCount() const;
        /**
        * SListCtrl::GetCountPerPage
        * @brief    获取每页条目总数
        * @return   返回int
        *
        * Describe  获取每页条目总数
        */
        int             GetCountPerPage(BOOL bPartial);
        /**
        * SListCtrl::DeleteItem
        * @brief    删除指定项
        * @param    int nItem -- 索引
        *
        * Describe  删除指定项
        */
        void            DeleteItem(int nItem);
        /**
        * SListCtrl::DeleteColumn
        * @brief    删除指定列
        * @param    int iCol -- 索引
        *
        * Describe  删除指定列
        */
        void            DeleteColumn(int iCol);
        /**
        * SListCtrl::DeleteAllItems
        * @brief    删除所有项
        *
        * Describe  删除所有项
        */
        void            DeleteAllItems();

        /**
        * SListCtrl::CreateChildren
        * @brief    创建新项
        * @param    pugi::xml_node xmlNode -- xml文件
        *
        * Describe  通过解析xml文件创建
        */
        virtual BOOL    CreateChildren(pugi::xml_node xmlNode);

        /**
        * SListCtrl::HitTest
        * @brief    获取鼠标相关信息
        * @param    CPoint &pt -- 坐标
        *
        * Describe  
        */
        int             HitTest(const CPoint& pt);
        /**
        * SListCtrl::SortItems
        * @brief    排序
        * @param    PFNLVCOMPAREEX pfnCompare -- 比较函数
        * @param    void * pContext -- 比较内容
        * @return   返回BOOL
        * 
        * Describe  排序
        */
        BOOL            SortItems( PFNLVCOMPAREEX pfnCompare, void * pContext );

        /**
        * SListCtrl::GetCheckState
        * @brief    获取某一行是否被选中
        * @param    int nItem -- 指定哪一行
        * @return   返回选中状态
        *
        * Describe  获取某一行是否被选中
        */
        BOOL            GetCheckState(int nItem);  

        /**
        * SListCtrl::SetCheckState
        * @brief   设置某一行的选中状态
        * @param   int nItem -- 指定行
        * @param   BOOL bCheck -- 状态
        * @return  返回函数执行是否成功
        *
        * Describe 设置某一行的选中状态
        */
        BOOL            SetCheckState(int nItem, BOOL bCheck);  

        /**
        * SListCtrl::GetCheckedItemCount
        * @brief   获取选中行的计数
        * @return  返回计数
        *
        * Describe 获取选中行的计数
        */
        int             GetCheckedItemCount();


        int             GetFirstCheckedItem();
        int             GetLastCheckedItem();
        VOID            EnableMultiSelection(BOOL enable) { m_bMultiSelection = enable; }
        VOID            EnableCheckBox(BOOL enable) { m_bCheckBox = enable; }
        VOID            EnableHotTrack(BOOL enable) { m_bHotTrack = enable; }
    protected:
        /**
        * SListCtrl::GetTopIndex
        * @brief    获取置顶项选项索引
        * @return   返回int
        *
        * Describe  设置选中项
        */
        int             GetTopIndex() const;
        /**
        * SListCtrl::GetItemRect
        * @brief    获取item位置
        * @param    int nItem -- 索引
        * @param    int nSubItem -- 子项索引
        * @return   返回int
        *
        * Describe  设置选中项
        */
        CRect           GetItemRect(int nItem, int nSubItem=0);

        /**
        * SListCtrl::DrawItem
        * @brief    绘制
        * @param    IRenderTarget *pRT -- 绘制设备
        * @param    CRect &rc -- 目标区域
        * @param    int iItem -- 选项
        *
        * Describe  绘制
        */
        virtual void    DrawItem(IRenderTarget *pRT, CRect rcItem, int nItem);
        /**
        * SListCtrl::RedrawItem
        * @brief    重绘选项
        * @param    int iItem  -- 索引
        *
        * Describe  重绘选项
        */
        void            RedrawItem(int nItem);

        /**
        * SListCtrl::NotifySelChange
        * @brief    修改选中项
        * @param    int nOldSel -- 旧选中项
        * @param    int nNewSel -- 新选中项
        *
        * Describe  修改选中项
        */
        void            NotifySelChange(int nOldSel, int nNewSel, BOOL checkBox = FALSE);
        /**
        * SListCtrl::OnPaint
        * @brief    绘制
        * @param    IRenderTarget *pRT -- 绘制设备
        *
        * Describe  消息响应函数 
        */
        void            OnPaint(IRenderTarget *pRT);
        /**
        * SListCtrl::OnDestroy
        * @brief    销毁
        *
        * Describe  销毁
        */
        void            OnDestroy();
        /**
        * SListCtrl::OnHeaderClick
        * @brief    列表头单击事件 -- 
        * @param    EventArgs *pEvt
        *
        * Describe  列表头单击事件
        */
        bool            OnHeaderClick(EventArgs *pEvt);
        /**
        * SListCtrl::OnHeaderSizeChanging
        * @brief    列表头大小改变
        * @param    EventArgs *pEvt -- 
        *
        * Describe  列表头大小改变
        */
        bool            OnHeaderSizeChanging(EventArgs *pEvt);
        /**
        * SListCtrl::OnHeaderSwap
        * @brief    列表头交换
        * @param    EventArgs *pEvt -- 
        *
        * Describe  列表头交换
        */
        bool            OnHeaderSwap(EventArgs *pEvt);
        /**
        * SListCtrl::OnScroll
        * @brief    滚动事件
        * @param    BOOL bVertical -- 是否是垂直
        * @param    UINT uCode -- 滚动类型
        * @param    int nPos -- 滚动位置
        *
        * Describe  窗口滚动事件
        */
        virtual BOOL    OnScroll(BOOL bVertical,UINT uCode,int nPos);
        /**
        * SListCtrl::OnLButtonDown
        * @brief    左键按下
        * @param    UINT nFlags -- 标志
        * @param    CPoint pt -- 坐标
        *
        * Describe  消息响应函数 
        */
        void    OnLButtonDown(UINT nFlags, CPoint pt);

		/**
		* SListBox::OnLButtonDbClick
		* @brief    左键双击
		* @param    UINT nFlags -- 标志
		* @param    CPoint pt -- 坐标
		*
		* Describe  消息响应函数
		*/
		void OnLButtonDbClick(UINT nFlags, CPoint pt);

        /**
        * SListCtrl::OnLButtonUp
        * @brief    左键抬起
        * @param    UINT nFlags -- 标志
        * @param    CPoint pt -- 坐标
        *
        * Describe  消息响应函数 
        */
        void    OnLButtonUp(UINT nFlags, CPoint pt);

        /**
        * OnMouseMove
        * @brief    处理鼠标移动
        * @param    UINT nFlags --  标志
        * @param    CPoint pt --  坐标
        * @return   void 
        * Describe  
        */    
        void    OnMouseMove(UINT nFlags, CPoint pt);

        /**
        * OnMouseLeave
        * @brief    处理鼠标离开消息
        * @return   void
        * Describe  
        */    
        void   OnMouseLeave();

        /**
        * SListCtrl::OnSize
        * @brief    消息响应函数
        * @param    UINT nType --
        * @param    CSize size -- 
        *
        * Describe  获取某项得索引  
        */
        void    OnSize(UINT nType, CSize size);
        /**
        * SListCtrl::UpdateChildrenPosition
        * @brief    更新子项位置
        *
        * Describe  更新子项位置
        */
        virtual void    UpdateChildrenPosition();
        /**
        * SListCtrl::GetListRect
        * @brief    获取list位置
        * @return   返回CRect 
        *
        * Describe  获取list位置
        */
        CRect           GetListRect();
        /**
        * SListCtrl::UpdateScrollBar
        * @brief    更新滚动条
        *
        * Describe  更新滚动条
        */
        void            UpdateScrollBar();
        /**
        * SListCtrl::UpdateHeaderCtrl
        * @brief    更新列表头控件
        *
        * Describe  更新列表头控件
        */
        void            UpdateHeaderCtrl();

        BOOL            HitCheckBox(const CPoint& pt);

    protected:
        int             m_nHeaderHeight;  /**< 列表头高度 */
        int             m_nItemHeight;  /**< 条目高度 */

        int             m_nSelectItem;  /**< 选中项 */
        int             m_nHoverItem;  /**< Hover状态项 */
        BOOL            m_bHotTrack;  /**<  */

        CPoint          m_ptIcon;  /**< 图标位置 */
        CPoint          m_ptText;  /**< 文本位置 */

        COLORREF        m_crItemBg;  /**< 背景色 */
        COLORREF        m_crItemBg2;  /**< 背景色 */
        COLORREF        m_crItemSelBg;  /**< 选中背景色 */
		COLORREF        m_crItemHotBg;  /**< Hot背景色 */
        COLORREF        m_crText;  /**< 文本颜色 */
        COLORREF        m_crSelText;  /**< 选中文本颜色 */

        ISkinObj*    m_pItemSkin;  /**< */
        ISkinObj*    m_pIconSkin;  /**< */
        ISkinObj*    m_pCheckSkin; /**< */
        BOOL        m_bCheckBox;
        BOOL        m_bMultiSelection;

    protected:
        typedef SArray<DXLVITEM> ArrLvItem;  /**< 保存item数组 */

        SHeaderCtrl*  m_pHeader;  /**< 列表头控件 */
        ArrLvItem       m_arrItems;  /**< */
        CPoint          m_ptOrigin;  /**< */

    protected:
        SOUI_ATTRS_BEGIN()
            ATTR_INT(L"headerHeight", m_nHeaderHeight, FALSE)
            ATTR_INT(L"itemHeight", m_nItemHeight, FALSE)
            ATTR_INT(L"checkBox", m_bCheckBox, TRUE)
            ATTR_INT(L"multiSelection", m_bMultiSelection, TRUE)
            ATTR_SKIN(L"itemSkin", m_pItemSkin, TRUE)
            ATTR_SKIN(L"iconSkin", m_pIconSkin, TRUE)
            ATTR_SKIN(L"checkSkin", m_pCheckSkin, TRUE)
            ATTR_COLOR(L"colorItemBkgnd", m_crItemBg, FALSE)
            ATTR_COLOR(L"colorItemBkgnd2", m_crItemBg2, FALSE)
            ATTR_COLOR(L"colorItemHotBkgnd", m_crItemHotBg, FALSE)
            ATTR_COLOR(L"colorItemSelBkgnd", m_crItemSelBg, FALSE)
            ATTR_COLOR(L"colorText", m_crText, FALSE)
            ATTR_COLOR(L"colorSelText", m_crSelText, FALSE)
            ATTR_INT(L"icon-x", m_ptIcon.x, FALSE)
            ATTR_INT(L"icon-y", m_ptIcon.y, FALSE)
            ATTR_INT(L"text-x", m_ptText.x, FALSE)
            ATTR_INT(L"text-y", m_ptText.y, FALSE)
            ATTR_INT(L"hotTrack", m_bHotTrack, FALSE)
        SOUI_ATTRS_END()

        SOUI_MSG_MAP_BEGIN()
            MSG_WM_PAINT_EX(OnPaint)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_SIZE(OnSize)
			MSG_WM_LBUTTONDBLCLK(OnLButtonDbClick)
			MSG_WM_LBUTTONDOWN(OnLButtonDown)
			MSG_WM_LBUTTONUP(OnLButtonUp)
            MSG_WM_MOUSEMOVE(OnMouseMove)
            MSG_WM_MOUSELEAVE(OnMouseLeave)
        SOUI_MSG_MAP_END()
    };

}//end of namespace
