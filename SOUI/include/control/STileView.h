#pragma once

#include "core/Swnd.h"
#include "core/SItemPanel.h"
#include "interface/Adapter-i.h"
#include "helper/STileViewItemLocator.h"

namespace SOUI
{

class SOUI_EXP STileView : public SPanel
    , protected IItemContainer
{
    SOUI_CLASS_NAME(STileView, L"tileview")
    
    friend class STileViewDataSetObserver;
public:
    STileView();
    ~STileView();
    
    BOOL SetAdapter(ILvAdapter *adapter);
    
    ILvAdapter *GetAdapter()
    {
        return m_adapter;
    }
    
    STileViewItemLocator *GetItemLocator()
    {
        return m_tvItemLocator;
    }
    
    void SetItemLocator(STileViewItemLocator *pItemLocator);
    void EnsureVisible(int iItem);
    
    void SetSel(int iItem, BOOL bNotify = FALSE);
    int  GetSel()const
    {
        return m_iSelItem;
    }
    
    SItemPanel *HitTest(CPoint &pt);

protected:
    virtual void OnItemSetCapture(SItemPanel *pItem, BOOL bCapture);
    virtual BOOL OnItemGetRect(SItemPanel *pItem, CRect &rcItem);
    virtual BOOL IsItemRedrawDelay();
    virtual void OnItemRequestRelayout(SItemPanel *pItem);
    
protected:
    void onDataSetChanged();
    void onDataSetInvalidated();
    void onItemDataChanged(int iItem);
protected:
    bool OnItemClick(EventArgs *pEvt);

protected:
    virtual BOOL OnScroll(BOOL bVertical, UINT uCode, int nPos);
    virtual int  GetScrollLineSize(BOOL bVertical);
    virtual BOOL CreateChildren(pugi::xml_node xmlNode);
    
    virtual BOOL OnUpdateToolTip(CPoint pt, SwndToolTipInfo &tipInfo);
    virtual UINT OnGetDlgCode();
    virtual BOOL OnSetCursor(const CPoint &pt);
    virtual void OnColorize(COLORREF cr);
	virtual void OnScaleChanged(int nScale);
	virtual HRESULT OnLanguageChanged();
protected:
    void DispatchMessage2Items(UINT uMsg,WPARAM wParam,LPARAM lParam);
    void UpdateScrollBar();
    void RedrawItem(SItemPanel *pItem);
    SItemPanel *GetItemPanel(int iItem);
    CRect CalcItemDrawRect(int iItem);//计算item实际绘制的位置
    
    void UpdateVisibleItems();
    
    void OnPaint(IRenderTarget *pRT);
    void OnSize(UINT nType, CSize size);
    void OnDestroy();
    
    LRESULT OnMouseEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);
    
    LRESULT OnKeyEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnKeyDown(TCHAR nChar, UINT nRepCnt, UINT nFlags);
    
    void OnMouseLeave();
    
    BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    
    void OnKillFocus(SWND wndFocus);
    
    void OnSetFocus(SWND wndOld);

    LRESULT OnSetScale(UINT uMsg, WPARAM wParam, LPARAM lParam);

    SOUI_MSG_MAP_BEGIN()
		MSG_WM_PAINT_EX(OnPaint)
		MSG_WM_SIZE(OnSize)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_MOUSEWHEEL(OnMouseWheel)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
		MSG_WM_KEYDOWN(OnKeyDown)
		MSG_WM_KILLFOCUS_EX(OnKillFocus)
		MSG_WM_SETFOCUS_EX(OnSetFocus)
        MESSAGE_HANDLER_EX(UM_SETSCALE, OnSetScale)
        MESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseEvent)
		MESSAGE_RANGE_HANDLER_EX(WM_KEYFIRST, WM_KEYLAST, OnKeyEvent)
		MESSAGE_RANGE_HANDLER_EX(WM_IME_STARTCOMPOSITION, WM_IME_KEYLAST, OnKeyEvent)
    SOUI_MSG_MAP_END()
    
    SOUI_ATTRS_BEGIN()
		//ATTR_INT(L"marginSize", m_nMarginSize, FALSE)
        ATTR_LAYOUTSIZE(L"marginSize", m_nMarginSize, FALSE)
		ATTR_INT(L"wantTab", m_bWantTab, FALSE)
    SOUI_ATTRS_END()
protected:
    CAutoRefPtr<ILvAdapter>           m_adapter;
    CAutoRefPtr<ILvDataSetObserver>   m_observer;
    CAutoRefPtr<STileViewItemLocator>  m_tvItemLocator;//列表项定位接口
    struct ItemInfo
    {
        SItemPanel *pItem;
        int nType;
    };
    
    int                             m_iFirstVisible;//第一个显示项索引
    SList<ItemInfo>                 m_lstItems; //当前正在显示的项
    SItemPanel                     *m_itemCapture;//The item panel that has been set capture.
    
    int                             m_iSelItem;
    SItemPanel                     *m_pHoverItem;
    
    SArray<SList<SItemPanel *> *>    m_itemRecycle; //item回收站,每一种样式在回收站中保持一个列表，以便重复利用
    
    pugi::xml_document              m_xmlTemplate;
    SLayoutSize                     m_nMarginSize;
    // int                             m_nMarginSize;
    BOOL                            m_bWantTab;
    BOOL                            m_bDatasetInvalidated;
};
}
