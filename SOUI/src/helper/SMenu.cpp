#include "souistd.h"
#include "SApp.h"
#include "helper/SMenu.h"
#include "helper/mybuffer.h"
#include "helper/SplitString.h"
#include "gdialpha.h"
#include <souicoll.h>

namespace SOUI
{

SMenuAttr::SMenuAttr()
    :m_pItemSkin(GETBUILTINSKIN(SKIN_SYS_MENU_SKIN))
    ,m_pSepSkin(GETBUILTINSKIN(SKIN_SYS_MENU_SEP))
    ,m_pCheckSkin(GETBUILTINSKIN(SKIN_SYS_MENU_CHECK))
    ,m_pIconSkin(NULL)
    ,m_hFont(0)
    ,m_nItemHei(0)
    ,m_nIconMargin(2)
    ,m_nTextMargin(5)
    ,m_szIcon(CX_ICON,CY_ICON)
    ,m_nMaxWidth(-1)
{
    m_crTxtNormal=GetSysColor(COLOR_MENUTEXT)|0xff000000;
    m_crTxtSel=GetSysColor(COLOR_HIGHLIGHTTEXT)|0xff000000;
    m_crTxtGray=GetSysColor(COLOR_GRAYTEXT)|0xff000000;
}

SMenuAttr::~SMenuAttr()
{
}

void SMenuAttr::OnInitFinished( pugi::xml_node xmlNode )
{
    SASSERT(m_pItemSkin);
    if(m_nItemHei==0) m_nItemHei=m_pItemSkin->GetSkinSize().cy;
    if(!m_hFont) m_hFont=SFontPool::getSingleton().GetFont(FF_DEFAULTFONT,100);
}

//////////////////////////////////////////////////////////////////////////

SMenuODWnd::SMenuODWnd(HWND hMenuOwner,SMenuAttr *pMenuAttr):m_hMenuOwner(hMenuOwner),m_attr(pMenuAttr)
{

}

SMenuODWnd::~SMenuODWnd()
{

}

void SMenuODWnd::OnInitMenu( HMENU menu )
{
    ::SendMessage(m_hMenuOwner,WM_INITMENU,(WPARAM)menu,0);
}

void SMenuODWnd::OnInitMenuPopup( HMENU menuPopup, UINT nIndex, BOOL bSysMenu )
{
    ::SendMessage(m_hMenuOwner,WM_INITMENUPOPUP,(WPARAM)menuPopup,MAKELPARAM(nIndex,bSysMenu));
}

void SMenuODWnd::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
    CRect rcItem=lpDrawItemStruct->rcItem;
    rcItem.MoveToXY(0,0);
    SMenuItemData *pdmmi=(SMenuItemData*)lpDrawItemStruct->itemData;

    HDC dc(lpDrawItemStruct->hDC);
    CAutoRefPtr<IRenderTarget> pRT;
    GETRENDERFACTORY->CreateRenderTarget(&pRT,rcItem.Width(),rcItem.Height());

    if(pdmmi)
    {
        MENUITEMINFO mii= {sizeof(MENUITEMINFO),MIIM_FTYPE,0};
        GetMenuItemInfo((HMENU)lpDrawItemStruct->hwndItem,lpDrawItemStruct->itemID,FALSE,&mii);

        BOOL bDisabled = lpDrawItemStruct->itemState & ODS_GRAYED;
        BOOL bSelected = lpDrawItemStruct->itemState & ODS_SELECTED;
        BOOL bChecked = lpDrawItemStruct->itemState & ODS_CHECKED;
        BOOL bRadio = mii.fType&MFT_RADIOCHECK;
        m_attr->m_pItemSkin->Draw(pRT,rcItem,bSelected?1:0);    //draw background

        //draw icon
        CRect rcIcon;
        rcIcon.left=rcItem.left+m_attr->m_nIconMargin;
        rcIcon.right=rcIcon.left+m_attr->m_szIcon.cx;
        rcIcon.top=rcItem.top+(rcItem.Height()-m_attr->m_szIcon.cy)/2;
        rcIcon.bottom=rcIcon.top+m_attr->m_szIcon.cy;
        if(bChecked)
        {
            if(m_attr->m_pCheckSkin)
            {
                m_attr->m_pCheckSkin->Draw(pRT,rcIcon,bRadio?1:0);
            }
        }
        else if(pdmmi->iIcon!=-1 && m_attr->m_pIconSkin)
        {
            m_attr->m_pIconSkin->Draw(pRT,rcIcon,pdmmi->iIcon);
        }
        rcItem.left=rcIcon.right+m_attr->m_nIconMargin;

        //draw text
        CRect rcTxt=rcItem;
        rcTxt.DeflateRect(m_attr->m_nTextMargin,0);

        COLORREF crOld=pRT->SetTextColor(bDisabled?m_attr->m_crTxtGray:(bSelected?m_attr->m_crTxtSel:m_attr->m_crTxtNormal));


        CAutoRefPtr<IFont> oldFont;
        pRT->SelectObject(m_attr->m_hFont,(IRenderObj**)&oldFont);
        pRT->DrawText(pdmmi->strText,pdmmi->strText.GetLength(),&rcTxt,DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_END_ELLIPSIS);
        pRT->SelectObject(oldFont);

        pRT->SetTextColor(crOld);

        if(bSelected && m_attr->m_pItemSkin->GetStates()>2)
        {
            //draw select mask
            CRect rcItem=lpDrawItemStruct->rcItem;
            rcItem.MoveToXY(0,0);
            m_attr->m_pItemSkin->Draw(pRT,rcItem,2);
        }
    }
    else  //if(strcmp("sep",pXmlItem->Value())==0)
    {
        if(m_attr->m_pSepSkin)
            m_attr->m_pSepSkin->Draw(pRT,rcItem,0);    //draw seperator
    }
    rcItem=lpDrawItemStruct->rcItem;
    
    HDC hmemdc=pRT->GetDC(0);
    BitBlt(dc,rcItem.left,rcItem.top,rcItem.Width(),rcItem.Height(),hmemdc,0,0,SRCCOPY);
    pRT->ReleaseDC(hmemdc);
}

void SMenuODWnd::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
{
    if(lpMeasureItemStruct->CtlType != ODT_MENU) return;

    SMenuItemData *pdmmi=(SMenuItemData*)lpMeasureItemStruct->itemData;
    if(pdmmi)
    {
        //menu item
        lpMeasureItemStruct->itemHeight = m_attr->m_nItemHei;
        lpMeasureItemStruct->itemWidth = m_attr->m_szIcon.cx+m_attr->m_nIconMargin*2;

        CAutoRefPtr<IRenderTarget> pRT;
        GETRENDERFACTORY->CreateRenderTarget(&pRT,0,0);
        CAutoRefPtr<IFont> oldFont;
        pRT->SelectObject(m_attr->m_hFont,(IRenderObj**)&oldFont);
        SIZE szTxt;
        pRT->MeasureText(pdmmi->strText,pdmmi->strText.GetLength(),&szTxt);
        lpMeasureItemStruct->itemWidth += szTxt.cx+m_attr->m_nTextMargin*2;
        if(m_attr->m_nMaxWidth!=-1 && (int)lpMeasureItemStruct->itemWidth>m_attr->m_nMaxWidth)
            lpMeasureItemStruct->itemWidth=m_attr->m_nMaxWidth;
        pRT->SelectObject(oldFont);
    }
    else
    {
        // separator
        lpMeasureItemStruct->itemHeight = m_attr->m_pSepSkin?m_attr->m_pSepSkin->GetSkinSize().cy:3;
        lpMeasureItemStruct->itemWidth=0;
    }

}

void SMenuODWnd::OnMenuSelect( UINT nItemID, UINT nFlags, HMENU menu )
{
    ::SendMessage(m_hMenuOwner,WM_MENUSELECT,MAKEWPARAM(nItemID,nFlags),(LPARAM)menu);
}

LRESULT SMenuODWnd::OnMenuChar(UINT nChar, UINT nFlags, HMENU hMenu)
{
    wchar_t cChar = tolower(nChar);
    
    int nMenuItems = ::GetMenuItemCount(hMenu);
    for(int i=0;i<nMenuItems;i++)
    {
        MENUITEMINFO mi={sizeof(mi),0};
        mi.fMask = MIIM_DATA;
        ::GetMenuItemInfo(hMenu,i,TRUE,&mi);
        SMenuItemData * pItemData =(SMenuItemData*)mi.dwItemData;
        if(pItemData && pItemData->vHotKey == cChar)
        {
            return MAKELONG(i,MNC_EXECUTE);
        }
    }
    return MAKELONG(0,MNC_IGNORE);
}

//////////////////////////////////////////////////////////////////////////

SMenu::SMenu():m_hMenu(0)
{
}

SMenu::SMenu( const SMenu & src )
{
    m_hMenu=src.m_hMenu;
}

SMenu::SMenu(HMENU hMenu)
{
	Attach(hMenu);
}

SMenu::~SMenu(void)
{
    DestroyMenu();
}

BOOL SMenu::LoadMenu( LPCTSTR pszResName ,LPCTSTR pszType)
{
    if(::IsMenu(m_hMenu)) return FALSE;

    pugi::xml_document xmlDoc;
    if(!LOADXML(xmlDoc,pszResName,pszType)) return FALSE;

    pugi::xml_node xmlMenu=xmlDoc.child(L"menu");
    if(!xmlMenu)  return FALSE;

    return LoadMenu(xmlMenu);
}


BOOL SMenu::LoadMenu( pugi::xml_node xmlMenu )
{
	SASSERT(m_hMenu==0);
    m_hMenu=CreatePopupMenu();
    if(!m_hMenu) return FALSE;

	SMenuAttr *pMenuAttr = new SMenuAttr;
    pMenuAttr->InitFromXml(xmlMenu);
    SASSERT(pMenuAttr->m_pItemSkin);
	SetMenuContextHelpId(m_hMenu,xmlMenu.attribute(L"contextHelpId").as_uint(0));
	SetMenuAttr(m_hMenu,pMenuAttr);
	pMenuAttr->Release();

    BuildMenu(m_hMenu,xmlMenu);

    return TRUE;
}

BOOL SMenu::LoadMenu(LPCTSTR pszResID)
{
    SStringTList strLst;
    if(2!=ParseResID(pszResID,strLst)) return FALSE;
    return LoadMenu(strLst[1],strLst[0]);
}

void SMenu::InitMenuItemData(SMenuItemData * itemInfo, const SStringW & strTextW)
{
	SMenuAttr * pMenuAttr = GetMenuAttr(m_hMenu);
    itemInfo->strText=S_CW2T(TR(strTextW,pMenuAttr->m_strTrCtx));
    //查找快捷键
    int iHotKey = strTextW.Find(L"&");
    if(iHotKey != -1 && iHotKey < strTextW.GetLength()-1)
    {
        itemInfo->vHotKey = tolower(strTextW[iHotKey+1]);
    }else
    {
        itemInfo->vHotKey = 0;
    }
}

BOOL SMenu::InsertMenu(UINT nPosition, UINT nFlags, UINT_PTR nIDNewItem,LPCTSTR strText, int iIcon)
{
    nFlags|=MF_OWNERDRAW;
    if(nFlags&MF_SEPARATOR)
    {
        return ::InsertMenu(m_hMenu,nPosition,nFlags,(UINT_PTR)0,(LPCTSTR)NULL);
    }

    SMenuItemData *pMenuData=new SMenuItemData;
    pMenuData->iIcon=iIcon;
    InitMenuItemData(pMenuData,S_CT2W(strText));

    if(!::InsertMenu(m_hMenu,nPosition,nFlags,nIDNewItem,(LPCTSTR)pMenuData))
    {
        delete pMenuData;
        return FALSE;
    }
	if(nFlags & MF_POPUP)
	{
		HMENU hSubMenu = (HMENU)nIDNewItem;
		SMenuAttr * pMenuAttr = GetMenuAttr(hSubMenu);
		if(!pMenuAttr)
		{
			pMenuAttr = GetMenuAttr(m_hMenu);
			SetMenuAttr(hSubMenu,pMenuAttr);
		}
	}
    return TRUE;
}

UINT SMenu::TrackPopupMenu(
    UINT uFlags,
    int x,
    int y,
    HWND hWnd,
    LPCRECT prcRect
)
{
    SASSERT(IsMenu(m_hMenu));

    SMenuODWnd menuOwner(hWnd,GetMenuAttr(m_hMenu));
    menuOwner.Create(NULL,WS_POPUP,WS_EX_NOACTIVATE,0,0,0,0,NULL,NULL);
    if(IsWindow(hWnd) && IsWindowEnabled(hWnd)) ::SetForegroundWindow(hWnd);
    UINT uNewFlags=uFlags|TPM_RETURNCMD;
    UINT uRet=::TrackPopupMenu(m_hMenu,uNewFlags,x,y,0,menuOwner.m_hWnd,prcRect);
    menuOwner.DestroyWindow();
    if(uRet && !(uFlags&TPM_RETURNCMD)) ::SendMessage(hWnd,WM_COMMAND,uRet,0);
    return uRet;
}

void SMenu::BuildMenu( HMENU menuPopup,pugi::xml_node xmlNode )
{
    pugi::xml_node xmlItem=xmlNode.first_child();

    while(xmlItem)
    {
        if(wcscmp(L"item",xmlItem.name())==0)
        {
            SMenuItemData *pdmmi=new SMenuItemData;
            pdmmi->iIcon=xmlItem.attribute(L"icon").as_int(-1);
			pdmmi->dwUserData = xmlItem.append_attribute(L"userData").as_uint();
            SStringW strText = xmlItem.text().get();
            strText.TrimBlank();
			if(strText.IsEmpty()) strText = xmlItem.attribute(L"text").as_string();
            strText = TR(GETSTRING(strText),L"");
            InitMenuItemData(pdmmi,strText);

            int nID=xmlItem.attribute(L"id").as_int(0);
            BOOL bCheck=xmlItem.attribute(L"check").as_bool(false);
            BOOL bRadio=xmlItem.attribute(L"radio").as_bool(false);
            BOOL bDisable=xmlItem.attribute(L"disable").as_bool(false);


            pugi::xml_writer_buff writer;
            xmlItem.print(writer,L"\t",pugi::format_default,pugi::encoding_utf16);
            SStringW str(writer.buffer(),writer.size());

            pugi::xml_node xmlChild=xmlItem.first_child();
            while(xmlChild && xmlChild.type()==pugi::node_pcdata) xmlChild=xmlChild.next_sibling();


            if(!xmlChild && !xmlItem.attribute(L"popup").as_bool(false))
            {
                UINT uFlag=MF_OWNERDRAW;
                if(bCheck) uFlag|=MF_CHECKED;
                if(bDisable) uFlag |= MF_GRAYED;
                if(bRadio) uFlag |= MFT_RADIOCHECK|MF_CHECKED;
				::AppendMenu(menuPopup,uFlag,(UINT_PTR)nID,(LPCTSTR)pdmmi);
            }
            else
            {
                HMENU hSubMenu=::CreatePopupMenu();
				SMenuAttr * pMenuAttr = GetMenuAttr(m_hMenu);
				SetMenuAttr(hSubMenu,pMenuAttr);
				SetMenuContextHelpId(hSubMenu,xmlItem.attribute(L"contextHelpId").as_uint(0));

                UINT uFlag=MF_OWNERDRAW|MF_POPUP;
                if(bDisable) uFlag |= MF_GRAYED;
				::AppendMenu(menuPopup,uFlag,(UINT_PTR)hSubMenu,(LPCTSTR)pdmmi);
                BuildMenu(hSubMenu,xmlItem);//build sub menu
            }
        }
        else if(wcscmp(L"sep",xmlItem.name())==0)
        {
			::AppendMenu(menuPopup,MF_SEPARATOR|MF_OWNERDRAW,(UINT_PTR)0,(LPCTSTR)NULL);
        }
        xmlItem=xmlItem.next_sibling();
    }
}


void SMenu::FreeMenuItemData(HMENU hMemu)
{
	SMenuAttr *pMenuAttr = GetMenuAttr(hMemu);
	pMenuAttr->Release();

	int cItems = ::GetMenuItemCount(hMemu);
	for(int i=0;i<cItems;i++)
	{
		MENUITEMINFO mii = {sizeof(mii),MIIM_DATA|MIIM_SUBMENU,0};
		GetMenuItemInfo(hMemu,i,TRUE,&mii);
		if(mii.dwItemData)
		{
			SMenuItemData * pData = (SMenuItemData*)mii.dwItemData;
			delete pData;
		}
		if(mii.hSubMenu)
		{
			FreeMenuItemData(mii.hSubMenu);
		}
	}
}

void SMenu::DestroyMenu()
{
	if(::IsMenu(m_hMenu))
	{
		//free item data
		FreeMenuItemData(m_hMenu);
		::DestroyMenu(m_hMenu);
	}
}

BOOL SMenu::ModifyMenuString(UINT uPosition, UINT uFlags,LPCTSTR lpItemString)
{
	MENUITEMINFO mi={sizeof(mi),0};
	mi.fMask = MIIM_DATA;
	if(!::GetMenuItemInfo(m_hMenu,uPosition,uFlags&MF_BYPOSITION,&mi))
		return FALSE;
	SMenuItemData * pItemData =(SMenuItemData*)mi.dwItemData;
	pItemData->strText = lpItemString;
	return TRUE;
}

BOOL SMenu::DeleteMenu(UINT uPosition, UINT uFlags)
{
	MENUITEMINFO mi={sizeof(mi),MIIM_DATA,0};
	if(!::GetMenuItemInfo(m_hMenu,uPosition,uFlags&MF_BYPOSITION,&mi))
		return FALSE;
	SMenuItemData * pItemData =(SMenuItemData*)mi.dwItemData;
	if(::DeleteMenu(m_hMenu,uPosition,uFlags))
	{
		delete pItemData;
		return TRUE;
	}
	return FALSE;
}


BOOL SMenu::AppendMenu(UINT uFlags,UINT_PTR uIDNewItem, LPCTSTR lpNewItem,int iIcon)
{
	return InsertMenu(-1,uFlags,uIDNewItem,lpNewItem,iIcon);
}

SMenuAttr * SMenu::GetMenuAttr(HMENU hMenu) const
{
	SASSERT(::IsMenu(hMenu));
	MENUINFO mi = {sizeof(mi),MIM_MENUDATA,0};
	GetMenuInfo(hMenu,&mi);
	return (SMenuAttr *)mi.dwMenuData;
}

void SMenu::SetMenuAttr(HMENU hMenu,SMenuAttr *pMenuAttr) const
{
	MENUINFO mi = {sizeof(mi),MIM_MENUDATA,0};
	mi.dwMenuData = (ULONG_PTR)pMenuAttr;
	pMenuAttr->AddRef();
	::SetMenuInfo(hMenu,&mi);
}

BOOL SMenu::Attach(HMENU hMenu)
{
	SMenuAttr * pMenuAttr = GetMenuAttr(hMenu);
	if(IsBadReadPtr(pMenuAttr,sizeof(SMenuAttr)))
		return FALSE;
	m_hMenu = hMenu;
	return TRUE;
}

HMENU SMenu::Detach()
{
	HMENU hRet = m_hMenu;
	m_hMenu = NULL;
	return hRet;
}

BOOL SMenu::SetMenuUserData(UINT uPosition, UINT uFlags,ULONG_PTR ulUserData)
{
	MENUITEMINFO mi={sizeof(mi),MIIM_DATA,0};
	if(!::GetMenuItemInfo(m_hMenu,uPosition,uFlags,&mi))
		return FALSE;
	SMenuItemData *pmid = (SMenuItemData*)mi.dwItemData;
	pmid->dwUserData = ulUserData;
	return TRUE;
}

ULONG_PTR SMenu::GetMenuUserData(UINT uPosition, UINT uFlags)
{
	MENUITEMINFO mi={sizeof(mi),MIIM_DATA,0};
	if(!::GetMenuItemInfo(m_hMenu,uPosition,uFlags,&mi))
		return 0;
	SMenuItemData *pmid = (SMenuItemData*)mi.dwItemData;
	return pmid->dwUserData;
}


}//namespace SOUI