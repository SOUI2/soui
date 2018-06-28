#include "stdafx.h"
#include "MagnetFrame.h"

using namespace SOUI;

const int KDistMin = 10;    //小于该值代表两个窗口自动吸附


CMagnetFrame::CMagnetFrame(void):m_hMainWnd(NULL)
{
}

CMagnetFrame::~CMagnetFrame(void)
{
}

BOOL CMagnetFrame::SubclassWnd(HWND hWnd)
{
    if(GetWindowLongPtr(hWnd,GWLP_USERDATA)!=0)
        return FALSE;//在吸附框架中的窗口不能使用了USERDATA，因为这个值要被吸附框架占用.
    WNDPROC pWndProc = (WNDPROC)GetWindowLongPtr(hWnd,GWLP_WNDPROC);
    if(pWndProc == &CMagnetFrame::MagnetWndProc) return FALSE;
    
    SetWindowLongPtr(hWnd,GWLP_USERDATA,(ULONG_PTR)this);
    SetWindowLongPtr(hWnd,GWLP_WNDPROC,(ULONG_PTR)&CMagnetFrame::MagnetWndProc);
    WndData wd={pWndProc,{AM_NULL,AA_NULL,0},0};
    m_mapOldProc[hWnd] = wd;
    return TRUE;
}

BOOL CMagnetFrame::UnsubclassWnd(HWND hWnd)
{
    SASSERT(m_mapOldProc.Lookup(hWnd));
    WNDPROC pWndProc = m_mapOldProc[hWnd].pProc;
    m_mapOldProc.RemoveKey(hWnd);
    SetWindowLongPtr(hWnd,GWLP_WNDPROC,(ULONG_PTR)pWndProc);
    SetWindowLongPtr(hWnd,GWLP_USERDATA,0);
    return TRUE;
}

bool IsNeighbour(int v1,int v2)
{
    return abs(v1-v2) < KDistMin;
}

int CalcDistance(int v1,int v2)
{
    int nDist = v2-v1;
    if(abs(nDist) < KDistMin) nDist =0;
    return nDist;
}

CMagnetFrame::AtatchInfo CMagnetFrame::CalcAttachMode(HWND hSubWnd,CRect rcSub)
{
    SASSERT(m_hMainWnd);
    CRect rcMain;
    ::GetWindowRect(m_hMainWnd,&rcMain);
    
    if(rcSub.IsRectEmpty()) ::GetWindowRect(hSubWnd,&rcSub);
    
    AtatchInfo ai = {AM_NULL,AA_NULL,0};
    
    if(IsNeighbour(rcMain.left,rcSub.right))
    {
        ai.am = AM_LEFT;
        int nDist = CalcDistance(rcMain.bottom,rcSub.bottom);
        if(nDist == 0) ai.aa = AA_BOTTOM;
        else if(0 == (nDist = CalcDistance(rcMain.top,rcSub.top)))
            ai.aa = AA_TOP;
        else
            ai.aa = AA_NULL,ai.nDist = nDist;
    }
    else if(IsNeighbour(rcMain.right,rcSub.left))
    {
        ai.am = AM_RIGHT;
        int nDist = CalcDistance(rcMain.bottom,rcSub.bottom);
        if(nDist == 0) ai.aa = AA_BOTTOM;
        else if(0 == (nDist = CalcDistance(rcMain.top,rcSub.top)))
            ai.aa = AA_TOP;
        else
            ai.aa = AA_NULL,ai.nDist = nDist;
    }else if(IsNeighbour(rcMain.top,rcSub.bottom))
    {
        ai.am = AM_TOP;
        int nDist = CalcDistance(rcMain.right,rcSub.right);
        if(nDist == 0) ai.aa = AA_RIGHT;
        else if(0 == (nDist = CalcDistance(rcMain.left,rcSub.left)))
            ai.aa = AA_LEFT;
        else
            ai.aa = AA_NULL,ai.nDist = nDist;
    }else if(IsNeighbour(rcMain.bottom,rcSub.top))
    {
        ai.am = AM_BOTTOM;
        int nDist = CalcDistance(rcMain.right,rcSub.right);
        if(nDist == 0) ai.aa = AA_RIGHT;
        else if(0 == (nDist = CalcDistance(rcMain.left,rcSub.left)))
            ai.aa = AA_LEFT;
        else
            ai.aa = AA_NULL,ai.nDist = nDist;
    }
        
    return ai;
}

void CMagnetFrame::UpdateSubWndPos(HWND hSubWnd,AtatchInfo ai)
{
    CRect rcSub,rcMain;
    ::GetWindowRect(m_hMainWnd,&rcMain);
    ::GetWindowRect(hSubWnd,&rcSub);
    switch(ai.am)
    {
    case AM_RIGHT:
        {
            int y;
            switch(ai.aa)
            {
            case AA_TOP:
                y=rcMain.top;
                break;
            case AA_BOTTOM:
                y=rcMain.bottom - rcSub.Height();
                break;
            default:
                y=rcMain.top + ai.nDist;
                break;
            }
            ::MoveWindow(hSubWnd,rcMain.right,y,rcSub.Width(),rcSub.Height(),TRUE);
        }
        break;
    case AM_LEFT:
        {
            int y;
            switch(ai.aa)
            {
            case AA_TOP:
                y=rcMain.top;
                break;
            case AA_BOTTOM:
                y=rcMain.bottom - rcSub.Height();
                break;
            default:
                y=rcMain.top + ai.nDist;
                break;
            }

            ::MoveWindow(hSubWnd,rcMain.left - rcSub.Width(),y,rcSub.Width(),rcSub.Height(),TRUE);
        }
        break;
    case AM_TOP:
        {
            int x;
            switch(ai.aa)
            {
            case AA_LEFT:
                x=rcMain.left;
                break;
            case AA_RIGHT:
                x=rcMain.right - rcSub.Width();
                break;
            default:
                x=rcMain.left + ai.nDist;
                break;
            }

            ::MoveWindow(hSubWnd,x,rcMain.top - rcSub.Height(),rcSub.Width(),rcSub.Height(),TRUE);
        }
        break;
    case AM_BOTTOM:
        {
            int x;
            switch(ai.aa)
            {
            case AA_LEFT:
                x=rcMain.left;
                break;
            case AA_RIGHT:
                x=rcMain.right - rcSub.Width();
                break;
            default:
                x=rcMain.left + ai.nDist;
                break;
            }

            ::MoveWindow(hSubWnd,x,rcMain.bottom,rcSub.Width(),rcSub.Height(),TRUE);
        }
        break;
    }
}

LRESULT CALLBACK CMagnetFrame::MagnetWndProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp)
{
    CMagnetFrame *pThis = (CMagnetFrame*)GetWindowLongPtr(hWnd,GWLP_USERDATA);
    
    SASSERT(pThis->m_mapOldProc.Lookup(hWnd));
    
    WndData & wd = pThis->m_mapOldProc[hWnd];
    
    if(uMsg == WM_MOVE || uMsg == WM_SIZE)
    {
        if(hWnd == pThis->m_hMainWnd)
        {//主窗口移动,移动所有和主窗口有吸附关系的附属窗口
            CRect rcMain;
            ::GetWindowRect(pThis->m_hMainWnd,&rcMain);
            
            SPOSITION pos = pThis->m_mapOldProc.GetStartPosition();
            while(pos)
            {
                WNDDATAMAP::CPair * pData = pThis->m_mapOldProc.GetNext(pos);
                if(pData->m_key == hWnd) continue;
                
                WndData & subwd = pData->m_value;
                if(subwd.ai.am!=AM_NULL)
                {
                    subwd.bFromThis = TRUE;
                    pThis->UpdateSubWndPos(pData->m_key,subwd.ai);
                    subwd.bFromThis = FALSE;
                }
            }
            
        }else if(!wd.bFromThis)
        {//附属窗口移动,更新与主窗口的吸附关系
             AtatchInfo ai = pThis->CalcAttachMode(hWnd);
             //自动吸附
             if(ai.am != AM_NULL)
             {
                wd.bFromThis = TRUE;
                pThis->UpdateSubWndPos(hWnd,ai);
                wd.bFromThis = FALSE;
             }
             wd.ai = ai;
        }
    }
    else if(uMsg == WM_WINDOWPOSCHANGING && ! wd.bFromThis && hWnd != pThis->m_hMainWnd)
    {//窗口移动前，检测窗口吸附类型，如果还在吸附范围内，则不移动窗口
        WINDOWPOS * pWndPos = (WINDOWPOS*)lp;
        if(!(pWndPos->flags & SWP_NOMOVE))
        {
            CRect rcMain,rcSub(CPoint(pWndPos->x,pWndPos->y),CSize(pWndPos->cx,pWndPos->cy));
            AtatchInfo ai = pThis->CalcAttachMode(hWnd,rcSub);
            ::GetWindowRect(pThis->m_hMainWnd,&rcMain);
            if(ai.am != AM_NULL)
            {
                switch(ai.am)
                {
                case AM_RIGHT:
                pWndPos->x = rcMain.right;
                break;
                case AM_LEFT:
                pWndPos->x = rcMain.left - pWndPos->cx;
                break;
                case AM_TOP:
                pWndPos->y = rcMain.top - pWndPos->cy;
                    break;
                case AM_BOTTOM:
                pWndPos->y = rcMain.bottom;
                    break;
                }
                
                switch(ai.am)
                {
                case AM_LEFT:case AM_RIGHT:
                    {
                        switch(ai.aa)
                        {
                        case AA_TOP:
                            pWndPos->y = rcMain.top;
                            break;
                        case AA_BOTTOM:
                            pWndPos->y = rcMain.bottom - rcSub.Height();
                            break;
                        }
                    }
                    break;
                case AM_TOP:case AM_BOTTOM:
                    {
                        switch(ai.aa)
                        {
                        case AA_LEFT:
                            pWndPos->x = rcMain.left;
                            break;
                        case AA_RIGHT:
                            pWndPos->x = rcMain.right - rcSub.Width();
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }
    return wd.pProc(hWnd,uMsg,wp,lp);
}


BOOL CMagnetFrame::RemoveSubWnd(HWND hWnd)
{
    SASSERT(m_hMainWnd);
    SASSERT(hWnd);
    if(!m_mapOldProc.Lookup(hWnd)) return FALSE;
    UnsubclassWnd(hWnd);
    return TRUE;
}

BOOL CMagnetFrame::AddSubWnd(HWND hWnd,ATTACHMODE am,ATTACHALIGN aa,int nDist/* = 0*/)
{
    SASSERT(m_hMainWnd);
    SASSERT(hWnd != m_hMainWnd);
    if(m_mapOldProc.Lookup(hWnd)) return FALSE;
    SubclassWnd(hWnd);
    
    if(am != AM_NULL)
    {
        AtatchInfo ai = {am,aa,nDist};
        WndData wd = m_mapOldProc[hWnd];
        wd.ai = ai;
        wd.bFromThis = TRUE;
        UpdateSubWndPos(hWnd,ai);
        wd.bFromThis = FALSE;
    }
    return TRUE;
}

void CMagnetFrame::SetMainWnd(HWND hWnd)
{
    SASSERT(!m_hMainWnd && hWnd);
    m_hMainWnd = hWnd;
    SubclassWnd(hWnd);
}

