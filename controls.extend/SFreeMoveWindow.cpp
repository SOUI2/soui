#include "stdafx.h"
#include "SFreeMoveWindow.h"

namespace SOUI
{
    SFreeMoveWindow::SFreeMoveWindow(void)
    :m_nFloatHAlign(HALIGN_RIGHT),m_nFloatVAlign(VALIGN_BOTTOM)
    ,m_nDistX(10),m_nDistY(10)
    ,m_bDraging(FALSE)
	, m_bResizable(TRUE)
		, m_dwHit(0)
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

	DWORD SFreeMoveWindow::HitTest(CPoint pt)
	{
		CRect rcWnd = GetWindowRect();
		CRect rcClient = GetClientRect();
		DWORD dwHit = HT_CAPTION;
		if (m_bResizable)
		{
			if (pt.x <= rcClient.left)
				dwHit |= HT_LEFT;
			else if (pt.x >= rcClient.right)
				dwHit |= HT_RIGHT;
			if (pt.y <= rcClient.top)
			{
				if (pt.y > rcWnd.top + GetStyle().GetMargin().bottom)
				{
					if(dwHit== 0) dwHit = HT_CAPTION;
				}
				else
				{
					dwHit |= HT_TOP;
				}
			}
			else if (pt.y >= rcClient.bottom)
			{
				dwHit |= HT_BOTTOM;
			}
		}
		return dwHit;
	}

	void SFreeMoveWindow::SetCursorWrapper(DWORD dwHit)
	{
		switch (dwHit)
		{
		case HT_CAPTION:
			::SetCursor(GETRESPROVIDER->LoadCursor(_T("sizeall")));
			break;
		case HT_LEFT:
		case HT_RIGHT:
			::SetCursor(GETRESPROVIDER->LoadCursor(_T("sizewe")));
			break;
		case HT_TOP:
		case HT_BOTTOM:
			::SetCursor(GETRESPROVIDER->LoadCursor(_T("sizens")));
			break;
		case HT_LEFT | HT_TOP:
		case HT_RIGHT | HT_BOTTOM:
			::SetCursor(GETRESPROVIDER->LoadCursor(_T("sizenwse")));
			break;
		case HT_LEFT | HT_BOTTOM:
		case HT_RIGHT | HT_TOP:
			::SetCursor(GETRESPROVIDER->LoadCursor(_T("sizenesw")));
			break;
		}
	}
	void SFreeMoveWindow::OnNcLButtonDown(UINT nFlags, CPoint pt)
	{
		if (m_bResizable)
		{
			SetCapture();
			m_bDraging = TRUE;
			m_ptClick = pt;
			m_rcClickWndPos = GetWindowRect();
			SetCursorWrapper(m_dwHit);
		}
		else
		{
			SetMsgHandled(FALSE);
		}
	}

	void SFreeMoveWindow::OnNcLButtonUp(UINT nFlags, CPoint pt)
	{
		if (m_bResizable)
		{
			m_bDraging = FALSE;
			ReleaseCapture();
		}
		else
		{
			SetMsgHandled(FALSE);
		}
	}

	void SFreeMoveWindow::OnNcMouseMove(UINT nFlags, CPoint pt)
	{
		if (!m_bDraging)
		{
			m_dwHit = HitTest(pt);
			SetCursorWrapper(m_dwHit);
		}
		else
		{
			CRect rcWnd = m_rcClickWndPos;
			switch (m_dwHit)
			{
			case HT_CAPTION:
				rcWnd.OffsetRect(pt - m_ptClick);
				break;
			case HT_LEFT:
				rcWnd.left += pt.x - m_ptClick.x;
				break;
			case HT_RIGHT:
				rcWnd.right += pt.x - m_ptClick.x;
				break;
			case HT_TOP:
				rcWnd.top += pt.y - m_ptClick.y;
				break;
			case HT_BOTTOM:
				rcWnd.bottom += pt.y - m_ptClick.y;
				break;
			case HT_LEFT|HT_TOP:
				rcWnd.left += pt.x - m_ptClick.x;
				rcWnd.top += pt.y - m_ptClick.y;
				break;
			case HT_RIGHT | HT_BOTTOM:
				rcWnd.right += pt.x - m_ptClick.x;
				rcWnd.bottom += pt.y - m_ptClick.y;
				break;
			case HT_LEFT|HT_BOTTOM:
				rcWnd.left += pt.x - m_ptClick.x;
				rcWnd.bottom += pt.y - m_ptClick.y;
				break;
			case HT_RIGHT | HT_TOP:
				rcWnd.right += pt.x - m_ptClick.x;
				rcWnd.top += pt.y - m_ptClick.y;
				break;
			}
			Move(rcWnd);

			CRect rcParent = GetParent()->GetClientRect();

			CPoint ptCenterWnd = rcWnd.CenterPoint();
			CPoint ptCenterParent = rcParent.CenterPoint();

			if (ptCenterWnd.x<ptCenterParent.x)
				m_nFloatHAlign = HALIGN_LEFT;
			else
				m_nFloatHAlign = HALIGN_RIGHT;

			if (ptCenterWnd.y < ptCenterParent.y)
				m_nFloatVAlign = VALIGN_TOP;
			else
				m_nFloatVAlign = VALIGN_BOTTOM;

			if (m_nFloatHAlign & HALIGN_LEFT)
			{
				m_nDistX = rcWnd.left - rcParent.left;
			}
			else if (m_nFloatHAlign & HALIGN_RIGHT)
			{
				m_nDistX = rcParent.right - rcWnd.right;
			}
			if (m_nFloatVAlign & VALIGN_TOP)
			{
				m_nDistY = rcWnd.top - rcParent.top;
			}
			else if (m_nFloatVAlign & VALIGN_BOTTOM)
			{
				m_nDistY = rcParent.bottom - rcWnd.bottom;
			}
			GetLayoutParam()->SetSpecifiedSize(Horz,SLayoutSize(rcWnd.Width(),SLayoutSize::px));
			GetLayoutParam()->SetSpecifiedSize(Vert,SLayoutSize(rcWnd.Height(),SLayoutSize::px));
		}
	}

	void SFreeMoveWindow::OnNcPaint(IRenderTarget * pRT)
	{
		__super::OnNcPaint(pRT);
		if (m_pCaptionSkin)
		{
			CRect rcWnd = GetWindowRect();
			CRect rcClient = GetClientRect();
			CRect rcMargin = GetStyle().GetMargin();
			rcMargin.top = rcMargin.bottom;
			rcWnd.DeflateRect(rcMargin);
			rcWnd.bottom = rcClient.top;
			m_pCaptionSkin->Draw(pRT, rcWnd, 0);
		}
	}

}
