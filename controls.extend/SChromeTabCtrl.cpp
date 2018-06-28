#include "stdafx.h"
#include "SChromeTabCtrl.h"
#include <control/SCmnCtrl.h>


namespace SOUI
{
    const wchar_t KXmlTabStyle[] = L"tabStyle";
    const wchar_t KXmlCloseBtnStyle[]=L"closeBtnStyle";
    const wchar_t KXmlNewBtnStyle[]=L"newBtnStyle";

    //////////////////////////////////////////////////////////////////////////
    // SChromeTab
    class SChromeTab : public SWindow , public SAnimator
    {
        SOUI_CLASS_NAME(SChromeTab,L"chromeTab")
        friend class SChromeTabCtrl;
    public:
        SChromeTab(SChromeTabCtrl* pHost);

        void MoveTo(const CRect & rcEnd);
        BOOL IsDragable() { return m_iOrder!=-1 && m_pHost->m_bEnableDrag;}
        
        SOUI_ATTRS_BEGIN()
            ATTR_INT(L"allowClose",m_bAllowClose,FALSE)
        SOUI_ATTRS_END()

		SOUI_MSG_MAP_BEGIN()
		    MSG_WM_MOUSEMOVE(OnMouseMove)
			MSG_WM_LBUTTONDOWN(OnLButtonDown)
            MSG_WM_LBUTTONUP(OnLButtonUp)
            MSG_WM_LBUTTONDBLCLK(OnLButtonDbClick)
		SOUI_MSG_MAP_END()
        
    protected:
        virtual void OnAnimatorState(int percent);
        virtual void OnFinalRelease(){delete this;}
		void OnMouseMove(UINT nFlags,CPoint pt);
		void OnLButtonUp(UINT nFlags,CPoint pt);
        void OnLButtonDown(UINT nFlags,CPoint pt);
        void OnLButtonDbClick(UINT nFlags,CPoint pt);
	
        CRect m_rcBegin, m_rcEnd;
        BOOL    m_bAllowClose;
        CPoint  m_ptDrag;
		int     m_iOrder;
        int     m_iTabIndex;
		bool    m_bDrag;
		SChromeTabCtrl* m_pHost;
    };
    
    SChromeTab::SChromeTab(SChromeTabCtrl* pHost)
        :m_bAllowClose(TRUE)
        ,m_pHost(pHost)
        ,m_iOrder(-1)
        ,m_iTabIndex(-1)
    {
        m_bClipClient = TRUE;
    }

    void SChromeTab::MoveTo( const CRect & rcEnd )
    {
        m_rcBegin = GetWindowRect();
        m_rcEnd = rcEnd;
        Stop();
        Start(200);
    }

    void SChromeTab::OnAnimatorState( int percent )
    {
        CRect rcTemp;
        rcTemp.left = m_rcBegin.left + (m_rcEnd.left-m_rcBegin.left)*percent/100;
        rcTemp.top  = m_rcBegin.top  + (m_rcEnd.top -m_rcBegin.top )*percent/100;
        rcTemp.right= m_rcBegin.right + (m_rcEnd.right-m_rcBegin.right)*percent/100;
        rcTemp.bottom= m_rcBegin.bottom + (m_rcEnd.bottom-m_rcBegin.bottom)*percent/100;
        Move(rcTemp);
    }

	void SChromeTab::OnMouseMove(UINT nFlags,CPoint pt)
	{
        if((nFlags & MK_LBUTTON) && IsDragable())
		{
			CRect rcWnd = GetWindowRect();
			if(m_pHost->m_tabAlign == SChromeTabCtrl::TDIR_HORZ)
			    rcWnd.OffsetRect(pt.x-m_ptDrag.x,0);
			else
			    rcWnd.OffsetRect(0,pt.y-m_ptDrag.y);
            Move(rcWnd);
            m_ptDrag = pt;
			m_pHost->ChangeTabPos(this,pt);
			m_bDrag = true;
		}
	}
	
	void SChromeTab::OnLButtonUp(UINT nFlags,CPoint pt)
	{
        SWindow::OnLButtonUp(nFlags,pt);
        if(m_bDrag)  m_pHost->UpdateChildrenPosition();		    
	}
	
	void SChromeTab::OnLButtonDown(UINT nFlags,CPoint pt)
	{
	    SWindow::OnLButtonDown(nFlags,pt);
        BringWindowToTop();
	    m_ptDrag = pt;
	    m_bDrag  = false;
	}

    void SChromeTab::OnLButtonDbClick(UINT nFlags,CPoint pt)
    {
        EventChromeTabDbClick evt(this);
        evt.iOrder = m_iOrder;
        evt.iTabIndex = m_iTabIndex;

        FireEvent(evt);        
    }

    //////////////////////////////////////////////////////////////////////////
    //  SChromeBtnNew
    class SChromeBtnNew : public SChromeTab
    {
    public:
        virtual void OnFinalRelease()
        {
            delete this;
        }
    };



    //////////////////////////////////////////////////////////////////////////
    // SChromeTabCtrl
    SChromeTabCtrl::SChromeTabCtrl(void)
    :m_pSelTab(NULL)
    ,m_tabAlign(TDIR_HORZ)
    ,m_nDesiredSize(200)
    ,m_bEnableDrag(TRUE)
    ,m_pBtnNew(NULL)
    {
        m_bClipClient=TRUE;
        m_evtSet.addEvent(EVENTID(EventChromeTabNew));
        m_evtSet.addEvent(EVENTID(EventChromeTabClose));
        m_evtSet.addEvent(EVENTID(EventChromeTabSelChanged));
    }

    SChromeTabCtrl::~SChromeTabCtrl(void)
    {
    }
	int SChromeTabCtrl::ChangeTabPos(SChromeTab* pCurMove,CPoint ptCur)
	{
        for(int i =0;i<(int)m_lstTabOrder.GetCount();i++)
        {
            if(m_lstTabOrder[i] == pCurMove)
            {
                continue ;
            }
            CRect rcWnd = m_lstTabOrder[i]->GetWindowRect();
            CPoint ptCenter = rcWnd.CenterPoint();
    
            if((m_tabAlign == TDIR_HORZ && ptCenter.x <= ptCur.x && rcWnd.right >= ptCur.x)
                || (m_tabAlign == TDIR_VERT && ptCenter.y <= ptCur.y && rcWnd.bottom >= ptCur.y))
            {
                if(m_tabAlign == TDIR_HORZ)
                {
                    if(pCurMove->m_iOrder > m_lstTabOrder[i]->m_iOrder)
                    {
                        rcWnd.OffsetRect(rcWnd.Width(),0); 
                    }
                    else
                    {
                        rcWnd.OffsetRect(-rcWnd.Width(),0); 
                    }
                }else
                {
                    if(pCurMove->m_iOrder > m_lstTabOrder[i]->m_iOrder)
                    {
                        rcWnd.OffsetRect(0,rcWnd.Height()); 
                    }
                    else
                    {
                        rcWnd.OffsetRect(0,-rcWnd.Height()); 
                    }
                }
                int order = pCurMove->m_iOrder ;
                pCurMove->m_iOrder = m_lstTabOrder[i]->m_iOrder;
                m_lstTabOrder[i]->m_iOrder = order;
                m_lstTabOrder[i]->Move(rcWnd);
                SChromeTab* pTemp = m_lstTabOrder[i];
                m_lstTabOrder[pCurMove->m_iOrder] = pCurMove;
                m_lstTabOrder[pTemp->m_iOrder] = pTemp;
            }
        }
        return 1;
    }

    BOOL SChromeTabCtrl::CreateChildren( pugi::xml_node xmlNode )
    {
        pugi::xml_node xmlTabStyle = xmlNode.child(KXmlTabStyle);
        if(xmlTabStyle)
        {
            m_xmlStyle.append_copy(xmlTabStyle);
        }

        pugi::xml_node xmlTabs = xmlNode.child(L"tabs");//所有tab都必须在tabs标签内

        for (pugi::xml_node xmlChild=xmlTabs.first_child(); xmlChild; xmlChild=xmlChild.next_sibling())
        {
            if(wcscmp(xmlChild.name() , SChromeTab::GetClassName())!=0) 
                continue;
            SChromeTab * pTab = new SChromeTab(this);
            SASSERT(pTab);
            pTab->m_iOrder = pTab->m_iTabIndex = m_lstTabOrder.GetCount();
            m_lstTabOrder.Add(pTab);
            InsertChild(pTab);
            if(xmlTabStyle)
                pTab->InitFromXml(xmlTabStyle);
            pTab->InitFromXml(xmlChild);
            pTab->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&SChromeTabCtrl::OnTabClick,this));
        }
        
        pugi::xml_node xmlNewBtn = xmlNode.child(KXmlNewBtnStyle);
        if(xmlNewBtn)
        {
            m_pBtnNew = new SChromeTab(this);
            InsertChild(m_pBtnNew);
            m_pBtnNew->InitFromXml(xmlNewBtn);
            m_pBtnNew->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&SChromeTabCtrl::OnBtnNewClick,this));
        }

        pugi::xml_node xmlCloseBtn =xmlNode.child(KXmlCloseBtnStyle);
        if(xmlCloseBtn)
        {
            m_xmlStyle.append_copy(xmlCloseBtn);

            for(UINT i = 0;i<m_lstTabOrder.GetCount();i++)
            {//自动插入一个closeBtn
                if(!m_lstTabOrder[i]->m_bAllowClose) continue;
                
                SWindow *pBtn = SApplication::getSingleton().CreateWindowByName(SImageButton::GetClassName());
                m_lstTabOrder[i]->InsertChild(pBtn);
                pBtn->InitFromXml(xmlCloseBtn);
                pBtn->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&SChromeTabCtrl::OnBtnCloseTabClick,this));
            }
        }
        return TRUE;
    }

    void SChromeTabCtrl::UpdateChildrenPosition()
    {
        CRect rcClient;
        GetClientRect(&rcClient);
        CSize szBtnNew;
        if(m_pBtnNew) szBtnNew=m_pBtnNew->GetDesiredSize(&rcClient);
        CRect rcTab=rcClient;
        if(m_tabAlign == TDIR_HORZ)
        {
            int nTabWid = 0;
            if(rcClient.Width() - szBtnNew.cx > (int)m_lstTabOrder.GetCount()*m_nDesiredSize)
            {
                nTabWid = m_nDesiredSize;
            }else
            {
                nTabWid = (rcClient.Width()-szBtnNew.cx) / m_lstTabOrder.GetCount();
            }
            rcTab.right = rcTab.left + nTabWid;
            for(UINT i=0;i<m_lstTabOrder.GetCount();i++)
            {
                m_lstTabOrder[i]->MoveTo(rcTab);
                rcTab.OffsetRect(nTabWid,0);
            }
            if(m_pBtnNew)
            {
                CRect rcNewBtn = CRect(rcTab.TopLeft(),szBtnNew);
                m_pBtnNew->MoveTo(rcNewBtn);
            }
            
        }else
        {
            int nTabHei = 0;
            if(rcClient.Height() - szBtnNew.cy > (int)m_lstTabOrder.GetCount()*m_nDesiredSize)
            {
                nTabHei = m_nDesiredSize;
            }else
            {
                nTabHei = (rcClient.Height()-szBtnNew.cx) / m_lstTabOrder.GetCount();
            }
            rcTab.bottom = rcTab.top + nTabHei;
            for(UINT i=0;i<m_lstTabOrder.GetCount();i++)
            {
                m_lstTabOrder[i]->MoveTo(rcTab);
                rcTab.OffsetRect(0,nTabHei);
            }
        }
        if(m_pBtnNew)
        {
            CRect rcNewBtn = CRect(rcTab.TopLeft(),szBtnNew);
            m_pBtnNew->MoveTo(rcNewBtn);
        }

    }

    bool SChromeTabCtrl::OnBtnNewClick( EventArgs *pEvt )
    {
        InsertTab(NULL,-1);
        return true;
    }


    BOOL SChromeTabCtrl::RemoveTab(int iTab)
    {
        if(iTab < 0 || iTab >= (int)m_lstTabOrder.GetCount()) return FALSE;
        return RemoveTab(m_lstTabOrder[iTab]);
    }

    BOOL SChromeTabCtrl::RemoveTab(SChromeTab *pTab)
    {
        SASSERT(pTab);
        
        if(pTab == m_pSelTab)
            m_pSelTab = NULL;
        
        int iOrder = pTab->m_iOrder;
        int iTabIndex = pTab->m_iTabIndex;
        SASSERT(iOrder!=-1);
        
        m_lstTabOrder.RemoveAt(iOrder);
        
        DestroyChild(pTab);
        UpdateChildrenPosition();

        //update tab order and index
        for(int i=0 ; i < (int)m_lstTabOrder.GetCount(); i++)
        {
            if(m_lstTabOrder[i]->m_iOrder > iOrder) 
                m_lstTabOrder[i]->m_iOrder--;
            if(m_lstTabOrder[i]->m_iTabIndex > iTabIndex)
                m_lstTabOrder[i]->m_iTabIndex--;
        }
        return TRUE;
    }


    bool SChromeTabCtrl::OnBtnCloseTabClick( EventArgs *pEvt )
    {
        SWindow *pBtn = (SWindow*)pEvt->sender;
        SChromeTab *pTab = (SChromeTab*)pBtn->GetParent();

        EventChromeTabClose evt(this);
        evt.pCloseTab = pTab;
        evt.iCloseTab = pTab->m_iOrder;
        FireEvent(evt);

        RemoveTab(pTab);
        return true;
    }

    bool SChromeTabCtrl::OnTabClick( EventArgs *pEvt )
    {
        SChromeTab *pTab = (SChromeTab*)pEvt->sender;
        
        SetCurSel(pTab->m_iOrder);

        return true;
    }
    
    BOOL SChromeTabCtrl::InsertTab( LPCTSTR pszTitle,int iPos )
    {
        SChromeTab *pNewTab = new SChromeTab(this);
        SASSERT(pNewTab);
        
        InsertChild(pNewTab);
        pugi::xml_node xmlTabStyle = m_xmlStyle.child(KXmlTabStyle);
        if(xmlTabStyle)
            pNewTab->InitFromXml(xmlTabStyle);
        if(pszTitle)
        {
            pNewTab->SetWindowText(pszTitle);
            pNewTab->SetAttribute(L"tip",S_CT2W(pszTitle));
        }

        pNewTab->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&SChromeTabCtrl::OnTabClick,this));
        

        if(iPos<0 || iPos > (int)m_lstTabOrder.GetCount()) 
            iPos = m_lstTabOrder.GetCount();

        pNewTab->m_iOrder = iPos;
        pNewTab->m_iTabIndex = m_lstTabOrder.GetCount();

        for(int i=0;i<(int)m_lstTabOrder.GetCount();i++)
        {
            if(m_lstTabOrder[i]->m_iOrder>=iPos)
                m_lstTabOrder[i]->m_iOrder++;
        }

        m_lstTabOrder.InsertAt(iPos,pNewTab);
        
        CRect rcClient;
        GetClientRect(&rcClient);
        CRect rcLeft;
        if(iPos>0)
        {
            m_lstTabOrder[iPos-1]->GetWindowRect(&rcLeft);
        }else
        {
            rcLeft=rcClient;
            rcLeft.right=rcLeft.left;
        }

        pugi::xml_node xmlCloseBtn = m_xmlStyle.child(KXmlCloseBtnStyle);
        if(xmlCloseBtn && pNewTab->m_bAllowClose)
        {
            SWindow *pBtn = SApplication::getSingleton().CreateWindowByName(SImageButton::GetClassName());
            pNewTab->InsertChild(pBtn);
            pBtn->InitFromXml(xmlCloseBtn);
            pBtn->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&SChromeTabCtrl::OnBtnCloseTabClick,this));
        }
        
        //插入到初始位置
        CRect rcInit=rcLeft;
        rcInit.left=rcInit.right;
        rcInit.right=rcInit.left+m_nDesiredSize;
        pNewTab->Move(rcInit);

        UpdateChildrenPosition();

        EventChromeTabNew evt(this);
        evt.pNewTab = pNewTab;
        evt.iNewTab = iPos;
        FireEvent(evt);

        return TRUE;
    }


    void SChromeTabCtrl::OnNextFrame()
    {
        for(UINT i=0; i< m_lstTabOrder.GetCount();i++)
        {
            m_lstTabOrder[i]->Update();
        }
        if(m_pBtnNew) m_pBtnNew->Update();
    }

    int SChromeTabCtrl::OnCreate( LPVOID )
    {
        int nRet = __super::OnCreate(NULL);
        if(nRet==0) 
            GetContainer()->RegisterTimelineHandler(this);
        return nRet;
    }

    void SChromeTabCtrl::OnDestroy()
    {
        GetContainer()->UnregisterTimelineHandler(this);
        __super::OnDestroy();
    }

    void SChromeTabCtrl::SetCurSel(int iTab,bool bSendNotify)
    {
        if(iTab >= (int)m_lstTabOrder.GetCount()) return;
        int iCurSel = m_pSelTab? m_pSelTab->m_iOrder: -1;
        if(iTab != iCurSel)
        {
            int oldSel = iCurSel;
            if(m_pSelTab)
            {
                m_pSelTab->ModifyState(0,WndState_Check,TRUE);
                m_pSelTab = NULL;
            }
            
            if(iTab != -1)
            {
                m_pSelTab = m_lstTabOrder[iTab];
                m_pSelTab->ModifyState(WndState_Check,0,TRUE);
            }
                        
            if(bSendNotify)
            {
                EventChromeTabSelChanged evt(this);
                evt.iOldSel = oldSel;
                evt.iNewSel = iTab;

                FireEvent(evt);
            }
        }
    }

    int SChromeTabCtrl::GetCurSel() const
    {
        return m_pSelTab?m_pSelTab->m_iOrder:-1;
    }

    int SChromeTabCtrl::GetTabIndex( int iTab ) const
    {
        SASSERT(iTab>=0 && iTab <= (int)m_lstTabOrder.GetCount()-1);
        return m_lstTabOrder[iTab]->m_iTabIndex;
    }

    int SChromeTabCtrl::GetTabOrder( int iTabIndex ) const
    {
        for(int i= 0;i<(int)m_lstTabOrder.GetCount();i++)
        {
            if(m_lstTabOrder[i]->m_iTabIndex == iTabIndex) return i;
        }
        return -1;
    }

    int SChromeTabCtrl::GetTabCount() const
    {
        return m_lstTabOrder.GetCount();
    }
    
    SWindow* SChromeTabCtrl::GetTabWindow(int iPos)
    {
        if(iPos >=0 && iPos < (int)m_lstTabOrder.GetCount())
            return m_lstTabOrder[iPos];
        else
            return NULL;
    }
}

