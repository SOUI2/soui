#include "souistd.h"
#include "control/SSplitWnd.h"
#include <algorithm>

namespace SOUI
{

#define DEF_SEPSIZE    5


    SSplitPane::SSplitPane():m_nPriority(0),m_nSizeIdeal(20),m_nSizeMin(0),m_nSizeMax(10000)
    {
        GetEventSet()->addEvent(EVENTID(EventSplitPaneMoved));
    }

    void SSplitPane::Move(CRect rc)
    {
        SWindow::Move(rc);
        EventSplitPaneMoved evt(this);
        evt.rcPane = rc;
        FireEvent(evt);
    }


    //////////////////////////////////////////////////////////////////////////
    SSplitWnd::SSplitWnd(void)
        :m_orintation(Vertical)
        ,m_bAdjustable(TRUE)
        ,m_spliterSize(DEF_SEPSIZE)
        ,m_pSkinSep(NULL)
        ,m_iDragSep(-1)
    {
    }

    SSplitWnd::~SSplitWnd(void)
    {
    }

    BOOL SSplitWnd::ShowPane(UINT iPane)
    {
        if (iPane < 0 || iPane >= m_lstPane.GetCount()) return FALSE;

        m_lstPane[iPane]->SetVisible(TRUE);
        Relayout(GetClientRect());
        return TRUE;
    }

    BOOL SSplitWnd::HidePane(UINT iPane)
    {
        if (iPane < 0 || iPane >= m_lstPane.GetCount()) return FALSE;

        m_lstPane[iPane]->SetVisible(FALSE);
        Relayout(GetClientRect());
        return TRUE;
    }

    BOOL SSplitWnd::CreateChildren( pugi::xml_node xmlNode )
    {
        if(!xmlNode) return FALSE;
        pugi::xml_node xmlPane=xmlNode.child(SSplitPane::GetClassName());
        while(xmlPane)
        {
            SSplitPane *pPane = new SSplitPane();
            InsertChild(pPane);
            if(pPane->InitFromXml(xmlPane))
            {
                InsertItem(pPane);
            }
            xmlPane=xmlPane.next_sibling(SSplitPane::GetClassName());
        }
        return TRUE;
    }


    BOOL SSplitWnd::OnSetCursor(const CPoint &pt)
    {
        if (!m_bAdjustable) return FALSE;
        HCURSOR hCursor=GETRESPROVIDER->LoadCursor((m_orintation == Vertical)?IDC_SIZEWE:IDC_SIZENS);
        SetCursor(hCursor);
        return TRUE;
    }

    void SSplitWnd::OnPaint( IRenderTarget * pRT )
    {
        if(!m_pSkinSep) 
            return;

        CRect rcClient;
        GetClientRect(&rcClient);

        CRect rcSep=rcClient;
        if(m_orintation == Vertical)
            rcSep.right=rcClient.left;
        else
            rcSep.bottom=rcSep.top;

        long &RB= (m_orintation == Vertical)?rcSep.right:rcSep.bottom;
        long &LT= (m_orintation == Vertical)?rcSep.left:rcSep.top;

        for(int i=0; i< (int)m_lstPane.GetCount()-1; i++)
        {
            if (!m_lstPane[i]->IsVisible()) continue;
            CRect rcPane = m_lstPane[i]->GetWindowRect();
            RB+= (m_orintation == Vertical)?rcPane.Width():rcPane.Height();
            LT=RB;
            RB+=m_spliterSize;
            m_pSkinSep->Draw(pRT,rcSep,0);
        }
    }

    void SSplitWnd::OnLButtonDown( UINT nFlags,CPoint pt )
    {
        __super::OnLButtonDown(nFlags,pt);

        if (!m_bAdjustable) return;

        m_ptDragPrev = pt;
        if(m_orintation == Vertical)
        {
            for(int i=0;i<(int)m_lstPane.GetCount();i++)
            {
                if(m_ptDragPrev.x < m_lstPane[i]->GetWindowRect().left)
                {
                    m_iDragSep = i-1;
                    break;
                }
            }
        }else
        {
            for(int i=0;i<(int)m_lstPane.GetCount();i++)
            {
                if(m_ptDragPrev.y < m_lstPane[i]->GetWindowRect().top)
                {
                    m_iDragSep = i-1;
                    break;
                }
            }
        }
    }

    void SSplitWnd::OnLButtonUp( UINT nFlags,CPoint pt )
    {
        __super::OnLButtonUp(nFlags,pt);
        m_iDragSep = -1;
    }

    void SSplitWnd::OnMouseMove( UINT nFlags,CPoint pt )
    {
        if(-1==m_iDragSep) return;


        //将列表分裂成两组。
        SplitPaneList lstPane1,lstPane2;          
        SplitPaneList lstPriority1,lstPriority2;

        for(int i=0;i<(int)m_lstPane.GetCount();i++)
        {
            if(i <= m_iDragSep)
                lstPane1.Add(m_lstPane[i]);
            else
                lstPane2.Add(m_lstPane[i]);
        }

        for(int i=0;i<(int)m_lstPriority.GetCount();i++)
        {
            SSplitPane *pane = m_lstPriority[i];
            int idx = ArrayFind(m_lstPane,pane);
            SASSERT(idx!=-1);
            if(idx<=m_iDragSep)
                lstPriority1.Add(pane);
            else
                lstPriority2.Add(pane);
        }

        CPoint diffPoint = m_ptDragPrev;
        m_ptDragPrev = pt;
        diffPoint = m_ptDragPrev - diffPoint;
        int diff = m_orintation == Vertical ? diffPoint.x : diffPoint.y;

        PANESIZELIST lstPaneSize1,lstPaneSize2;
        FatchPaneSizeInfo(lstPriority1,lstPaneSize1);
        FatchPaneSizeInfo(lstPriority2,lstPaneSize2);

        if(diff > 0)
        {
            int maxShrink = 0, maxExtent =0;
            for(int i=0; i< (int)lstPaneSize1.GetCount();i++)
            {
                maxExtent += lstPaneSize1[i].maximum - lstPaneSize1[i].actural;
            }
            for(int i=0;i< (int)lstPaneSize2.GetCount();i++)
            {
                maxShrink += lstPaneSize2[i].actural - lstPaneSize2[i].minimum;
            }
            diff = (std::min)(diff, (std::min)(maxShrink,maxExtent));
            if(diff==0) return;

            //伸长part1
            int idxPrev = ArrayFind(lstPriority1,lstPane1.GetAt(lstPane1.GetCount()-1));
            SASSERT(idxPrev != -1);
            PANESIZE & paneSize1 = lstPaneSize1[idxPrev];

            int max_digest =paneSize1.maximum - paneSize1.actural;
            if(max_digest>diff)
            {
                paneSize1.actural += diff;
            }else
            {
                paneSize1.actural += max_digest;
                PANESIZE szBackup = paneSize1;
                lstPaneSize1.RemoveAt(idxPrev);
                int remain = diff - max_digest;
                AdjustPanesSize(lstPaneSize1,remain);
                lstPaneSize1.InsertAt(idxPrev,szBackup);
            }

            //压缩part2
            int idxNext = ArrayFind(lstPriority2,lstPane2[0]);
            SASSERT(idxNext != -1);
            PANESIZE & paneSize2 = lstPaneSize2[idxNext];

            max_digest =paneSize2.actural - paneSize2.minimum;
            if(max_digest > diff)
            {
                paneSize2.actural -= diff;
            }else
            {
                paneSize2.actural -= max_digest;
                PANESIZE szBackup=paneSize2;
                lstPaneSize2.RemoveAt(idxNext);
                int remain = max_digest - diff;//remain < 0
                AdjustPanesSize(lstPaneSize2,remain);
                lstPaneSize2.InsertAt(idxNext,szBackup);
            }

        }else
        {
            int maxShrink = 0, maxExtent =0;
            for(int i=0; i< (int)lstPaneSize2.GetCount();i++)
            {
                maxExtent += lstPaneSize2[i].maximum - lstPaneSize2[i].actural;
            }
            for(int i=0;i< (int)lstPaneSize1.GetCount();i++)
            {
                maxShrink += lstPaneSize1[i].actural - lstPaneSize1[i].minimum;
            }

            diff *= -1;
            diff = (std::min)(diff, (std::min)(maxShrink,maxExtent));
            if(diff == 0) return;

            //压缩part1
            int idxPrev = ArrayFind(lstPriority1,lstPane1.GetAt(lstPane1.GetCount()-1));
            SASSERT(idxPrev != -1);
            PANESIZE & paneSize1 = lstPaneSize1[idxPrev];

            int max_digest =paneSize1.actural - paneSize1.minimum;
            if(max_digest>diff)
            {
                paneSize1.actural -= diff;
            }else
            {
                paneSize1.actural -= max_digest;
                PANESIZE szBackup = paneSize1;
                lstPaneSize1.RemoveAt(idxPrev);
                int remain = max_digest - diff; //remain < 0
                AdjustPanesSize(lstPaneSize1,remain);
                lstPaneSize1.InsertAt(idxPrev,szBackup);
            }

            //伸长part2
            int idxNext = ArrayFind(lstPriority2,lstPane2[0]);
            SASSERT(idxNext != -1);
            PANESIZE & paneSize2 = lstPaneSize2[idxNext];

            max_digest = paneSize2.maximum - paneSize2.actural;
            if(max_digest > diff)
            {
                paneSize2.actural += diff;
            }else
            {
                paneSize2.actural += max_digest;
                PANESIZE szBackup=paneSize2;
                lstPaneSize2.RemoveAt(idxNext);
                int remain = diff- max_digest;
                AdjustPanesSize(lstPaneSize2,remain);
                lstPaneSize2.InsertAt(idxNext,szBackup);
            }
            diff *= -1;
        }
        //根据新分配的窗口大小重置窗口位置
        CRect rcClient = GetClientRect();
        int nOffset = m_orintation == Vertical? rcClient.left: rcClient.top;
        nOffset = ResetPanesPostion(lstPane1,lstPriority1,lstPaneSize1,nOffset);
        ResetPanesPostion(lstPane2,lstPriority2,lstPaneSize2,nOffset);
        Invalidate();
		UpdateWindow();
    }

    int SSplitWnd::PaneIndex( const SStringW & strName ) const
    {
        for(UINT i = 0;i<m_lstPane.GetCount();i++)
        {
            if(m_lstPane[i]->GetName() == strName) return i;
        }
        return -1;
    }

    SSplitPane * SSplitWnd::GetPane( UINT iPane )
    {
        if(iPane>=m_lstPane.GetCount()) return FALSE;
        return m_lstPane[iPane];
    }

    void SSplitWnd::UpdateChildrenPosition()
    {
        Relayout(GetClientRect());
    }

    void SSplitWnd::RemoveItem(SSplitPane * pane)
    {
        int index = ArrayFind(m_lstPane,pane);
        if(index == -1) return;

        m_lstPane.RemoveAt(index);

        index = ArrayFind(m_lstPriority,pane);
        m_lstPriority.RemoveAt(index);

        Relayout(GetClientRect());
    }

    int SSplitWnd::InsertItem(SSplitPane* pane, int index /*= -1 */)
    {
        //禁止重复插入
        if (ArrayFind(m_lstPane,pane)!=-1)
        {
            SASSERT(FALSE);
            return -1;
        }

        if(index <0 )
        {
            index = (int)m_lstPane.GetCount();
        }

        m_lstPane.InsertAt(index, pane);
        m_lstPriority.Add(pane);
        SortPriorityList(m_lstPriority);

        CRect rcContainer;
        GetClientRect(&rcContainer);        

        if (!rcContainer.IsRectEmpty())
        {
            PANESIZELIST lstPaneSize;
            FatchPaneSizeInfo(m_lstPriority, lstPaneSize);

            const int index = ArrayFind(m_lstPriority,pane);
            lstPaneSize[index].actural = lstPaneSize[index].preferred;

            Relayout(rcContainer, lstPaneSize);
        }
        return index;
    }

    void SSplitWnd::Relayout(const CRect &rc,PANESIZELIST lstPaneSize/*=PANESIZELIST()*/)
    {
        if (lstPaneSize.IsEmpty())
        {
            FatchPaneSizeInfo(m_lstPriority, lstPaneSize);
        }
        int actural=0;
        for(int i=0; i< (int)lstPaneSize.GetCount();i++)
            actural += lstPaneSize[i].actural;

        int nVisiblePanelCount = 0;
        for(int i=0;i< (int)m_lstPriority.GetCount();i++)
        {
            if(m_lstPriority[i]->IsVisible())
                ++nVisiblePanelCount;
        }

        int remain = ((Vertical == m_orintation)? rc.Width():rc.Height()) - actural - (nVisiblePanelCount-1)*m_spliterSize;
        AdjustPanesSize(lstPaneSize,remain);
        int nOffset = (Vertical == m_orintation)? rc.left:rc.top;
        ResetPanesPostion(m_lstPane,m_lstPriority,lstPaneSize,nOffset);
    }

    int SSplitWnd::AdjustPanesSize(PANESIZELIST & lstPriority, int remain)
    {
        //step 1: 将残余量按优先级从高到低顺序根据期望size在窗口间分配
        for(int i=0;i< (int)lstPriority.GetCount();i++)
        {
            PANESIZE & pane = lstPriority[i];
            if(remain > 0)
            {//扩大窗口
                if(pane.preferred < pane.actural) continue;

                int max_digest = pane.preferred - pane.actural;
                if(max_digest >= remain)
                {
                    pane.actural+=remain;
                    remain = 0;
                    break;
                }else
                {
                    pane.actural += max_digest;
                    remain -= max_digest;
                }
            }else
            {//缩小窗口
                if(pane.preferred > pane.actural) continue;

                int max_digest = pane.actural - pane.preferred;
                if(max_digest > -remain)
                {
                    pane.actural += remain;
                    remain = 0;
                    break;
                }else
                {
                    pane.actural -= max_digest;
                    remain += max_digest;
                }
            }
        }
        //step 2: 将残余量按优先级从低到高顺序在窗口的极值间分配
        if(remain)
        {
            for(int i= (int)lstPriority.GetCount()-1;i>=0;i--)
            {
                PANESIZE & pane = lstPriority[i];
                if(remain > 0)
                {//扩大窗口
                    int max_digest = pane.maximum - pane.actural;
                    if(max_digest >= remain)
                    {
                        pane.actural += remain;
                        remain = 0;
                        break;
                    }else
                    {
                        pane.actural += max_digest;
                        remain -= max_digest;
                    }
                }else
                {//缩小窗口
                    int max_digest = pane.actural - pane.minimum;
                    if(max_digest > -remain)
                    {
                        pane.actural += remain;
                        remain = 0;
                        break;
                    }else
                    {
                        pane.actural -= max_digest;
                        remain += max_digest;
                    }
                }
            }
        }
        return remain;
    }

    int SSplitWnd::ResetPanesPostion(SplitPaneList & lstPane, SplitPaneList & lstPanePriority, PANESIZELIST & lstPaneSize, int offset)
    {
        CRect rc = GetClientRect();
            
        for(int i=0;i< (int)lstPane.GetCount();i++)
        {
            int idx = ArrayFind(lstPanePriority,lstPane[i]);
            SASSERT(idx!=-1);
            if(m_orintation == Vertical)
            {
                rc.left = (offset);
                rc.right =(offset +lstPaneSize[idx].actural);
            }
            else
            {
                rc.top = (offset);
                rc.bottom = (offset + lstPaneSize[idx].actural);
            }
            if(lstPane[i]->IsVisible()) //窗口不可见时不修改窗口size
            {
                lstPane[i]->Move(rc);
                offset += lstPaneSize[idx].actural + m_spliterSize;
            }
            else
            {
                offset += lstPaneSize[idx].actural;
            }
        }
        return offset;
    }

    void SSplitWnd::FatchPaneSizeInfo(const SplitPaneList & lstPane, PANESIZELIST & lstPaneSize)
    {
        lstPaneSize.SetCount(lstPane.GetCount());//分配空间

        for(int i=0;i< (int)lstPane.GetCount();i++)
        {
            if(!lstPane[i]->IsVisible())
            {
                lstPaneSize[i].actural = 0;
                lstPaneSize[i].preferred = 0;
                lstPaneSize[i].minimum = 0;
                lstPaneSize[i].maximum = 0;
                continue;
            }

            lstPaneSize[i].preferred = lstPane[i]->m_nSizeIdeal;
            lstPaneSize[i].minimum = lstPane[i]->m_nSizeMin;
            lstPaneSize[i].maximum = lstPane[i]->m_nSizeMax;

            CRect rcItem;
            lstPane[i]->GetWindowRect(&rcItem);
            if(m_orintation == Vertical)
            {
                lstPaneSize[i].actural = rcItem.Width();
            }
            else
            {
                lstPaneSize[i].actural = rcItem.Height();
            }
            if(lstPaneSize[i].actural==0)
                lstPaneSize[i].actural = lstPane[i]->m_nSizeMin;
        }
    }
    
    int SSplitWnd::FunComp(const void * p1,const void * p2)
    {
        SSplitPane* pane1 = *(SSplitPane**)p1;
        SSplitPane* pane2 = *(SSplitPane**)p2;
        return pane2->m_nPriority - pane1->m_nPriority;
    }

    void SSplitWnd::SortPriorityList(SplitPaneList & lstPane)
    {
        qsort(lstPane.GetData(),lstPane.GetCount(),sizeof(SSplitPane*),FunComp);
    }


}//namespace SOUI