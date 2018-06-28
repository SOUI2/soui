//////////////////////////////////////////////////////////////////////////
//  Class Name: STreeCtrl
//     Version: 2012.12.16 - 1.1 - Create
//////////////////////////////////////////////////////////////////////////


#pragma once
#include "core/SPanel.h"
#include "stree.hpp"

namespace SOUI{

enum {
    STVIMask_Toggle         = 0x00000001UL, 
    STVIMask_CheckBox         = 0x00000002UL, 
    STVIMask_Icon             = 0x00000004UL
};

enum {
    STVICheckBox_UnChecked,        //未选中
    STVICheckBox_Checked,            //选中
    STVICheckBox_PartChecked        //部分选中
};

enum {
    STVIBtn_None,                    
    STVIBtn_Toggle, 
    STVIBtn_CheckBox
};

typedef struct tagTVITEM {    
    SStringT        strText;
    int            nImage;
    int            nSelectedImage;
    LPARAM      lParam;    

    HSTREEITEM    hItem;                    
    BOOL        bCollapsed;
    BOOL        bVisible;
    BOOL        bHasChildren;
    int            nLevel;
    int            nCheckBoxValue;
    int         nContentWidth;
    DWORD        dwToggleState;
    DWORD       dwCheckBoxState;

    tagTVITEM()
    {
        nImage = -1;
        nSelectedImage = -1;
        lParam = NULL;

        hItem = NULL;
        bCollapsed = FALSE;
        bVisible = TRUE;
        bHasChildren = FALSE;
        nLevel = 0;
        nCheckBoxValue = STVICheckBox_UnChecked;
        nContentWidth = 0;
        dwToggleState = WndState_Normal;
        dwCheckBoxState = WndState_Normal;
    }

} TVITEM, *LPTVITEM;


class SOUI_EXP STreeCtrl
    : public SScrollView
    , protected CSTree<LPTVITEM>
{
    SOUI_CLASS_NAME(STreeCtrl, L"treectrl")
public:

    STreeCtrl();

    virtual ~STreeCtrl();
    
    HSTREEITEM InsertItem(LPCTSTR lpszItem, HSTREEITEM hParent=STVI_ROOT, HSTREEITEM hInsertAfter=STVI_LAST,BOOL bEnsureVisible=TRUE);
    HSTREEITEM InsertItem(LPCTSTR lpszItem, int nImage,
        int nSelectedImage, HSTREEITEM hParent=STVI_ROOT, HSTREEITEM hInsertAfter=STVI_LAST,BOOL bEnsureVisible=TRUE);        
    HSTREEITEM InsertItem(LPCTSTR lpszItem, int nImage,
        int nSelectedImage, LPARAM lParam,
        HSTREEITEM hParent=STVI_ROOT, HSTREEITEM hInsertAfter=STVI_LAST,BOOL bEnsureVisible=TRUE);

    BOOL RemoveItem(HSTREEITEM hItem);
    void RemoveAllItems();

    HSTREEITEM GetRootItem();
    HSTREEITEM GetNextSiblingItem(HSTREEITEM hItem);
    HSTREEITEM GetPrevSiblingItem(HSTREEITEM hItem);
    HSTREEITEM GetChildItem(HSTREEITEM hItem,BOOL bFirst =TRUE);
    HSTREEITEM GetParentItem(HSTREEITEM hItem);
    HSTREEITEM GetSelectedItem();
    HSTREEITEM GetNextItem(HSTREEITEM hItem){return CSTree<LPTVITEM>::GetNextItem(hItem);}
    
    typedef int (__cdecl *FunSortCallback)(void * pCtx,const void * phItem1,const void * phItem2);
    void SortChildren(HSTREEITEM hItem,FunSortCallback sortFunc,void *pCtx);
    
    BOOL SelectItem(HSTREEITEM hItem,BOOL bEnsureVisible=TRUE);
    
    BOOL GetItemText(HSTREEITEM hItem, SStringT& strText) const;
    BOOL SetItemText(HSTREEITEM hItem, LPCTSTR lpszItem);
    BOOL GetItemImage(HSTREEITEM hItem, int& nImage, int& nSelectedImage) const;
    BOOL SetItemImage(HSTREEITEM hItem, int nImage, int nSelectedImage);
    LPARAM GetItemData(HSTREEITEM hItem) const;
    BOOL SetItemData(HSTREEITEM hItem, LPARAM lParam);
    BOOL ItemHasChildren(HSTREEITEM hItem);

    BOOL GetCheckState(HSTREEITEM hItem) const;    
    BOOL SetCheckState(HSTREEITEM hItem, BOOL bCheck);    

    BOOL Expand(HSTREEITEM hItem , UINT nCode = TVE_EXPAND );
    BOOL EnsureVisible(HSTREEITEM hItem);

    void PageUp();
    void PageDown();

	HSTREEITEM HitTest(CPoint &pt);

protected:

    virtual BOOL CreateChildren(pugi::xml_node xmlNode);
    virtual void LoadBranch(HSTREEITEM hParent,pugi::xml_node xmlNode);
    virtual void LoadItemAttribute(pugi::xml_node xmlNode, LPTVITEM pItem);
    
    HSTREEITEM InsertItem(LPTVITEM pItemObj,HSTREEITEM hParent,HSTREEITEM hInsertAfter,BOOL bEnsureVisible);
    HSTREEITEM InsertItem(pugi::xml_node xmlNode,HSTREEITEM hParent=STVI_ROOT, HSTREEITEM hInsertAfter=STVI_LAST,BOOL bEnsureVisible=FALSE);
    
    BOOL IsAncestor(HSTREEITEM hItem1,HSTREEITEM hItem2);

    void SetChildrenVisible(HSTREEITEM hItem,BOOL bVisible);

    void SetChildrenState(HSTREEITEM hItem, int nCheckValue);
    BOOL CheckChildrenState(HSTREEITEM hItem, BOOL bCheck);    
    void CheckState(HSTREEITEM hItem, BOOL bCheck, BOOL bCheckChild = TRUE);    

    virtual void ItemLayout();
    virtual void CalcItemContentWidth(LPTVITEM pItem);
    virtual int  CalcItemWidth(const LPTVITEM pItem);
    virtual int  GetMaxItemWidth();
    virtual int  GetMaxItemWidth(HSTREEITEM hItem);
    int  GetItemShowIndex(HSTREEITEM hItemObj);
    BOOL GetItemRect( LPTVITEM pItem,CRect &rcItem );

    void RedrawItem(HSTREEITEM hItem);
    virtual void DrawItem(IRenderTarget *pRT, CRect & rc, HSTREEITEM hItem);

    int ItemHitTest(HSTREEITEM hItem,CPoint &pt) const;
    void ModifyToggleState(HSTREEITEM hItem, DWORD dwStateAdd, DWORD dwStateRemove);
    void ModifyChekcBoxState(HSTREEITEM hItem, DWORD dwStateAdd, DWORD dwStateRemove);

    void ItemLButtonDown(HSTREEITEM hItem, UINT nFlags,CPoint pt);
    void ItemLButtonUp(HSTREEITEM hItem, UINT nFlags,CPoint pt);
    void ItemLButtonDbClick(HSTREEITEM hItem, UINT nFlags,CPoint pt);

    void ItemMouseMove(HSTREEITEM hItem, UINT nFlags,CPoint pt);
    void ItemMouseLeave(HSTREEITEM hItem);
    
protected:

    void OnDestroy();

    void OnPaint(IRenderTarget *pRT);

    void OnLButtonDown(UINT nFlags,CPoint pt);
    void OnLButtonUp(UINT nFlags,CPoint pt);
    void OnLButtonDbClick(UINT nFlags,CPoint pt);
    void OnRButtonDown(UINT nFlags, CPoint pt);
    void OnRButtonUp(UINT nFlags,CPoint pt);

    void OnMouseMove(UINT nFlags,CPoint pt);
    void OnMouseLeave();

    virtual void OnNodeFree(LPTVITEM & pItemData){
        delete pItemData;
    }
    virtual void OnInsertItem(LPTVITEM & pItemData)
    {
    }
protected:

    HSTREEITEM    m_hSelItem;
    HSTREEITEM    m_hHoverItem;
    HSTREEITEM    m_hCaptureItem;

    int            m_nVisibleItems;
    int         m_nMaxItemWidth;

    UINT        m_uItemMask;
    int         m_nItemOffset;
    CRect       m_rcToggle;
    CRect       m_rcCheckBox;
    CRect       m_rcIcon;
    int            m_nItemHoverBtn;
    int            m_nItemPushDownBtn;

    int            m_nItemHei,m_nIndent, m_nItemMargin;
    BOOL        m_bCheckBox;
    BOOL        m_bRightClickSel;
    ISkinObj * m_pItemBgSkin, * m_pItemSelSkin;
    ISkinObj * m_pIconSkin, * m_pToggleSkin, * m_pCheckSkin;
    COLORREF m_crItemBg,m_crItemSelBg;
    COLORREF m_crItemText,m_crItemSelText;


    SOUI_ATTRS_BEGIN()        
        ATTR_INT(L"indent", m_nIndent, TRUE)
        ATTR_INT(L"itemHeight", m_nItemHei, TRUE)
        ATTR_INT(L"itemMargin", m_nItemMargin, TRUE)
        ATTR_INT(L"checkBox", m_bCheckBox, TRUE)
        ATTR_INT(L"rightClickSel", m_bRightClickSel, TRUE)
        ATTR_SKIN(L"itemBkgndSkin", m_pItemBgSkin, TRUE)
        ATTR_SKIN(L"itemSelSkin", m_pItemSelSkin, TRUE)
        ATTR_SKIN(L"toggleSkin", m_pToggleSkin, TRUE)            
        ATTR_SKIN(L"iconSkin", m_pIconSkin, TRUE)
        ATTR_SKIN(L"checkSkin", m_pCheckSkin, TRUE)
        ATTR_COLOR(L"colorItemBkgnd",m_crItemBg,FALSE)
        ATTR_COLOR(L"colorItemSelBkgnd",m_crItemSelBg,FALSE)
        ATTR_COLOR(L"colorItemText",m_crItemText,FALSE)
        ATTR_COLOR(L"colorItemSelText",m_crItemSelText,FALSE)
    SOUI_ATTRS_END()

    SOUI_MSG_MAP_BEGIN()
        MSG_WM_PAINT_EX(OnPaint)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONDBLCLK(OnLButtonDbClick)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_RBUTTONDOWN(OnRButtonDown);
        MSG_WM_MOUSEMOVE(OnMouseMove)
        MSG_WM_MOUSELEAVE(OnMouseLeave)
    SOUI_MSG_MAP_END()
};

}//namespace SOUI
