//////////////////////////////////////////////////////////////////////////
//  Class Name: SlistCtrlEx
// Description: A DuiWindow Based ListCtrl Control. Can contain control as an item
//     Creator: soui
//     Version: 2015.2.5 - 1.0 - Create
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SlistCtrlex.h"
#include "helper/mybuffer.h"
#include "SListboxex.h"

#pragma warning(disable:4018)
#pragma warning(disable:4267)

namespace SOUI
{


	SListCtrlEx::SListCtrlEx()
		: m_nItemHeight(30)
		,m_nHeaderHeight(40)
		, m_pHeader(NULL)
		, m_iScrollSpeed(-1)
		, m_iSelItem(-1)
		, m_iHoverItem(-1)
		, m_pCapturedFrame(NULL)
		, m_pItemSkin(NULL)
		, m_strSelectRangeSkin(L"")
		, m_crItemBg(CR_INVALID)
		, m_crItemSelBg(CR_INVALID)
		, m_crItemHotBg(CR_INVALID)
		, m_bItemRedrawDelay(TRUE)
		,m_pWndRectangle(NULL)
		,m_ptTmp(0,0)
		,m_bMouseDown(FALSE)
		,m_bStartSelect(FALSE)
		, m_bCheckBox(FALSE)
		, m_bMultiSelection(FALSE)
	{
		m_bFocusable=TRUE;
		m_evtSet.addEvent(EVENTID(EventOfPanel));
		m_evtSet.addEvent(EVENTID(EventLBGetDispInfo));
		m_evtSet.addEvent(EVENTID(EventLBSelChanging));
		m_evtSet.addEvent(EVENTID(EventLBSelChanged));
	}

	SListCtrlEx::~SListCtrlEx()
	{
	}
	
	void SListCtrlEx::DeleteAllItems(BOOL bUpdate/*=TRUE*/)
	{
		for(int i=0; i<GetItemCount(); i++)
		{
			m_arrItems[i]->Release();
		}
		m_arrItems.RemoveAll();
		m_iSelItem=-1;
		m_iHoverItem=-1;
		m_pCapturedFrame=NULL;
		ReleaseCapture();

		SetViewSize(CSize(0,0));
		if(bUpdate) Invalidate();
	}

	void SListCtrlEx::DeleteItem(int iItem)
	{
		if(iItem<0 || iItem>=GetItemCount()) return;
		if(m_pCapturedFrame == m_arrItems[iItem])
		{
			m_pCapturedFrame=NULL;
			ReleaseCapture();
		}

		m_arrItems[iItem]->Release();
		m_arrItems.RemoveAt(iItem);

		if(m_iSelItem==iItem) m_iSelItem=-1;
		else if(m_iSelItem>iItem) m_iSelItem--;
		if(m_iHoverItem==iItem) m_iHoverItem=-1;
		else if(m_iHoverItem>iItem) m_iHoverItem--;

		UpdatePanelsIndex(iItem,-1);

		CRect rcClient;
		SWindow::GetClientRect(&rcClient);
		CSize szView(rcClient.Width(),GetItemCount()*m_nItemHeight);
		if(szView.cy>rcClient.Height()) szView.cx-=GetSbWidth();
		SetViewSize(szView);
	}

	int SListCtrlEx::InsertItem(int iItem,pugi::xml_node xmlNode,LPARAM dwData/*=0*/)
	{
		SItemPanel *pItemObj=SItemPanel::Create(this,xmlNode,this);

		if(iItem==-1 || iItem>=GetItemCount())
		{
			iItem=GetItemCount();
		}
		pItemObj->SetItemData(dwData);
		pItemObj->Move(CRect(0,0,m_pHeader->GetTotalWidth(),m_nItemHeight));
		if(m_pItemSkin) pItemObj->SetSkin(m_pItemSkin);
		pItemObj->SetColor(m_crItemBg,m_crItemSelBg);

		m_arrItems.InsertAt(iItem,pItemObj);

		if(m_iSelItem>=iItem) m_iSelItem++;
		if(m_iHoverItem>=iItem) m_iHoverItem++;

		UpdatePanelsIndex(iItem,-1);

		CRect rcClient;
		SWindow::GetClientRect(&rcClient);
		CSize szView(rcClient.Width(),GetItemCount()*m_nItemHeight);
		if(szView.cy>rcClient.Height()) szView.cx-=GetSbWidth();
		SetViewSize(szView);

		return iItem;
	}

	int SListCtrlEx::InsertItem(int iItem,LPCWSTR pszXml,LPARAM dwData/*=0*/)
	{
		if(pszXml)
		{
			pugi::xml_document xmlDoc;
			if(!xmlDoc.load_buffer(pszXml,wcslen(pszXml)*sizeof(wchar_t),pugi::parse_default,pugi::encoding_utf16)) return -1;
			return InsertItem(iItem,xmlDoc.first_child(),dwData);
		}else
		{
			pugi::xml_node xmlNode = m_xmlTempl.child(L"template");
			if(!xmlNode) return -1;
			return InsertItem(iItem,xmlNode,dwData);
		}
	}

	BOOL SListCtrlEx::SetCurSel(int iItem)
	{
		if(iItem>=GetItemCount()) return FALSE;
		if(iItem < 0 ) iItem =-1;

		if(m_iSelItem==iItem) return FALSE;
		int nOldSel=m_iSelItem;
		m_iSelItem=iItem;
		if(nOldSel!=-1)
		{
			m_arrItems[nOldSel]->ModifyItemState(0,WndState_Check);
			if(IsVisible(TRUE)) RedrawItem(nOldSel);
		}
		if(m_iSelItem!=-1)
		{
			m_arrItems[m_iSelItem]->ModifyItemState(WndState_Check,0);
			if(IsVisible(TRUE)) RedrawItem(m_iSelItem);
		}
		return TRUE;
	}

	void SListCtrlEx::EnsureVisible( int iItem )
	{
		if(iItem<0 || iItem>=GetItemCount()) return;
		int iFirstVisible=(m_ptOrigin.y + m_nItemHeight -1) / m_nItemHeight;
		CRect rcClient;
		GetClientRect(&rcClient);
		int nVisibleItems=rcClient.Height()/m_nItemHeight;
		if(iItem<iFirstVisible || iItem> iFirstVisible+nVisibleItems-1)
		{
			int nOffset=GetScrollPos(TRUE);
			if(iItem<iFirstVisible) nOffset=(iItem-iFirstVisible)*m_nItemHeight;
			else nOffset=(iItem - iFirstVisible-nVisibleItems +1)*m_nItemHeight;
			nOffset-=nOffset%m_nItemHeight;//让当前行刚好显示
			OnScroll(TRUE,SB_THUMBPOSITION,nOffset + GetScrollPos(TRUE));
		}
	}

	int SListCtrlEx::GetCurSel()
	{
		return m_iSelItem;
	}
	
	SWindow * SListCtrlEx::GetItemPanel(int iItem)
	{
		if(iItem<0 || iItem>= GetItemCount()) return NULL;
		return m_arrItems[iItem];
	}
	
	LPARAM SListCtrlEx::GetItemData(int iItem)
	{
		SASSERT(iItem>=0 || iItem< GetItemCount());
		return m_arrItems[iItem]->GetItemData();
	}

	void SListCtrlEx::SetItemData( int iItem,LPARAM lParam )
	{
		SASSERT(iItem>=0 || iItem< GetItemCount());
		m_arrItems[iItem]->SetItemData(lParam);
	}

	BOOL SListCtrlEx::SetItemCount(int nItems,LPCTSTR pszXmlTemplate)
	{
		if(m_arrItems.GetCount()!=0) return FALSE;
		pugi::xml_document xmlDoc;
		pugi::xml_node xmlTemplate = m_xmlTempl.child(L"template");
		if(pszXmlTemplate)
		{
			SStringA strUtf8=S_CT2A(pszXmlTemplate,CP_UTF8);
			if(!xmlDoc.load_buffer((LPCSTR)strUtf8,strUtf8.GetLength(),pugi::parse_default,pugi::encoding_utf8)) return FALSE;
			xmlTemplate = xmlDoc.first_child();
		}
		if(!xmlTemplate) 
			return FALSE;


		m_arrItems.SetCount(nItems);
		for(int i=0;i<nItems;i++)
		{
			SItemPanel *pItemObj= SItemPanel::Create(this,xmlTemplate,this);
			pItemObj->Move(CRect(0,0,m_rcClient.Width(),m_nItemHeight));
			if(m_pItemSkin) pItemObj->SetSkin(m_pItemSkin);
			pItemObj->SetColor(m_crItemBg,m_crItemSelBg);
			m_arrItems[i] = pItemObj;
			pItemObj->SetItemIndex(i);
		}

		CRect rcClient;
		SWindow::GetClientRect(&rcClient);
		CSize szView(rcClient.Width(),GetItemCount()*m_nItemHeight);
		if(szView.cy>rcClient.Height()) szView.cx-=GetSbWidth();
		SetViewSize(szView);

		return TRUE;
	}

	int SListCtrlEx::GetItemCount()
	{
		return m_arrItems.GetCount();
	}

	void SListCtrlEx::RedrawItem(int iItem)
	{

		if(!IsVisible(TRUE)) return;
		CRect rcHead;
		m_pHeader->GetClientRect(&rcHead);
		CRect rcList = GetListRect();
		rcList.right = rcList.left + rcHead.Width();
		int nTopItem = GetTopIndex();
		int nPageItems    = (rcList.Height()+m_nItemHeight-1)/m_nItemHeight;

		CRect rcClient;
		GetClientRect(&rcClient);
		CRect rcItem=GetItemRect(iItem);
		CRect rcInter;
		rcInter.IntersectRect(&rcClient,&rcItem);
		if(rcInter.IsRectEmpty()) return;
  
		IRenderTarget * pRT=GetRenderTarget(&rcItem,OLEDC_PAINTBKGND);

		SSendMessage(WM_ERASEBKGND,(WPARAM)pRT);
		DrawItem(pRT,rcItem,iItem);

		ReleaseRenderTarget(pRT);
	}

	//自动修改pt的位置为相对当前项的偏移量
	int SListCtrlEx::HitTest(CPoint &pt)
	{
		CRect rcHead;
		m_pHeader->GetClientRect(&rcHead);
		CRect rcClient = GetListRect();
		rcClient.right = rcClient.left + rcHead.Width();

		CPoint pt2=pt;
		pt2.y -= rcClient.top - m_ptOrigin.y;
		int nRet=pt2.y/m_nItemHeight;
		if(nRet >= GetItemCount() || nRet < 0) nRet=-1;
		else
		{
			pt.x-=rcClient.left - m_ptOrigin.x;
			pt.y=pt2.y%m_nItemHeight;
		}

		return nRet;
	}
	
	void SListCtrlEx::OnPaint(IRenderTarget * pRT)
	{
		SPainter painter;
		BeforePaint(pRT, painter);
		CRect rcList = GetListRect();
		CRect rcHead;
		m_pHeader->GetClientRect(&rcHead);
		rcList.right = rcList.left + rcHead.Width();
		int nTopItem = GetTopIndex();
		pRT->PushClipRect(&rcList);
		CRect rcItem(rcList);

		rcItem.bottom = rcItem.top;
		rcItem.OffsetRect(-m_ptOrigin.x,-(m_ptOrigin.y%m_nItemHeight));
		for (int nItem = nTopItem; nItem <= (nTopItem+GetCountPerPage(TRUE)) && nItem<GetItemCount(); rcItem.top = rcItem.bottom, nItem++)
		{
			rcItem.bottom = rcItem.top + m_nItemHeight;

			DrawItem(pRT, rcItem, nItem);
		}
		pRT->PopClip();
		AfterPaint(pRT, painter);
	}
	
	void SListCtrlEx::OnSize( UINT nType, CSize size )
	{
		__super::OnSize(nType,size);
		Relayout();
	}

	void SListCtrlEx::DrawItem(IRenderTarget *pRT, CRect &rcItem, int iItem)
	{
		if (iItem < 0 || iItem >= GetItemCount()) return;

		BOOL bTextColorChanged = FALSE;
		int nBgImg = 0; 
		COLORREF crItemBg = m_crItemBg;  

		if ( iItem == m_iSelItem) 
		{//和下面那个if的条件分开，才会有sel和hot的区别
			if (m_pItemSkin != NULL)
				nBgImg = 2;
			else if (CR_INVALID != m_crItemSelBg)
				crItemBg = m_crItemSelBg;

		}
		else if  (((iItem == m_iHoverItem || (m_iHoverItem==-1 && iItem== m_iSelItem))) && m_bHotTrack)
		{
			if (m_pItemSkin != NULL)
				nBgImg = 1;
			else if (CR_INVALID != m_crItemHotBg)
				crItemBg = m_crItemHotBg; 
		}


		//绘制背景
		if (CR_INVALID != crItemBg)//先画背景
			pRT->FillSolidRect( rcItem, crItemBg);

		if (m_pItemSkin != NULL)//有skin，则覆盖背景
			m_pItemSkin->Draw(pRT, rcItem, nBgImg);

		CRect rcCol(rcItem);
		rcCol.right = rcCol.left;
		for (int nCol = 0; nCol < GetColumnCount(); nCol++)
		{
			SHDITEM hdi;
			hdi.mask=SHDI_WIDTH|SHDI_ORDER;
			m_pHeader->GetItem(nCol,&hdi);
			rcCol.left=rcCol.right;
			rcCol.right = rcCol.left + hdi.cx.toPixelSize(GetScale());

			CRect rcVisiblePart(rcCol);

			SWindow *pChild = m_arrItems[iItem]->GetWindow(GSW_FIRSTCHILD);
			int iIndex = 0;
			while(pChild)
			{
				if (iIndex == hdi.iOrder )
				{//更新列位置
					rcVisiblePart.OffsetRect( - rcItem.TopLeft()  );
					pChild->Move(rcVisiblePart);
					break;
				}
				iIndex++;
				pChild = pChild->GetWindow(GSW_NEXTSIBLING);
			}
		}

		EventLBGetDispInfo evt(this);
		evt.bHover=iItem == m_iHoverItem;
		evt.bSel =iItem == m_iSelItem;
		evt.pItem = m_arrItems[iItem];
		evt.iItem = iItem;
		FireEvent(evt);
		if (!m_bHotTrack)//不需要热追踪
			(m_arrItems[iItem])->ModifyItemState(0,WndState_Hover);
		m_arrItems[iItem]->Draw(pRT,rcItem);
	}

	BOOL SListCtrlEx::CreateChildren(pugi::xml_node xmlNode)
	{
		if (!__super::CreateChildren(xmlNode))
			return FALSE;

		if(!xmlNode) return TRUE;
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
		m_pHeader->GetEventSet()->subscribeEvent(EventHeaderItemChanging::EventID, Subscriber(&SListCtrlEx::OnHeaderSizeChanging,this));
		m_pHeader->GetEventSet()->subscribeEvent(EventHeaderItemSwap::EventID, Subscriber(&SListCtrlEx::OnHeaderSwap,this));

		//创建矩形选择框
		m_pWndRectangle=new SWindow();
		SApplication::getSingleton().SetSwndDefAttr(m_pWndRectangle);
		InsertChild(m_pWndRectangle,ICWND_FIRST);
		m_pWndRectangle->SetID(IDC_LSTCEX_SELECT);
		m_pWndRectangle->SetVisible(FALSE);
		if (m_strSelectRangeSkin.GetLength())
		{
			m_pWndRectangle->SetAttribute(L"skin",m_strSelectRangeSkin,TRUE);
			m_bMultiSelection = TRUE;//设置了selectskin，自动开启多选模式
		} 
		m_pWndRectangle->SetAttribute(L"msgTransparent",L"1",TRUE);

		pugi::xml_node xmlTempl=xmlNode.child(L"template");
		pugi::xml_node xmlItems=xmlNode.child(L"items");

		if(xmlTempl) m_xmlTempl.append_copy(xmlTempl);

		if(xmlItems)
		{
			pugi::xml_node xmlItem=xmlItems.child(L"item");
			while(xmlItem)
			{
				int dwData=xmlItem.attribute(L"itemdata").as_int(0);
				InsertItem(-1,xmlItem,dwData);
				xmlItem=xmlItem.next_sibling(L"item");
			}
			SetCurSel(xmlItems.attribute(L"cursel").as_int(-1));
		}

		return TRUE;
	}
	
	void SListCtrlEx::NotifySelChange( int nOldSel,int nNewSel, BOOL checkBox)
	{
		EventLBSelChanging evt1(this);

		evt1.nOldSel=nOldSel;
		evt1.nNewSel=nNewSel;
		FireEvent(evt1);
		if(evt1.bCancel) return ;


		if (checkBox) {//checkbox强选
			m_arrItems[nNewSel]->SetCheck(!m_arrItems[nNewSel]->IsChecked());
			m_iSelItem = m_arrItems[nNewSel]->IsChecked()?nNewSel:-1;
			RedrawItem(nNewSel); 
		} else  {
			if ((m_bMultiSelection || m_bCheckBox) && GetKeyState(VK_CONTROL) < 0) {
				if (nNewSel != -1) {
					if (m_arrItems[nNewSel]->IsChecked())
					{
						m_arrItems[nNewSel]->SetCheck(FALSE);
						m_iSelItem = -1;
					}
					else
					{
						m_arrItems[nNewSel]->SetCheck(TRUE);
						m_iSelItem = nNewSel;
					} 
					Invalidate();
					RedrawItem(nNewSel); 
				}
			} else if ((m_bMultiSelection || m_bCheckBox) && GetKeyState(VK_SHIFT) < 0) {
				//shift选
				if (nNewSel != -1) {
					if (nOldSel == -1)
						nOldSel = 0;

					int imax = (nOldSel > nNewSel) ? nOldSel : nNewSel;
					int imin = (imax == nOldSel) ? nNewSel : nOldSel;
					for (int i = 0; i < GetItemCount(); i++)
					{
						BOOL last = m_arrItems[i]->IsChecked();
						if (i >= imin && i<= imax) {
							m_arrItems[i]->SetCheck(TRUE);
						} else {
							m_arrItems[i]->SetCheck(FALSE);
						}


						if (last != m_arrItems[i]->IsChecked())
							RedrawItem(i);
					}
				}
			} else if ((m_bMultiSelection || m_bCheckBox) && m_bStartSelect) {
				//框选
				CPoint ptTopLeft = CPoint(m_rcWnd.left,m_rcWnd.top);
				CPoint ptBottomRight = CPoint(m_rcWnd.right,m_rcWnd.bottom);
				int iTop = HitTest(ptTopLeft);
				int iBottom = HitTest(ptBottomRight);
				if (iBottom < 0)
					iBottom = GetItemCount()-1;
 
				for (int i = 0; i < iTop; i ++)
				{
					BOOL last = m_arrItems[i]->IsChecked();
					m_arrItems[i]->SetCheck(FALSE);
					if (last != m_arrItems[i]->IsChecked())
						RedrawItem(i);
				}
				if (iTop >= 0)
				{
					for (int i = iTop; i <= iBottom; i ++)
					{
						BOOL last = m_arrItems[i]->IsChecked();
						m_arrItems[i]->SetCheck(TRUE);
						if (last != m_arrItems[i]->IsChecked())
							RedrawItem(i);
					}
				}
				else
				{
					for (int i = 0; i < GetItemCount(); i ++)
					{
						BOOL last = m_arrItems[i]->IsChecked();
						m_arrItems[i]->SetCheck(FALSE);
						if (last != m_arrItems[i]->IsChecked())
							RedrawItem(i);
					}
					goto lblEnd;
				}
				for (int i = iBottom + 1; i < GetItemCount(); i ++)
				{
					BOOL last = m_arrItems[i]->IsChecked();
					m_arrItems[i]->SetCheck(FALSE);
					if (last != m_arrItems[i]->IsChecked())
						RedrawItem(i);
				}
			}
			else {//一般是鼠标点击，单选
				m_iSelItem = -1;
				for (int i = 0; i < GetItemCount(); i++)
				{ 
					BOOL last = m_arrItems[i]->IsChecked();
					m_arrItems[i]->SetCheck(FALSE);
					if (last != m_arrItems[i]->IsChecked())
						RedrawItem(i);
				}
				if (nNewSel != -1) {
					m_arrItems[nNewSel]->SetCheck(TRUE);
					m_iSelItem = nNewSel;
					RedrawItem(nNewSel);
				}
			}
		}

lblEnd:
		EventLBSelChanged evt2(this);
		evt2.nOldSel=nOldSel;
		evt2.nNewSel=nNewSel;
		FireEvent(evt2);

	}

	void SListCtrlEx::OnMouseLeave()
	{
		__super::OnMouseLeave();
		if(m_iHoverItem != -1)
		{
			int nOldHover=m_iHoverItem;
			m_iHoverItem=-1;
			m_arrItems[nOldHover]->DoFrameEvent(WM_MOUSELEAVE,0,0);
		}
	}

	BOOL SListCtrlEx::OnSetCursor(const CPoint &pt)
	{
		BOOL bRet=FALSE;
		if(m_pCapturedFrame)
		{
			CRect rcItem=m_pCapturedFrame->GetItemRect();
			bRet=m_pCapturedFrame->DoFrameEvent(WM_SETCURSOR,0,MAKELPARAM(pt.x-rcItem.left,pt.y-rcItem.top))!=0;
		}
		else if(m_iHoverItem!=-1)
		{
			CRect rcItem=GetItemRect(m_iHoverItem);
			bRet=m_arrItems[m_iHoverItem]->DoFrameEvent(WM_SETCURSOR,0,MAKELPARAM(pt.x-rcItem.left,pt.y-rcItem.top))!=0;
		}
		if(!bRet)
		{
			bRet=__super::OnSetCursor(pt);
		}
		return bRet;
	}

	void SListCtrlEx::OnKeyDown( TCHAR nChar, UINT nRepCnt, UINT nFlags )
	{
		int  nNewSelItem = -1;
		SWindow *pOwner = GetOwner();
		if (pOwner && (nChar == VK_ESCAPE))
		{
			pOwner->SSendMessage(WM_KEYDOWN, nChar, MAKELONG(nFlags, nRepCnt));
			return;
		}

		if (nChar == VK_DOWN && m_iSelItem < GetItemCount() - 1)
			nNewSelItem = m_iSelItem+1;
		else if (nChar == VK_UP && m_iSelItem > 0)
			nNewSelItem = m_iSelItem-1;
		else if (pOwner && nChar == VK_RETURN)
			nNewSelItem = m_iSelItem;
		else if(nChar == VK_PRIOR)
		{
			OnScroll(TRUE,SB_PAGEUP,0);
		}else if(nChar == VK_NEXT)
		{
			OnScroll(TRUE,SB_PAGEDOWN,0);
		}

		if(nNewSelItem!=-1)
		{
			EnsureVisible(nNewSelItem);
			NotifySelChange(m_iSelItem,nNewSelItem);
		}
	}

	void SListCtrlEx::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		SWindow *pOwner = GetOwner();
		if (pOwner)
			pOwner->SSendMessage(WM_CHAR, nChar, MAKELONG(nFlags, nRepCnt));
	}

	UINT SListCtrlEx::OnGetDlgCode()
	{
		return SC_WANTALLKEYS;
	}

	void SListCtrlEx::OnDestroy()
	{
		DeleteAllItems(FALSE);
		__super::OnDestroy();
	}

	BOOL SListCtrlEx::OnUpdateToolTip(CPoint pt, SwndToolTipInfo & tipInfo)
	{
		if(m_iHoverItem == -1)
			return __super::OnUpdateToolTip(pt,tipInfo);
		return m_arrItems[m_iHoverItem]->OnUpdateToolTip(pt,tipInfo);
	}

	void SListCtrlEx::OnItemSetCapture(SItemPanel *pItem,BOOL bCapture )
	{
		if (m_bStartSelect == TRUE)
			return;
		if(bCapture)
		{
			SetCapture();
			m_pCapturedFrame=pItem;
		}
		else if(pItem==m_pCapturedFrame)
		{
			ReleaseCapture();
			m_pCapturedFrame=NULL;
		}
	}
	
	CRect SListCtrlEx::GetItemRect( int iItem )
	{
		CRect rcClient = GetListRect();
		CRect rcHead;
		m_pHeader->GetClientRect(&rcHead);
		rcClient.right = rcClient.left + rcHead.Width();
		CRect rcRet(CPoint(0,iItem*m_nItemHeight),CSize(rcClient.Width(),m_nItemHeight));
		rcRet.OffsetRect(rcClient.TopLeft()-m_ptOrigin);
		return rcRet;
	}

	BOOL SListCtrlEx::OnItemGetRect(SItemPanel *pItem,CRect &rcItem )
	{
		int iItem=pItem->GetItemIndex();
		rcItem=GetItemRect(iItem);
		return TRUE;
	}

	LRESULT SListCtrlEx::OnMouseEvent( UINT uMsg,WPARAM wParam,LPARAM lParam )
	{

		LRESULT lRet=0;
		CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		switch (uMsg)
		{
		case WM_LBUTTONDOWN:
			if (((GetKeyState(VK_SHIFT) >= 0) && (GetKeyState(VK_CONTROL) >= 0)) && (m_bMultiSelection || m_bCheckBox))
			{
				if (m_bMouseDown == FALSE)
				{
					m_bMouseDown = TRUE;
					m_bStartSelect = FALSE;
					m_ptTmp = pt;
					SetCapture();
				}
			}
			else
			{
				m_bMouseDown = FALSE;
				m_bStartSelect = FALSE;
			}
			break;
		case WM_LBUTTONUP:
			m_bMouseDown = FALSE;
			if (m_bStartSelect == FALSE )
			{
				NotifySelChange(m_iSelItem,m_iHoverItem);
			}
			else
			{
				m_pWndRectangle->SetVisible(FALSE);
				Invalidate();
			}
			m_bStartSelect = FALSE;
			ReleaseCapture();
			break;
		case WM_MOUSEMOVE:
			if (m_bMouseDown == TRUE)
			{
				CRect rcClient = GetListRect();
				CPoint ptTmp = pt; //保证落在Client之内
				ptTmp.x = max(ptTmp.x,rcClient.left);
				ptTmp.y = max(ptTmp.y,rcClient.top);
				ptTmp.x = min(ptTmp.x,rcClient.right);
				ptTmp.y = min(ptTmp.y,rcClient.bottom);

				CPoint pt1,pt2;//分配左上和右下的点
				pt1.x = min(m_ptTmp.x,ptTmp.x);
				pt1.y = min(m_ptTmp.y,ptTmp.y);
				pt2.x = max(m_ptTmp.x,ptTmp.x);
				pt2.y = max(m_ptTmp.y,ptTmp.y);

				m_rcWnd.left = pt1.x;
				m_rcWnd.top = pt1.y;
				m_rcWnd.right = pt2.x;
				m_rcWnd.bottom = pt2.y; 
				if (m_rcWnd.Width() > 5 || m_rcWnd.Height() > 5)
				{//这才算拖拉框
					m_bStartSelect = TRUE; 
					m_iSelItem = -1;//都开始拖拉了，就放弃单独那条了
					m_pWndRectangle->Move(m_rcWnd);
					m_pWndRectangle->SetVisible(TRUE);
					int iHover = HitTest(pt);
					//	if (iHover != m_iHoverItem)
					{
						NotifySelChange(m_iHoverItem,iHover); 	
					}
				}
			} 



			break;
		default:
			break;
		}   


		if(m_pCapturedFrame)
		{
			CRect rcItem=m_pCapturedFrame->GetItemRect();
			pt.Offset(-rcItem.TopLeft());
			lRet = m_pCapturedFrame->DoFrameEvent(uMsg,wParam,MAKELPARAM(pt.x,pt.y));
		}
		else
		{
			if(m_bFocusable && (uMsg==WM_LBUTTONDOWN || uMsg== WM_RBUTTONDOWN || uMsg==WM_LBUTTONDBLCLK))
				SetFocus();

			int iHover=HitTest(pt);
			if(iHover!=m_iHoverItem)
			{
				int nOldHover=m_iHoverItem;
				m_iHoverItem=iHover;
				if(nOldHover!=-1)
				{
					RedrawItem(nOldHover);
					m_arrItems[nOldHover]->DoFrameEvent(WM_MOUSELEAVE,0,0);
				}
				if(m_iHoverItem!=-1)
				{
					RedrawItem(m_iHoverItem);
					m_arrItems[m_iHoverItem]->DoFrameEvent(WM_MOUSEHOVER,wParam,MAKELPARAM(pt.x,pt.y));
				}
			}
			if(uMsg==WM_LBUTTONDOWN && m_iSelItem!=-1 && m_iSelItem != m_iHoverItem )
			{//选择一个新行的时候原有行失去焦点
				m_arrItems[m_iSelItem]->GetFocusManager()->SetFocusedHwnd(0);
			}
			if(m_iHoverItem!=-1)
			{
				m_arrItems[m_iHoverItem]->DoFrameEvent(uMsg,wParam,MAKELPARAM(pt.x,pt.y));
			}
		}
		return 0;

	}

	LRESULT SListCtrlEx::OnKeyEvent( UINT uMsg,WPARAM wParam,LPARAM lParam )
	{
		LRESULT lRet=0;
		if(m_pCapturedFrame)
		{
			lRet=m_pCapturedFrame->DoFrameEvent(uMsg,wParam,lParam);
			SetMsgHandled(m_pCapturedFrame->IsMsgHandled());
		}
		else if(m_iSelItem!=-1)
		{
			lRet=m_arrItems[m_iSelItem]->DoFrameEvent(uMsg,wParam,lParam);
			SetMsgHandled(m_arrItems[m_iSelItem]->IsMsgHandled());
		}else
		{
			SetMsgHandled(FALSE);
		}
		return lRet;
	}

	//同步在SItemPanel中的index属性，在执行了插入，删除等操作后使用
	void SListCtrlEx::UpdatePanelsIndex(UINT nFirst,UINT nLast)
	{
		for(UINT i=nFirst;i<m_arrItems.GetCount() && i<nLast;i++)
		{
			m_arrItems[i]->SetItemIndex(i);
		}
	}

	void SListCtrlEx::OnSetFocus(SWND wndOld)
	{
		__super::OnSetFocus(wndOld);
		if(m_iSelItem!=-1) m_arrItems[m_iSelItem]->DoFrameEvent(WM_SETFOCUS,0,0);

	}

	void SListCtrlEx::OnKillFocus(SWND wndFocus)
	{
		__super::OnKillFocus(wndFocus);
		if(m_iSelItem!=-1) m_arrItems[m_iSelItem]->DoFrameEvent(WM_KILLFOCUS,0,0);
		if(m_iSelItem!=-1) RedrawItem(m_iSelItem);
	}

	LRESULT SListCtrlEx::OnNcCalcSize( BOOL bCalcValidRects, LPARAM lParam )
	{
		LRESULT lRet=__super::OnNcCalcSize(bCalcValidRects,lParam);
		Relayout();
		return lRet;
	}

	void SListCtrlEx::Relayout()
	{
		CRect rcHead;
		m_pHeader->GetClientRect(&rcHead);
		for(int i=0; i<GetItemCount(); i++)
			m_arrItems[i]->Move(CRect(0,0,rcHead.Width(),m_nItemHeight));
	}

	void SListCtrlEx::OnViewOriginChanged( CPoint ptOld,CPoint ptNew )
	{
		if(m_iSelItem!=-1 && GetContainer()->GetFocus()==m_swnd)
		{//这里需要重新设置一下选中行的焦点状态来更新光标位置
			m_arrItems[m_iSelItem]->DoFrameEvent(WM_KILLFOCUS,0,0);
			m_arrItems[m_iSelItem]->DoFrameEvent(WM_SETFOCUS,0,0);
		}
	}

	BOOL SListCtrlEx::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
	{
		return __super::OnMouseWheel(nFlags,zDelta,pt);
	}

	int SListCtrlEx::GetTopIndex() const
	{
		return m_ptOrigin.y / m_nItemHeight;
	}

	BOOL SListCtrlEx::OnScroll(BOOL bVertical,UINT uCode,int nPos)
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
		//  重新计算客户区及非客户区
		SSendMessage(WM_NCCALCSIZE);
		return bRet;
	}

	void SListCtrlEx::UpdateHeaderCtrl()
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		CRect rcHeader(rcClient);
		rcHeader.bottom=rcHeader.top+m_nHeaderHeight;
		rcHeader.left-=m_ptOrigin.x;
		if(m_pHeader) m_pHeader->Move(rcHeader);
	}

	CRect SListCtrlEx::GetListRect()
	{
		CRect rcList;
		GetClientRect(&rcList);
		rcList.top += m_nHeaderHeight;
		return rcList;
	}

	int SListCtrlEx::InsertColumn(int nIndex, LPCTSTR pszText, int nWidth, LPARAM lParam/*=0*/)
	{
		SASSERT(m_pHeader);
		int nRet = m_pHeader->InsertItem(nIndex, pszText, nWidth, ST_NULL, lParam);
		UpdateScrollBar();
		return nRet;
	}

	void SListCtrlEx::UpdateChildrenPosition()
	{
		__super::UpdateChildrenPosition();
		UpdateHeaderCtrl();
	}
	//基类SScrollView的UpdateScrollBar需要声明成virtual
	void SListCtrlEx::UpdateScrollBar()
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

	int SListCtrlEx::GetCountPerPage(BOOL bPartial)
	{
		CRect rcClient = GetListRect();

		// calculate number of items per control height (include partial item)
		div_t divHeight = div(rcClient.Height(), m_nItemHeight);

		// round up to nearest item count
		return max(bPartial && divHeight.rem > 0 ? divHeight.quot + 1 : divHeight.quot, 1);
	}

	bool SListCtrlEx::OnHeaderClick(EventArgs *pEvt)
	{
		return true;
	}

	bool SListCtrlEx::OnHeaderSizeChanging(EventArgs *pEvt)
	{
		UpdateScrollBar();
		InvalidateRect(GetListRect());

		return true;
	}

	bool SListCtrlEx::OnHeaderSwap(EventArgs *pEvt)
	{
		InvalidateRect(GetListRect());
		return true;
	}

	int SListCtrlEx::GetColumnCount() const
	{
		if (!m_pHeader)
			return 0;

		return m_pHeader->GetItemCount();
	}

    void SListCtrlEx::OnItemRequestRelayout(SItemPanel *pItem)
    {
        CRect rcWnd = pItem->GetWindowRect();
        pItem->Move(rcWnd);
    }

	void SListCtrlEx::SwapItem(int srcIdx,int desIdx)
	{
		if ((srcIdx >= 0) && srcIdx < m_arrItems.GetCount())
		{
			if ((desIdx >= 0) && desIdx < m_arrItems.GetCount())
			{
				SItemPanel* pSrcWnd= m_arrItems[srcIdx];
				pSrcWnd->AddRef();
				m_arrItems[srcIdx] = m_arrItems[desIdx];
				m_arrItems[srcIdx]->SetItemIndex(desIdx);
				m_arrItems[desIdx] = pSrcWnd;
				m_arrItems[desIdx]->SetItemIndex(srcIdx);
				pSrcWnd->Release();
			}
		}
	}	

	BOOL SListCtrlEx::SortItems(
		PFNLVCOMPAREEX pfnCompare,
		void * pContext
	)
	{
		void *data = m_arrItems.GetData();
		qsort_s(m_arrItems.GetData(), m_arrItems.GetCount(), sizeof(SOUI::SItemPanel*), pfnCompare, pContext);
		m_iSelItem = -1;
		m_iHoverItem = -1;
		UpdatePanelsIndex();
		InvalidateRect(GetListRect());
		return TRUE;
	}
}//namespace SOUI