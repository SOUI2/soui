#pragma once

#include <sobject/Sobject.hpp>
#include "core/SimpleWnd.h"
#include "res.mgr/Sskinpool.h"

namespace SOUI
{

#define CX_ICON    16        //支持的图标的宽度
#define CY_ICON    16        //支持的图标的高度

class SMenuAttr:public SObject, public TObjRefImpl<IObjRef>
{
    friend class SMenu;
	friend class SMenuODWnd;
    SOUI_CLASS_NAME(SMenuAttr, L"menuattr")
public:
    SMenuAttr();
	~SMenuAttr();
    virtual void OnInitFinished(pugi::xml_node xmlNode);

    SOUI_ATTRS_BEGIN()
        ATTR_SKIN(L"itemSkin",m_pItemSkin,FALSE)
        ATTR_SKIN(L"iconSkin",m_pIconSkin,FALSE)
        ATTR_SKIN(L"sepSkin",m_pSepSkin,FALSE)
        ATTR_SKIN(L"checkSkin",m_pCheckSkin,FALSE)
        ATTR_INT(L"itemHeight",m_nItemHei,FALSE)
        ATTR_INT(L"iconMargin",m_nIconMargin,FALSE)
        ATTR_INT(L"textMargin",m_nTextMargin,FALSE)
        ATTR_INT(L"maxWidth",m_nMaxWidth,FALSE)
        ATTR_SIZE(L"iconSize",m_szIcon,FALSE)
        ATTR_FONT2(L"font",m_hFont,FALSE)
        ATTR_COLOR(L"colorText",m_crTxtNormal,FALSE)
        ATTR_COLOR(L"colorTextSel",m_crTxtSel,FALSE)
        ATTR_COLOR(L"cororTextGray",m_crTxtGray,FALSE)
        ATTR_STRINGW(L"trCtx",m_strTrCtx,FALSE)
    SOUI_ATTRS_END()
    

protected:
	int GetScale() const{return 100;}

    ISkinObj *m_pItemSkin;    //菜单项皮肤，包含2种状态：正常状态+选中状态
    ISkinObj *m_pIconSkin;    //菜单图标
    ISkinObj *m_pSepSkin;    //分割栏皮肤
    ISkinObj *m_pCheckSkin;    //选中状态,包含两种状态:勾选+圈选
    int              m_nItemHei;    //菜单项高度
    int              m_nIconMargin;//图标边缘空间
    int              m_nTextMargin;//文本边缘空间
    COLORREF      m_crTxtNormal;//正常文本颜色
    COLORREF      m_crTxtSel;    //选中文本颜色
    COLORREF      m_crTxtGray;    //灰文本颜色
    CSize          m_szIcon;        //图标尺寸
    CAutoRefPtr<IFont>  m_hFont;
    SStringW      m_strTrCtx;   //翻译上下文
    int          m_nMaxWidth;   //菜单项最大宽度
};

struct SMenuItemData
{
	SMenuItemData():iIcon(-1),vHotKey(0),dwUserData(0)
	{

	}
	int iIcon;
	SStringT strText;
	UINT vHotKey;
	ULONG_PTR dwUserData;
};

template <class T>
class SOwnerDraw
{
public:
    // Message map and handlers
    BEGIN_MSG_MAP_EX(SOwnerDraw< T >)
    MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
    MESSAGE_HANDLER(WM_MEASUREITEM, OnMeasureItem)
    MESSAGE_HANDLER(WM_COMPAREITEM, OnCompareItem)
    MESSAGE_HANDLER(WM_DELETEITEM, OnDeleteItem)
    MESSAGE_HANDLER(WM_MENUCHAR,OnMenuChar)
    ALT_MSG_MAP(1)
    MESSAGE_HANDLER(OCM_DRAWITEM, OnDrawItem)
    MESSAGE_HANDLER(OCM_MEASUREITEM, OnMeasureItem)
    MESSAGE_HANDLER(OCM_COMPAREITEM, OnCompareItem)
    MESSAGE_HANDLER(OCM_DELETEITEM, OnDeleteItem)
    END_MSG_MAP()

    LRESULT OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
    {
        T* pT = static_cast<T*>(this);
        pT->SetMsgHandled(TRUE);
        pT->DrawItem((LPDRAWITEMSTRUCT)lParam);
        bHandled = pT->IsMsgHandled();
        return (LRESULT)TRUE;
    }

    LRESULT OnMeasureItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
    {
        T* pT = static_cast<T*>(this);
        pT->SetMsgHandled(TRUE);
        pT->MeasureItem((LPMEASUREITEMSTRUCT)lParam);
        bHandled = pT->IsMsgHandled();
        return (LRESULT)TRUE;
    }

    LRESULT OnCompareItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
    {
        T* pT = static_cast<T*>(this);
        pT->SetMsgHandled(TRUE);
        bHandled = pT->IsMsgHandled();
        return (LRESULT)pT->CompareItem((LPCOMPAREITEMSTRUCT)lParam);
    }

    LRESULT OnDeleteItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
    {
        T* pT = static_cast<T*>(this);
        pT->SetMsgHandled(TRUE);
        pT->DeleteItem((LPDELETEITEMSTRUCT)lParam);
        bHandled = pT->IsMsgHandled();
        return (LRESULT)TRUE;
    }

    LRESULT OnMenuChar(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        T* pT = static_cast<T*>(this);
        pT->SetMsgHandled(TRUE);
        
        UINT nChar = LOWORD(wParam);
        UINT nFlags = HIWORD(wParam);
        HMENU hMenu =(HMENU)lParam;
        LRESULT lRes = pT->OnMenuChar(nChar,nFlags,hMenu);
        
        bHandled = pT->IsMsgHandled();
        return lRes;
    }
    
    // Overrideables
    void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
    {
        // must be implemented
        SASSERT(FALSE);
    }

    void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
    {
        if(lpMeasureItemStruct->CtlType != ODT_MENU)
        {
            // return default height for a system font
            T* pT = static_cast<T*>(this);
            HWND hWnd = pT->GetDlgItem(lpMeasureItemStruct->CtlID);
            CClientDC dc(hWnd);
            TEXTMETRIC tm = { 0 };
            dc.GetTextMetrics(&tm);

            lpMeasureItemStruct->itemHeight = tm.tmHeight;
        }
        else
            lpMeasureItemStruct->itemHeight = ::GetSystemMetrics(SM_CYMENU);
    }

    int CompareItem(LPCOMPAREITEMSTRUCT /*lpCompareItemStruct*/)
    {
        // all items are equal
        return 0;
    }

    void DeleteItem(LPDELETEITEMSTRUCT /*lpDeleteItemStruct*/)
    {
        // default - nothing
    }
};

class SMenuODWnd : public CSimpleWnd
    ,public SOwnerDraw<SMenuODWnd>
{
    friend class SOwnerDraw<SMenuODWnd>;
public:
    SMenuODWnd(HWND hMenuOwner,SMenuAttr * pMenuAttr);

	~SMenuODWnd();
protected:
    void OnInitMenu(HMENU menu);
    void OnInitMenuPopup(HMENU menuPopup, UINT nIndex, BOOL bSysMenu);

    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

    void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

    void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU menu);

    LRESULT OnMenuChar(UINT nChar,  UINT nFlags, HMENU hMenu);
        
    BEGIN_MSG_MAP_EX(SMenuODWnd)
		MSG_WM_INITMENU(OnInitMenu)
		MSG_WM_INITMENUPOPUP(OnInitMenuPopup)
		MSG_WM_MENUSELECT(OnMenuSelect)
		CHAIN_MSG_MAP(SOwnerDraw<SMenuODWnd>)
		REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()

    HWND m_hMenuOwner;
	CAutoRefPtr<SMenuAttr> m_attr;
};


class SOUI_EXP SMenu
{
public:
    SMenu();
	SMenu(const SMenu & src);
	SMenu(HMENU hMenu);
    ~SMenu(void);

	BOOL Attach(HMENU hMenu);

	HMENU Detach();

    BOOL LoadMenu(LPCTSTR pszResID);

    BOOL LoadMenu(LPCTSTR pszResName ,LPCTSTR pszType);

    BOOL LoadMenu(pugi::xml_node xmlMenu);

    BOOL InsertMenu(UINT uPosition, UINT uFlags, UINT_PTR nIDNewItem,LPCTSTR strText, int iIcon);

	BOOL AppendMenu(UINT uFlags,UINT_PTR uIDNewItem, LPCTSTR lpNewItem,int iIcon);

	BOOL DeleteMenu(UINT uPosition, UINT uFlags);

    UINT TrackPopupMenu( UINT uFlags, int x, int y, HWND hWnd, LPCRECT prcRect=NULL);

    void DestroyMenu();

	BOOL ModifyMenuString(UINT uPosition, UINT uFlags,LPCTSTR lpItemString);

	BOOL SetMenuUserData(UINT uPosition, UINT uFlags,ULONG_PTR ulUserData);

	ULONG_PTR GetMenuUserData(UINT uPosition, UINT uFlags);

    HMENU m_hMenu;

protected:

    void BuildMenu(HMENU menuPopup,pugi::xml_node xmlNode);
    void InitMenuItemData(SMenuItemData * itemInfo, const SStringW & strText);
	void FreeMenuItemData(HMENU hMemu);

	SMenuAttr * GetMenuAttr(HMENU hMenu) const;
	void SetMenuAttr(HMENU hMenu,SMenuAttr *pMenuAttr) const;
};

}//namespace SOUI