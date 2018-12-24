#include "souistd.h"
#include "control/SMCListView.h"
#include "helper/SListViewItemLocator.h"

#pragma warning(disable : 4267 4018)

#define ITEM_MARGIN 4
namespace SOUI
{
    class SMCListViewDataSetObserver : public TObjRefImpl<ILvDataSetObserver>
    {
    public:
        SMCListViewDataSetObserver(SMCListView *pView):m_pOwner(pView)
        {
        }
        virtual void onChanged();
        virtual void onInvalidated();

		virtual void OnItemChanged(int iItem);
    protected:
        SMCListView * m_pOwner;
    };

    //////////////////////////////////////////////////////////////////////////
    void SMCListViewDataSetObserver::onChanged()
    {
        m_pOwner->onDataSetChanged();
    }

    void SMCListViewDataSetObserver::onInvalidated()
    {
        m_pOwner->onDataSetInvalidated();
    }

	void SMCListViewDataSetObserver::OnItemChanged(int iItem)
	{
		m_pOwner->onItemDataChanged(iItem);
	}

//////////////////////////////////////////////////////////////////////////
//  SMCListView

    SMCListView::SMCListView()
        :m_pHeader(NULL)
        ,m_iSelItem(-1)
        ,m_iFirstVisible(-1)
        ,m_pHoverItem(NULL)
        ,m_itemCapture(NULL)
        ,m_pSkinDivider(NULL)
        ,m_bWantTab(FALSE)
        ,m_bDatasetInvalidated(TRUE)
    {
        m_bFocusable = TRUE;
        m_bClipClient = TRUE;
        m_dwUpdateInterval=40;
		m_nHeaderHeight.fromString(L"25dp");
        m_observer.Attach(new SMCListViewDataSetObserver(this));

        m_evtSet.addEvent(EVENTID(EventLVSelChanging));
        m_evtSet.addEvent(EVENTID(EventLVSelChanged));
    }

    SMCListView::~SMCListView()
    {
        m_observer=NULL;
        m_lvItemLocator=NULL;
    }


    BOOL SMCListView::SetAdapter(IMcAdapter * adapter)
    {
        if(!m_lvItemLocator)
        {
            SASSERT_FMT(FALSE,_T("error: A item locator is in need before setting adapter!!!"));
            return FALSE;
        }

        if(m_adapter)
        {
            m_adapter->unregisterDataSetObserver(m_observer);

            //free all itemPanels in recycle
            for(size_t i=0;i<m_itemRecycle.GetCount();i++)
            {
                SList<SItemPanel*> *lstItemPanels = m_itemRecycle.GetAt(i);
                SPOSITION pos = lstItemPanels->GetHeadPosition();
                while(pos)
                {
                    SItemPanel * pItemPanel = lstItemPanels->GetNext(pos);
                    pItemPanel->DestroyWindow();
                }
                delete lstItemPanels;
            }
            m_itemRecycle.RemoveAll();

            //free all visible itemPanels
            SPOSITION pos=m_lstItems.GetHeadPosition();
            while(pos)
            {
                ItemInfo ii = m_lstItems.GetNext(pos);
                ii.pItem->DestroyWindow();
            }
            m_lstItems.RemoveAll();
        }

        m_adapter = adapter;
        if(m_lvItemLocator)
            m_lvItemLocator->SetAdapter(adapter);
        if(m_adapter) 
        {
            m_adapter->InitByTemplate(m_xmlTemplate.first_child());
            m_adapter->registerDataSetObserver(m_observer);
            for(int i=0;i<m_adapter->getViewTypeCount();i++)
            {
                m_itemRecycle.Add(new SList<SItemPanel*>());
            }
            onDataSetChanged();
        }
        return TRUE;
    }
    
int SMCListView::InsertColumn(int nIndex, LPCTSTR pszText, int nWidth, LPARAM lParam)
{
    SASSERT(m_pHeader);

    int nRet = m_pHeader->InsertItem(nIndex, pszText, nWidth, ST_NULL, lParam);
    UpdateScrollBar();
    return nRet;
}

BOOL SMCListView::CreateChildren(pugi::xml_node xmlNode)
{
    //  listctrl的子控件只能是一个header控件
    if (!__super::CreateChildren(xmlNode))
        return FALSE;
        
    pugi::xml_node xmlTemplate = xmlNode.child(L"template");
    if(xmlTemplate)
    {
        m_xmlTemplate.append_copy(xmlTemplate);
		SLayoutSize nItemHei = GETLAYOUTSIZE(xmlTemplate.attribute(L"itemHeight").value());
        if(nItemHei.fSize>0.0f)
        {//指定了itemHeight属性时创建一个固定行高的定位器
            IListViewItemLocator * pItemLocator = new  SListViewItemLocatorFix(nItemHei,m_nDividerSize);
            SetItemLocator(pItemLocator);
            pItemLocator->Release();
        }else
        {//创建一个行高可变的行定位器，从defHeight属性中获取默认行高
			IListViewItemLocator * pItemLocator = new  SListViewItemLocatorFlex(GETLAYOUTSIZE(xmlTemplate.attribute(L"defHeight").as_string(L"30dp")),m_nDividerSize);
            SetItemLocator(pItemLocator);
            pItemLocator->Release();
        }
    }

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
    strPos.Format(L"0,0,-0,%d",GetHeaderHeight());
    m_pHeader->SetAttribute(L"pos",strPos,TRUE);

    m_pHeader->GetEventSet()->subscribeEvent(EventHeaderItemChanging::EventID, Subscriber(&SMCListView::OnHeaderSizeChanging,this));
    m_pHeader->GetEventSet()->subscribeEvent(EventHeaderItemSwap::EventID, Subscriber(&SMCListView::OnHeaderSwap,this));
    m_pHeader->GetEventSet()->subscribeEvent(EventHeaderClick::EventID, Subscriber(&SMCListView::OnHeaderClick,this));

    return TRUE;
}


CRect SMCListView::GetListRect()
{
    CRect rcList;

    GetClientRect(&rcList);
    rcList.top += GetHeaderHeight();

    return rcList;
}

//  更新滚动条
void SMCListView::UpdateScrollBar()
{
    CSize szView;
    szView.cx = m_pHeader->GetTotalWidth();
    szView.cy = m_lvItemLocator->GetTotalHeight();

    CRect rcClient;
    SWindow::GetClientRect(&rcClient);//不计算滚动条大小
    rcClient.top+=GetHeaderHeight();
    if(rcClient.bottom<rcClient.top) 
        rcClient.bottom=rcClient.top;
    CSize size = rcClient.Size();
    //  关闭滚动条
    m_wBarVisible = SSB_NULL;

    if (size.cy<szView.cy || (size.cy<szView.cy+GetSbWidth() && size.cx<szView.cx))
    {
        //  需要纵向滚动条
        m_wBarVisible |= SSB_VERT;
        m_siVer.nMin  = 0;
        m_siVer.nMax  = szView.cy-1;
        m_siVer.nPage = rcClient.Height();

        if (size.cx-GetSbWidth() < szView.cx)
        {
            //  需要横向滚动条
            m_wBarVisible |= SSB_HORZ;
            m_siVer.nPage=size.cy-GetSbWidth() > 0 ? size.cy-GetSbWidth() : 0;//注意同时调整纵向滚动条page信息

            m_siHoz.nMin  = 0;
            m_siHoz.nMax  = szView.cx-1;
            m_siHoz.nPage = (size.cx-GetSbWidth()) > 0 ? (size.cx-GetSbWidth()) : 0;
        }
        else
        {
            //  不需要横向滚动条
            m_siHoz.nPage = size.cx;
            m_siHoz.nMin  = 0;
            m_siHoz.nMax  = m_siHoz.nPage-1;
            m_siHoz.nPos  = 0;
        }
    }
    else
    {
        //  不需要纵向滚动条
        m_siVer.nPage = size.cy;
        m_siVer.nMin  = 0;
        m_siVer.nMax  = size.cy-1;
        m_siVer.nPos  = 0;

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
        }
    }

    //  根据需要调整原点位置
    if (HasScrollBar(FALSE) && m_siHoz.nPos+m_siHoz.nPage>szView.cx)
    {
        m_siHoz.nPos = szView.cx-m_siHoz.nPage;
    }

    if (HasScrollBar(TRUE) && m_siVer.nPos +m_siVer.nPage>szView.cy)
    {
        m_siVer.nPos = szView.cy-m_siVer.nPage;
    }
    
    SetScrollPos(TRUE, m_siVer.nPos, TRUE);
    SetScrollPos(FALSE, m_siHoz.nPos, TRUE);

    //  重新计算客户区及非客户区
    SSendMessage(WM_NCCALCSIZE);

    Invalidate();
}

//更新表头位置
void SMCListView::UpdateHeaderCtrl()
{
    CRect rcClient;
    GetClientRect(&rcClient);
    CRect rcHeader(rcClient);
    rcHeader.bottom=rcHeader.top+GetHeaderHeight();
    rcHeader.left-=m_siHoz.nPos;
    if(m_pHeader) m_pHeader->Move(rcHeader);
}


void SMCListView::DeleteColumn( int iCol )
{
    if(m_pHeader->DeleteItem(iCol))
    {
        UpdateScrollBar();
    }
}


int SMCListView::GetColumnCount() const
{
    if (!m_pHeader)
        return 0;

    return m_pHeader->GetItemCount();
}


void SMCListView::UpdateChildrenPosition()
{
    __super::UpdateChildrenPosition();
    UpdateHeaderCtrl();
}

int SMCListView::GetHeaderHeight() const
{
	return m_nHeaderHeight.toPixelSize(GetScale());
}


bool SMCListView::OnHeaderClick(EventArgs *pEvt)
{
    EventHeaderClick *pEvt2 = sobj_cast<EventHeaderClick>(pEvt);
    SASSERT(pEvt2);
    SHDITEM hi;
    hi.mask = SHDI_ORDER|SHDI_SORTFLAG;
    SHDSORTFLAG *pstFlags = new SHDSORTFLAG[m_pHeader->GetItemCount()];
    int *pOrders = new int[m_pHeader->GetItemCount()];
    int iCol = -1;
    for(int i=0;i<m_pHeader->GetItemCount();i++)
    {
        m_pHeader->GetItem(i,&hi);
        pstFlags[hi.iOrder]=hi.stFlag;
        pOrders[hi.iOrder]=i;
        if(i == pEvt2->iItem) iCol = hi.iOrder;
    }
    if(m_adapter && m_adapter->OnSort(iCol,pstFlags,m_pHeader->GetItemCount()))
    {
        //更新表头的排序状态
        for(int i=0;i<m_pHeader->GetItemCount();i++)
        {
           m_pHeader->SetItemSort(pOrders[i],pstFlags[i]);
        }
        onDataSetChanged();
    }
    delete []pOrders;
    delete []pstFlags;
    return true;
}

bool SMCListView::OnHeaderSizeChanging(EventArgs *pEvt)
{
    UpdateScrollBar();
    UpdateHeaderCtrl();
    SPOSITION pos = m_lstItems.GetHeadPosition();
    while(pos)
    {
        ItemInfo ii = m_lstItems.GetNext(pos);
        CRect rcItem = ii.pItem->GetWindowRect();
        rcItem.right = m_pHeader->GetTotalWidth();
        ii.pItem->Move(rcItem);
        CRect rcSubItem(rcItem);
        rcSubItem.right=rcSubItem.left=0;
        for(int i=0;i<m_pHeader->GetItemCount();i++)
        {
            SHDITEM hi;
            hi.mask = SHDI_ORDER;
            m_pHeader->GetItem(i,&hi);
            rcSubItem.left = rcSubItem.right;
            rcSubItem.right += m_pHeader->GetItemWidth(i);
            SStringW strColName=m_adapter->GetColumnName(hi.iOrder);
            SWindow *pCol=ii.pItem->FindChildByName(strColName);
            if(pCol)
            {
                pCol->Move(rcSubItem);
            }
        }
        SASSERT(rcSubItem.right == m_pHeader->GetTotalWidth());
    }

    InvalidateRect(GetListRect());    
    return true;
}

bool SMCListView::OnHeaderSwap(EventArgs *pEvt)
{
    OnHeaderSizeChanging(NULL);
    return true;
}


void SMCListView::onDataSetChanged()
{
    if(!m_adapter) return;

	//更新列显示状态
	m_pHeader->GetEventSet()->setMutedState(true);
	for(size_t i=0;i<m_pHeader->GetItemCount();i++)
	{
 		m_pHeader->SetItemVisible(i,m_adapter->IsColumnVisible(i));
	}
	m_pHeader->GetEventSet()->setMutedState(false);

    if(m_lvItemLocator) m_lvItemLocator->OnDataSetChanged();
    if(m_iSelItem >= m_adapter->getCount()) 
        m_iSelItem = -1;
    
	
    UpdateScrollBar();
    UpdateVisibleItems();
}

void SMCListView::onDataSetInvalidated()
{
    m_bDatasetInvalidated = TRUE;
    Invalidate();
}

void SMCListView::onItemDataChanged(int iItem)
{
	if(iItem<m_iFirstVisible) return;
	if(iItem>=m_iFirstVisible + (int)m_lstItems.GetCount()) return;
	if(m_lvItemLocator->IsFixHeight())
		UpdateVisibleItem(iItem);
	else
		UpdateVisibleItems();
}

void SMCListView::OnPaint(IRenderTarget *pRT)
{
    if(m_bDatasetInvalidated)
    {
        UpdateVisibleItems();
        m_bDatasetInvalidated=FALSE;
    }
    SPainter duiDC;
    BeforePaint(pRT,duiDC);


    int iFirst = m_iFirstVisible;
    if(iFirst!=-1)
    {
        CRect rcClient;
        GetClientRect(&rcClient);
        rcClient.top += GetHeaderHeight();
        
        pRT->PushClipRect(&rcClient,RGN_AND);

        CRect rcClip,rcInter;
        pRT->GetClipBox(&rcClip);

        
        SPOSITION pos= m_lstItems.GetHeadPosition();
        int i=0;
        for(;pos;i++)
        {

            ItemInfo ii = m_lstItems.GetNext(pos);
            CRect rcItem = _OnItemGetRect(iFirst+i);
            rcInter.IntersectRect(&rcClip,&rcItem);
            if(!rcInter.IsRectEmpty())
                ii.pItem->Draw(pRT,rcItem);
            rcItem.top = rcItem.bottom;
            rcItem.bottom += m_lvItemLocator->GetDividerSize();
            if(m_pSkinDivider)
            {//绘制分隔线
                m_pSkinDivider->Draw(pRT,rcItem,0);
            }
        }

        pRT->PopClip();
    }
    AfterPaint(pRT,duiDC);
}

BOOL SMCListView::OnScroll(BOOL bVertical,UINT uCode,int nPos)
{
    int nOldPos = bVertical?m_siVer.nPos:m_siHoz.nPos;
    __super::OnScroll(bVertical, uCode, nPos);
    int nNewPos = bVertical?m_siVer.nPos:m_siHoz.nPos;
    if(nOldPos != nNewPos)
    {
        if(bVertical)
            UpdateVisibleItems();
        else
            UpdateHeaderCtrl();
        //加速滚动时UI的刷新
        if (uCode==SB_THUMBTRACK)
            ScrollUpdate();

        return TRUE;
    }
    return FALSE;
}


void SMCListView::UpdateVisibleItems()
{
    if(!m_adapter) return;
    int iOldFirstVisible = m_iFirstVisible;
    int iOldLastVisible = m_iFirstVisible + m_lstItems.GetCount();
    int nOldTotalHeight = m_lvItemLocator->GetTotalHeight();

    int iNewFirstVisible = m_lvItemLocator->Position2Item(m_siVer.nPos);
    int iNewLastVisible = iNewFirstVisible;
    int pos = m_lvItemLocator->Item2Position(iNewFirstVisible);
    int iHoverItem = m_pHoverItem?(int)m_pHoverItem->GetItemIndex():-1;
    m_pHoverItem = NULL;
    
    ItemInfo *pItemInfos = new ItemInfo[m_lstItems.GetCount()];
    SPOSITION spos = m_lstItems.GetHeadPosition();
    int i=0;
    while(spos)
    {
        pItemInfos[i++]=m_lstItems.GetNext(spos);
    }

    m_lstItems.RemoveAll();

    if(iNewFirstVisible!=-1)
    {
        while(pos < m_siVer.nPos + (int)m_siVer.nPage && iNewLastVisible < m_adapter->getCount())
        {
            DWORD dwState = WndState_Normal;
            if(iHoverItem == iNewLastVisible) dwState |= WndState_Hover;
            if(m_iSelItem == iNewLastVisible) dwState |= WndState_Check;
            
            ItemInfo ii={NULL,-1};
            ii.nType = m_adapter->getItemViewType(iNewLastVisible,dwState);

            if(iNewLastVisible>=iOldFirstVisible && iNewLastVisible < iOldLastVisible)
            {//use the old visible item
                int iItem = iNewLastVisible-iOldFirstVisible;//(iNewLastVisible-iNewFirstVisible) + (iNewFirstVisible-iOldFirstVisible);
                SASSERT(iItem>=0 && iItem <= (iOldLastVisible-iOldFirstVisible));
                if(pItemInfos[iItem].nType == ii.nType)
                {//类型相同才能重用
                    ii = pItemInfos[iItem];
                    pItemInfos[iItem].pItem = NULL;//标记该行已经被重用
                }
            }
            if(!ii.pItem)
            {//create new visible item
                SList<SItemPanel *> *lstRecycle = m_itemRecycle.GetAt(ii.nType);
                if(lstRecycle->IsEmpty())
                {//创建一个新的列表项
                    ii.pItem = SItemPanel::Create(this,pugi::xml_node(),this);
                    ii.pItem->GetEventSet()->subscribeEvent(EventItemPanelClick::EventID,Subscriber(&SMCListView::OnItemClick,this));
                }else
                {
                    ii.pItem = lstRecycle->RemoveHead();
                }
                ii.pItem->SetItemIndex(iNewLastVisible);
            }
            ii.pItem->SetVisible(TRUE);
            CRect rcItem(0,0,m_pHeader->GetTotalWidth(),100000);
            if(m_lvItemLocator->IsFixHeight())
            {
                rcItem.bottom=m_lvItemLocator->GetItemHeight(iNewLastVisible);
                ii.pItem->Move(rcItem);
            }
            
            //设置状态，同时暂时禁止应用响应statechanged事件。
            ii.pItem->GetEventSet()->setMutedState(true);
            ii.pItem->ModifyItemState(dwState,0);
            ii.pItem->GetEventSet()->setMutedState(false);
            if(dwState & WndState_Hover) m_pHoverItem=ii.pItem;
            
            //应用可以根据ii.pItem的状态来决定如何初始化列表数据
            m_adapter->getView(iNewLastVisible,ii.pItem,m_xmlTemplate.first_child());
			ii.pItem->DoColorize(GetColorizeColor());

            if(!m_lvItemLocator->IsFixHeight())
            {//计算出列表行高度
                SIZE szView = m_adapter->getViewDesiredSize(iNewLastVisible,ii.pItem,&rcItem);
                m_lvItemLocator->SetItemHeight(iNewLastVisible,szView.cy);                    
                rcItem.bottom = szView.cy;
                ii.pItem->Move(rcItem);
            }
            ii.pItem->UpdateLayout();
            
            //调整网格大小
            CRect rcSubItem(rcItem);
            rcSubItem.right=rcSubItem.left;
            for(int i=0;i<m_pHeader->GetItemCount();i++)
			{					
				SHDITEM hditem;
				hditem.mask=SHDI_ORDER|SHDI_WIDTH;
				m_pHeader->GetItem(i,&hditem);
				SStringW strColName = m_adapter->GetColumnName(hditem.iOrder);
				SWindow *pColWnd=ii.pItem->FindChildByName(strColName);
				SASSERT(pColWnd);
				if(m_pHeader->IsItemVisible(i))
				{
					pColWnd->SetVisible(true);
					rcSubItem.left = rcSubItem.right;
					rcSubItem.right += hditem.cx.toPixelSize(GetScale());
					pColWnd->Move(rcSubItem);
				}else
				{
					pColWnd->SetVisible(false);
				}
            }

            m_lstItems.AddTail(ii);
            pos += rcItem.bottom + m_lvItemLocator->GetDividerSize();

            iNewLastVisible ++;
        }
    }

    //move old visible items which were not reused to recycle
    for(int i=0;i<(iOldLastVisible-iOldFirstVisible);i++)
    {
        ItemInfo ii = pItemInfos[i];
        if(!ii.pItem) continue;

        if(ii.pItem->GetState() & WndState_Hover)
        {
            ii.pItem->DoFrameEvent(WM_MOUSELEAVE,0,0);
        }
        ii.pItem->GetEventSet()->setMutedState(true);
        if(ii.pItem->GetState() & WndState_Check)
        {
            ii.pItem->ModifyItemState(0,WndState_Check);
            ii.pItem->GetFocusManager()->SetFocusedHwnd(0);
        }
        ii.pItem->SetVisible(FALSE);
        ii.pItem->GetEventSet()->setMutedState(false);
        m_itemRecycle[ii.nType]->AddTail(ii.pItem);    
    }
    delete [] pItemInfos;

    m_iFirstVisible = iNewFirstVisible;

    if(!m_lvItemLocator->IsFixHeight() && m_lvItemLocator->GetTotalHeight() != nOldTotalHeight)
    {//update scroll range
        UpdateScrollBar();
        UpdateVisibleItems();//根据新的滚动条状态重新记录显示列表项
    }
}


void SMCListView::UpdateVisibleItem(int iItem)
{
	SASSERT(m_lvItemLocator->IsFixHeight());
	SItemPanel * pItem = GetItemPanel(iItem);
	SASSERT(pItem);
	m_adapter->getView(iItem,pItem,m_xmlTemplate.first_child());
}


void SMCListView::OnSize(UINT nType, CSize size)
{
    __super::OnSize(nType,size);
    UpdateScrollBar();
    UpdateHeaderCtrl();

    //update item window
    CRect rcClient=GetClientRect();
    SPOSITION pos = m_lstItems.GetHeadPosition();
    while(pos)
    {
        ItemInfo ii = m_lstItems.GetNext(pos);
        int idx = (int)ii.pItem->GetItemIndex();
        int nHei = m_lvItemLocator->GetItemHeight(idx);
        CRect rcItem(0,0,m_pHeader->GetTotalWidth(),nHei);
        ii.pItem->Move(rcItem);
    }

    UpdateVisibleItems();
}

void SMCListView::OnDestroy()
{
	if(m_adapter)
	{
		m_adapter->unregisterDataSetObserver(m_observer);
	}

    //destroy all itempanel
    SPOSITION pos = m_lstItems.GetHeadPosition();
    while(pos)
    {
        ItemInfo ii = m_lstItems.GetNext(pos);
        ii.pItem->Release();
    }
    m_lstItems.RemoveAll();

    for(int i=0;i<(int)m_itemRecycle.GetCount();i++)
    {
        SList<SItemPanel*> *pLstTypeItems = m_itemRecycle[i];
        SPOSITION pos = pLstTypeItems->GetHeadPosition();
        while(pos)
        {
            SItemPanel *pItem = pLstTypeItems->GetNext(pos);
            pItem->Release();
        }
        delete pLstTypeItems;
    }
    m_itemRecycle.RemoveAll();

    __super::OnDestroy();
}


//////////////////////////////////////////////////////////////////////////
void SMCListView::OnItemRequestRelayout(SItemPanel *pItem)
{
    //pItem->UpdateChildrenPosition();

}

BOOL SMCListView::IsItemRedrawDelay()
{
    return TRUE;
}

BOOL SMCListView::OnItemGetRect(SItemPanel *pItem,CRect &rcItem)
{
    int iPosition = (int)pItem->GetItemIndex();
    rcItem = _OnItemGetRect(iPosition);
    return TRUE;
}

CRect SMCListView::_OnItemGetRect(int iPosition)
{
    int nOffset = m_lvItemLocator->Item2Position(iPosition)-m_siVer.nPos;
    CRect rcItem = GetClientRect();
    rcItem.top += GetHeaderHeight() + nOffset;
    rcItem.bottom = rcItem.top + m_lvItemLocator->GetItemHeight(iPosition);
    rcItem.left -= m_siHoz.nPos;
    rcItem.right = rcItem.left + m_pHeader->GetTotalWidth();
    return rcItem;
}


void SMCListView::OnItemSetCapture(SItemPanel *pItem,BOOL bCapture)
{
    if(bCapture)
    {
        GetContainer()->OnSetSwndCapture(m_swnd);
        m_itemCapture=pItem;
    }else
    {
        GetContainer()->OnReleaseSwndCapture();
        m_itemCapture=NULL;
    }
}

void SMCListView::RedrawItem(SItemPanel *pItem)
{
    pItem->InvalidateRect(NULL);
}

SItemPanel * SMCListView::HitTest(CPoint & pt)
{
    SPOSITION pos = m_lstItems.GetHeadPosition();
    while(pos)
    {
        ItemInfo ii = m_lstItems.GetNext(pos);
        CRect rcItem = ii.pItem->GetItemRect();
        if(rcItem.PtInRect(pt)) 
        {
            pt-=rcItem.TopLeft();
            return ii.pItem;
        }
    }
    return NULL;
}

LRESULT SMCListView::OnMouseEvent(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    SetMsgHandled(FALSE);

    if(!m_adapter)
    {
        return 0;
    }

    LRESULT lRet=0;
    CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

    if(m_itemCapture)
    {
        CRect rcItem=m_itemCapture->GetItemRect();
        pt.Offset(-rcItem.TopLeft());
        lRet = m_itemCapture->DoFrameEvent(uMsg,wParam,MAKELPARAM(pt.x,pt.y));
    }
    else
    {
        if(uMsg==WM_LBUTTONDOWN || uMsg== WM_RBUTTONDOWN || uMsg==WM_MBUTTONDOWN)
        {//交给panel处理
            __super::ProcessSwndMessage(uMsg,wParam,lParam,lRet);
        }

        SItemPanel * pHover=HitTest(pt);
        if(pHover!=m_pHoverItem)
        {
            SItemPanel * nOldHover=m_pHoverItem;
            m_pHoverItem=pHover;
            if(nOldHover)
            {
                nOldHover->DoFrameEvent(WM_MOUSELEAVE,0,0);
                RedrawItem(nOldHover);
            }
            if(m_pHoverItem)
            {
                m_pHoverItem->DoFrameEvent(WM_MOUSEHOVER,wParam,MAKELPARAM(pt.x,pt.y));
                RedrawItem(m_pHoverItem);
            }
        }
        if(m_pHoverItem)
        {
            m_pHoverItem->DoFrameEvent(uMsg,wParam,MAKELPARAM(pt.x,pt.y));
        }
        else if (uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN || uMsg == WM_MBUTTONDOWN)
        {
            // 点击空白区域取消选中
            SetSel(-1, TRUE);
        }
        
    }
    
    if(uMsg==WM_LBUTTONUP || uMsg== WM_RBUTTONUP || uMsg==WM_MBUTTONUP)
    {//交给panel处理
        __super::ProcessSwndMessage(uMsg,wParam,lParam,lRet);
    }
    SetMsgHandled(TRUE);
    return 0;
}

LRESULT SMCListView::OnKeyEvent(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    LRESULT lRet=0;
    SItemPanel *pItem = GetItemPanel(m_iSelItem);
    if(pItem)
    {
        lRet=pItem->DoFrameEvent(uMsg,wParam,lParam);
        SetMsgHandled(pItem->IsMsgHandled());
    }else
    {
        SetMsgHandled(FALSE);
    }
    return lRet;
}

void SMCListView::OnMouseLeave()
{
	__super::OnMouseLeave();
    if(m_pHoverItem)
    {
        m_pHoverItem->DoFrameEvent(WM_MOUSELEAVE,0,0);
        m_pHoverItem = NULL;
    }

}

void SMCListView::OnKeyDown( TCHAR nChar, UINT nRepCnt, UINT nFlags )
{
    if(!m_adapter)
    {
        SetMsgHandled(FALSE);
        return;
    }
    
    if(m_iSelItem!=-1 && m_bWantTab)
    {
        SItemPanel *pItem = GetItemPanel(m_iSelItem);
        if(pItem)
        {
            pItem->DoFrameEvent(WM_KEYDOWN,nChar, MAKELONG(nFlags, nRepCnt));
            if(pItem->IsMsgHandled()) return;
        }
    }

    int  nNewSelItem = -1;
    SWindow *pOwner = GetOwner();
    if (pOwner && (nChar == VK_ESCAPE || nChar == VK_RETURN))
    {
        pOwner->SSendMessage(WM_KEYDOWN, nChar, MAKELONG(nFlags, nRepCnt));
        return;
    }

    if (nChar == VK_DOWN && m_iSelItem < m_adapter->getCount() - 1)
        nNewSelItem = m_iSelItem+1;
    else if (nChar == VK_UP && m_iSelItem > 0)
        nNewSelItem = m_iSelItem-1;
    else
    {        
        switch(nChar)
        {
        case VK_PRIOR:
            OnScroll(TRUE,SB_PAGEUP,0);
            break;
        case VK_NEXT:
            OnScroll(TRUE,SB_PAGEDOWN,0);
            break;
        case VK_HOME:
            OnScroll(TRUE,SB_TOP,0);
            break;
        case VK_END:
            OnScroll(TRUE,SB_BOTTOM,0);
            break;
        }

        if(nChar == VK_PRIOR || nChar == VK_HOME)
        {
            if(!m_lstItems.IsEmpty())
            {
                nNewSelItem = (int)m_lstItems.GetHead().pItem->GetItemIndex();
            }
        }else if(nChar == VK_NEXT || nChar == VK_END)
        {
            if(!m_lstItems.IsEmpty())
            {
                nNewSelItem = (int)m_lstItems.GetTail().pItem->GetItemIndex();
            }
        }
    }

    if(nNewSelItem!=-1)
    {
        EnsureVisible(nNewSelItem);
        // 取消选择通知
        SetSel(nNewSelItem, TRUE);
    }
}

void SMCListView::EnsureVisible( int iItem )
{
    if(iItem<0 || iItem>=m_adapter->getCount()) return;

    int iFirstVisible= m_iFirstVisible;
    int iLastVisible = m_iFirstVisible + m_lstItems.GetCount();

    if(iItem>=iFirstVisible && iItem<iLastVisible)
    {
        if(iItem == iFirstVisible)
        {
            int pos = m_lvItemLocator->Item2Position(iItem);
            OnScroll(TRUE,SB_THUMBPOSITION,pos);
        }else if(iItem == iLastVisible-1)
        {
            if(iItem == m_adapter->getCount()-1)
                OnScroll(TRUE,SB_BOTTOM,0);
            else
            {
                int pos = m_lvItemLocator->Item2Position(iItem+1) - m_siVer.nPage;
                OnScroll(TRUE,SB_THUMBPOSITION,pos);
            }
        }
        
        return;
    }


    if(iItem < iFirstVisible)
    {//scroll up
        int pos = m_lvItemLocator->Item2Position(iItem);
        OnScroll(TRUE,SB_THUMBPOSITION,pos);
    }else // if(iItem >= iLastVisible)
    {//scroll down
        if(iItem == m_adapter->getCount()-1)
        {
            OnScroll(TRUE,SB_BOTTOM,0);
        }else
        {
            int pos = m_lvItemLocator->Item2Position(iItem+1)-m_siVer.nPage;
            OnScroll(TRUE,SB_THUMBPOSITION,pos);
        }
    }
}

BOOL SMCListView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    SItemPanel *pSelItem = GetItemPanel(m_iSelItem);
    if(pSelItem)
    {
        CRect rcItem = pSelItem->GetItemRect();
        CPoint pt2=pt-rcItem.TopLeft();
        if(pSelItem->DoFrameEvent(WM_MOUSEWHEEL,MAKEWPARAM(nFlags,zDelta),MAKELPARAM(pt2.x,pt2.y)))
            return TRUE;
    }
    return __super::OnMouseWheel(nFlags, zDelta, pt);
}

int SMCListView::GetScrollLineSize(BOOL bVertical)
{
    return m_lvItemLocator->GetScrollLineSize();
}

SItemPanel * SMCListView::GetItemPanel(int iItem)
{
    if(iItem<0 || iItem>=m_adapter->getCount()) 
        return NULL; 
    SPOSITION pos = m_lstItems.GetHeadPosition();
    while(pos)
    {
        ItemInfo ii = m_lstItems.GetNext(pos);
        if((int)ii.pItem->GetItemIndex() == iItem)
            return ii.pItem;
    }
    return NULL;
}


void SMCListView::SetItemLocator(IListViewItemLocator *pItemLocator)
{
    m_lvItemLocator = pItemLocator;
    if(m_lvItemLocator) 
	{
		m_lvItemLocator->SetAdapter(GetAdapter());
		m_lvItemLocator->SetScale(GetScale());
	}
    onDataSetChanged();
}

BOOL SMCListView::OnUpdateToolTip(CPoint pt, SwndToolTipInfo & tipInfo)
{
    if(!m_pHoverItem)
        return __super::OnUpdateToolTip(pt,tipInfo);
    return m_pHoverItem->OnUpdateToolTip(pt,tipInfo);
}

void SMCListView::SetSel(int iItem,BOOL bNotify)
{
    if(!m_adapter) return;

    if(iItem>=m_adapter->getCount())
        return;

    if(iItem<0) iItem = -1;

    int nOldSel = m_iSelItem;
    int nNewSel = iItem;

    m_iSelItem = nNewSel;
    if(bNotify)
    {
        EventLVSelChanging evt(this);
		evt.bCancel = FALSE;
		evt.iOldSel = nOldSel;
        evt.iNewSel = nNewSel;
        FireEvent(evt);
        if(evt.bCancel) 
        {//Cancel SetSel and restore selection state
            m_iSelItem = nOldSel;
            return;
        }
    }

    if(nOldSel == nNewSel)
        return;

    m_iSelItem = nOldSel;
    SItemPanel *pItem = GetItemPanel(nOldSel);
    if(pItem)
    {
        pItem->GetFocusManager()->SetFocusedHwnd((SWND)-1);
        pItem->ModifyItemState(0,WndState_Check);
        RedrawItem(pItem);
    }
    m_iSelItem = nNewSel;
    pItem = GetItemPanel(nNewSel);
    if(pItem)
    {
        pItem->ModifyItemState(WndState_Check,0);
        RedrawItem(pItem);
    }
    
    if(bNotify)
    {
        EventLVSelChanged evt(this);
        evt.iOldSel = nOldSel;
        evt.iNewSel = nNewSel;
        FireEvent(evt);
    }

}


UINT SMCListView::OnGetDlgCode()
{
    if(m_bWantTab) return SC_WANTALLKEYS;
    else return SC_WANTARROWS|SC_WANTSYSKEY;
}

void SMCListView::OnKillFocus(SWND wndFocus)
{
    __super::OnKillFocus(wndFocus);

    if(m_iSelItem==-1) return;

    SItemPanel *pSelPanel = GetItemPanel(m_iSelItem);
    if(pSelPanel) pSelPanel->GetFocusManager()->StoreFocusedView();
}

void SMCListView::OnSetFocus(SWND wndOld)
{
    __super::OnSetFocus(wndOld);
    if(m_iSelItem==-1) return;

    SItemPanel *pSelPanel = GetItemPanel(m_iSelItem);
    if(pSelPanel) pSelPanel->GetFocusManager()->RestoreFocusedView();
}

BOOL SMCListView::OnSetCursor(const CPoint &pt)
{
    BOOL bRet=FALSE;
    if(m_itemCapture)
    {
        CRect rcItem=m_itemCapture->GetItemRect();
        bRet=m_itemCapture->DoFrameEvent(WM_SETCURSOR,0,MAKELPARAM(pt.x-rcItem.left,pt.y-rcItem.top))!=0;
    }
    else if(m_pHoverItem)
    {
        CRect rcItem=m_pHoverItem->GetItemRect();
        bRet=m_pHoverItem->DoFrameEvent(WM_SETCURSOR,0,MAKELPARAM(pt.x-rcItem.left,pt.y-rcItem.top))!=0;
    }
    if(!bRet)
    {
        bRet=__super::OnSetCursor(pt);
    }
    return bRet;

}

bool SMCListView::OnItemClick(EventArgs *pEvt)
{
    SItemPanel *pItemPanel = sobj_cast<SItemPanel>(pEvt->sender);
    SASSERT(pItemPanel);
    int iItem = (int)pItemPanel->GetItemIndex();
    if(iItem != m_iSelItem)
    {
        SetSel(iItem,TRUE);
    }
    return true;
 
}

void SMCListView::OnColorize(COLORREF cr)
{
    __super::OnColorize(cr);
	DispatchMessage2Items(UM_SETCOLORIZE,cr,0);
    SPOSITION pos = m_lstItems.GetHeadPosition();
    while(pos)
    {
        ItemInfo ii = m_lstItems.GetNext(pos);
        ii.pItem->DoColorize(cr);
    }
}


void SMCListView::OnScaleChanged(int nScale)
{
	__super::OnScaleChanged(nScale);
	if(m_lvItemLocator)	m_lvItemLocator->SetScale(nScale);
	DispatchMessage2Items(UM_SETSCALE,nScale,0);
}

HRESULT SMCListView::OnLanguageChanged()
{
	HRESULT hret =__super::OnLanguageChanged();
	DispatchMessage2Items(UM_SETLANGUAGE,0,0);
	return hret;
}

void SMCListView::DispatchMessage2Items(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	SPOSITION pos = m_lstItems.GetHeadPosition();
	while (pos)
	{
		ItemInfo ii = m_lstItems.GetNext(pos);
		ii.pItem->SDispatchMessage(uMsg, wParam, lParam);
	}
	for(UINT i=0;i<m_itemRecycle.GetCount();i++)
	{
		SList<SItemPanel*> *pLstTypeItems = m_itemRecycle[i];
		SPOSITION pos = pLstTypeItems->GetHeadPosition();
		while(pos)
		{
			SItemPanel *pItem = pLstTypeItems->GetNext(pos);
			pItem->SDispatchMessage(uMsg, wParam, lParam);
		}
	}
}

}//end of namespace 
