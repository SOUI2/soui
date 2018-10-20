#include "StdAfx.h"
#include "SPropertyGrid.h"
#include "propitem/SPropertyItem-Text.h"
#include "propitem/SPropertyItem-Option.h"
#include "propitem/SPropertyItem-Color.h"
#include "propitem/SPropertyItem-Size.h"

const int KPropItemIndent   = 10;

const COLORREF KColorHead  = RGBA(128,128,128,255);
const COLORREF KColorGroup = RGBA(128,128,128,255);
const COLORREF KColorItem  = RGBA(255,255,255,255);
const COLORREF KColorItemSel  = RGBA(0,0,128,255);
const COLORREF KColorBorder = RGBA(0,0,0,255);

namespace SOUI
{
    //////////////////////////////////////////////////////////////////////////
    void SPropertyGroup::DrawItem( IRenderTarget *pRT,CRect rc )
    {
        pRT->FillSolidRect(rc,KColorGroup);
    }

    
    //////////////////////////////////////////////////////////////////////////
    SPropItemMap SPropItemMap::s_mapPropItem;
    
    SPropItemMap::SPropItemMap()
    {
        SetAt(SPropertyItemText::GetClassName(),SPropertyItemText::CreatePropItem);
        SetAt(SPropertyItemOption::GetClassName(),SPropertyItemOption::CreatePropItem);
        SetAt(SPropertyItemColor::GetClassName(),SPropertyItemColor::CreatePropItem);
        SetAt(SPropertyItemSize::GetClassName(),SPropertyItemSize::CreatePropItem);
        SetAt(SPropertyGroup::GetClassName(),SPropertyGroup::CreatePropItem);
    }

    void SPropItemMap::RegPropItem( const SStringW & strName, FunCreatePropItem funCreate )
    {
        s_mapPropItem.SetAt(strName,funCreate);
    }

    IPropertyItem * SPropItemMap::CreatePropItem( const SStringW & strName ,SPropertyGrid *pOwner )
    {
        if(!s_mapPropItem.Lookup(strName)) return NULL;
        return s_mapPropItem[strName](pOwner);
    }

    //////////////////////////////////////////////////////////////////////////
    SPropertyGrid::SPropertyGrid(void)
    :m_nIndent(KPropItemIndent)
    ,m_nNameWidth(100)
    ,m_switchSkin(NULL)
    ,m_bDraging(FALSE)
    ,m_pInplaceActiveWnd(NULL)
    {
        GetEventSet()->addEvent(EVENTID(EventPropGridValueChanged));
        GetEventSet()->subscribeEvent(EventLBSelChanged::EventID,Subscriber(&SPropertyGrid::OnSelChanged,this));
    }

    SPropertyGrid::~SPropertyGrid(void)
    {
        SPOSITION pos = m_lstGroup.GetHeadPosition();
        while(pos)
        {
            SPropertyGroup * pGroup = m_lstGroup.GetNext(pos);
            pGroup->Release();        
        }
        m_lstGroup.RemoveAll();
    }

    int SPropertyGrid::GetIndent()
    {
        return m_nIndent;
    }

    void SPropertyGrid::OnItemExpanded( IPropertyItem *pItem)
    {
        int iInsert = IndexOfPropertyItem(pItem);
        if(pItem->IsExpand())
        {
            ExpandChildren(pItem,iInsert);
        }else
        {
            CollapseChildren(pItem,iInsert);
        }
    }

    int SPropertyGrid::IndexOfPropertyItem( const IPropertyItem *pItem ) const
    {
        for(int i=0;i<GetCount();i++)
        {
            if(pItem == (IPropertyItem *)GetItemData(i)) return i;
        }
        return -1;
    }
    
    CRect SPropertyGrid::GetItemRect( IPropertyItem *pItem ) const
    {
        int idx = IndexOfPropertyItem(pItem);
        SASSERT(idx != -1);
        int iTopIdx = GetTopIndex();
        
		int nItemHei = m_itemHeight.toPixelSize(GetScale());
        int nPageItems = (m_rcClient.Height()+nItemHei-1)/nItemHei+1;
        if(iTopIdx + nPageItems > GetCount()) nPageItems = GetCount() - iTopIdx;
        CRect rcItem;
        if(idx >= iTopIdx && idx <= iTopIdx+nPageItems)
        {
            rcItem = CRect(0,0,m_rcClient.Width(),nItemHei);
            rcItem.OffsetRect(0,nItemHei*idx-m_ptOrigin.y);
            rcItem.OffsetRect(m_rcClient.TopLeft());
        }
        return rcItem;
    }

    void SPropertyGrid::SortInsert( IPropertyItem *pItem )
    {
        int iInsert = -1;
        for(int i=0;i<GetCount();i++)
        {
            IPropertyItem *p = (IPropertyItem *)GetItemData(i);
            if(pItem->GetName()<p->GetName()) 
            {
                iInsert = i;
                break;
            }
        }
        InsertString(iInsert,NULL,-1,(LPARAM)pItem);
    }

    BOOL SPropertyGrid::InsertGroup( SPropertyGroup * pGroup,SPropertyGroup* pInertAfter/*=IG_LAST*/ )
    {
        SPOSITION pos = m_lstGroup.Find(pGroup);
        if(pos) return FALSE;
        if(pInertAfter == IG_FIRST)
        {
            m_lstGroup.InsertBefore(0,pGroup);
        }else if(pInertAfter == IG_LAST)
        {
            m_lstGroup.InsertAfter(0,pGroup);
        }else
        {
            pos = m_lstGroup.Find(pInertAfter);
            if(!pos) return FALSE;
            m_lstGroup.InsertAfter(pos,pGroup);
        }
        pGroup->AddRef();

        switch(m_orderType)
        {
        case OT_NULL:
            {
                IPropertyItem *pChild=pGroup->GetItem(IPropertyItem::GPI_FIRSTCHILD);
                while(pChild)
                {
                    InsertString(-1,NULL,-1,(LPARAM)pChild);
                    pChild = pChild->GetItem(IPropertyItem::GPI_NEXTSIBLING);
                }
                //展开子项s
                pChild=pGroup->GetItem(IPropertyItem::GPI_FIRSTCHILD);
                while(pChild)
                {
                    if(pChild->ChildrenCount() && pChild->IsExpand())
                    {
                        int iInsert = IndexOfPropertyItem(pChild);
                        ExpandChildren(pChild,iInsert);
                    }
                    pChild = pChild->GetItem(IPropertyItem::GPI_NEXTSIBLING);
                }

            }
            break;
        case OT_GROUP:
            {
                int iInserted = InsertString(-1,NULL,-1,(LPARAM)pGroup);
                if(pGroup->IsExpand())
                {
                    ExpandChildren(pGroup,iInserted);
                }
            }
            break;
        case OT_NAME:
            {
                IPropertyItem *pChild=pGroup->GetItem(IPropertyItem::GPI_FIRSTCHILD);
                while(pChild)
                {
                    SortInsert(pChild);
                    pChild = pChild->GetItem(IPropertyItem::GPI_NEXTSIBLING);
                }
                //展开子项s
                pChild=pGroup->GetItem(IPropertyItem::GPI_FIRSTCHILD);
                while(pChild)
                {
                    if(pChild->ChildrenCount() && pChild->IsExpand())
                    {
                        int iInsert = IndexOfPropertyItem(pChild);
                        ExpandChildren(pChild,iInsert);
                    }
                    pChild = pChild->GetItem(IPropertyItem::GPI_NEXTSIBLING);
                }

            }
            break;
        }
        return TRUE;
    }

    int SPropertyGrid::ExpandChildren( const IPropertyItem *pItem,int iInsert )
    {
        SASSERT(pItem->IsExpand());
        SASSERT(iInsert != -1);
        int nRet =0;
        SList<IPropertyItem*> lstChilds;
        IPropertyItem *pChild = pItem->GetItem(IPropertyItem::GPI_FIRSTCHILD);
        while(pChild)
        {
            lstChilds.AddTail(pChild);
            pChild = pChild->GetItem(IPropertyItem::GPI_NEXTSIBLING);
        }

        if(m_orderType == OT_NAME)
        {
            SortItems(lstChilds);
        }

        SPOSITION pos = lstChilds.GetHeadPosition();
        while(pos)
        {
            IPropertyItem *pChild = lstChilds.GetNext(pos);
            InsertString(++iInsert,NULL,-1,(LPARAM)pChild);
            nRet ++;
            if(pChild->ChildrenCount() && pChild->IsExpand())
            {
                int nAdded = ExpandChildren(pChild,iInsert);
                nRet += nAdded;
                iInsert += nAdded;
            }
        }

        return nRet;
    }

    void SPropertyGrid::CollapseChildren( const IPropertyItem *pItem,int idx)
    {
        int nChilds = pItem->ChildrenCount();
        for(int i=0;i<nChilds;i++)
        {
            IPropertyItem *pChild = (IPropertyItem *)GetItemData(idx+1);
            if(pChild->ChildrenCount() && pChild->IsExpand())
            {
                CollapseChildren(pChild,idx+1);
            }
            DeleteString(idx+1);
        }
    }

    void SPropertyGrid::DrawItem( IRenderTarget *pRT, CRect &rc, int iItem )
    {
        IPropertyItem *pItem = (IPropertyItem*)GetItemData(iItem);
        
        CRect rcSwitch = rc;
        CRect rcNameBack = rc;
        rcSwitch.right = rcSwitch.left +rcSwitch.Height();
        rcNameBack.left = rcSwitch.right;
        rcNameBack.right = rcNameBack.left + m_nNameWidth;
        pRT->FillSolidRect(rcSwitch,KColorGroup);
        pRT->FillSolidRect(rcNameBack,iItem == SListBox::GetCurSel()? KColorItemSel:(pItem->IsGroup()?KColorGroup:KColorItem));
        int iLevel = pItem->GetLevel();
        if(iLevel>1) rcSwitch.OffsetRect(rcSwitch.Width()*(iLevel-1),0);
        if(pItem->ChildrenCount() && m_switchSkin)
        {
            int iState = pItem->IsExpand()?GROUP_EXPANDED:GROUP_COLLAPSED;
            if(!pItem->IsGroup()) iState += 2;
            CRect rcDraw = rcSwitch;
            rcDraw.DeflateRect((rcSwitch.Size()-m_switchSkin->GetSkinSize())/2);
            m_switchSkin->Draw(pRT,rcDraw,iState);
        }
        
        CRect rcName = rcNameBack;
        rcName.left = rcSwitch.right;
        
        SStringT strName = S_CW2T(pItem->GetName2());
        pRT->DrawText(strName,strName.GetLength(),rcName,DT_SINGLELINE|DT_VCENTER);
        CRect rcItem = rc;
        rcItem.left= rcNameBack.right;
        if(pItem->HasButton()) rcItem.right -= rcItem.Height();
        
        pItem->DrawItem(pRT,rcItem);
        
        CAutoRefPtr<IPen> pen,oldPen;
        pRT->CreatePen(PS_SOLID,KColorBorder,1,&pen);
        pRT->SelectObject(pen,(IRenderObj**)&oldPen);
        CPoint pts[2]={CPoint(rc.left+rc.Height(),rc.bottom-1),CPoint(rc.right,rc.bottom-1)};
        pRT->DrawLines(pts,2);
        CPoint pts2[2]={CPoint(rcNameBack.right,rcNameBack.top),rcNameBack.BottomRight()};
        pRT->DrawLines(pts2,2);
        pRT->SelectObject(oldPen);

    }

    void SPropertyGrid::OnLButtonDbClick( UINT nFlags, CPoint point )
    {
        SListBox::OnLButtonDbClick(nFlags,point);
        int iItem = SListBox::HitTest(point);
        if(iItem != -1)
        {
            IPropertyItem *pItem = (IPropertyItem*)GetItemData(iItem);
            if(pItem->ChildrenCount())
            {
                pItem->Expand(!pItem->IsExpand());
            }else if(!pItem->IsGroup() && !pItem->IsInplaceActive()) 
            {
                pItem->OnInplaceActive(true);
            }
        }
    }

    BOOL SPropertyGrid::CreateChildren( pugi::xml_node xmlNode )
    {
        pugi::xml_node xmlCmdBtn = xmlNode.child(L"cmdbtnstyle");
        SASSERT(xmlCmdBtn);
        m_pCmdBtn = SApplication::getSingleton().CreateWindowByName(SWindow::GetClassName());
        InsertChild(m_pCmdBtn);
        m_pCmdBtn->InitFromXml(xmlCmdBtn);
        m_pCmdBtn->SetVisible(FALSE);
        m_pCmdBtn->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&SPropertyGrid::OnCmdBtnClicked,this));
        
        pugi::xml_node xmlGroups = xmlNode.child(L"groups");
        if(xmlGroups)
        {
            pugi::xml_node xmlChild = xmlGroups.child(SPropertyGroup::GetClassName());
            while(xmlChild)
            {
                SPropertyGroup *pGroup = (SPropertyGroup *)SPropertyGroup::CreatePropItem(this);
                pGroup->InitFromXml(xmlChild);
                InsertGroup(pGroup);
                pGroup->Release();
                xmlChild = xmlChild.next_sibling(SPropertyGroup::GetClassName());
            }
        }
        return TRUE;
    }

    void SPropertyGrid::OnSize( UINT nType, CSize size )
    {
        __super::OnSize(nType,size);
        UpdateChildrenPos();
    }

    SPropertyGrid::ITEMPART SPropertyGrid::HitTest(int iItem,const CPoint &pt )
    {
        if(iItem==-1) return IP_NULL;
        IPropertyItem *pItem = (IPropertyItem*)GetItemData(iItem);
        CRect rcItem=GetItemRect(pItem);
        int iLevel = pItem->GetLevel();
        CRect rcSwitch = rcItem;
        rcSwitch.right = rcSwitch.left+ rcSwitch.Height();
        if(iLevel>1) rcSwitch.OffsetRect(rcSwitch.Width()*(iLevel-1),0);
        if(pt.x<rcSwitch.left) return IP_NULL;
        if(pt.x<rcSwitch.right && pItem->ChildrenCount()) return IP_SWITCH;
        CRect rcName = rcItem;
        rcName.right = rcItem.left + m_nNameWidth + rcItem.Height();
        if(pt.x<rcName.right) return IP_NAME;
        return IP_VALUE;
    }

    void SPropertyGrid::OnLButtonDown( UINT nFlags,CPoint pt )
    {
        CRect rcClient;
        GetClientRect(&rcClient);
		int nItemHei = m_itemHeight.toPixelSize(GetScale());
        if(pt.x-rcClient.left>=nItemHei+m_nNameWidth-1
        && pt.x-rcClient.left<=nItemHei+m_nNameWidth+1)
        {
            SWindow::OnLButtonDown(nFlags,pt);
            m_ptDrag = pt;
            m_bDraging = TRUE;
        }else
        {
            SListBox::OnLButtonDown(nFlags,pt);

            int iItem = SListBox::HitTest(CPoint(pt));
            if(iItem!=-1)
            {
                ITEMPART ip = HitTest(iItem,pt);
                if(ip == IP_SWITCH)
                {
                    IPropertyItem *pItem = (IPropertyItem*)GetItemData(iItem);
                    if(pItem->ChildrenCount())
                    {
                        pItem->Expand(!pItem->IsExpand());
                    }
                }else if(ip==IP_VALUE)
                {
                    IPropertyItem *pItem = (IPropertyItem*)GetItemData(iItem);
                    pItem->OnInplaceActive(true);
                }
            }
        }

    }

    void SPropertyGrid::SortItems( SList<IPropertyItem*> & lstItems )
    {
        SList<IPropertyItem*> lstCpy;
        SPOSITION pos = lstItems.GetHeadPosition();
        while(pos)
        {
            IPropertyItem *pItem = lstItems.GetNext(pos);
            SPOSITION pos2 = lstCpy.GetHeadPosition();
            while(pos2)
            {
                IPropertyItem* pItem2=lstCpy.GetAt(pos2);
                if(pItem->GetName()<pItem2->GetName())
                {
                    lstCpy.InsertBefore(pos2,pItem);
                    break;
                }
                lstCpy.GetNext(pos2);
            }
            if(!pos2)
            {
                lstCpy.InsertAfter(NULL,pItem);
            }
        }
        lstItems.RemoveAll();
        pos = lstCpy.GetHeadPosition();
        while(pos)
        {
            IPropertyItem *pItem = lstItems.GetNext(pos);
            lstItems.InsertAfter(NULL,pItem);
        }
    }

    BOOL SPropertyGrid::OnSetCursor( const CPoint &pt )
    {
        CRect rcClient;
        GetClientRect(&rcClient);
		int nItemHei = m_itemHeight.toPixelSize(GetScale());
        if(m_bDraging 
        ||  (pt.x-rcClient.left>=nItemHei+m_nNameWidth-1
            && pt.x-rcClient.left<=nItemHei+m_nNameWidth+1))
        {
            SetCursor(SApplication::getSingleton().LoadCursor(MAKEINTRESOURCE(IDC_SIZEWE)));
        }else
        {
            int iItem = SListBox::HitTest(CPoint(pt));
            if(iItem<0) return FALSE;
            ITEMPART ip = HitTest(iItem,pt);
            if(ip==IP_SWITCH)
                SetCursor(SApplication::getSingleton().LoadCursor(MAKEINTRESOURCE(IDC_HAND)));
            else
                return FALSE;            
        }
        return TRUE;
    }

    void SPropertyGrid::OnLButtonUp( UINT nFlags,CPoint pt )
    {
        if(m_bDraging)
        {
            m_bDraging = FALSE;
            SWindow::OnLButtonUp(nFlags,pt);
        }else
        {
            SListBox::OnLButtonUp(nFlags,pt);
        }
    }

    void SPropertyGrid::OnMouseMove( UINT nFlags,CPoint pt )
    {
        SListBox::OnMouseMove(nFlags,pt);
        if(m_bDraging)
        {
            m_nNameWidth += pt.x-m_ptDrag.x;
            m_ptDrag = pt;
            Invalidate();
        }
    }

    bool SPropertyGrid::OnSelChanged( EventArgs *pEvt )
    {
        UpdateChildrenPos(CHILD_CMDBTN);
        return true;
    }

    bool SPropertyGrid::OnCmdBtnClicked( EventArgs *pEvt )
    {
        int nCurSel = GetCurSel();
        SASSERT(nCurSel!=-1);
        IPropertyItem *pItem =(IPropertyItem*)GetItemData(nCurSel);
        SASSERT(pItem->HasButton());
        pItem->OnButtonClick();
        return true;
    }
    
    BOOL SPropertyGrid::OnScroll( BOOL bVertical,UINT uCode,int nPos )
    {
        BOOL bRet = SListBox::OnScroll(bVertical,uCode,nPos);
        UpdateChildrenPos();
        return bRet;
    }

    void SPropertyGrid::OnInplaceActiveWndCreate( IPropertyItem *pItem,SWindow *pWnd ,pugi::xml_node xmlInit)
    {
        SASSERT(m_pInplaceActiveWnd == NULL);
        InsertChild(pWnd);
        pWnd->InitFromXml(xmlInit);//this line will trigger WM_CREATE proc. 

        CRect rcItem = GetItemRect(pItem);
        CRect rcValue= rcItem;
        rcValue.left += rcItem.Height()+m_nNameWidth;
        if(pItem->HasButton()) rcValue.right -= rcValue.Height();
        pItem->AdjustInplaceActiveWndRect(rcValue);
        pWnd->Move(rcValue);
        pWnd->SetFocus();
        m_pInplaceActiveWnd = pWnd;
    }

    void SPropertyGrid::OnInplaceActiveWndDestroy( IPropertyItem *pItem,SWindow *pWnd )
    {
        SASSERT(m_pInplaceActiveWnd == pWnd);
		pWnd->SSendMessage(WM_DESTROY);
        RemoveChild(pWnd);
        m_pInplaceActiveWnd = NULL;
    }

    void SPropertyGrid::UpdateChildrenPos(UINT childs)
    {
        int nCurSel = GetCurSel();
        if(nCurSel==-1) return;
        
        IPropertyItem * pItem = (IPropertyItem*)GetItemData(nCurSel);
        CRect rcItem = GetItemRect(pItem);
        rcItem.left += rcItem.Height()+m_nNameWidth;
        
        CRect rcValue = rcItem;
        if(pItem->HasButton())
        {
            rcValue.right -= rcValue.Height();
        }
        
        if(m_pInplaceActiveWnd && childs&CHILD_INPLACEWND)
        {
            pItem->AdjustInplaceActiveWndRect(rcValue);
            m_pInplaceActiveWnd->Move(rcValue);
        }
        
        if(childs & CHILD_CMDBTN)
        {
            m_pCmdBtn->SetVisible(FALSE);
            if(pItem->HasButton())
            {
                CRect rcBtn = rcItem;
                rcBtn.left = rcBtn.right-rcBtn.Height();
                m_pCmdBtn->Move(&rcBtn);
                m_pCmdBtn->SetVisible(TRUE,TRUE);
            }
        }
    }

    void SPropertyGrid::OnItemValueChanged( IPropertyItem *pItem )
    {
        EventPropGridValueChanged evt(this);
        evt.pItem = pItem;
        FireEvent(evt);
    }


}
