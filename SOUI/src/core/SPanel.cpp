//////////////////////////////////////////////////////////////////////////
//   File Name: SPanel.h
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "souistd.h"
#include "core/Spanel.h"

#define DEF_UPDATEINTERVAL    0

namespace SOUI
{

SPanel::SPanel()
    :m_dragSb(DSB_NULL)
    ,m_wBarVisible(0)
    ,m_wBarEnable(SSB_BOTH)
    ,m_dwUpdateInterval(DEF_UPDATEINTERVAL)
    ,m_nScrollSpeed(10)
	, m_zDelta(0)
{
	m_nSbWid.setInvalid();
	m_nSbArrowSize.setInvalid();

    ISkinObj *pSkin=GETBUILTINSKIN(SKIN_SYS_SCROLLBAR);
    if(pSkin && pSkin->IsClass(SSkinScrollbar::GetClassName()))
        m_pSkinSb=(SSkinScrollbar*)pSkin;

    memset(&m_siHoz,0,sizeof(SCROLLINFO));
    memset(&m_siVer,0,sizeof(SCROLLINFO));
    m_HitInfo.uSbCode=(DWORD)-1;
    m_siHoz.nTrackPos=(DWORD)-1;
    m_siVer.nTrackPos=(DWORD)-1;
    
}

BOOL SPanel::ShowScrollBar( int wBar, BOOL bShow )
{
    if(bShow) m_wBarVisible|=wBar;
    else m_wBarVisible&=~wBar;
    SSendMessage(WM_NCCALCSIZE);
    Invalidate();
    return TRUE;
}


BOOL SPanel::EnableScrollBar( int wBar,BOOL bEnable )
{
    if(bEnable) m_wBarEnable|=wBar;
    else m_wBarEnable&=~wBar;
    if(wBar & SSB_VERT)
    {
        CRect rcSb=GetScrollBarRect(TRUE);
        InvalidateRect(rcSb);
    }
    if(wBar & SSB_HORZ)
    {
        CRect rcSb=GetScrollBarRect(FALSE);
        InvalidateRect(rcSb);
    }
    return TRUE;
}


BOOL SPanel::IsScrollBarEnable( BOOL bVertical )
{
    return m_wBarEnable&(bVertical?SSB_VERT:SSB_HORZ);
}

void SPanel::SetScrollInfo(SCROLLINFO si,BOOL bVertical)
{
    SCROLLINFO *psi=bVertical?(&m_siVer):(&m_siHoz);
    if(si.fMask & SIF_RANGE)
    {
        psi->nMin=si.nMin;
        psi->nMax=si.nMax;
    }
    if(si.fMask & SIF_PAGE) psi->nPage=si.nPage;
    if (si.fMask & SIF_POS) psi->nPos=si.nPos;
    if(si.fMask & SIF_TRACKPOS) psi->nTrackPos=si.nTrackPos;
    if(si.nPage==0) psi->nPos=0;
    else
    {
        if(psi->nPos>(psi->nMax-(int)psi->nPage+1)) psi->nPos=(psi->nMax-psi->nPage+1);
        if(psi->nPos<psi->nMin) psi->nPos=psi->nMin;
    }
}

BOOL SPanel::SetScrollPos(BOOL bVertical, int nNewPos,BOOL bRedraw)
{
    SCROLLINFO *psi=bVertical?(&m_siVer):(&m_siHoz);

    //make sure nNewPos is valid.
    if(nNewPos<psi->nMin) nNewPos=psi->nMin;
    if(nNewPos>psi->nMax - (int)psi->nPage+1) nNewPos=psi->nMax-psi->nPage+1;

    psi->nPos=nNewPos;

    if(bRedraw)
    {
        CRect rcSb=GetScrollBarRect(bVertical);
        InvalidateRect(rcSb);
    }
    OnScroll(bVertical,SB_THUMBPOSITION,nNewPos);
    return TRUE;
}

int SPanel::GetScrollPos(BOOL bVertical)
{
    return bVertical?m_siVer.nPos:m_siHoz.nPos;
}


BOOL SPanel::SetScrollRange( BOOL bVertical,int nMinPos,int nMaxPos,BOOL bRedraw )
{
    SCROLLINFO *psi=bVertical?(&m_siVer):(&m_siHoz);
    psi->nMin=nMinPos;
    psi->nMax=nMaxPos;

    if(psi->nPos<psi->nMin) psi->nPos=psi->nMin;
    if(psi->nPos>psi->nMax - (int)psi->nPage+1) psi->nPos=psi->nMax-psi->nPage+1;

    if(bRedraw)
    {
        CRect rcSb=GetScrollBarRect(bVertical);
        InvalidateRect(rcSb);
    }
    return TRUE;
}

BOOL SPanel::GetScrollRange( BOOL bVertical, LPINT lpMinPos, LPINT lpMaxPos )
{
    SCROLLINFO *psi=bVertical?(&m_siVer):(&m_siHoz);
    if(lpMaxPos) *lpMaxPos=psi->nMax;
    if(lpMinPos) *lpMinPos=psi->nMin;
    return TRUE;
}

BOOL SPanel::HasScrollBar(BOOL bVertical)
{
    return m_wBarVisible&(bVertical?SSB_VERT:SSB_HORZ);
}


CRect SPanel::GetSbPartRect(BOOL bVertical,UINT uSBCode)
{
    SASSERT(m_pSkinSb);
    CRect rcSb=GetScrollBarRect(bVertical);
    if(rcSb.IsRectEmpty()) 
		return rcSb;
	
    SCROLLINFO *psi=bVertical?(&m_siVer):(&m_siHoz);
    if(psi->nMax < psi->nMin)
        return CRect();
    __int64 nTrackPos=psi->nTrackPos;
    int nMax=psi->nMax;
    if(nMax<psi->nMin+(int)psi->nPage-1) nMax=psi->nMin+psi->nPage-1;

    if(nTrackPos==-1)
        nTrackPos=psi->nPos;
    int nLength=(bVertical?rcSb.Height():rcSb.Width());
    int nInterHei=nLength-2*GetSbArrowSize();
    if(nInterHei<0)
        nInterHei=0;
    int    nSlideHei=psi->nPage*nInterHei/(nMax-psi->nMin+1);
    if(nMax==(int)(psi->nMin+psi->nPage-1))
        nSlideHei=nInterHei;
    if(nSlideHei<THUMB_MINSIZE)
        nSlideHei=THUMB_MINSIZE;
    if(nInterHei<THUMB_MINSIZE)
        nSlideHei=0;
    int nEmptyHei=nInterHei-nSlideHei;
    int nArrowHei=GetSbArrowSize();
    if(nInterHei==0)
        nArrowHei=nLength/2;

    CRect rcRet(0,0,rcSb.Width(),nArrowHei);
    if(uSBCode==SB_LINEUP) goto end;
    rcRet.top=rcRet.bottom;
    if((psi->nMax-psi->nMin-psi->nPage+1)==0)
        rcRet.bottom+=nEmptyHei/2;
    else
        rcRet.bottom+=(long)(nEmptyHei*nTrackPos/(psi->nMax-psi->nMin-psi->nPage+1));
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
    if(!bVertical)
    {
        rcRet.left=rcRet.top;
        rcRet.right=rcRet.bottom;
        rcRet.top=0;
        rcRet.bottom=rcSb.Height();	
    }
    rcRet.OffsetRect(rcSb.TopLeft());
    return rcRet;
}

CRect SPanel::GetSbRailwayRect( BOOL bVertical )
{
    SASSERT(m_pSkinSb);
    CRect rcSb=GetScrollBarRect(bVertical);
    if(rcSb.IsRectEmpty()) return rcSb;

    if(bVertical)
    {
        rcSb.DeflateRect(0,GetSbArrowSize());
    }else
    {
        rcSb.DeflateRect(GetSbArrowSize(),0);
    }
    return rcSb;
}

CRect SPanel::GetScrollBarRect(BOOL bVertical)
{
    CRect rcSb;
    if(!HasScrollBar(bVertical)) return rcSb;

    SWindow::GetClientRect(&rcSb);

    if(bVertical)
    {
        rcSb.left=rcSb.right-GetSbWidth();
    }
    else
    {
        rcSb.top=rcSb.bottom-GetSbWidth();
    }
    if(HasScrollBar(!bVertical))
    {
        if(bVertical)
        {
            rcSb.bottom-=GetSbWidth();
        }
        else
        {
            rcSb.right-=GetSbWidth();
        }
    }

	if(bVertical)
	{
		rcSb.top += m_nSbTop.toPixelSize(GetScale());
		rcSb.bottom -= m_nSbBottom.toPixelSize(GetScale());
	}
	else
	{
		rcSb.left += m_nSbLeft.toPixelSize(GetScale());
		rcSb.right -= m_nSbRight.toPixelSize(GetScale());
	}
    return rcSb;
}

SBHITINFO SPanel::HitTest(CPoint pt)
{
    SBHITINFO hi= {(DWORD)-1,(DWORD)0};
    CRect rcSbVer=GetScrollBarRect(TRUE);
    CRect rcSbHoz=GetScrollBarRect(FALSE);
    if(rcSbHoz.PtInRect(pt)) hi.bVertical=FALSE;
    else if(rcSbVer.PtInRect(pt)) hi.bVertical=TRUE;
    else return hi;

    if(GetSbPartRect(hi.bVertical,SB_LINEUP).PtInRect(pt)) hi.uSbCode=SB_LINEUP;
    else if(GetSbPartRect(hi.bVertical,SB_LINEDOWN).PtInRect(pt)) hi.uSbCode= SB_LINEDOWN;
    else if(GetSbPartRect(hi.bVertical,SB_THUMBTRACK).PtInRect(pt)) hi.uSbCode= SB_THUMBTRACK;
    else if(GetSbPartRect(hi.bVertical,SB_PAGEUP).PtInRect(pt)) hi.uSbCode= SB_PAGEUP;
    else if(GetSbPartRect(hi.bVertical,SB_PAGEDOWN).PtInRect(pt)) hi.uSbCode= SB_PAGEDOWN;

    return hi;

}

int SPanel::OnCreate(LPVOID)
{
    int nRet=__super::OnCreate(NULL);
    if(nRet!=0) return nRet;
    SASSERT(m_pSkinSb);
    return 0;
}

void SPanel::OnNcPaint(IRenderTarget *pRT)
{
    __super::OnNcPaint(pRT);
    CRect rcDest;
    //绘制滚动条
    if(HasScrollBar(TRUE))
    {
        int nState=(IsDisabled(TRUE) || !IsScrollBarEnable(TRUE))?SBST_DISABLE:SBST_INACTIVE;
        rcDest=GetSbPartRect(TRUE,SB_LINEUP);
        m_pSkinSb->Draw(pRT,rcDest,MAKESBSTATE(SB_LINEUP,nState,TRUE));
        rcDest=GetSbRailwayRect(TRUE);
        m_pSkinSb->Draw(pRT,rcDest,MAKESBSTATE(SB_PAGEUP,m_dragSb==DSB_VERT?SBST_HOVER:nState,TRUE));
        rcDest=GetSbPartRect(TRUE,SB_THUMBTRACK);
        int nStateThumb = nState;
        if(m_dragSb == DSB_VERT) 
            nStateThumb = SBST_PUSHDOWN;
        else if(m_HitInfo.bVertical && m_HitInfo.uSbCode == SB_THUMBTRACK)
            nStateThumb= SBST_HOVER;
        m_pSkinSb->Draw(pRT,rcDest,MAKESBSTATE(SB_THUMBTRACK,nStateThumb,TRUE));
        rcDest=GetSbPartRect(TRUE,SB_LINEDOWN);
        m_pSkinSb->Draw(pRT,rcDest,MAKESBSTATE(SB_LINEDOWN,nState,TRUE));
    }
    if(HasScrollBar(FALSE))
    {
         int nState=(IsDisabled(TRUE) || !IsScrollBarEnable(FALSE))?SBST_DISABLE:SBST_INACTIVE;
         rcDest=GetSbPartRect(FALSE,SB_LINEUP);
         m_pSkinSb->Draw(pRT,rcDest,MAKESBSTATE(SB_LINEUP,nState,FALSE));
         rcDest=GetSbRailwayRect(FALSE);
         m_pSkinSb->Draw(pRT,rcDest,MAKESBSTATE(SB_PAGEUP,m_dragSb==DSB_HORZ?SBST_HOVER:nState,FALSE));
         rcDest=GetSbPartRect(FALSE,SB_THUMBTRACK);
         int nStateThumb = nState;
         if(m_dragSb == DSB_HORZ) 
             nStateThumb = SBST_PUSHDOWN;
         else if(!m_HitInfo.bVertical && m_HitInfo.uSbCode == SB_THUMBTRACK)
             nStateThumb= SBST_HOVER;
         m_pSkinSb->Draw(pRT,rcDest,MAKESBSTATE(SB_THUMBTRACK,nStateThumb,FALSE));
         rcDest=GetSbPartRect(FALSE,SB_LINEDOWN);
         m_pSkinSb->Draw(pRT,rcDest,MAKESBSTATE(SB_LINEDOWN,nState,FALSE));
    }

    if(HasScrollBar(TRUE) && HasScrollBar(FALSE))
    {
        CRect rcDest;
        __super::GetClientRect(&rcDest);
        rcDest.left=rcDest.right-GetSbWidth();
        rcDest.top=rcDest.bottom-GetSbWidth();
        m_pSkinSb->Draw(pRT,rcDest,SB_CORNOR);
    }
}

void SPanel::GetClientRect(LPRECT pRect) const
{
    pRect->left=m_rcClient.left;
    pRect->right=m_rcClient.right;
    pRect->top=m_rcClient.top;
    pRect->bottom=m_rcClient.bottom;
}

CRect SPanel::GetClientRect() const
{
    return m_rcClient;
}

BOOL SPanel::OnNcHitTest(CPoint pt)
{
    if(m_dragSb!=DSB_NULL) return TRUE;
    return !m_rcClient.PtInRect(pt);
}

void SPanel::OnNcLButtonDown(UINT nFlags, CPoint point)
{
    m_HitInfo=HitTest(point);
    if(m_HitInfo.uSbCode!=WORD(-1) && IsScrollBarEnable(m_HitInfo.bVertical))
    {
        SetCapture();
        if(m_HitInfo.uSbCode!=SB_THUMBTRACK)
        {
            if(m_HitInfo.uSbCode==SB_LINEUP || m_HitInfo.uSbCode== SB_LINEDOWN)
            {
                CRect rc=GetSbPartRect(m_HitInfo.bVertical,m_HitInfo.uSbCode);
                CAutoRefPtr<IRenderTarget> pRT=GetRenderTarget(&rc,OLEDC_PAINTBKGND,FALSE);
                m_pSkinSb->Draw(pRT,rc,MAKESBSTATE(m_HitInfo.uSbCode,SBST_PUSHDOWN,m_HitInfo.bVertical));
                ReleaseRenderTarget(pRT);
            }
            OnScroll(m_HitInfo.bVertical,m_HitInfo.uSbCode,m_HitInfo.bVertical?m_siVer.nPos:m_siHoz.nPos);
            SetTimer(TIMER_SBWAIT,500);
        }
        else
        {
            m_dragSb = m_HitInfo.bVertical?DSB_VERT:DSB_HORZ;
            m_ptDragSb=point;
            m_nDragPos=m_HitInfo.bVertical?m_siVer.nPos:m_siHoz.nPos;
            m_dwUpdateTime=GetTickCount()-m_dwUpdateInterval;//让第一次滚动消息能够即时刷新

            CRect rcSlide=GetSbPartRect(m_HitInfo.bVertical,SB_THUMBTRACK);
            CAutoRefPtr<IRenderTarget> pRT=GetRenderTarget(&rcSlide,OLEDC_PAINTBKGND,FALSE);
            CRect rcRail = GetSbRailwayRect(m_HitInfo.bVertical);
            m_pSkinSb->Draw(pRT,rcRail,MAKESBSTATE(SB_PAGEUP,SBST_HOVER,m_HitInfo.bVertical));
            m_pSkinSb->Draw(pRT,rcSlide,MAKESBSTATE(SB_THUMBTRACK,SBST_PUSHDOWN,m_HitInfo.bVertical));
            ReleaseRenderTarget(pRT);
        }
    }
}

void SPanel::OnNcLButtonUp(UINT nFlags,CPoint pt)
{
    ReleaseCapture();
    if(m_dragSb!=DSB_NULL)
    {
        m_dragSb=DSB_NULL;
        SCROLLINFO *psi=m_HitInfo.bVertical?(&m_siVer):(&m_siHoz);
        if(psi->nTrackPos!=-1) OnScroll(m_HitInfo.bVertical,SB_THUMBPOSITION,psi->nTrackPos);

        CRect rcRail=GetScrollBarRect(m_HitInfo.bVertical);
        if(m_HitInfo.bVertical)
		{
			rcRail.DeflateRect(0,GetSbArrowSize());
		}
        else
		{
			rcRail.DeflateRect(GetSbArrowSize(),0);
		}
        CAutoRefPtr<IRenderTarget> pRT=GetRenderTarget(&rcRail,OLEDC_PAINTBKGND,FALSE);
        m_pSkinSb->Draw(pRT,rcRail,MAKESBSTATE(SB_PAGEDOWN,SBST_NORMAL,m_HitInfo.bVertical));
        psi->nTrackPos=-1;
        CRect rcSlide=GetSbPartRect(m_HitInfo.bVertical,SB_THUMBTRACK);
        m_pSkinSb->Draw(pRT,rcSlide,MAKESBSTATE(SB_THUMBTRACK,SBST_NORMAL,m_HitInfo.bVertical));
        ReleaseRenderTarget(pRT);
    }
    else if(m_HitInfo.uSbCode!=WORD(-1) && IsScrollBarEnable(m_HitInfo.bVertical))
    {
        if(m_HitInfo.uSbCode==SB_LINEUP||m_HitInfo.uSbCode==SB_LINEDOWN)
        {
            CRect rc=GetSbPartRect(m_HitInfo.bVertical,m_HitInfo.uSbCode);
            CAutoRefPtr<IRenderTarget> pRT=GetRenderTarget(&rc,OLEDC_PAINTBKGND,FALSE);
            m_pSkinSb->Draw(pRT,rc,MAKESBSTATE(m_HitInfo.uSbCode,SBST_NORMAL,m_HitInfo.bVertical));
            ReleaseRenderTarget(pRT);
        }
    }
    KillTimer(TIMER_SBWAIT);
    KillTimer(TIMER_SBGO);
    m_HitInfo.uSbCode=(DWORD)-1;
    OnNcMouseMove(nFlags,pt);
}

void SPanel::OnNcMouseMove(UINT nFlags, CPoint point)
{
    if(m_dragSb!=DSB_NULL)
    {
        if(m_HitInfo.uSbCode!=SB_THUMBTRACK) return;
        CRect rcSb=GetScrollBarRect(m_HitInfo.bVertical);
        CRect rcRail=rcSb;
        if(m_HitInfo.bVertical)
		{
			rcRail.DeflateRect(0,GetSbArrowSize());
		}
		else
		{
			rcRail.DeflateRect(GetSbArrowSize(),0);
		}

        int nInterHei=m_HitInfo.bVertical?rcRail.Height():rcRail.Width();
        SCROLLINFO *psi=m_HitInfo.bVertical?(&m_siVer):(&m_siHoz);

        int    nSlideLen=GetSbSlideLength(m_HitInfo.bVertical);
        int nEmptyHei=nInterHei-nSlideLen;

        int nDragLen=m_HitInfo.bVertical?(point.y-m_ptDragSb.y):(point.x-m_ptDragSb.x);

        int nSlide=(nEmptyHei==0)?0:(nDragLen*(__int64)(psi->nMax-psi->nMin-psi->nPage+1)/nEmptyHei);//防止溢出,需要使用__int64
        int nNewTrackPos=m_nDragPos+nSlide;
        if(nNewTrackPos<psi->nMin)
        {
            nNewTrackPos=psi->nMin;
        }
        else if(nNewTrackPos>(int)(psi->nMax-psi->nMin-psi->nPage+1))
        {
            nNewTrackPos=psi->nMax-psi->nMin-psi->nPage+1;
        }

        CRect rcSlide=GetSbSlideRectByPos(m_HitInfo.bVertical,m_nDragPos);
        if(m_HitInfo.bVertical)
        {
            if(nDragLen>0 && nDragLen>rcRail.bottom-rcSlide.bottom)
                nDragLen=rcRail.bottom-rcSlide.bottom;
            if(nDragLen<0 && nDragLen<rcRail.top-rcSlide.top)
                nDragLen=rcRail.top-rcSlide.top;
            rcSlide.OffsetRect(0,nDragLen);
        }
        else
        {
            if(nDragLen>0 && nDragLen>rcRail.right-rcSlide.right)
                nDragLen=rcRail.right-rcSlide.right;
            if(nDragLen<0 && nDragLen<rcRail.left-rcSlide.left)
                nDragLen=rcRail.left-rcSlide.left;
            rcSlide.OffsetRect(nDragLen,0);
        }

        CAutoRefPtr<IRenderTarget> pRT=GetRenderTarget(&rcRail,OLEDC_PAINTBKGND,FALSE);
        m_pSkinSb->Draw(pRT,rcRail,MAKESBSTATE(SB_PAGEUP,SBST_HOVER,m_HitInfo.bVertical));
        m_pSkinSb->Draw(pRT,rcSlide,MAKESBSTATE(SB_THUMBTRACK,SBST_PUSHDOWN,m_HitInfo.bVertical));
        ReleaseRenderTarget(pRT);

        if(nNewTrackPos!=psi->nTrackPos)
        {
            psi->nTrackPos=nNewTrackPos;
            OnScroll(m_HitInfo.bVertical,SB_THUMBTRACK,psi->nTrackPos);
        }
    }
    else
    {
        SBHITINFO uHit=HitTest(point);
        SBHITINFO uHitOrig=m_HitInfo;//备份出来，防止在其它过程中修改

        if(uHit.uSbCode==WORD(-1))
            OnNcMouseLeave();
        else if(uHit != uHitOrig)
        {
            if(uHitOrig.uSbCode==WORD(-1) || uHitOrig.bVertical != uHit.bVertical)
            {//鼠标进入当前滚动条
                if(uHitOrig.uSbCode!=WORD(-1))
                    OnNcMouseLeave();//切换滚动条，源滚动条失活
                if(IsScrollBarEnable(uHit.bVertical))
                {
                    CRect rc=GetScrollBarRect(uHit.bVertical);
                    CAutoRefPtr<IRenderTarget> pRT=GetRenderTarget(&rc,OLEDC_PAINTBKGND,FALSE);
                    if(uHit.uSbCode!=SB_LINEUP) 
                    {
                        rc=GetSbPartRect(uHit.bVertical,SB_LINEUP);
                        m_pSkinSb->Draw(pRT,rc,MAKESBSTATE(SB_LINEUP,SBST_NORMAL,uHit.bVertical));
                    }
                    rc=GetSbRailwayRect(uHit.bVertical);
                    m_pSkinSb->Draw(pRT,rc,MAKESBSTATE(SB_PAGEUP,SBST_HOVER,uHit.bVertical));
                    if(uHit.uSbCode!=SB_LINEDOWN)
                    {
                        rc=GetSbPartRect(uHit.bVertical,SB_LINEDOWN);
                        m_pSkinSb->Draw(pRT,rc,MAKESBSTATE(SB_LINEDOWN,SBST_NORMAL,uHit.bVertical));
                    }
                    if(uHit.uSbCode!=SB_THUMBTRACK)
                    {
                        rc=GetSbPartRect(uHit.bVertical,SB_THUMBTRACK);
                        m_pSkinSb->Draw(pRT,rc,MAKESBSTATE(SB_THUMBTRACK,SBST_NORMAL,uHit.bVertical));
                    }
                    if(uHit.uSbCode!=SB_PAGEUP && uHit.uSbCode!=SB_PAGEDOWN)
                    {
                        rc=GetSbPartRect(uHit.bVertical,uHit.uSbCode);
                        m_pSkinSb->Draw(pRT,rc,MAKESBSTATE(uHit.uSbCode,SBST_HOVER,uHit.bVertical));
                    }
                    ReleaseRenderTarget(pRT);
                }
            }else
            {//鼠标在当前滚动条内移动
                if(IsScrollBarEnable(uHitOrig.bVertical))
                {
                    if(uHitOrig.uSbCode==SB_LINEUP || uHitOrig.uSbCode==SB_LINEDOWN)
                    {
                        CRect rc=GetSbPartRect(uHitOrig.bVertical,uHitOrig.uSbCode);
                        CAutoRefPtr<IRenderTarget> pRT=GetRenderTarget(&rc,OLEDC_PAINTBKGND,FALSE);
                        m_pSkinSb->Draw(pRT,rc,MAKESBSTATE(uHitOrig.uSbCode,SBST_NORMAL,uHitOrig.bVertical));
                        ReleaseRenderTarget(pRT);
                    }else if(uHitOrig.uSbCode==SB_THUMBTRACK)
                    {//需要先画轨道，再画拖动条,以处理拖动条可能出现的半透明
                        CRect rc=GetSbRailwayRect(uHitOrig.bVertical);
                        CAutoRefPtr<IRenderTarget> pRT=GetRenderTarget(&rc,OLEDC_PAINTBKGND,FALSE);
                        m_pSkinSb->Draw(pRT,rc,MAKESBSTATE(SB_PAGEUP,SBST_HOVER,uHitOrig.bVertical));
                        rc=GetSbPartRect(uHitOrig.bVertical,SB_THUMBTRACK);
                        m_pSkinSb->Draw(pRT,rc,MAKESBSTATE(SB_THUMBTRACK,SBST_NORMAL,uHitOrig.bVertical));
                        ReleaseRenderTarget(pRT);
                    }
                }
                if(uHit.uSbCode!=WORD(-1) && IsScrollBarEnable(uHit.bVertical))
                {
                    if(uHit.uSbCode==SB_LINEUP || uHit.uSbCode==SB_LINEDOWN)
                    {
                        CRect rc=GetSbPartRect(uHit.bVertical,uHit.uSbCode);
                        CAutoRefPtr<IRenderTarget> pRT=GetRenderTarget(&rc,OLEDC_PAINTBKGND,FALSE);
                        m_pSkinSb->Draw(pRT,rc,MAKESBSTATE(uHit.uSbCode,SBST_HOVER,uHit.bVertical));
                        ReleaseRenderTarget(pRT);
                    }else if(uHit.uSbCode==SB_THUMBTRACK)
                    {//需要先画轨道，再画拖动条,以处理拖动条可能出现的半透明
                        CRect rc=GetSbRailwayRect(uHit.bVertical);
                        CAutoRefPtr<IRenderTarget> pRT=GetRenderTarget(&rc,OLEDC_PAINTBKGND,FALSE);
                        m_pSkinSb->Draw(pRT,rc,MAKESBSTATE(SB_PAGEUP,SBST_NORMAL,uHit.bVertical));
                        rc=GetSbPartRect(uHit.bVertical,SB_THUMBTRACK);
                        m_pSkinSb->Draw(pRT,rc,MAKESBSTATE(SB_THUMBTRACK,SBST_HOVER,uHit.bVertical));
                        ReleaseRenderTarget(pRT);
                    }
                }
            }

            m_HitInfo=uHit;
        }
    }
}

void SPanel::OnNcMouseLeave()
{
    if(m_dragSb!=DSB_NULL || m_HitInfo.uSbCode==WORD(-1) || !IsVisible(TRUE)) return;

    SBHITINFO uHit= {(DWORD)-1,(DWORD)0};

    if(IsScrollBarEnable(m_HitInfo.bVertical))
    {
        CRect rcSb=GetScrollBarRect(m_HitInfo.bVertical);
        CAutoRefPtr<IRenderTarget> pRT=GetRenderTarget(&rcSb,OLEDC_PAINTBKGND,FALSE);
        CRect rcDest=GetSbPartRect(m_HitInfo.bVertical,SB_LINEUP);
        m_pSkinSb->Draw(pRT,rcDest,MAKESBSTATE(SB_LINEUP,SBST_INACTIVE,m_HitInfo.bVertical));
        rcDest=GetSbRailwayRect(m_HitInfo.bVertical);
        m_pSkinSb->Draw(pRT,rcDest,MAKESBSTATE(SB_PAGEUP,SBST_INACTIVE,m_HitInfo.bVertical));
        rcDest=GetSbPartRect(m_HitInfo.bVertical,SB_THUMBTRACK);
        m_pSkinSb->Draw(pRT,rcDest,MAKESBSTATE(SB_THUMBTRACK,SBST_INACTIVE,m_HitInfo.bVertical));
        rcDest=GetSbPartRect(m_HitInfo.bVertical,SB_LINEDOWN);
        m_pSkinSb->Draw(pRT,rcDest,MAKESBSTATE(SB_LINEDOWN,SBST_INACTIVE,m_HitInfo.bVertical));
        ReleaseRenderTarget(pRT);
    }
    m_HitInfo=uHit;
}

//滚动条显示或者隐藏时发送该消息
LRESULT SPanel::OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam)
{
    SWindow::GetClientRect(&m_rcClient);

    if(HasScrollBar(TRUE)) m_rcClient.right-=GetSbWidth();
    if(HasScrollBar(FALSE)) m_rcClient.bottom-=GetSbWidth();
    InvalidateRect(NULL);
    return 0;
}

BOOL SPanel::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	m_zDelta += zDelta;
    short delta = m_zDelta /WHEEL_DELTA;
	m_zDelta %= WHEEL_DELTA;

    short nLines = delta>0?delta:(-delta);
    short i = 0;
    for (; i < nLines; i++)
    {
        // 返回FALSE代表无法再再进行滚动,可以透传给父窗口
        if (!OnScroll(TRUE, delta>0?SB_LINEUP:SB_LINEDOWN, 0)) 
        {
            break;
        }
		ScrollUpdate();
	}
    if(i<nLines)
    {// 返回FALSE代表无法再再进行滚动,可以透传给父窗口
        delta = (nLines-i)*WHEEL_DELTA*(delta>0?1:-1);
        SWindow::OnMouseWheel(nFlags, delta, pt);
    }
	if (nLines != 0)
    {
        HWND hHost=GetContainer()->GetHostHwnd();
        ::SendMessage(hHost,WM_MOUSEMOVE,nFlags,MAKELPARAM(pt.x,pt.y));
        ::SendMessage(hHost,WM_SETCURSOR,WPARAM(hHost),MAKELPARAM(HTCLIENT,WM_MOUSEMOVE));
    }
    return FALSE;
}


BOOL SPanel::OnScroll(BOOL bVertical,UINT uCode,int nPos)
{
    SCROLLINFO *psi=bVertical?(&m_siVer):(&m_siHoz);
    int nNewPos=psi->nPos;
    switch(uCode)
    {
    case SB_LINEUP:
        nNewPos-=GetScrollLineSize(bVertical);
        break;
    case SB_LINEDOWN:
        nNewPos+=GetScrollLineSize(bVertical);
        break;
    case SB_PAGEUP:
        nNewPos-=psi->nPage;
        break;
    case SB_PAGEDOWN:
        nNewPos+=psi->nPage;
        break;
    case SB_THUMBTRACK:
        nNewPos=nPos;
        break;
    case SB_THUMBPOSITION:
        nNewPos=nPos;
        break;
    case SB_TOP:
        nNewPos = psi->nMin;
        break;
    case SB_BOTTOM:
        nNewPos = psi->nMax-psi->nPage+1;
        break;
    }

    if(nNewPos<psi->nMin) nNewPos=psi->nMin;
    if(nNewPos>psi->nMax - (int)psi->nPage+1) nNewPos=psi->nMax-psi->nPage+1;
    if(psi->nPage==0) nNewPos=0;

    if(nNewPos!=psi->nPos)
    {
        psi->nPos=nNewPos;
        if(uCode!=SB_THUMBTRACK && IsVisible(TRUE)&& IsScrollBarEnable(bVertical))
        {
            CRect rcRail=GetScrollBarRect(bVertical);
            if(bVertical)
			{
				rcRail.DeflateRect(0,GetSbArrowSize());
			}
			else
			{
				rcRail.DeflateRect(GetSbArrowSize(),0);
			}
            CAutoRefPtr<IRenderTarget> pRT=GetRenderTarget(&rcRail,OLEDC_PAINTBKGND,FALSE);
            m_pSkinSb->Draw(pRT,rcRail,MAKESBSTATE(SB_PAGEDOWN,SBST_NORMAL,bVertical));
            psi->nTrackPos=-1;
            CRect rcSlide=GetSbPartRect(bVertical,SB_THUMBTRACK);
            m_pSkinSb->Draw(pRT,rcSlide,MAKESBSTATE(SB_THUMBTRACK,SBST_NORMAL,bVertical));
            ReleaseRenderTarget(pRT);
        }
        Invalidate();
        return TRUE;
    }
    return FALSE;
}

int SPanel::GetSbSlideLength(BOOL bVertical)
{
    SCROLLINFO *psi=bVertical?(&m_siVer):(&m_siHoz);
    CRect rcSb=GetScrollBarRect(bVertical);
    int nInterHei=(bVertical?rcSb.Height():rcSb.Width())- 2*GetSbArrowSize();

    int    nSlideHei=psi->nPage*nInterHei/(psi->nMax-psi->nMin+1);
    if(nSlideHei<THUMB_MINSIZE) nSlideHei=THUMB_MINSIZE;
    if(nInterHei<THUMB_MINSIZE) nSlideHei=0;

    return nSlideHei;
}

CRect SPanel::GetSbSlideRectByPos(BOOL bVertical,int nPos)
{
    SCROLLINFO *psi=bVertical?(&m_siVer):(&m_siHoz);
    int nOldPos=psi->nTrackPos;
    psi->nTrackPos=nPos;
    CRect rcRet=GetSbPartRect(bVertical,SB_THUMBTRACK);
    psi->nTrackPos=nOldPos;
    return rcRet;
}

void SPanel::OnTimer( char cTimerID )
{
    if(cTimerID==TIMER_SBWAIT)
    {
        KillTimer(cTimerID);
        SetTimer(TIMER_SBGO,50);
        OnTimer(TIMER_SBGO);
    }
    else if(cTimerID==TIMER_SBGO)
    {
        if(m_HitInfo.uSbCode>=SB_LINEUP && m_HitInfo.uSbCode<=SB_PAGEDOWN)
        {
            OnScroll(m_HitInfo.bVertical,m_HitInfo.uSbCode,0);
        }
    }
}

void SPanel::ScrollUpdate()
{
    DWORD dwTime=GetTickCount();
    if(dwTime-m_dwUpdateTime>=m_dwUpdateInterval)
    {
        UpdateWindow();
        m_dwUpdateTime=dwTime;
    }
}

void SPanel::OnShowWindow( BOOL bShow, UINT nStatus )
{
    if(!bShow && m_dragSb!=DSB_NULL)
    {//隐藏窗口时正有可能正在拖动滚动条，需要处理一下。
        OnNcLButtonUp(0,CPoint());
    }
    __super::OnShowWindow(bShow,nStatus);
}

HRESULT SPanel::OnAttrScrollbarSkin( SStringW strValue,BOOL bLoading )
{
    ISkinObj *pSbSkin=GETSKIN(strValue,GetScale());
    SASSERT(pSbSkin);
    if(!pSbSkin->IsClass(SSkinScrollbar::GetClassName())) return E_FAIL;
    m_pSkinSb=(SSkinScrollbar*)pSbSkin;
    SASSERT(m_pSkinSb);
    return bLoading?S_FALSE:S_OK;
}

int SPanel::GetScrollLineSize( BOOL bVertical )
{
    return m_nScrollSpeed;
}

void SPanel::OnVScroll(UINT nSBCode, UINT nPos, HWND)
{
    OnScroll(TRUE,nSBCode,nPos);
}

void SPanel::OnHScroll(UINT nSBCode, UINT nPos, HWND)
{
    OnScroll(FALSE,nSBCode,nPos);
}

void SPanel::OnColorize(COLORREF cr)
{
    __super::OnColorize(cr);
    if(m_pSkinSb) m_pSkinSb->OnColorize(cr);
}

void SPanel::OnScaleChanged(int nScale)
{
	__super::OnScaleChanged(nScale);
	if(m_pSkinSb)
	{
		ISkinObj *pSkin = m_pSkinSb;
		GetScaleSkin(pSkin,nScale);
		m_pSkinSb = (SSkinScrollbar*)pSkin;
		SSendMessage(WM_NCCALCSIZE);
	}
}

int SPanel::GetSbArrowSize()
{
	if(m_nSbArrowSize.isValid()) return m_nSbArrowSize.toPixelSize(GetScale());
	SASSERT(m_pSkinSb);
	if(!m_pSkinSb->HasArrow()) return 0;
	return m_pSkinSb->GetIdealSize();
}

int SPanel::GetSbWidth()
{
	if(m_nSbWid.isValid()) return m_nSbWid.toPixelSize(GetScale());
	SASSERT(m_pSkinSb);
	return m_pSkinSb->GetIdealSize();
}

//////////////////////////////////////////////////////////////////////////
SScrollView::SScrollView():m_bAutoViewSize(FALSE)
{
    m_bClipClient=TRUE;
    GetEventSet()->addEvent(EVENTID(EventScrollViewOriginChanged));
    GetEventSet()->addEvent(EVENTID(EventScrollViewSizeChanged));
    GetEventSet()->addEvent(EVENTID(EventScroll));
}


void SScrollView::OnSize(UINT nType,CSize size)
{
    __super::OnSize(nType,size);
    if(m_bAutoViewSize)
    {//计算viewSize
        CSize szOld = m_szView;
		m_szView = GetLayout()->MeasureChildren(this,size.cx,size.cy);
        if(szOld != m_szView)
        {
            OnViewSizeChanged(szOld,m_szView);
        }
    }
    UpdateScrollBar();
}

void SScrollView::OnViewOriginChanged( CPoint ptOld,CPoint ptNew )
{
    UpdateChildrenPosition();
    EventScrollViewOriginChanged evt(this);
    evt.ptOldOrigin = ptOld;
    evt.ptNewOrigin = ptNew;
    FireEvent(evt);
}

void SScrollView::OnViewSizeChanged( CSize szOld,CSize szNew )
{
    EventScrollViewSizeChanged evt(this);
    evt.szOldViewSize = szOld;
    evt.szNewViewSize = szNew;
    FireEvent(evt);
}

void SScrollView::SetViewOrigin(CPoint pt)
{
    if(m_ptOrigin==pt) return;
    CPoint ptOld=m_ptOrigin;
    m_ptOrigin=pt;
    SetScrollPos(FALSE,m_ptOrigin.x,TRUE);
    SetScrollPos(TRUE,m_ptOrigin.y,TRUE);

	m_layoutDirty = dirty_self;
    OnViewOriginChanged(ptOld,pt);

    Invalidate();
}

CPoint SScrollView::GetViewOrigin()
{
    return m_ptOrigin;
}


void SScrollView::SetViewSize(CSize szView)
{
    if(szView == m_szView) return;
    
    CSize oldViewSize=m_szView;
    m_szView=szView;
    UpdateScrollBar();
    OnViewSizeChanged(oldViewSize,szView);
}

CSize SScrollView::GetViewSize()
{
    return m_szView;
}

void SScrollView::UpdateScrollBar()
{
    CRect rcClient;
    SWindow::GetClientRect(&rcClient);

    CSize size=rcClient.Size();
    m_wBarVisible=SSB_NULL;    //关闭滚动条
	CPoint ptOrigin = m_ptOrigin;//backup

    if(size.cy<m_szView.cy || (size.cy<m_szView.cy+GetSbWidth() && size.cx<m_szView.cx))
    {
        //需要纵向滚动条
        m_wBarVisible|=SSB_VERT;
        m_siVer.nMin=0;
        m_siVer.nMax=m_szView.cy-1;
        m_siVer.nPage=size.cy;
		if(m_siVer.nPos + (int)m_siVer.nPage > m_siVer.nMax)
		{
			m_siVer.nPos = m_siVer.nMax - m_siVer.nPage;
			m_ptOrigin.y = m_siVer.nPos;
		}
        if(size.cx<m_szView.cx+GetSbWidth())
        {
            //需要横向滚动条
            m_wBarVisible |= SSB_HORZ;
            m_siVer.nPage=size.cy-GetSbWidth() > 0 ? size.cy-GetSbWidth() : 0;

            m_siHoz.nMin=0;
            m_siHoz.nMax=m_szView.cx-1;
            m_siHoz.nPage=size.cx-GetSbWidth() > 0 ? size.cx-GetSbWidth() : 0;
			if(m_siHoz.nPos + (int)m_siHoz.nPage > m_siHoz.nMax)
			{
				m_siHoz.nPos = m_siHoz.nMax - m_siHoz.nPage;
				m_ptOrigin.x = m_siHoz.nPos;
			}
        }
        else
        {
            //不需要横向滚动条
            m_siHoz.nPage=size.cx;
            m_siHoz.nMin=0;
            m_siHoz.nMax=m_siHoz.nPage-1;
            m_siHoz.nPos=0;
            m_ptOrigin.x=0;
        }
    }
    else
    {
        //不需要纵向滚动条
        m_siVer.nPage=size.cy;
        m_siVer.nMin=0;
        m_siVer.nMax=size.cy-1;
        m_siVer.nPos=0;
        m_ptOrigin.y=0;

        if(size.cx<m_szView.cx)
        {
            //需要横向滚动条
            m_wBarVisible|=SSB_HORZ;
            m_siHoz.nMin=0;
            m_siHoz.nMax=m_szView.cx-1;
            m_siHoz.nPage=size.cx;
			if(m_siHoz.nPos + (int)m_siHoz.nPage > m_siHoz.nMax)
			{
				m_siHoz.nPos = m_siHoz.nMax - m_siHoz.nPage;
				m_ptOrigin.x = m_siHoz.nPos;
			}
        }
        //不需要横向滚动条
        else
        {
            m_siHoz.nPage=size.cx;
            m_siHoz.nMin=0;
            m_siHoz.nMax=m_siHoz.nPage-1;
            m_siHoz.nPos=0;
            m_ptOrigin.x=0;
        }
    }

    SetScrollPos(TRUE,m_siVer.nPos,TRUE);
    SetScrollPos(FALSE,m_siHoz.nPos,TRUE);

    SSendMessage(WM_NCCALCSIZE);

	if(m_ptOrigin  != ptOrigin)
	{
		OnViewOriginChanged(ptOrigin,m_ptOrigin);
	}
    Invalidate();
}

BOOL SScrollView::OnScroll(BOOL bVertical,UINT uCode,int nPos)
{
    BOOL bRet=__super::OnScroll(bVertical,uCode,nPos);
    if(bRet)
    {
        int nPos=GetScrollPos(bVertical);
        CPoint ptOrigin=m_ptOrigin;

        if(bVertical) ptOrigin.y=nPos;
        else ptOrigin.x=nPos;

        if(ptOrigin!=m_ptOrigin)
            SetViewOrigin(ptOrigin);

        if(uCode==SB_THUMBTRACK)
            ScrollUpdate();
            
        EventScroll evt(this);
        evt.bVertical = bVertical;
        evt.uSbCode = uCode;
        evt.nPos = nPos;
        FireEvent(evt);
    }
    return bRet;
}

HRESULT SScrollView::OnAttrViewSize(const SStringW & strValue,BOOL bLoading)
{
    CSize szView;
    swscanf(strValue,L"%d,%d",&szView.cx,&szView.cy);
    
    m_bAutoViewSize = (szView.cx<=0 || szView.cy<=0) ;
    
    if(m_bAutoViewSize)
    {
        if(!bLoading)
        {
            CRect rcClient = SWindow::GetClientRect();
            OnSize(0,rcClient.Size());
        }
    }else 
    {
        if(bLoading)
        {
            m_szView = szView;
        }else
        {
            SetViewSize(szView);
        }
    }
    return S_FALSE;
}


CRect SScrollView::GetChildrenLayoutRect()
{
	CRect rcRet=__super::GetChildrenLayoutRect();
	rcRet.OffsetRect(-m_ptOrigin);
	rcRet.right=rcRet.left+m_szView.cx;
	rcRet.bottom=rcRet.top+m_szView.cy;
	return rcRet;
}

void SScrollView::UpdateChildrenPosition()
{
	if(!m_bAutoViewSize)
	{
		__super::UpdateChildrenPosition();
	}
	else{//计算viewSize
		CSize szOld = m_szView;
		CRect rcWnd = GetClientRect();
		CRect rcMargin = GetStyle().GetMargin();
		rcWnd.DeflateRect(rcMargin);
		rcWnd.DeflateRect(GetStyle().GetPadding());
		m_szView = GetLayout()->MeasureChildren(this,rcWnd.Width(),rcWnd.Height());
		m_szView.cx += rcMargin.left + rcMargin.right;
		m_szView.cy += rcMargin.top + rcMargin.bottom;

		__super::UpdateChildrenPosition();
		UpdateScrollBar();

		if(szOld != m_szView)
		{
			OnViewSizeChanged(szOld,m_szView);
		}
	}
}

}//namespace SOUI