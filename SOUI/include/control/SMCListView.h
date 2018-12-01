/**
* Copyright (C) 2014-2050 SOUI团队
* All rights reserved.
* 
* @file       SMCListView.h
* @brief      
* @version    v1.0      
* @author     soui      
* @date       2014-07-06
* 
* Describe    ListCtrl控件 
*/
#pragma once
#include "core/SPanel.h"
#include "core/SItemPanel.h"
#include "SHeaderCtrl.h"

namespace SOUI
{

    //////////////////////////////////////////////////////////////////////////
    //  SMCListView
    class SOUI_EXP SMCListView : public SPanel
        , protected IItemContainer
    {
        SOUI_CLASS_NAME(SMCListView, L"mclistview")
        friend class SMCListViewDataSetObserver;
    public:
        /**
        * SMCListView::SMCListView
        * @brief    构造函数
        *
        * Describe  构造函数  
        */
        SMCListView();

        /**
        * SMCListView::~SMCListView
        * @brief    析构函数
        *
        * Describe  析构函数  
        */
        virtual ~SMCListView();
        /**
        * SMCListView::InsertColumn
        * @brief    插入一列
        * @param    int nIndex -- 索引
        * @param    LPCTSTR pszText -- 标题
        * @param    int nWidth -- 宽度
        * @param    LPARAM lParam -- 附加参数
        *
        * Describe  插入一列
        */
        int             InsertColumn(int nIndex, LPCTSTR pszText, int nWidth, LPARAM lParam=0);

        void DeleteColumn( int iCol );

        int GetColumnCount() const;


        BOOL SetAdapter(IMcAdapter * adapter);

        ILvAdapter * GetAdapter() {
            return m_adapter;
        }

        IListViewItemLocator * GetItemLocator(){
            return m_lvItemLocator;
        }

		void UpdateVisibleItems();
		void UpdateVisibleItem(int iItem);

        void SetItemLocator(IListViewItemLocator *pItemLocator);
        void EnsureVisible( int iItem );

        void SetSel(int iItem,BOOL bNotify=FALSE);
        int  GetSel()const{return m_iSelItem;}
        SItemPanel * HitTest(CPoint & pt);

    protected:
        virtual void OnItemSetCapture(SItemPanel *pItem,BOOL bCapture);
        virtual BOOL OnItemGetRect(SItemPanel *pItem,CRect &rcItem);
        virtual BOOL IsItemRedrawDelay();
        virtual void OnItemRequestRelayout(SItemPanel *pItem);

    protected:
        void onDataSetChanged();
        void onDataSetInvalidated();
        void onItemDataChanged(int iItem);
    protected:
        bool OnItemClick(EventArgs *pEvt);

    protected:
        virtual BOOL OnScroll(BOOL bVertical,UINT uCode,int nPos);
        virtual int  GetScrollLineSize(BOOL bVertical);
        virtual BOOL CreateChildren(pugi::xml_node xmlNode);

        virtual BOOL OnUpdateToolTip(CPoint pt, SwndToolTipInfo & tipInfo);
        virtual UINT OnGetDlgCode();
        virtual BOOL OnSetCursor(const CPoint &pt);
        virtual void OnColorize(COLORREF cr);
		virtual void OnScaleChanged(int nScale);
		virtual HRESULT OnLanguageChanged();
    protected:
		void DispatchMessage2Items(UINT uMsg,WPARAM wParam,LPARAM lParam);

        void UpdateScrollBar();
        void RedrawItem(SItemPanel *pItem);
        SItemPanel * GetItemPanel(int iItem);


        void OnPaint(IRenderTarget *pRT);
        void OnSize(UINT nType, CSize size);
        void OnDestroy();

        LRESULT OnMouseEvent(UINT uMsg,WPARAM wParam,LPARAM lParam);

        LRESULT OnKeyEvent( UINT uMsg,WPARAM wParam,LPARAM lParam );
        void OnKeyDown( TCHAR nChar, UINT nRepCnt, UINT nFlags );

        void OnMouseLeave();

        BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
        void OnKillFocus(SWND wndFocus);
        void OnSetFocus(SWND wndOld);

        SOUI_MSG_MAP_BEGIN()
            MSG_WM_PAINT_EX(OnPaint)
            MSG_WM_SIZE(OnSize)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_MOUSEWHEEL(OnMouseWheel)
            MSG_WM_MOUSELEAVE(OnMouseLeave)
            MSG_WM_KEYDOWN(OnKeyDown)
            MSG_WM_SETFOCUS_EX(OnSetFocus)
            MSG_WM_KILLFOCUS_EX(OnKillFocus)
            MESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST,WM_MOUSELAST,OnMouseEvent)
            MESSAGE_RANGE_HANDLER_EX(WM_KEYFIRST,WM_KEYLAST,OnKeyEvent)
            MESSAGE_RANGE_HANDLER_EX(WM_IME_STARTCOMPOSITION,WM_IME_KEYLAST,OnKeyEvent)
        SOUI_MSG_MAP_END()

        SOUI_ATTRS_BEGIN()
            ATTR_LAYOUTSIZE(L"headerHeight", m_nHeaderHeight, FALSE)
            ATTR_INT(L"hotTrack", m_bHotTrack, FALSE)
            ATTR_SKIN(L"dividerSkin",m_pSkinDivider,TRUE)
            ATTR_LAYOUTSIZE(L"dividerSize",m_nDividerSize,FALSE)
            ATTR_INT(L"wantTab",m_bWantTab,FALSE)
        SOUI_ATTRS_END()
    protected:
        CAutoRefPtr<IMcAdapter>           m_adapter;
        CAutoRefPtr<ILvDataSetObserver>   m_observer;
        CAutoRefPtr<IListViewItemLocator>  m_lvItemLocator;//列表项定位接口
        struct ItemInfo
        {
            SItemPanel *pItem;
            int nType;
        };

        int                             m_iFirstVisible;//第一个显示项索引
        SList<ItemInfo>                 m_lstItems; //当前正在显示的项
        SItemPanel*                     m_itemCapture;//The item panel that has been set capture.

        int                             m_iSelItem;
        SItemPanel*                     m_pHoverItem;

        SArray<SList<SItemPanel*> *>    m_itemRecycle;//item回收站,每一种样式在回收站中保持一个列表，以便重复利用

        pugi::xml_document              m_xmlTemplate;
        ISkinObj*                       m_pSkinDivider;
        SLayoutSize                     m_nDividerSize;
        BOOL                            m_bWantTab;
        BOOL                            m_bDatasetInvalidated;
    protected:

        /**
        * SMCListView::OnHeaderClick
        * @brief    列表头单击事件 -- 
        * @param    EventArgs *pEvt
        *
        * Describe  列表头单击事件
        */
        bool            OnHeaderClick(EventArgs *pEvt);
        /**
        * SMCListView::OnHeaderSizeChanging
        * @brief    列表头大小改变
        * @param    EventArgs *pEvt -- 
        *
        * Describe  列表头大小改变
        */
        bool            OnHeaderSizeChanging(EventArgs *pEvt);
        /**
        * SMCListView::OnHeaderSwap
        * @brief    列表头交换
        * @param    EventArgs *pEvt -- 
        *
        * Describe  列表头交换
        */
        bool            OnHeaderSwap(EventArgs *pEvt);

        /**
        * SMCListView::GetListRect
        * @brief    获取list位置
        * @return   返回CRect 
        *
        * Describe  获取list位置
        */
        CRect           GetListRect();

        /**
        * SMCListView::UpdateHeaderCtrl
        * @brief    更新列表头控件
        *
        * Describe  更新列表头控件
        */
        void            UpdateHeaderCtrl();
        void            UpdateChildrenPosition();
        
		int		GetHeaderHeight() const;
    protected:
        CRect _OnItemGetRect(int iPosition);
	protected:
        SHeaderCtrl     *m_pHeader;
        SLayoutSize     m_nHeaderHeight;  /**< 列表头高度 */
        BOOL            m_bHotTrack;  /**<  */
    };

}//end of namespace
