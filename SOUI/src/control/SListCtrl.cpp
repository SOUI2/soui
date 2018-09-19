#include "souistd.h"

#include "control/SListCtrl.h"
#include <algorithm>

#pragma warning(disable : 4267 4018)

#define ITEM_MARGIN 4
namespace SOUI
{
//////////////////////////////////////////////////////////////////////////
//  SListCtrl
SListCtrl::SListCtrl()
    : m_nHeaderHeight(20)
    , m_nItemHeight(20)
    , m_pHeader(NULL)
    , m_nSelectItem(-1)
    , m_crItemBg(RGBA(255,255,255,255))
    , m_crItemBg2(RGBA(226,226,226,255))
	, m_crItemSelBg(RGBA(57,145,209,255))
	, m_crItemHotBg(RGBA(57,145,209,128))
    , m_crText(RGBA(0,0,0,255))
    , m_crSelText(RGBA(255,255,0,255))
    , m_pItemSkin(NULL)
    , m_pIconSkin(NULL)
    , m_pCheckSkin(GETBUILTINSKIN(SKIN_SYS_CHECKBOX))
    , m_ptIcon(-1,-1)
    , m_ptText(-1,-1)
    , m_bHotTrack(FALSE)
    , m_bCheckBox(FALSE)
    , m_bMultiSelection(FALSE)
{
    m_bClipClient = TRUE;
    m_bFocusable = TRUE;
    m_evtSet.addEvent(EVENTID(EventLCSelChanging));
    m_evtSet.addEvent(EVENTID(EventLCSelChanged));
    m_evtSet.addEvent(EVENTID(EventLCDbClick));
	m_evtSet.addEvent(EVENTID(EventLCItemDeleted));
}

SListCtrl::~SListCtrl()
{
}

int SListCtrl::InsertColumn(int nIndex, LPCTSTR pszText, int nWidth, LPARAM lParam)
{
    SASSERT(m_pHeader);

    int nRet = m_pHeader->InsertItem(nIndex, pszText, nWidth, ST_NULL, lParam);
    for(int i=0;i<GetItemCount();i++)
    {
        m_arrItems[i].arSubItems->SetCount(GetColumnCount());
    }
    UpdateScrollBar();
    return nRet;
}

BOOL SListCtrl::CreateChildren(pugi::xml_node xmlNode)
{
    //  listctrl的子控件只能是一个header控件
    if (!__super::CreateChildren(xmlNode))
        return FALSE;
    m_pHeader=NULL;
    
    SWindow *pChild=GetWindow(GSW_FIRSTCHILD);
    while(pChild)
    {
        if(pChild->IsClass(SHeaderCtrl::GetClassName()))
        {
            m_pHeader=(SHeaderCtrl*)pChild;
            break;
        }
        pChild=pChild->GetWindow(GSW_NEXTSIBLING);
    }
    if(!m_pHeader) return FALSE;
        
    SStringW strPos;
    strPos.Format(L"0,0,-0,%d",m_nHeaderHeight);
    m_pHeader->SetAttribute(L"pos",strPos,TRUE);

    m_pHeader->GetEventSet()->subscribeEvent(EventHeaderItemChanging::EventID, Subscriber(&SListCtrl::OnHeaderSizeChanging,this));
    m_pHeader->GetEventSet()->subscribeEvent(EventHeaderItemSwap::EventID, Subscriber(&SListCtrl::OnHeaderSwap,this));

    return TRUE;
}

int SListCtrl::InsertItem(int nItem, LPCTSTR pszText, int nImage)
{
    if(GetColumnCount()==0) return -1;
    if (nItem<0 || nItem>GetItemCount())
        nItem = GetItemCount();

    DXLVITEM lvi;
    lvi.dwData = 0;
    lvi.arSubItems=new ArrSubItem();
    lvi.arSubItems->SetCount(GetColumnCount());
    
    DXLVSUBITEM &subItem=lvi.arSubItems->GetAt(0);
    subItem.strText = _tcsdup(pszText);
    subItem.cchTextMax = _tcslen(pszText);
    subItem.nImage  = nImage;

    m_arrItems.InsertAt(nItem, lvi);

    UpdateScrollBar();

    return nItem;
}

BOOL SListCtrl::SetItemData(int nItem, LPARAM dwData)
{
    if (nItem >= GetItemCount())
        return FALSE;

    m_arrItems[nItem].dwData = dwData;

    return TRUE;
}

LPARAM SListCtrl::GetItemData(int nItem)
{
    if (nItem >= GetItemCount())
        return 0;

    DXLVITEM& lvi = m_arrItems[nItem];

    return lvi.dwData;
}

BOOL SListCtrl::SetSubItem(int nItem, int nSubItem, const DXLVSUBITEM* plv)
{
    if (nItem>=GetItemCount() || nSubItem>=GetColumnCount())
        return FALSE;
    DXLVSUBITEM & lvsi_dst=m_arrItems[nItem].arSubItems->GetAt(nSubItem);
    if(plv->mask & S_LVIF_TEXT)
    {
        if(lvsi_dst.strText) free(lvsi_dst.strText);
        lvsi_dst.strText=_tcsdup(plv->strText);
        lvsi_dst.cchTextMax=_tcslen(plv->strText);
    }
    if(plv->mask&S_LVIF_IMAGE)
        lvsi_dst.nImage=plv->nImage;
    if(plv->mask&S_LVIF_INDENT)
        lvsi_dst.nIndent=plv->nIndent;
    RedrawItem(nItem);
    return TRUE;
}

BOOL SListCtrl::GetSubItem(int nItem, int nSubItem, DXLVSUBITEM* plv) const
{
    if (nItem>=GetItemCount() || nSubItem>=GetColumnCount())
        return FALSE;

    const DXLVSUBITEM & lvsi_src=m_arrItems[nItem].arSubItems->GetAt(nSubItem);
    if(plv->mask & S_LVIF_TEXT)
    {
        _tcscpy_s(plv->strText,plv->cchTextMax,lvsi_src.strText);
    }
    if(plv->mask&S_LVIF_IMAGE)
        plv->nImage=lvsi_src.nImage;
    if(plv->mask&S_LVIF_INDENT)
        plv->nIndent=lvsi_src.nIndent;
    return TRUE;
}

BOOL SListCtrl::SetSubItemText(int nItem, int nSubItem, LPCTSTR pszText)
{
    if (nItem < 0 || nItem >= GetItemCount())
        return FALSE;

    if (nSubItem < 0 || nSubItem >= GetColumnCount())
        return FALSE;

    DXLVSUBITEM &lvi=m_arrItems[nItem].arSubItems->GetAt(nSubItem);
    if(lvi.strText)
    {
        free(lvi.strText);
    }
    lvi.strText = _tcsdup(pszText);
    lvi.cchTextMax= _tcslen(pszText);
    
    CRect rcItem=GetItemRect(nItem,nSubItem);
    InvalidateRect(rcItem);
    return TRUE;
}


SStringT SListCtrl::GetSubItemText( int nItem, int nSubItem ) const
{
    if (nItem>=GetItemCount() || nSubItem>=GetColumnCount())
        return _T("");

    const DXLVSUBITEM & lvsi_src=m_arrItems[nItem].arSubItems->GetAt(nSubItem);
    return lvsi_src.strText;
}

int SListCtrl::GetSelectedItem()
{
    return m_nSelectItem;
}

void SListCtrl::SetSelectedItem(int nItem)
{
	if (nItem != m_nSelectItem)
	{
		NotifySelChange(m_nSelectItem, nItem);
	}
}

int SListCtrl::GetItemCount() const
{
    if (GetColumnCount() <= 0)
        return 0;

    return m_arrItems.GetCount();
}

BOOL SListCtrl::SetItemCount( int nItems ,int nGrowBy)
{
    int nOldCount=GetItemCount();
    if(nItems<nOldCount) return FALSE;
    
    BOOL bRet=m_arrItems.SetCount(nItems,nGrowBy);
    if(bRet)
    {
        for(int i=nOldCount;i<nItems;i++)
        {
            DXLVITEM & lvi=m_arrItems[i];
            lvi.arSubItems=new ArrSubItem;
            lvi.arSubItems->SetCount(GetColumnCount());
        }
    }
    UpdateScrollBar();

    return bRet;
}

CRect SListCtrl::GetListRect()
{
    CRect rcList;

    GetClientRect(&rcList);
    rcList.top += m_nHeaderHeight;

    return rcList;
}

//////////////////////////////////////////////////////////////////////////
//  更新滚动条
void SListCtrl::UpdateScrollBar()
{
    CSize szView;
    szView.cx = m_pHeader->GetTotalWidth();
    szView.cy = GetItemCount()*m_nItemHeight;

    CRect rcClient;
    SWindow::GetClientRect(&rcClient);//不计算滚动条大小
    rcClient.top+=m_nHeaderHeight;

    CSize size = rcClient.Size();
    //  关闭滚动条
    m_wBarVisible = SSB_NULL;

    if (size.cy<szView.cy || (size.cy<szView.cy+GetSbWidth() && size.cx<szView.cx))
    {
        //  需要纵向滚动条
        m_wBarVisible |= SSB_VERT;
        m_siVer.nMin  = 0;
        m_siVer.nMax  = szView.cy-1;
        m_siVer.nPage = GetCountPerPage(FALSE)*m_nItemHeight;

        if (size.cx-GetSbWidth() < szView.cx)
        {
            //  需要横向滚动条
            m_wBarVisible |= SSB_HORZ;
            m_siVer.nPage=size.cy-GetSbWidth() > 0 ? size.cy-GetSbWidth() : 0;//注意同时调整纵向滚动条page信息

            m_siHoz.nMin  = 0;
            m_siHoz.nMax  = szView.cx-1;
            m_siHoz.nPage = size.cx-GetSbWidth() > 0 ? size.cx-GetSbWidth() : 0;
        }
        else
        {
            //  不需要横向滚动条
            m_siHoz.nPage = size.cx;
            m_siHoz.nMin  = 0;
            m_siHoz.nMax  = m_siHoz.nPage-1;
            m_siHoz.nPos  = 0;
            m_ptOrigin.x  = 0;
        }
    }
    else
    {
        //  不需要纵向滚动条
        m_siVer.nPage = size.cy;
        m_siVer.nMin  = 0;
        m_siVer.nMax  = size.cy-1;
        m_siVer.nPos  = 0;
        m_ptOrigin.y  = 0;

        if (size.cx < szView.cx)
        {
            //  需要横向滚动条
            m_wBarVisible |= SSB_HORZ;
            m_siHoz.nMin  = 0;
            m_siHoz.nMax  = szView.cx-1;
            m_siHoz.nPage = size.cx;
        }
        else
        {
            //  不需要横向滚动条
            m_siHoz.nPage = size.cx;
            m_siHoz.nMin  = 0;
            m_siHoz.nMax  = m_siHoz.nPage-1;
            m_siHoz.nPos  = 0;
            m_ptOrigin.x  = 0;
        }
    }

    SetScrollPos(TRUE, m_siVer.nPos, TRUE);
    SetScrollPos(FALSE, m_siHoz.nPos, TRUE);

    //  重新计算客户区及非客户区
    SSendMessage(WM_NCCALCSIZE);

    //  根据需要调整原点位置
    if (HasScrollBar(FALSE) && m_ptOrigin.x+m_siHoz.nPage>szView.cx)
    {
        m_ptOrigin.x = szView.cx-m_siHoz.nPage;
    }

    if (HasScrollBar(TRUE) && m_ptOrigin.y+m_siVer.nPage>szView.cy)
    {
        m_ptOrigin.y = szView.cy-m_siVer.nPage;
    }

    Invalidate();
}

//更新表头位置
void SListCtrl::UpdateHeaderCtrl()
{
    CRect rcClient;
    GetClientRect(&rcClient);
    CRect rcHeader(rcClient);
    rcHeader.bottom=rcHeader.top+m_nHeaderHeight;
    rcHeader.left-=m_ptOrigin.x;
    if(m_pHeader) m_pHeader->Move(rcHeader);
}

void SListCtrl::DeleteItem(int nItem)
{
    if (nItem>=0 && nItem < GetItemCount())
    {
        DXLVITEM &lvi=m_arrItems[nItem];

		EventLCItemDeleted evt2(this);
		evt2.nItem = nItem;
		evt2.dwData = lvi.dwData;
		FireEvent(evt2);

        for(int i=0;i<GetColumnCount();i++)
        {
            DXLVSUBITEM &lvsi =lvi.arSubItems->GetAt(i);
            if(lvsi.strText) free(lvsi.strText);
        }
        delete lvi.arSubItems;
        m_arrItems.RemoveAt(nItem);

        UpdateScrollBar();
    }
}

void SListCtrl::DeleteColumn( int iCol )
{
    if(m_pHeader->DeleteItem(iCol))
    {
		int nColumnCount = m_pHeader->GetItemCount();

        for(int i=0;i<GetItemCount();i++)
        {
			DXLVITEM &lvi = m_arrItems[i];

			if (0 == nColumnCount)
			{
				EventLCItemDeleted evt2(this);
				evt2.nItem = i;
				evt2.dwData = lvi.dwData;
				FireEvent(evt2);
			}

            DXLVSUBITEM &lvsi=lvi.arSubItems->GetAt(iCol);
            if(lvsi.strText) free(lvsi.strText);
            m_arrItems[i].arSubItems->RemoveAt(iCol);
        }
        UpdateScrollBar();
    }
}

void SListCtrl::DeleteAllItems()
{
	m_nSelectItem = -1;
    for(int i=0;i<GetItemCount();i++)
    {
		DXLVITEM &lvi = m_arrItems[i];

		EventLCItemDeleted evt2(this);
		evt2.nItem = i;
		evt2.dwData = lvi.dwData;
		FireEvent(evt2);

        for(int j=0;j<GetColumnCount();j++)
        {
            DXLVSUBITEM &lvsi =lvi.arSubItems->GetAt(j);
            if(lvsi.strText) free(lvsi.strText);
        }
        delete lvi.arSubItems;
    }
    m_arrItems.RemoveAll();

    UpdateScrollBar();
}


CRect SListCtrl::GetItemRect(int nItem, int nSubItem)
{
    if (!(nItem>=0 && nItem<GetItemCount() && nSubItem>=0 && nSubItem<GetColumnCount()))
        return CRect();

    CRect rcItem;
    rcItem.top    = m_nItemHeight*nItem;
    rcItem.bottom = rcItem.top+m_nItemHeight;
    rcItem.left   = 0;
    rcItem.right  = 0;

    for (int nCol = 0; nCol < GetColumnCount(); nCol++)
    {
        SHDITEM hdi;
        hdi.mask = SHDI_WIDTH|SHDI_ORDER;

        m_pHeader->GetItem(nCol, &hdi);
        rcItem.left  = rcItem.right;
        rcItem.right = rcItem.left+hdi.cx.toPixelSize(GetScale());
        if (hdi.iOrder == nSubItem)
            break;
    }

    CRect rcList = GetListRect();
    //  变换到窗口坐标
    rcItem.OffsetRect(rcList.TopLeft());
    //  根据原点坐标修正
    rcItem.OffsetRect(-m_ptOrigin);

    return rcItem;
}

//////////////////////////////////////////////////////////////////////////
//  自动修改pt的位置为相对当前项的偏移量
int SListCtrl::HitTest(const CPoint& pt)
{
    CRect rcList = GetListRect();

    CPoint pt2 = pt;
    pt2.y -= rcList.top - m_ptOrigin.y;

    int nRet = pt2.y / m_nItemHeight;
    if (nRet >= GetItemCount())
    {
        nRet = -1;
    }

    return nRet;
}

void SListCtrl::RedrawItem(int nItem)
{
    if (!IsVisible(TRUE))
        return;

    CRect rcList = GetListRect();

    int nTopItem = GetTopIndex();
    int nPageItems    = (rcList.Height()+m_nItemHeight-1)/m_nItemHeight;

    if (nItem>=nTopItem && nItem<GetItemCount() && nItem<=nTopItem+nPageItems)
    {
        CRect rcItem(0,0,rcList.Width(),m_nItemHeight);
        rcItem.OffsetRect(0, m_nItemHeight*nItem-m_ptOrigin.y);
        rcItem.OffsetRect(rcList.TopLeft());
        CRect rcDC;
        rcDC.IntersectRect(rcItem,rcList);
        IRenderTarget *pRT = GetRenderTarget(&rcDC, OLEDC_PAINTBKGND);
        SSendMessage(WM_ERASEBKGND, (WPARAM)pRT);

        DrawItem(pRT, rcItem, nItem);

        ReleaseRenderTarget(pRT);
    }
}

int SListCtrl::GetCountPerPage(BOOL bPartial)
{
    CRect rcClient = GetListRect();

    // calculate number of items per control height (include partial item)
    div_t divHeight = div(rcClient.Height(), m_nItemHeight);

    // round up to nearest item count
    return (std::max)((int)(bPartial && divHeight.rem > 0 ? divHeight.quot + 1 : divHeight.quot), 1);
}
BOOL SListCtrl::SortItems(
               PFNLVCOMPAREEX pfnCompare,
               void * pContext 
               )
{
    qsort_s(m_arrItems.GetData(),m_arrItems.GetCount(),sizeof(DXLVITEM),pfnCompare,pContext);
    m_nSelectItem=-1;
    m_nHoverItem=-1;
    InvalidateRect(GetListRect());
    return TRUE;
}

void SListCtrl::OnPaint(IRenderTarget * pRT)
{
    SPainter painter;
    BeforePaint(pRT, painter);
    CRect rcList = GetListRect();
    int nTopItem = GetTopIndex();
    pRT->PushClipRect(&rcList);
    CRect rcItem(rcList);

    rcItem.bottom = rcItem.top;
    rcItem.OffsetRect(0,-(m_ptOrigin.y%m_nItemHeight));
    for (int nItem = nTopItem; nItem <= (nTopItem+GetCountPerPage(TRUE)) && nItem<GetItemCount(); rcItem.top = rcItem.bottom, nItem++)
    {
        rcItem.bottom = rcItem.top + m_nItemHeight;

        DrawItem(pRT, rcItem, nItem);
    }
    pRT->PopClip();
    AfterPaint(pRT, painter);
}

BOOL SListCtrl::HitCheckBox(const CPoint& pt)
{
    if (!m_bCheckBox)
        return FALSE;

    CRect rect = GetListRect();
    rect.left += ITEM_MARGIN;
    rect.OffsetRect(-m_ptOrigin.x,0);

    CSize sizeSkin = m_pCheckSkin->GetSkinSize();
    int nOffsetX = 3;
    CRect rcCheck;
    rcCheck.SetRect(0, 0, sizeSkin.cx, sizeSkin.cy);
    rcCheck.OffsetRect(rect.left + nOffsetX, 0);

    if (pt.x >= rcCheck.left && pt.x <= rcCheck.right)
        return TRUE;
    return FALSE;
}

void SListCtrl::DrawItem(IRenderTarget * pRT, CRect rcItem, int nItem)
{
    BOOL bTextColorChanged = FALSE;
    int nBgImg = 0;
    COLORREF crOldText=RGBA(0xFF,0xFF,0xFF,0xFF);
    COLORREF crItemBg = m_crItemBg;
    COLORREF crText = m_crText;
    DXLVITEM lvItem = m_arrItems[nItem];
    CRect rcIcon, rcText;


	if (nItem % 2)
	{
		//         if (m_pItemSkin != NULL)
		//             nBgImg = 1;
		//         else if (CR_INVALID != m_crItemBg2)
		//             crItemBg = m_crItemBg2;
		//上面的代码不要了，因为skin间隔效果没必要，只留下颜色间隔就好了
		if (CR_INVALID != m_crItemBg2)
			crItemBg = m_crItemBg2;
	}

 

	if ( lvItem.checked) 
	{//和下面那个if的条件分开，才会有sel和hot的区别
		if (m_pItemSkin != NULL)
			nBgImg = 2;
		else if (CR_INVALID != m_crItemSelBg)
			crItemBg = m_crItemSelBg;

		if (CR_INVALID != m_crSelText)
			crText = m_crSelText;
	}
	else if  (m_bHotTrack && nItem == m_nHoverItem)
	{
		if (m_pItemSkin != NULL)
			nBgImg = 1;
		else if (CR_INVALID != m_crItemHotBg)
			crItemBg = m_crItemHotBg;

		if (CR_INVALID != m_crSelText)
			crText = m_crSelText;
	}

	//绘制背景
	//     if (m_pItemSkin != NULL)
	//         m_pItemSkin->Draw(pRT, rc, nBgImg);
	//     else if (CR_INVALID != crItemBg)
	//         pRT->FillSolidRect( rc, crItemBg);
	//上面的代码在某些时候，【指定skin的时候，会导致背景异常】，所以颠倒一下顺序
	if (CR_INVALID != crItemBg)//先画背景
		pRT->FillSolidRect( rcItem, crItemBg);

	if (m_pItemSkin != NULL)//有skin，则覆盖背景
		m_pItemSkin->Draw(pRT, rcItem, nBgImg); 

    //  左边加上空白
	rcItem.left += ITEM_MARGIN;

    if (CR_INVALID != crText)
    {
        bTextColorChanged = TRUE;
        crOldText = pRT->SetTextColor(crText);
    }

    CRect rcCol(rcItem);
    rcCol.right = rcCol.left;
    rcCol.OffsetRect(-m_ptOrigin.x,0);

    for (int nCol = 0; nCol < GetColumnCount(); nCol++)
    {
        CRect rcVisiblePart;

        SHDITEM hdi;
        hdi.mask=SHDI_WIDTH|SHDI_ORDER;
        m_pHeader->GetItem(nCol,&hdi);
        rcCol.left=rcCol.right;
        rcCol.right = rcCol.left + hdi.cx.toPixelSize(GetScale());

        rcVisiblePart.IntersectRect(rcItem, rcCol);

        if (rcVisiblePart.IsRectEmpty())
            continue;

        // 绘制 checkbox
        if (nCol == 0 && m_bCheckBox && m_pCheckSkin)
        {
            CSize sizeSkin = m_pCheckSkin->GetSkinSize();
            int nOffsetX = 3;
            int nOffsetY = (m_nItemHeight - sizeSkin.cy) / 2;
            CRect rcCheck;
            rcCheck.SetRect(0, 0, sizeSkin.cx, sizeSkin.cy);
            rcCheck.OffsetRect(rcCol.left + nOffsetX, rcCol.top + nOffsetY);
            m_pCheckSkin->Draw(pRT, rcCheck, lvItem.checked ? 4 : 0);

            rcCol.left = sizeSkin.cx + 6 + rcCol.left;
        }

        DXLVSUBITEM& subItem = lvItem.arSubItems->GetAt(hdi.iOrder);

        if (subItem.nImage != -1 && m_pIconSkin)
        {
            int nOffsetX = m_ptIcon.x;
            int nOffsetY = m_ptIcon.y;
            CSize sizeSkin = m_pIconSkin->GetSkinSize();
            rcIcon.SetRect(0, 0, sizeSkin.cx, sizeSkin.cy);

            if (m_ptIcon.x == -1)
                nOffsetX = m_nItemHeight / 6;

            if (m_ptIcon.y == -1)
                nOffsetY = (m_nItemHeight - sizeSkin.cy) / 2;

            rcIcon.OffsetRect(rcCol.left + nOffsetX, rcCol.top + nOffsetY);
            m_pIconSkin->Draw(pRT, rcIcon, subItem.nImage);
        }

        UINT align = DT_SINGLELINE;
        rcText = rcCol;

        if (m_ptText.x == -1)
            rcText.left = rcIcon.Width() > 0 ? rcIcon.right + m_nItemHeight / 6 : rcCol.left;
        else
            rcText.left = rcCol.left + m_ptText.x;

        if (m_ptText.y == -1)
            align |= DT_VCENTER;
        else
            rcText.top = rcCol.top + m_ptText.y;

        pRT->DrawText(subItem.strText, subItem.cchTextMax, rcText, align);
    }

    if (bTextColorChanged)
        pRT->SetTextColor(crOldText);
}

void SListCtrl::OnDestroy()
{
    DeleteAllItems();

    __super::OnDestroy();
}

int SListCtrl::GetColumnCount() const
{
    if (!m_pHeader)
        return 0;

    return m_pHeader->GetItemCount();
}

int SListCtrl::GetTopIndex() const
{
    return m_ptOrigin.y / m_nItemHeight;
}

void SListCtrl::NotifySelChange(int nOldSel, int nNewSel, BOOL checkBox)
{
    EventLCSelChanging evt1(this);
	evt1.bCancel = FALSE;
    evt1.nOldSel=nOldSel;
    evt1.nNewSel=nNewSel;

    FireEvent(evt1);
    if(evt1.bCancel) return;

    if (checkBox) {
            DXLVITEM &newItem = m_arrItems[nNewSel];
            newItem.checked = newItem.checked? FALSE:TRUE;
            m_nSelectItem = nNewSel;
            RedrawItem(nNewSel);
    } else  {
        if ((m_bMultiSelection || m_bCheckBox) && GetKeyState(VK_CONTROL) < 0) {
            if (nNewSel != -1) {
                DXLVITEM &newItem = m_arrItems[nNewSel];
                newItem.checked = newItem.checked? FALSE:TRUE;
                m_nSelectItem = nNewSel;
                RedrawItem(nNewSel);
            }
        } else if ((m_bMultiSelection || m_bCheckBox) && GetKeyState(VK_SHIFT) < 0) {
            if (nNewSel != -1) {
                if (nOldSel == -1)
                    nOldSel = 0;

                int imax = (nOldSel > nNewSel) ? nOldSel : nNewSel;
                int imin = (imax == nOldSel) ? nNewSel : nOldSel;
                for (int i = 0; i < GetItemCount(); i++)
                {
                    DXLVITEM &lvItem = m_arrItems[i];
                    BOOL last = lvItem.checked;
                    if (i >= imin && i<= imax) {
                        lvItem.checked = TRUE;
                    } else {
                        lvItem.checked = FALSE;
                    }
                    if (last != lvItem.checked)
                        RedrawItem(i);
                }
            }
        } else {
            m_nSelectItem = -1;
            for (int i = 0; i < GetItemCount(); i++)
            {
                DXLVITEM &lvItem = m_arrItems[i];
                if (i != nNewSel && lvItem.checked) 
                {
                    BOOL last = lvItem.checked;
                    lvItem.checked = FALSE;
                    if (last != lvItem.checked)
                        RedrawItem(i);
                }
            }
            if (nNewSel != -1) {
                DXLVITEM &newItem = m_arrItems[nNewSel];
                newItem.checked = TRUE;
                m_nSelectItem = nNewSel;
                RedrawItem(nNewSel);
            }
        }
    }
    
    EventLCSelChanged evt2(this);
    evt2.nOldSel=nOldSel;
    evt2.nNewSel=nNewSel;
    FireEvent(evt2);
}

BOOL SListCtrl::OnScroll(BOOL bVertical, UINT uCode, int nPos)
{
    BOOL bRet = __super::OnScroll(bVertical, uCode, nPos);

    if (bVertical)
    {
        m_ptOrigin.y = m_siVer.nPos;
    }
    else
    {
        m_ptOrigin.x = m_siHoz.nPos;
        //  处理列头滚动
        UpdateHeaderCtrl();
    }

    Invalidate();
    if (uCode==SB_THUMBTRACK)
        ScrollUpdate();

    return bRet;
}

void SListCtrl::OnLButtonDown(UINT nFlags, CPoint pt)
{
    __super::OnLButtonDown(nFlags,pt);
    m_nHoverItem = HitTest(pt);
    BOOL hitCheckBox = HitCheckBox(pt);

    if (hitCheckBox)
        NotifySelChange(m_nSelectItem, m_nHoverItem, TRUE);
    else if (m_nHoverItem!=m_nSelectItem && !m_bHotTrack)
        NotifySelChange(m_nSelectItem, m_nHoverItem);
    else if (m_nHoverItem != -1 || m_nSelectItem != -1)
        NotifySelChange(m_nSelectItem, m_nHoverItem);
}

void SListCtrl::OnLButtonDbClick(UINT nFlags, CPoint pt)
{
	m_nHoverItem = HitTest(pt);
	if (m_nHoverItem != m_nSelectItem)
		NotifySelChange(m_nSelectItem, m_nHoverItem);

	EventLCDbClick evt2(this);
	evt2.nCurSel = m_nHoverItem;
	FireEvent(evt2);
}

void SListCtrl::OnLButtonUp(UINT nFlags, CPoint pt)
{
    __super::OnLButtonUp(nFlags,pt);
}


void SListCtrl::UpdateChildrenPosition()
{
    __super::UpdateChildrenPosition();
    UpdateHeaderCtrl();
}

void SListCtrl::OnSize(UINT nType, CSize size)
{
    __super::OnSize(nType,size);
    UpdateScrollBar();
    UpdateHeaderCtrl();
}

bool SListCtrl::OnHeaderClick(EventArgs *pEvt)
{
    return true;
}

bool SListCtrl::OnHeaderSizeChanging(EventArgs *pEvt)
{
    UpdateScrollBar();
    InvalidateRect(GetListRect());

    return true;
}

bool SListCtrl::OnHeaderSwap(EventArgs *pEvt)
{
    InvalidateRect(GetListRect());

    return true;
}

void SListCtrl::OnMouseMove( UINT nFlags, CPoint pt )
{
    int nHoverItem = HitTest(pt);
    if(m_bHotTrack && nHoverItem != m_nHoverItem)
    {
        m_nHoverItem= nHoverItem;
        Invalidate();
    }
}

void SListCtrl::OnMouseLeave()
{
    if(m_bHotTrack)
    {
        m_nHoverItem=-1;
        Invalidate();
    }
    __super::OnMouseLeave();
}

BOOL SListCtrl::GetCheckState(int nItem)
{
    if (nItem >= GetItemCount())
        return FALSE;

    const DXLVITEM lvItem = m_arrItems[nItem];
    return lvItem.checked;
}

BOOL SListCtrl::SetCheckState(int nItem, BOOL bCheck)
{
    if (!m_bCheckBox) return FALSE;

    if (nItem >= GetItemCount())
        return FALSE;

    DXLVITEM &lvItem = m_arrItems[nItem];
    lvItem.checked = bCheck;

    return TRUE;
}

int SListCtrl::GetCheckedItemCount()
{
    int ret = 0;

    for (int i = 0; i < GetItemCount(); i++)
    {
        const DXLVITEM lvItem = m_arrItems[i];
        if (lvItem.checked)
            ret++;
    }

    return ret;
}

int SListCtrl::GetFirstCheckedItem()
{
    int ret = -1;
    for (int i = 0; i < GetItemCount(); i++)
    {
        const DXLVITEM lvItem = m_arrItems[i];
        if (lvItem.checked) {
            ret = i;
            break;
        }
    }

    return ret;
}

int SListCtrl::GetLastCheckedItem()
{
    int ret = -1;
    for (int i = GetItemCount() - 1; i >= 0; i--)
    {
        const DXLVITEM lvItem = m_arrItems[i];
        if (lvItem.checked) {
            ret = i;
            break;
        }
    }

    return ret;
}

}//end of namespace 
