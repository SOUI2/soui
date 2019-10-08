// Sscrollbar.h : implementation file
//    模块:滚动条控件
//////////////////////////////////////////////////////////////////////////
#include "souistd.h"
#include "control/Sscrollbar.h"

#define TIMERID_NOTIFY1    1
#define TIMERID_DELAY1    2

namespace SOUI
{


/////////////////////////////////////////////////////////////////////////////
// SScrollBar
SScrollBar::SScrollBar()
    : m_pSkin(GETBUILTINSKIN(SKIN_SYS_SCROLLBAR))
    , m_bDrag(FALSE)
    , m_uClicked((UINT)-1)
    , m_bNotify(FALSE)
    , m_uHtPrev((UINT)-1)
    , m_uAllowSize((UINT)-1)
    , m_bVertical(TRUE)
{
    memset(&m_si,0,sizeof(SCROLLINFO));
    m_si.nTrackPos=-1;
    m_evtSet.addEvent(EVENTID(EventScroll));
}

SScrollBar::~SScrollBar()
{
}

BOOL SScrollBar::IsVertical()
{
    return m_bVertical;
}
UINT SScrollBar::HitTest(CPoint pt)
{
    CRect rc;
    rc=GetPartRect(SB_LINEUP);
    if(rc.PtInRect(pt)) return SB_LINEUP;
    rc=GetPartRect(SB_LINEDOWN);
    if(rc.PtInRect(pt)) return SB_LINEDOWN;
    rc=GetPartRect(SB_THUMBTRACK);
    if(rc.PtInRect(pt)) return SB_THUMBTRACK;
    rc=GetPartRect(SB_PAGEUP);
    if(rc.PtInRect(pt)) return SB_PAGEUP;
    rc=GetPartRect(SB_PAGEDOWN);
    if(rc.PtInRect(pt)) return SB_PAGEDOWN;

    return (UINT)-1;
}
int SScrollBar::SetPos(int nPos)
{
    if(nPos>(m_si.nMax-(int)m_si.nPage+1)) nPos=(m_si.nMax-m_si.nPage+1);
    if(nPos<m_si.nMin) nPos=m_si.nMin;
    if(nPos!=m_si.nPos)
    {
        if(m_si.nTrackPos==-1)
        {
            CRect rcOldThumb=GetPartRect(SB_THUMBTRACK);
            m_si.nTrackPos=nPos;
            CRect rcNewThumb=GetPartRect(SB_THUMBTRACK);
            CRect rcUnion;
            rcUnion.UnionRect(&rcOldThumb,&rcNewThumb);
			if (IsVisible())
			{
				IRenderTarget *pRT = GetRenderTarget(&rcUnion, OLEDC_PAINTBKGND);
				m_pSkin->Draw(pRT, rcUnion, MAKESBSTATE(SB_PAGEDOWN, SBST_NORMAL, IsVertical()));
				m_pSkin->Draw(pRT, rcNewThumb, MAKESBSTATE(SB_THUMBTRACK, SBST_NORMAL, IsVertical()));
				ReleaseRenderTarget(pRT);
			}         
            m_si.nTrackPos=-1;
        }
        m_si.nPos=nPos;
    }
    return m_si.nPos;
}

int SScrollBar::GetPos()
{
    return m_si.nPos;
}

int SScrollBar::GetMax()
{
	return m_si.nMax;
}

int SScrollBar::GetMin()
{
	return m_si.nMin;
}

// Generated message map functions
CRect SScrollBar::GetPartRect(UINT uSBCode)
{
    SASSERT(m_pSkin);
    int nTrackPos=m_si.nTrackPos;
    int nMax=m_si.nMax;
    if(nMax<m_si.nMin+(int)m_si.nPage-1) nMax=m_si.nMin+m_si.nPage-1;

    if(nTrackPos==-1)
        nTrackPos=m_si.nPos;
    CRect rcWnd = GetWindowRect();
    int nLength=(IsVertical()?rcWnd.Height():rcWnd.Width());
    int nInterHei=nLength-2*m_uAllowSize;
    if(nInterHei<0)
        nInterHei=0;
    int    nSlideHei=m_si.nPage*nInterHei/(nMax-m_si.nMin+1);
    if(nMax==(int)(m_si.nMin+m_si.nPage-1))
        nSlideHei=nInterHei;
    if(nSlideHei<THUMB_MINSIZE)
        nSlideHei=THUMB_MINSIZE;
    if(nInterHei<THUMB_MINSIZE)
        nSlideHei=0;
    int nEmptyHei=nInterHei-nSlideHei;
    int nArrowHei=m_uAllowSize;
    if(nInterHei==0)
        nArrowHei=nLength/2;
    CRect rcRet(0,0,rcWnd.Width(),nArrowHei);
    if(uSBCode==SB_LINEUP) goto end;
    rcRet.top=rcRet.bottom;
    if((m_si.nMax-m_si.nMin-m_si.nPage+1)==0)
        rcRet.bottom+=nEmptyHei/2;
    else
        rcRet.bottom+=nEmptyHei*nTrackPos/(m_si.nMax-m_si.nMin-m_si.nPage+1);
    if(uSBCode==SB_PAGEUP) goto end;
    rcRet.top=rcRet.bottom;
    rcRet.bottom+=nSlideHei;
    if(uSBCode==SB_THUMBTRACK) goto end;
    rcRet.top=rcRet.bottom;
    rcRet.bottom=nLength-nArrowHei;
    if(uSBCode==SB_PAGEDOWN) goto end;
    rcRet.top=rcRet.bottom;
    rcRet.bottom=nLength;
    if(uSBCode==SB_LINEDOWN) goto end;
end:
    if(!IsVertical())
    {
        rcRet.left=rcRet.top;
        rcRet.right=rcRet.bottom;
        rcRet.top=0;
        rcRet.bottom=rcWnd.Height();
    }
    rcRet.OffsetRect(rcWnd.TopLeft());
    return rcRet;
}

void SScrollBar::OnInitFinished(pugi::xml_node xmlNode)
{
    __super::OnInitFinished(xmlNode);
    SASSERT(m_pSkin);
    if(m_uAllowSize==-1)
    {
        m_uAllowSize=m_pSkin->GetSkinSize().cx/9;
    }
}

void SScrollBar::OnPaint(IRenderTarget * pRT)
{
    if(!m_pSkin) return;

    int nState=IsDisabled(TRUE)?3:0;
    CRect rcDest;
    rcDest=GetPartRect(SB_LINEUP);
    m_pSkin->Draw(pRT,rcDest,MAKESBSTATE(SB_LINEUP,nState,m_bVertical));
    rcDest=GetPartRect(SB_PAGEUP);
    m_pSkin->Draw(pRT,rcDest,MAKESBSTATE(SB_PAGEUP,nState,m_bVertical));
    rcDest=GetPartRect(SB_THUMBTRACK);
    m_pSkin->Draw(pRT,rcDest,MAKESBSTATE(SB_THUMBTRACK,nState,m_bVertical));
    rcDest=GetPartRect(SB_PAGEDOWN);
    m_pSkin->Draw(pRT,rcDest,MAKESBSTATE(SB_PAGEDOWN,nState,m_bVertical));
    rcDest=GetPartRect(SB_LINEDOWN);
    m_pSkin->Draw(pRT,rcDest,MAKESBSTATE(SB_LINEDOWN,nState,m_bVertical));
}

void SScrollBar::OnLButtonUp(UINT nFlags, CPoint point)
{
    ReleaseCapture();
    if(m_bDrag)
    {
        m_bDrag=FALSE;
        m_si.nPos=m_si.nTrackPos;
        m_si.nTrackPos=-1;
        OnMouseMove(nFlags,point);
        NotifySbCode(SB_THUMBPOSITION,m_si.nPos);
    }
    else if(m_uClicked!=-1)
    {
        if(m_bNotify)
        {
            KillTimer(TIMERID_NOTIFY1);
            m_bNotify=FALSE;
        }
        else
        {
            KillTimer(TIMERID_DELAY1);
        }
        if(m_uClicked==SB_LINEUP||m_uClicked==SB_LINEDOWN)
        {
            CRect rc=GetPartRect(m_uClicked);
            IRenderTarget *pRT=GetRenderTarget(&rc,OLEDC_PAINTBKGND);
            m_pSkin->Draw(pRT,rc,MAKESBSTATE(m_uClicked,SBST_NORMAL,m_bVertical));
            ReleaseRenderTarget(pRT);
        }
        m_uClicked=(UINT)-1;
    }
}

void SScrollBar::OnLButtonDown(UINT nFlags, CPoint point)
{
    SetCapture();
    UINT uHit=HitTest(point);
    if(uHit==SB_THUMBTRACK)
    {
        m_bDrag=TRUE;
        m_ptDrag=point;
        m_si.nTrackPos=m_si.nPos;
        m_nDragPos=m_si.nPos;
    }
    else
    {
        m_uClicked=uHit;

        if(uHit==SB_LINEUP || uHit== SB_LINEDOWN)
        {
            CRect rc=GetPartRect(uHit);
            IRenderTarget *pRT=GetRenderTarget(&rc,OLEDC_PAINTBKGND);
            m_pSkin->Draw(pRT,rc,MAKESBSTATE(uHit,SBST_PUSHDOWN,m_bVertical));
            ReleaseRenderTarget(pRT);
            NotifySbCode(uHit,m_si.nPos);
        }
        else if(uHit == SB_PAGEUP || uHit == SB_PAGEDOWN)
        {
            NotifySbCode(uHit,m_si.nPos);
        }
    }
}


void SScrollBar::OnMouseMove(UINT nFlags, CPoint point)
{
    if(m_bDrag)
    {
        CRect rcWnd = GetWindowRect();
        int nInterHei=(IsVertical()?rcWnd.Height():rcWnd.Width())-2*m_uAllowSize;
        int    nSlideHei=m_si.nPage*nInterHei/(m_si.nMax-m_si.nMin+1);
        if(nSlideHei<THUMB_MINSIZE) nSlideHei=THUMB_MINSIZE;
        if(nInterHei<THUMB_MINSIZE) nSlideHei=0;
        int nEmptyHei=nInterHei-nSlideHei;
        int nDragLen=IsVertical()?(point.y-m_ptDrag.y):(point.x-m_ptDrag.x);
        int nSlide=(nEmptyHei==0)?0:(nDragLen*(int)(m_si.nMax-m_si.nMin-m_si.nPage+1)/nEmptyHei);
        int nNewTrackPos=m_nDragPos+nSlide;
        if(nNewTrackPos<m_si.nMin)
        {
            nNewTrackPos=m_si.nMin;
        }
        else if(nNewTrackPos>(int)(m_si.nMax-m_si.nMin-m_si.nPage+1))
        {
            nNewTrackPos=m_si.nMax-m_si.nMin-m_si.nPage+1;
        }
        if(nNewTrackPos!=m_si.nTrackPos)
        {
            CRect rcOldThumb=GetPartRect(SB_THUMBTRACK);
            m_si.nTrackPos=nNewTrackPos;
            CRect rcThumb=GetPartRect(SB_THUMBTRACK);
            CRect rcUnion;
            rcUnion.UnionRect(rcOldThumb,rcThumb);
            IRenderTarget *pRT=GetRenderTarget(&rcUnion,OLEDC_PAINTBKGND);

            m_pSkin->Draw(pRT,rcUnion,MAKESBSTATE(SB_PAGEUP,SBST_NORMAL,m_bVertical));
            m_pSkin->Draw(pRT,rcThumb,MAKESBSTATE(SB_THUMBTRACK,SBST_HOVER,m_bVertical));

            ReleaseRenderTarget(pRT);
            NotifySbCode(SB_THUMBTRACK,m_si.nTrackPos);
        }
    }
    else
    {
        UINT uHit=HitTest(point);
        if(uHit!=m_uHtPrev)
        {
            if(m_uHtPrev!=-1)
            {
                CRect rc=GetPartRect(m_uHtPrev);
                IRenderTarget *pRT=GetRenderTarget(&rc,OLEDC_PAINTBKGND);
                m_pSkin->Draw(pRT,rc,MAKESBSTATE(m_uHtPrev,SBST_NORMAL,m_bVertical));
                ReleaseRenderTarget(pRT);
            }
            if(uHit!=-1)
            {
                CRect rc=GetPartRect(uHit);
                IRenderTarget *pRT=GetRenderTarget(&rc,OLEDC_PAINTBKGND);
                m_pSkin->Draw(pRT,rc,MAKESBSTATE(uHit,SBST_HOVER,m_bVertical));
                ReleaseRenderTarget(pRT);
            }
            m_uHtPrev=uHit;
        }
    }
}

void SScrollBar::OnTimer(char nIDEvent)
{
    // TODO: Add your message handler code here and/or call default
    if(nIDEvent==TIMERID_NOTIFY1)
    {
        SASSERT(m_uClicked!=-1 && m_uClicked!=SB_THUMBTRACK);

        switch(m_uClicked)
        {
        case SB_LINEUP:
            if(m_si.nPos==m_si.nMin)
            {
                KillTimer(TIMERID_NOTIFY1);
                break;
            }
            break;
        case SB_LINEDOWN:
            if(m_si.nPos==m_si.nMax)
            {
                KillTimer(TIMERID_NOTIFY1);
                break;
            }
            break;
        case SB_PAGEUP:
        case SB_PAGEDOWN:
        {
            CPoint pt;
            ::GetCursorPos(&pt);
            ::ScreenToClient(GetContainer()->GetHostHwnd(),&pt);
            CRect rc=GetPartRect(SB_THUMBTRACK);
            if(rc.PtInRect(pt))
            {
                KillTimer(TIMERID_NOTIFY1);
                break;
            }
        }
        break;
        default:
            SASSERT(FALSE);
            break;
        }
    }
}

void SScrollBar::OnMouseLeave()
{
    if(!m_bDrag)
    {
        if(m_uHtPrev!=-1)
        {
            OnMouseMove(0,CPoint(-1,-1));
        }
    }
}


LRESULT SScrollBar::OnSetScrollInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    BOOL bRedraw=wParam!=0;
    LPSCROLLINFO lpScrollInfo=(LPSCROLLINFO)lParam;
    if(lpScrollInfo->fMask&SIF_PAGE) m_si.nPage=lpScrollInfo->nPage;
    if(lpScrollInfo->fMask&SIF_POS) m_si.nPos=lpScrollInfo->nPos;
    if(lpScrollInfo->fMask&SIF_RANGE)
    {
        m_si.nMin=lpScrollInfo->nMin;
        m_si.nMax=lpScrollInfo->nMax;
    }
    if(m_si.nPos>(m_si.nMax-(int)m_si.nPage+1)) m_si.nPos=(m_si.nMax-m_si.nPage+1);
    if(m_si.nPos<m_si.nMin) m_si.nPos=m_si.nMin;
    if(bRedraw)    Invalidate();

    return TRUE;
}

LRESULT SScrollBar::OnGetScrollInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LPSCROLLINFO lpScrollInfo=(LPSCROLLINFO)lParam;
    int nMask=lpScrollInfo->fMask;
    if(nMask&SIF_PAGE) lpScrollInfo->nPage=m_si.nPage;
    if(nMask&SIF_POS) lpScrollInfo->nPos=m_si.nPos;
    if(nMask&SIF_TRACKPOS) lpScrollInfo->nTrackPos=m_si.nTrackPos;
    if(nMask&SIF_RANGE)
    {
        lpScrollInfo->nMin=m_si.nMin;
        lpScrollInfo->nMax=m_si.nMax;
    }
    return TRUE;
}

LRESULT SScrollBar::NotifySbCode(UINT uCode,int nPos)
{
    EventScroll evt(this);
    evt.uSbCode=uCode;
    evt.nPos=nPos;
    evt.bVertical=IsVertical();
    return FireEvent(evt);
}

}//namespace SOUI