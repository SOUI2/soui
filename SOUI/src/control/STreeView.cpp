#include "souistd.h"
#include "control/STreeView.h"
#include <algorithm>

namespace SOUI 
{
	class STreeViewDataSetObserver : public TObjRefImpl<ITvDataSetObserver>
	{
	public:
		STreeViewDataSetObserver(STreeView *pView):m_pOwner(pView)
		{
		}
		virtual void onBranchChanged(HTREEITEM hBranch)
		{
		    m_pOwner->onBranchChanged(hBranch);
		}
		
		virtual void onBranchInvalidated(HTREEITEM hBranch)
		{
		    m_pOwner->onBranchInvalidated(hBranch);
		}
		
        virtual void onBranchExpandedChanged(HTREEITEM hBranch,BOOL bExpandedOld,BOOL bExpandedNew)
        {
            m_pOwner->onBranchExpandedChanged(hBranch,bExpandedOld,bExpandedNew);
        }
	protected:
		STreeView * m_pOwner;
	};

	//////////////////////////////////////////////////////////////////////////
    STreeViewItemLocator::STreeViewItemLocator(int nIndent) :m_nLineHeight(50),m_nIndent(nIndent),m_szDef(10,50)
    {

    }

    STreeViewItemLocator::~STreeViewItemLocator()
    {

    }

    void STreeViewItemLocator::SetAdapter(ITvAdapter *pAdapter)
    {
        m_adapter = pAdapter;
    }

    void STreeViewItemLocator::_InitBranch(HTREEITEM hItem)
    {
        if(hItem != ITvAdapter::ITEM_ROOT)
        {
            _SetItemHeight(hItem,m_szDef.cy);
            _SetItemWidth(hItem,m_szDef.cx);
        }else
        {
            _SetItemHeight(hItem,0);
            _SetItemWidth(hItem,0);
        }
        if(m_adapter->HasChildren(hItem))
        {//有子节点
            HTREEITEM hChild = m_adapter->GetFirstChildItem(hItem);
            int nBranchHeight = 0;
            while(hChild != ITvAdapter::ITEM_NULL)
            {
                //设置偏移
                _SetItemOffset(hChild,nBranchHeight);
                _InitBranch(hChild);
                nBranchHeight += _GetItemVisibleHeight(hChild);
                hChild = m_adapter->GetNextSiblingItem(hChild);
            }
            _SetBranchHeight(hItem,nBranchHeight);
            //设置默认宽度
            _SetBranchWidth(hItem,m_szDef.cx + m_nIndent);
        }else
        {//无子节点
            _SetBranchHeight(hItem,0);
            _SetBranchWidth(hItem,0);
        }
    }

    BOOL STreeViewItemLocator::_IsItemVisible(HTREEITEM hItem) const
    {
        return m_adapter->IsItemVisible(hItem);
    }

    HTREEITEM STreeViewItemLocator::_Position2Item(int position,HTREEITEM hParent,int nParentPosition) const
    {
        if(position<nParentPosition || position>=(nParentPosition+_GetItemVisibleHeight(hParent)))
            return ITvAdapter::ITEM_NULL;

        int nItemHeight = GetItemHeight(hParent);
        int nPos = nParentPosition + nItemHeight;

        if(nPos>position) return hParent;

        SASSERT(IsItemExpanded(hParent));

        int nParentBranchHeight = _GetBranchHeight(hParent);

        if(position-nPos < nParentBranchHeight/2)
        {//从first开始查找
            HTREEITEM hItem = m_adapter->GetFirstChildItem(hParent);
            while(hItem)
            {
                int nBranchHeight = _GetItemVisibleHeight(hItem);
                if(nPos + nBranchHeight > position)
                {
                    return _Position2Item(position,hItem,nPos);
                }
                nPos += nBranchHeight;
                hItem = m_adapter->GetNextSiblingItem(hItem);
            }
        }else
        {//从last开始查找
            nPos += nParentBranchHeight;

            HTREEITEM hItem = m_adapter->GetLastChildItem(hParent);
            while(hItem)
            {
                int nBranchHeight = _GetItemVisibleHeight(hItem);
                nPos -= nBranchHeight;
                if(nPos <= position)
                {
                    return _Position2Item(position,hItem,nPos);
                }
                hItem = m_adapter->GetPrevSiblingItem(hItem);
            }

        }

        SASSERT(FALSE);//不应该走到这里来
        return ITvAdapter::ITEM_NULL;
    }

    int STreeViewItemLocator::_GetItemVisibleWidth(HTREEITEM hItem) const
    {
        int nRet = GetItemWidth(hItem);
        if(m_adapter->GetFirstChildItem(hItem)!=ITvAdapter::ITEM_NULL)
        {
            int nIndent = m_adapter->GetParentItem(hItem) == ITvAdapter::ITEM_ROOT?0:m_nIndent;
            nRet = (std::max)(nRet,_GetBranchWidth(hItem)+nIndent);
        }
        return nRet;
    }

    int STreeViewItemLocator::_GetItemVisibleHeight(HTREEITEM hItem) const
    {
        int nRet = GetItemHeight(hItem);
        if(IsItemExpanded(hItem)) nRet += _GetBranchHeight(hItem);
        return nRet;
    }

    void STreeViewItemLocator::_SetItemHeight(HTREEITEM hItem,int nHeight)
    {
        m_adapter->SetItemDataByIndex(hItem,ITvAdapter::DATA_INDEX_ITEM_HEIGHT,nHeight);
    }

    void STreeViewItemLocator::_SetItemOffset(HTREEITEM hItem, int nOffset)
    {
        m_adapter->SetItemDataByIndex(hItem,ITvAdapter::DATA_INDEX_ITEM_OFFSET,nOffset);
    }

    int STreeViewItemLocator::_GetItemOffset(HTREEITEM hItem) const
    {
        return (int)m_adapter->GetItemDataByIndex(hItem,ITvAdapter::DATA_INDEX_ITEM_OFFSET);
    }

    void STreeViewItemLocator::_UpdateSiblingsOffset(HTREEITEM hItem)
    {
        int nOffset = _GetItemOffset(hItem);
        nOffset += _GetItemVisibleHeight(hItem);

        HTREEITEM hSib = m_adapter->GetNextSiblingItem(hItem);
        while(hSib != ITvAdapter::ITEM_NULL)
        {
            _SetItemOffset(hSib,nOffset);
            nOffset += _GetItemVisibleHeight(hSib);
            hSib = m_adapter->GetNextSiblingItem(hSib);
        }
        //注意更新各级父节点的偏移量
        HTREEITEM hParent = m_adapter->GetParentItem(hItem);
        if(hParent != ITvAdapter::ITEM_NULL && hParent != ITvAdapter::ITEM_ROOT && IsItemExpanded(hParent))
        {
            _UpdateSiblingsOffset(hParent);
        }
    }

    void STreeViewItemLocator::_UpdateBranchHeight(HTREEITEM hItem,int nDiff)
    {
        HTREEITEM hParent = m_adapter->GetParentItem(hItem);
        while(hParent != ITvAdapter::ITEM_NULL)
        {
            int nBranchHeight = _GetBranchHeight(hParent);
            _SetBranchHeight(hParent,nBranchHeight+nDiff);
            hParent = m_adapter->GetParentItem(hParent);
        }
    }

    void STreeViewItemLocator::_SetBranchHeight(HTREEITEM hItem ,int nHeight)
    {
        m_adapter->SetItemDataByIndex(hItem,ITvAdapter::DATA_INDEX_BRANCH_HEIGHT,nHeight);
    }

    int STreeViewItemLocator::_GetBranchHeight(HTREEITEM hItem) const
    {
        return (int)m_adapter->GetItemDataByIndex(hItem,ITvAdapter::DATA_INDEX_BRANCH_HEIGHT);
    }

    void STreeViewItemLocator::_SetItemWidth(HTREEITEM hItem,int nWidth)
    {
        m_adapter->SetItemDataByIndex(hItem,ITvAdapter::DATA_INDEX_ITEM_WIDTH,nWidth);
    }

    void STreeViewItemLocator::_SetBranchWidth(HTREEITEM hBranch,int nWidth)
    {
        m_adapter->SetItemDataByIndex(hBranch,ITvAdapter::DATA_INDEX_BRANCH_WIDTH,nWidth);
    }

    int STreeViewItemLocator::_GetBranchWidth(HTREEITEM hBranch) const
    {
        return (int)m_adapter->GetItemDataByIndex(hBranch,ITvAdapter::DATA_INDEX_BRANCH_WIDTH);
    }

    void STreeViewItemLocator::_UpdateBranchWidth(HTREEITEM hItem,int nOldWidth,int nNewWidth)
    {
        HTREEITEM hParent = m_adapter->GetParentItem(hItem);
        if(hParent == ITvAdapter::ITEM_NULL)
            return;
        int nCurBranchWidth = _GetBranchWidth(hParent);

        int nIndent = hParent==ITvAdapter::ITEM_ROOT?0:m_nIndent;
        if(nCurBranchWidth != nOldWidth+nIndent) 
        {//父节点的宽度不是由当前结点控制的
            if(nCurBranchWidth < nNewWidth+nIndent)
            {//新宽度扩展了父节点的显示宽度
                _SetBranchWidth(hParent,nNewWidth + nIndent);
                if(IsItemExpanded(hParent)) _UpdateBranchWidth(hParent,nCurBranchWidth,nNewWidth + nIndent);
            }
        }else
        {//父节点的宽度正好是由hItem的显示宽度
            int nNewBranchWidth;
            if(nNewWidth>nOldWidth)
            {
                nNewBranchWidth = nNewWidth + nIndent;
            }else
            {
                HTREEITEM hSib = m_adapter->GetFirstChildItem(hParent);
                nNewBranchWidth = 0;
                while(hSib!=ITvAdapter::ITEM_NULL)
                {
                    nNewBranchWidth = (std::max)(nNewBranchWidth,_GetItemVisibleWidth(hSib));
                    hSib = m_adapter->GetNextSiblingItem(hSib);
                }
                nNewBranchWidth += nIndent;
            }
            _SetBranchWidth(hParent,nNewBranchWidth);
            if(IsItemExpanded(hParent)) _UpdateBranchWidth(hParent,nCurBranchWidth,nNewBranchWidth);
        }
    }

    BOOL STreeViewItemLocator::IsItemExpanded(HTREEITEM hItem) const
    {
        return (BOOL)m_adapter->IsItemExpanded(hItem);
    }

    int STreeViewItemLocator::GetItemIndent(HTREEITEM hItem) const
    {
        int nRet = 0;
        for(;;)
        {
            hItem = m_adapter->GetParentItem(hItem);
            if(hItem == ITvAdapter::ITEM_ROOT) break;
            nRet += m_nIndent;
        }
        return nRet;
    }

    int STreeViewItemLocator::GetItemHeight(HTREEITEM hItem) const
    {
        return (int)m_adapter->GetItemDataByIndex(hItem,ITvAdapter::DATA_INDEX_ITEM_HEIGHT);
    }

    int STreeViewItemLocator::GetItemWidth(HTREEITEM hItem) const
    {
        return (int)m_adapter->GetItemDataByIndex(hItem,ITvAdapter::DATA_INDEX_ITEM_WIDTH);
    }

    void STreeViewItemLocator::SetItemHeight(HTREEITEM hItem,int nHeight)
    {
        int nOldHeight = GetItemHeight(hItem);
        _SetItemHeight(hItem,nHeight);
        if(nOldHeight != nHeight)
        {
            _UpdateBranchHeight(hItem,nHeight-nOldHeight);
            _UpdateSiblingsOffset(hItem);
        }
    }

    void STreeViewItemLocator::SetItemWidth(HTREEITEM hItem,int nWidth)
    {
        int nOldWidth = GetItemWidth(hItem);
        if(nOldWidth == nWidth) return;
        int nOldBranchWidth = _GetItemVisibleWidth(hItem);
        _SetItemWidth(hItem,nWidth);
        int nNewBranchWidth = _GetItemVisibleWidth(hItem);
        if(nOldBranchWidth == nNewBranchWidth) return;
        _UpdateBranchWidth(hItem,nOldBranchWidth,nNewBranchWidth);
    }

    HTREEITEM STreeViewItemLocator::Position2Item(int position) const
    {
        return _Position2Item(position,ITvAdapter::ITEM_ROOT,0);
    }

    int STreeViewItemLocator::Item2Position(HTREEITEM hItem) const
    {
        if(!_IsItemVisible(hItem))
        {
            SASSERT(FALSE);
            return -1;
        }

        int nRet = 0;
        //获得父节点开始位置
        HTREEITEM hParent = m_adapter->GetParentItem(hItem);
        if(hParent!=ITvAdapter::ITEM_NULL && hParent != ITvAdapter::ITEM_ROOT)
        {
            nRet = Item2Position(hParent);
            //越过父节点
            nRet += GetItemHeight(hParent);
        }
        //越过前面兄弟结点
        nRet += _GetItemOffset(hItem);

        return nRet;
    }

    int STreeViewItemLocator::GetScrollLineSize() const
    {
        return m_nLineHeight;
    }

    int STreeViewItemLocator::GetTotalWidth() const
    {
        return (int)m_adapter->GetItemDataByIndex(ITvAdapter::ITEM_ROOT,ITvAdapter::DATA_INDEX_BRANCH_WIDTH);
    }

    int STreeViewItemLocator::GetTotalHeight() const
    {
        return (int)m_adapter->GetItemDataByIndex(ITvAdapter::ITEM_ROOT,ITvAdapter::DATA_INDEX_BRANCH_HEIGHT);
    }

    void STreeViewItemLocator::OnBranchExpandedChanged(HTREEITEM hItem,BOOL bExpandedOld,BOOL bExpandedNew)
    {
        if(bExpandedNew == bExpandedOld) return;

        int nBranchHei = _GetBranchHeight(hItem);
        HTREEITEM hParent = m_adapter->GetParentItem(hItem);
        while(hParent != ITvAdapter::ITEM_NULL)
        {
            _SetBranchHeight(hParent,_GetBranchHeight(hParent)+nBranchHei*(bExpandedNew?1:-1));
            if(!IsItemExpanded(hParent)) break;
            hParent = m_adapter->GetParentItem(hParent);
        }
        _UpdateSiblingsOffset(hItem);
    }

    void STreeViewItemLocator::OnBranchChanged(HTREEITEM hItem)
    {
        //初始化列表项高度等数据
        int nVisibleHeightOld = _GetItemVisibleHeight(hItem);
        _InitBranch(hItem);
        int nVisibleHeightNew = _GetItemVisibleHeight(hItem);
        int nDiff = nVisibleHeightNew - nVisibleHeightOld;
        if(nDiff == 0 || hItem == ITvAdapter::ITEM_ROOT) 
            return;

        HTREEITEM hParent = m_adapter->GetParentItem(hItem);
        while(hParent!=ITvAdapter::ITEM_NULL)
        {
            _SetBranchHeight(hParent,_GetBranchHeight(hParent)+nDiff);
            hParent = m_adapter->GetParentItem(hParent);    
        }
        _UpdateSiblingsOffset(hItem);
    }

	//////////////////////////////////////////////////////////////////////////
	STreeView::STreeView()
		: m_itemCapture(NULL)
		, m_pHoverItem(NULL)
		, m_hSelected(ITvAdapter::ITEM_NULL)
		, m_pVisibleMap(new VISIBLEITEMSMAP)
		, m_bWantTab(FALSE)
	{
		m_bFocusable = TRUE;
		
		m_evtSet.addEvent(EVENTID(EventTVSelChanging));
        m_evtSet.addEvent(EVENTID(EventTVSelChanged));
		m_observer.Attach(new STreeViewDataSetObserver(this));
		m_tvItemLocator.Attach(new STreeViewItemLocator);
	}

	STreeView::~STreeView()
	{
	    delete m_pVisibleMap;
	}
	
	BOOL STreeView::SetAdapter( ITvAdapter * adapter )
	{
		if (m_adapter)
		{
			m_adapter->unregisterDataSetObserver(m_observer);
		}
		m_adapter = adapter;
		if (m_adapter)
		{
			m_adapter->registerDataSetObserver(m_observer);
			
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
            SPOSITION pos=m_pVisibleMap->GetStartPosition();
            while(pos)
            {
                ItemInfo ii = m_pVisibleMap->GetNext(pos)->m_value;
                ii.pItem->DestroyWindow();
            }
            m_pVisibleMap->RemoveAll();
		}
		
		if(m_tvItemLocator)
		{
            m_adapter->InitByTemplate(m_xmlTemplate.first_child());

		    m_tvItemLocator->SetAdapter(adapter);
		    
            for(int i=0;i<m_adapter->getViewTypeCount();i++)
            {
                m_itemRecycle.Add(new SList<SItemPanel*>());
            }

		}
		onBranchChanged(ITvAdapter::ITEM_ROOT);
		return TRUE;
	}
	
	
	BOOL STreeView::CreateChildren(pugi::xml_node xmlNode) {
		pugi::xml_node xmlTemplate = xmlNode.child(L"template");
		if (xmlTemplate)
		{
			m_xmlTemplate.append_copy(xmlTemplate);
		}
		return TRUE;
	}
	
	void STreeView::OnPaint( IRenderTarget * pRT )
	{
		SPainter painter;
		BeforePaint(pRT, painter);
		
        CRect rcClient;
        GetClientRect(&rcClient);
        pRT->PushClipRect(&rcClient, RGN_AND);

        CRect rcClip, rcInter;
        pRT->GetClipBox(&rcClip);

		CPoint pt(0,-1);
		for(SPOSITION pos = m_visible_items.GetHeadPosition();pos;)
		{
		    ItemInfo ii = m_visible_items.GetNext(pos);
		    HTREEITEM hItem = (HTREEITEM)ii.pItem->GetItemIndex();
		    if(pt.y == -1)
		    {
		        pt.y = m_tvItemLocator->Item2Position(hItem)-m_siVer.nPos;
		    }
		    CSize szItem(m_tvItemLocator->GetItemWidth(hItem),m_tvItemLocator->GetItemHeight(hItem));
		    
		    pt.x = m_tvItemLocator->GetItemIndent(hItem) - m_siHoz.nPos;
		    
		    CRect rcItem(pt,szItem);
		    rcItem.OffsetRect(rcClient.TopLeft());
		    if(!(rcItem & rcClip).IsRectEmpty())
		    {//draw the item
		        ii.pItem->Draw(pRT,rcItem);
		    }
            pt.y += m_tvItemLocator->GetItemHeight(hItem);
		}
		
        pRT->PopClip();

		AfterPaint(pRT, painter);
	}

	void STreeView::OnSize( UINT nType, CSize size )
	{
		__super::OnSize(nType, size);
		if(!m_adapter) return;
		UpdateScrollBar();
		UpdateVisibleItems();
	}

	void STreeView::OnDestroy()
	{
		if(m_adapter)
		{
			m_adapter->unregisterDataSetObserver(m_observer);
		}

		//destroy all itempanel
		SPOSITION pos = m_visible_items.GetHeadPosition();
		while(pos)
		{
			ItemInfo ii = m_visible_items.GetNext(pos);
			ii.pItem->Release();
		}
		m_visible_items.RemoveAll();

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

	void STreeView::EnsureVisible(HTREEITEM hItem)
	{
	    //保证hItem被正确展开
	    HTREEITEM hParent = m_adapter->GetParentItem(hItem);
	    while(hParent!=ITvAdapter::ITEM_ROOT)
	    {
	        m_adapter->ExpandItem(hParent,ITvAdapter::TVC_EXPAND);
	        hParent = m_adapter->GetParentItem(hParent);
	    }
	    //滚动视图
        int nPos = m_tvItemLocator->Item2Position(hItem);
        int nHeight = m_tvItemLocator->GetItemHeight(hItem);
        if(nPos + nHeight <= m_siVer.nPos)// && nPos < m_siVer.nPos + m_siVer.nPage) 
        {
            OnScroll(TRUE,SB_THUMBPOSITION,nPos);
        }
        else if(nPos > m_siVer.nPos + (int)m_siVer.nPage)
        {
            OnScroll(TRUE,SB_THUMBPOSITION,nPos + nHeight - m_siVer.nPage);
        }
        int nIndent = m_tvItemLocator->GetItemIndent(hItem);
        int nWidth = m_tvItemLocator->GetItemWidth(hItem);
        
        if(nIndent + nWidth <= m_siVer.nPos)
        {
            OnScroll(FALSE,SB_THUMBPOSITION,nIndent);        
        }else if(nIndent < m_siHoz.nPos + (int)m_siHoz.nPage)
        {
            OnScroll(FALSE,SB_THUMBPOSITION,nIndent + nWidth - m_siHoz.nPage);
        }
	}

    void STreeView::SetSel(HTREEITEM hItem,BOOL bNotify/*=FALSE*/)
    {
        if(!m_adapter) return;

        if(bNotify)
        {
            EventTVSelChanging evt(this);
			evt.bCancel = FALSE;
            evt.hOldSel = m_hSelected;
            evt.hNewSel = hItem;
            FireEvent(evt);
            if(evt.bCancel) 
            {//Cancel SetSel and restore selection state
                return;
            }
        }
        
        if(m_hSelected == hItem)
            return;

        SItemPanel *pItem = GetItemPanel(m_hSelected);
        if(pItem)
        {
            pItem->GetFocusManager()->SetFocusedHwnd((SWND)-1);
            pItem->ModifyItemState(0,WndState_Check);
            RedrawItem(pItem);
        }
        m_hSelected = hItem;
        pItem = GetItemPanel(m_hSelected);
        if(pItem)
        {
            pItem->ModifyItemState(WndState_Check,0);
            RedrawItem(pItem);
        }   
        
        if(bNotify)
        {
            EventTVSelChanged evt(this);
            evt.hOldSel = m_hSelected;
            evt.hNewSel = hItem;
            FireEvent(evt);
        }
     
    }

    void STreeView::OnKeyDown(TCHAR nChar, UINT nRepCnt, UINT nFlags)
    {
        if(!m_adapter)
        {
            SetMsgHandled(FALSE);
            return;
        }

        if(m_hSelected != ITvAdapter::ITEM_NULL  && m_bWantTab)
        {
            SItemPanel *pItem = GetItemPanel(m_hSelected);
            if(pItem)
            {
                pItem->DoFrameEvent(WM_KEYDOWN,nChar, MAKELONG(nFlags, nRepCnt));
                if(pItem->IsMsgHandled()) return;
            }
        }

        SWindow *pOwner = GetOwner();
        if (pOwner && (nChar == VK_ESCAPE || nChar == VK_RETURN))
        {
            pOwner->SSendMessage(WM_KEYDOWN, nChar, MAKELONG(nFlags, nRepCnt));
            return;
        }
        
        HTREEITEM  nNewSelItem = ITvAdapter::ITEM_NULL;
        switch(nChar)
        {
        case VK_DOWN:   nNewSelItem = (m_hSelected == ITvAdapter::ITEM_NULL)? m_adapter->GetFirstVisibleItem():m_adapter->GetNextVisibleItem(m_hSelected);break;
        case VK_UP:     nNewSelItem = (m_hSelected == ITvAdapter::ITEM_NULL)? m_adapter->GetFirstVisibleItem():m_adapter->GetPrevVisibleItem(m_hSelected);break;
        case VK_PRIOR:  OnScroll(TRUE,SB_PAGEUP,0);         break;
        case VK_NEXT:   OnScroll(TRUE,SB_PAGEDOWN,0);       break;
        case VK_HOME:   OnScroll(TRUE,SB_TOP,0);            break;
        case VK_END:    OnScroll(TRUE,SB_BOTTOM,0);         break;
        case VK_LEFT:
            if(m_hSelected != ITvAdapter::ITEM_NULL)
            {
                if(m_adapter->HasChildren(m_hSelected) && m_adapter->IsItemExpanded(m_hSelected))
                    m_adapter->ExpandItem(m_hSelected,ITvAdapter::TVC_COLLAPSE);//collapse the selected item
                else
                    nNewSelItem = m_adapter->GetPrevVisibleItem(m_hSelected);
            }
            break;
        case VK_RIGHT:
            if(m_hSelected != ITvAdapter::ITEM_NULL)
            {
                if(m_adapter->HasChildren(m_hSelected) && !m_adapter->IsItemExpanded(m_hSelected))
                    m_adapter->ExpandItem(m_hSelected,ITvAdapter::TVC_EXPAND);//collapse the selected item
                else
                    nNewSelItem = m_adapter->GetNextVisibleItem(m_hSelected);
            }
            break;
        }        

        

        if(nNewSelItem!= ITvAdapter::ITEM_NULL)
        {
            EnsureVisible(nNewSelItem);
            SetSel(nNewSelItem,TRUE);
        }
    }

	LRESULT STreeView::OnKeyEvent(UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
        LRESULT lRet = 0;
        SItemPanel *pItem = GetItemPanel(m_hSelected);
        if (pItem)
        {
            lRet = pItem->DoFrameEvent(uMsg, wParam, lParam);
            SetMsgHandled(pItem->IsMsgHandled());
        }
        else
        {
            SetMsgHandled(FALSE);
        }
        return lRet;
    }

	void STreeView::UpdateScrollBar()
	{
        CSize szView;
        szView.cx = m_tvItemLocator->GetTotalWidth();
        szView.cy = m_tvItemLocator->GetTotalHeight();
        
        CRect rcClient;
        SWindow::GetClientRect(&rcClient);//不计算滚动条大小
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

            if (size.cx-GetSbWidth() < szView.cx && !m_adapter->isViewWidthMatchParent())
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

            if (size.cx < szView.cx && !m_adapter->isViewWidthMatchParent())
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
        if (HasScrollBar(FALSE) && m_siHoz.nPos+(int)m_siHoz.nPage>szView.cx)
        {
            m_siHoz.nPos = szView.cx-m_siHoz.nPage;
        }

        if (HasScrollBar(TRUE) && m_siVer.nPos +(int)m_siVer.nPage>szView.cy)
        {
            m_siVer.nPos = szView.cy-m_siVer.nPage;
        }

        SetScrollPos(TRUE, m_siVer.nPos, TRUE);
        SetScrollPos(FALSE, m_siHoz.nPos, TRUE);

        //  重新计算客户区及非客户区
        SSendMessage(WM_NCCALCSIZE);

        Invalidate();
	}

	void STreeView::UpdateVisibleItems()
	{
        if(!m_adapter) return;
        HTREEITEM hItem = m_tvItemLocator->Position2Item(m_siVer.nPos);
		if (hItem == ITvAdapter::ITEM_NULL)
		{
			//如果没有可显示的，则移除所有item
			m_visible_items.RemoveAll();
			m_pVisibleMap->RemoveAll();
			return;
		}
        
        CSize szOldView;
        szOldView.cx = m_tvItemLocator->GetTotalWidth();
        szOldView.cy = m_tvItemLocator->GetTotalHeight();
        
        VISIBLEITEMSMAP *pMapOld = m_pVisibleMap;
        m_pVisibleMap = new VISIBLEITEMSMAP;
        
        CRect rcClient = GetClientRect();
        CRect rcContainer=rcClient;
        rcContainer.MoveToXY(0,0);
        rcContainer.bottom=10000;
        
        int nOffset = m_tvItemLocator->Item2Position(hItem) - m_siVer.nPos;
        
        m_visible_items.RemoveAll();
        while(hItem != ITvAdapter::ITEM_NULL)
        {
            VISIBLEITEMSMAP::CPair *pFind = pMapOld->Lookup(hItem);
            ItemInfo ii;
			ii.nType = m_adapter->getViewType(hItem);
            if(pFind && pFind->m_value.nType == ii.nType)
            {//re use the previous item;
					ii = pFind->m_value;
					pMapOld->RemoveKey(hItem);
            }else
            {
                SList<SItemPanel *> *lstRecycle = m_itemRecycle.GetAt(ii.nType);
                if(lstRecycle->IsEmpty())
                {//创建一个新的列表项
                    ii.pItem = SItemPanel::Create(this,pugi::xml_node(),this);
                    ii.pItem->GetEventSet()->subscribeEvent(EventItemPanelClick::EventID,Subscriber(&STreeView::OnItemClick,this));
                    ii.pItem->GetEventSet()->subscribeEvent(EventItemPanelDbclick::EventID,Subscriber(&STreeView::OnItemDblClick,this));
                }else
                {
                    ii.pItem = lstRecycle->RemoveHead();
                }
                ii.pItem->SetItemIndex(hItem);
            }
            m_pVisibleMap->SetAt(hItem,ii);
            ii.pItem->SetVisible(TRUE);
            
            if(hItem == m_hSelected)
                ii.pItem->ModifyItemState(WndState_Check,0);
            else
                ii.pItem->ModifyItemState(0,WndState_Check);
                
            if(m_pHoverItem && hItem == (HTREEITEM)m_pHoverItem->GetItemIndex())
                ii.pItem->ModifyItemState(WndState_Hover,0);
            else
                ii.pItem->ModifyItemState(0,WndState_Hover);
                
            m_adapter->getView(hItem,ii.pItem,m_xmlTemplate.first_child());
			ii.pItem->DoColorize(GetColorizeColor());

            rcContainer.left = m_tvItemLocator->GetItemIndent(hItem);
            CSize szItem = m_adapter->getViewDesiredSize(hItem,ii.pItem,rcContainer);
            ii.pItem->Move(CRect(0,0,szItem.cx,szItem.cy));
            m_tvItemLocator->SetItemWidth(hItem,szItem.cx);
            m_tvItemLocator->SetItemHeight(hItem,szItem.cy);
                        
            m_visible_items.AddTail(ii);
            nOffset += szItem.cy;
            if(nOffset>=rcClient.Height())
                break;
            hItem = m_adapter->GetNextVisibleItem(hItem);
        }

        SPOSITION pos = pMapOld->GetStartPosition();
        while(pos)
        {
            ItemInfo ii = pMapOld->GetNextValue(pos);
            
            if(ii.pItem == m_pHoverItem)
            {
                m_pHoverItem->DoFrameEvent(WM_MOUSELEAVE, 0, 0);
                m_pHoverItem = NULL;
            }
            
            ii.pItem->GetEventSet()->setMutedState(true);
            if((HTREEITEM)ii.pItem->GetItemIndex() == m_hSelected)
            {
                ii.pItem->ModifyItemState(0, WndState_Check);
                ii.pItem->GetFocusManager()->SetFocusedHwnd(0);
            }
            ii.pItem->SetVisible(FALSE);//防止执行SItemPanel::OnTimeFrame()
            ii.pItem->GetEventSet()->setMutedState(false);

            m_itemRecycle[ii.nType]->AddTail(ii.pItem);    
        }
        delete pMapOld;
        
        CSize szNewView;
        szNewView.cx = m_tvItemLocator->GetTotalWidth();
        szNewView.cy = m_tvItemLocator->GetTotalHeight();
        if(szOldView != szNewView)
        {//update scroll range
            UpdateScrollBar();
            UpdateVisibleItems();//根据新的滚动条状态重新记算显示列表项
        }
	}


	void STreeView::OnItemSetCapture( SItemPanel *pItem,BOOL bCapture )
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

	BOOL STreeView::OnItemGetRect( SItemPanel *pItem,CRect &rcItem )
	{
        HTREEITEM hItem = (HTREEITEM)pItem->GetItemIndex();
		if(m_pVisibleMap->Lookup(hItem) == NULL) return FALSE;

        int nOffset = m_tvItemLocator->Item2Position(hItem) - m_siVer.nPos;
        rcItem = GetClientRect();
        rcItem.top += nOffset;
        rcItem.bottom = rcItem.top + m_tvItemLocator->GetItemHeight(hItem);
        rcItem.left += m_tvItemLocator->GetItemIndent(hItem) - m_siHoz.nPos;
        rcItem.right = rcItem.left + m_tvItemLocator->GetItemWidth(hItem);
        return TRUE;
	}

	BOOL STreeView::IsItemRedrawDelay()
	{
		return TRUE;
	}

	void STreeView::OnItemRequestRelayout( SItemPanel *pItem )
	{
		  //pItem->UpdateChildrenPosition();
	}

	void STreeView::onBranchChanged(HTREEITEM hBranch)
	{
		if (m_adapter == NULL)
		{
			return; 
		}
        if(m_tvItemLocator) m_tvItemLocator->OnBranchChanged(hBranch);
        UpdateScrollBar();
        UpdateVisibleItems();
	}
	
    void STreeView::onBranchInvalidated(HTREEITEM hBranch)
    {
        onBranchChanged(hBranch);
    }

    void STreeView::onBranchExpandedChanged(HTREEITEM hBranch,BOOL bExpandedOld,BOOL bExpandedNew)
    {
        if (m_adapter == NULL)
        {
            return; 
        }
        if(m_tvItemLocator) m_tvItemLocator->OnBranchExpandedChanged(hBranch,bExpandedOld,bExpandedNew);
        UpdateScrollBar();
        UpdateVisibleItems();
    }

	
	LRESULT STreeView::OnMouseEvent( UINT uMsg,WPARAM wParam,LPARAM lParam )
	{
        SetMsgHandled(FALSE);
		if(m_adapter == NULL) {
			return 0;
		}

		LRESULT lRet = 0;
		CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

		if(m_itemCapture) {
			CRect rcItem = m_itemCapture->GetItemRect();
			pt.Offset(-rcItem.TopLeft());
			lRet = m_itemCapture->DoFrameEvent(uMsg, wParam, MAKELPARAM(pt.x, pt.y));
		}
		else {
            if(uMsg==WM_LBUTTONDOWN || uMsg== WM_RBUTTONDOWN || uMsg==WM_MBUTTONDOWN)
			{//交给panel处理
				SItemPanel* pPanel = HitTest(CPoint(pt));
				if (!pPanel && m_hSelected)  //hit in none-item area,so make item to killfocus 
				{
					SItemPanel *pSelItem = GetItemPanel(m_hSelected);
					if (pSelItem) pSelItem->DoFrameEvent(WM_KILLFOCUS, 0, 0);
					//m_hSelected = NULL;
				}

				__super::ProcessSwndMessage(uMsg,wParam,lParam,lRet);
			}

			SItemPanel * pHover = HitTest(pt);
			if (pHover != m_pHoverItem)
			{
				SItemPanel * oldHover = m_pHoverItem;
				m_pHoverItem = pHover;
				if (oldHover)
				{
					oldHover->DoFrameEvent(WM_MOUSELEAVE, 0, 0);
					oldHover->InvalidateRect(NULL);
				}
				if (m_pHoverItem)
				{
					m_pHoverItem->DoFrameEvent(WM_MOUSEHOVER, 0, 0);
					m_pHoverItem->InvalidateRect(NULL);
				}
			}

			if (m_pHoverItem)
			{
				m_pHoverItem->DoFrameEvent(uMsg, wParam, MAKELPARAM(pt.x, pt.y));
			}
		}

        if(uMsg==WM_LBUTTONUP || uMsg== WM_RBUTTONUP || uMsg==WM_MBUTTONUP)
        {//交给panel处理
            __super::ProcessSwndMessage(uMsg,wParam,lParam,lRet);
        }
        SetMsgHandled(TRUE);
		return 0;
	}

	void STreeView::RedrawItem(SItemPanel *pItem)
	{
		pItem->InvalidateRect(NULL);
	}

	void STreeView::OnMouseLeave()
	{
		__super::OnMouseLeave();

		if(m_pHoverItem)
		{
			m_pHoverItem->DoFrameEvent(WM_MOUSELEAVE,0,0);
			m_pHoverItem = NULL;
		}
	}

	BOOL STreeView::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
	{
        SItemPanel *pSelItem = GetItemPanel(m_hSelected);
        if(pSelItem)
        {
            CRect rcItem = pSelItem->GetItemRect();
            CPoint pt2=pt-rcItem.TopLeft();
            if(pSelItem->DoFrameEvent(WM_MOUSEWHEEL,MAKEWPARAM(nFlags,zDelta),MAKELPARAM(pt2.x,pt2.y)))
                return TRUE;
        }
		return __super::OnMouseWheel(nFlags, zDelta, pt);
	}
	
	void STreeView::OnKillFocus(SWND wndFocus)
	{
		__super::OnKillFocus(wndFocus);
		SItemPanel * itemPanel = GetItemPanel(m_hSelected);
		if (itemPanel) itemPanel->GetFocusManager()->StoreFocusedView();
	}

	void STreeView::OnSetFocus(SWND wndOld)
	{
		__super::OnSetFocus(wndOld);
		SItemPanel * itemPanel = GetItemPanel(m_hSelected);
		if (itemPanel)
		{
			itemPanel->GetFocusManager()->RestoreFocusedView();
		}
		
	}
	BOOL STreeView::OnScroll( BOOL bVertical,UINT uCode,int nPos )
	{
		int nOldPos = m_siVer.nPos;
		__super::OnScroll(bVertical, uCode, nPos);                      
		int nNewPos = m_siVer.nPos;
		if(nOldPos != nNewPos)
		{
			UpdateVisibleItems();

			//加速滚动时UI的刷新
			if (uCode==SB_THUMBTRACK)
				ScrollUpdate();

            return TRUE;
        }
        return FALSE;
	}

	int STreeView::GetScrollLineSize( BOOL bVertical )
	{
		return m_tvItemLocator->GetScrollLineSize();
	}

    SItemPanel * STreeView::GetItemPanel(HTREEITEM hItem)
    {
        VISIBLEITEMSMAP::CPair *pNode = m_pVisibleMap->Lookup(hItem);
        if(!pNode) return NULL;
        return pNode->m_value.pItem;
    }

    SItemPanel * STreeView::HitTest(CPoint & pt)
    {
        SPOSITION pos = m_visible_items.GetHeadPosition();
        while(pos)
        {
            ItemInfo ii = m_visible_items.GetNext(pos);
            CRect rcItem = ii.pItem->GetItemRect();
            if(rcItem.PtInRect(pt)) 
            {
                pt-=rcItem.TopLeft();
                return ii.pItem;
            }
        }
        return NULL;
    }
    

    bool STreeView::OnItemClick(EventArgs *pEvt)
    {
        SItemPanel *pItemPanel = sobj_cast<SItemPanel>(pEvt->sender);
        SASSERT(pItemPanel);
        HTREEITEM hItem = (HTREEITEM)pItemPanel->GetItemIndex();
        if(hItem != m_hSelected)
        {
            SetSel(hItem,TRUE);
        }
        return true;
    }

    bool STreeView::OnItemDblClick(EventArgs *pEvt)
    {
        SItemPanel *pItemPanel = sobj_cast<SItemPanel>(pEvt->sender);
        SASSERT(pItemPanel);
        HTREEITEM hItem = (HTREEITEM)pItemPanel->GetItemIndex();
        if(m_adapter->HasChildren(hItem))
        {
            m_adapter->ExpandItem(hItem,ITvAdapter::TVC_TOGGLE);
        }
        return true;
    }

    UINT STreeView::OnGetDlgCode()
    {
        if(m_bWantTab) return SC_WANTALLKEYS;
        else return SC_WANTARROWS|SC_WANTSYSKEY;
    }

    BOOL STreeView::OnSetCursor(const CPoint &pt)
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


    BOOL STreeView::OnUpdateToolTip(CPoint pt, SwndToolTipInfo & tipInfo)
    {
        if(!m_pHoverItem)
            return __super::OnUpdateToolTip(pt,tipInfo);
        return m_pHoverItem->OnUpdateToolTip(pt,tipInfo);
    }

    HRESULT STreeView::OnAttrIndent(const SStringW & strValue,BOOL bLoading)
    {
        if(!bLoading) return E_FAIL;
        m_tvItemLocator.Attach(new STreeViewItemLocator(_wtoi(strValue)));
        return S_OK;
    }

    void STreeView::OnColorize(COLORREF cr)
    {
        __super::OnColorize(cr);
		DispatchMessage2Items(UM_SETCOLORIZE,cr,0);
    }


	void STreeView::OnScaleChanged(int nScale)
	{
		__super::OnScaleChanged(nScale);
		DispatchMessage2Items(UM_SETSCALE,nScale,0);
	}

	HRESULT STreeView::OnLanguageChanged()
	{
		HRESULT hret =__super::OnLanguageChanged();
		DispatchMessage2Items(UM_SETLANGUAGE,0,0);
		return hret;
	}

	void STreeView::DispatchMessage2Items(UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		SPOSITION pos = m_visible_items.GetHeadPosition();
		while (pos)
		{
			ItemInfo ii = m_visible_items.GetNext(pos);
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

}
