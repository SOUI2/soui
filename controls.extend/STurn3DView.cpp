//////////////////////////////////////////////////////////////////////////
//   File Name: STurn3DView.h
// Description: STurn3DView
//     Creator: ZhangZhiBin QQ->276883782
//     Version: 2014.02.06 - 1.0 - Create
//              2012.08.18 - 2.0 huangjianxiong
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "STurn3DView.h"

namespace SOUI
{
    STurn3dView::STurn3dView()
        : m_bTurn2Front(FALSE)
        , m_nZStep(50)
        , m_nYStep(10)
        , m_bTurning(FALSE)
    {
        m_bVisible=FALSE;
        memset(&m_3dparam,0,sizeof(m_3dparam));
        m_evtSet.addEvent(EVENTID(EventTurn3d));
    }

    STurn3dView::~STurn3dView()
    {
    }

    void STurn3dView::OnTimer(char idEvent)
    {
        int n = m_bTurn2Front?-1:1;

        int nMaxBeforeFrame, nMaxAfterFrame;
        nMaxBeforeFrame = 90 / m_nYStep;
        nMaxAfterFrame = 180 / m_nYStep;

        if(m_nFrameIndex >= nMaxAfterFrame)
        {//turn over finished
            KillTimer(idEvent);  
            GetWindow(GSW_PREVSIBLING)->SetVisible(TRUE,FALSE);
            SetVisible(FALSE,TRUE);//隐藏窗口
            m_bTurning = FALSE;
            m_bmpBefore = NULL;
            m_bmpAfter = NULL;
            m_bmpTrans = NULL;
            EventTurn3d evt(this,m_bTurn2Front);
            FireEvent(evt);
            return;      
        }
        
        IBitmap *pBmp;
        if(m_nFrameIndex <= nMaxBeforeFrame)
        {
            m_3dparam.nOffsetZ = m_nZStep * m_nFrameIndex;
            m_3dparam.nRotateY = n * m_nYStep * m_nFrameIndex;
            pBmp = m_bmpBefore;
        }
        else// if(m_nFrameIndex < nMaxAfterFrame)
        {
            m_3dparam.nOffsetZ = m_nZStep * (nMaxAfterFrame - m_nFrameIndex);
            m_3dparam.nRotateY = n * m_nYStep * (m_nFrameIndex - nMaxAfterFrame);
            pBmp = m_bmpAfter;
        }
        //render transform image
        IMAGE3D::C3DTransform image3d;
        SIZE szImg = pBmp->Size();
        LPVOID pBitSrc = pBmp->LockPixelBits();
        LPVOID pBitDst = m_bmpTrans->LockPixelBits();
        image3d.SetImage((LPBYTE)pBitSrc,(LPBYTE)pBitDst,szImg.cx,szImg.cy,32);
        image3d.Render(m_3dparam);
        
        pBmp->UnlockPixelBits(pBitSrc);
        m_bmpTrans->UnlockPixelBits(pBitDst);
        
        m_nFrameIndex++;
        Invalidate();
    }

    BOOL STurn3dView::Turn(SWindow *pWndFront,SWindow *pWndBack,BOOL bTurn2Front/*=TRUE*/)
    {
        SASSERT(pWndBack && pWndFront);
        if(m_bTurning) return FALSE;
        
        CRect rcWnd;
        GetWindowRect(&rcWnd);

        //复制正面窗体的图像
        SWindow *pFrmWnd = GetWindow(GSW_PREVSIBLING);
        if(!pFrmWnd) return FALSE;
                
        
        IRenderTarget *pRT=pFrmWnd->GetRenderTarget(rcWnd,OLEDC_NODRAW,FALSE);
        pWndFront->SetVisible(TRUE,FALSE);
        pWndBack->SetVisible(FALSE,FALSE);
        
        //渲染窗口变化前状态
        pRT->ClearRect(&rcWnd,0);
        pFrmWnd->RedrawRegion(pRT,NULL);
        
        CAutoRefPtr<IRenderTarget> pRTCopy1;
        GETRENDERFACTORY->CreateRenderTarget(&pRTCopy1,rcWnd.Width(),rcWnd.Height());

        pRTCopy1->BitBlt(CRect(CPoint(0,0),rcWnd.Size()),pRT,rcWnd.left,rcWnd.top,SRCCOPY);
        m_bmpBefore = (IBitmap*)pRTCopy1->GetCurrentObject(OT_BITMAP);        
        
        //渲染窗口变化后状态
        pWndFront->SetVisible(FALSE,FALSE);
        pWndBack->SetVisible(TRUE,FALSE);

        pRT->ClearRect(&rcWnd,0);
        pFrmWnd->RedrawRegion(pRT,NULL);
        
        CAutoRefPtr<IRenderTarget> pRTCopy2;
        GETRENDERFACTORY->CreateRenderTarget(&pRTCopy2,rcWnd.Width(),rcWnd.Height());
        pRTCopy2->BitBlt(CRect(CPoint(0,0),rcWnd.Size()),pRT,rcWnd.left,rcWnd.top,SRCCOPY);
        m_bmpAfter = (IBitmap*)pRTCopy2->GetCurrentObject(OT_BITMAP);        
        
        pFrmWnd->ReleaseRenderTarget(pRT);

        
        //先把FrameWindow HIDE
        pFrmWnd->SetVisible(FALSE,FALSE);

        GETRENDERFACTORY->CreateBitmap(&m_bmpTrans);
        m_bmpTrans->Init(rcWnd.Width(),rcWnd.Height());
        
        //让窗口可见
        SetVisible(TRUE,TRUE);
        m_3dparam.nOffsetZ = m_nZStep ;
        m_3dparam.nRotateY = -10;

        m_nFrameIndex = 0;
        m_bTurn2Front = bTurn2Front;
        
        m_bTurning = TRUE;
        SetTimer(1, 20);
        OnTimer(1);
        return TRUE;
    }

    void STurn3dView::OnPaint( IRenderTarget *pRT )
    {
        CRect rcWnd;
        GetWindowRect(&rcWnd);
        pRT->DrawBitmap(&rcWnd,m_bmpTrans,0,0);
    }

}