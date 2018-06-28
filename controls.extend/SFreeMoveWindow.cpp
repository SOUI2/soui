#include "stdafx.h"
#include "SFreeMoveWindow.h"

namespace SOUI
{
    SFreeMoveWindow::SFreeMoveWindow(void)
    :m_nFloatHAlign(HALIGN_RIGHT),m_nFloatVAlign(VALIGN_BOTTOM)
    ,m_nDistX(10),m_nDistY(10)
    ,m_bDraging(FALSE)
    {
        m_bFloat = TRUE;
    }

    SFreeMoveWindow::~SFreeMoveWindow(void)
    {
    }

    void SFreeMoveWindow::OnUpdateFloatPosition(const CRect & rcParent)
    {
        CSize sz(GetLayoutParam()->GetSpecifiedSize(Horz).toPixelSize(GetScale()),GetLayoutParam()->GetSpecifiedSize(Vert).toPixelSize(GetScale()));
        CRect rcWnd(CPoint(),sz);
        
        if(m_nFloatHAlign & HALIGN_LEFT)
        {
            rcWnd.MoveToX(rcParent.left+m_nDistX);
        }else if(m_nFloatHAlign & HALIGN_RIGHT)
        {
            rcWnd.MoveToX(rcParent.right - rcWnd.Width() - m_nDistX);
        }
        if(m_nFloatVAlign & VALIGN_TOP)
        {
            rcWnd.MoveToY(rcParent.top + m_nDistY);
        }else if(m_nFloatVAlign & VALIGN_BOTTOM)
        {
            rcWnd.MoveToY(rcParent.bottom - rcWnd.Height() -m_nDistY);
        }
        
        Move(rcWnd);
    }

    
    void SFreeMoveWindow::OnLButtonDown(UINT nFlags,CPoint pt)
    {
        SetCapture();
        m_bDraging = TRUE;
        m_ptClick = pt - GetWindowRect().TopLeft();
        ::SetCursor(GETRESPROVIDER->LoadCursor(_T("sizeall")));
    }

    void SFreeMoveWindow::OnLButtonUp(UINT nFlags,CPoint pt)
    {
        ReleaseCapture();
        m_bDraging = FALSE;
    }

    void SFreeMoveWindow::OnMouseMove(UINT nFlags,CPoint pt)
    {
        if(m_bDraging)
        {
            CPoint ptLT = pt - m_ptClick;
            CRect rcWnd = GetWindowRect();
            rcWnd.MoveToXY(ptLT);
            
            Move(rcWnd);
            
            CRect rcParent = GetParent()->GetClientRect();

            CPoint ptCenterWnd = rcWnd.CenterPoint();
            CPoint ptCenterParent = rcParent.CenterPoint();
            
            if(ptCenterWnd.x<ptCenterParent.x)
                m_nFloatHAlign = HALIGN_LEFT;
            else
                m_nFloatHAlign = HALIGN_RIGHT;
            
            if(ptCenterWnd.y < ptCenterParent.y)
                m_nFloatVAlign = VALIGN_TOP;
            else
                m_nFloatVAlign =VALIGN_BOTTOM;
                
            if(m_nFloatHAlign & HALIGN_LEFT)
            {
                m_nDistX = rcWnd.left - rcParent.left;
            }else if(m_nFloatHAlign & HALIGN_RIGHT)
            {
                m_nDistX = rcParent.right - rcWnd.right;
            }
            if(m_nFloatVAlign & VALIGN_TOP)
            {
                m_nDistY = rcWnd.top -rcParent.top;
            }else if(m_nFloatVAlign & VALIGN_BOTTOM)
            {
                m_nDistY = rcParent.bottom - rcWnd.bottom;
            }
        }
    }

}
