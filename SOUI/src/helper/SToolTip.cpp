#include "souistd.h"
#include "helper\SToolTip.h"
//增加对多显示器的支持
#define  COMPILE_MULTIMON_STUBS
#include <multimon.h>

namespace SOUI
{

    #define TIMERID_DELAY    1
    #define TIMERID_SPAN     2

    #define MARGIN_TIP        5

    STipCtrl::STipCtrl(void):m_nDelay(500),m_nShowSpan(5000),m_font(0)
    {
        m_id.dwHi = m_id.dwLow = 0;
    }

    STipCtrl::~STipCtrl(void)
    {
        if(m_font) DeleteObject(m_font);
    }

    BOOL STipCtrl::Create()
    {
        HWND hWnd=CSimpleWnd::Create(_T("soui tooltip"),WS_POPUP|TTS_NOPREFIX|TTS_ALWAYSTIP,WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_NOACTIVATE,0,0,0,0,NULL,NULL);
        if(!hWnd) return FALSE;

        LOGFONT lf;
        GetObject(GetStockObject(DEFAULT_GUI_FONT),sizeof(lf),&lf);
        lf.lfHeight=-12;
        _tcscpy(lf.lfFaceName, _T("宋体"));
        m_font = CreateFontIndirect(&lf);

        return TRUE;
    }

    void STipCtrl::RelayEvent( const MSG *pMsg )
    {
        switch(pMsg->message)
        {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_MBUTTONDOWN:
            OnTimer(TIMERID_SPAN);//hide tip
            break;
        case WM_MOUSEMOVE:
            {
                CPoint pt(GET_X_LPARAM(pMsg->lParam),GET_Y_LPARAM(pMsg->lParam));
                if(!m_rcTarget.PtInRect(pt))
                {
                    OnTimer(TIMERID_SPAN);//hide tip
                }
                else if(!IsWindowVisible() && !m_strTip.IsEmpty())
                {
                    KillTimer(TIMERID_DELAY);
                    SetTimer(TIMERID_DELAY,m_nDelay);
                    ::ClientToScreen(pMsg->hwnd,&pt);
                    SetWindowPos(0,pt.x,pt.y+24,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOSENDCHANGING|SWP_NOACTIVATE);
                }
            }
            break;
        }
    }

    void STipCtrl::UpdateTip(const TIPID &id, CRect rc,LPCTSTR pszTip, int nScale)
    {
        if(m_id.dwHi == id.dwHi && m_id.dwLow== id.dwLow) return;

        m_id = id;
        m_rcTarget=rc;
        m_strTip=pszTip;
        if (m_nScale != nScale)
        {
            if (m_font) DeleteObject(m_font);
            LOGFONT lf;
            GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
            lf.lfHeight = -12 * nScale / 100;
            _tcscpy(lf.lfFaceName, _T("宋体"));
            m_font = CreateFontIndirect(&lf);
            m_nScale = nScale;
        }
        m_strTip.Replace(_T("\\n"),_T("\n"));

        if(IsWindowVisible())
        {
            ShowTip(TRUE);
        }
    }

    void STipCtrl::ClearTip()
    {
        m_id.dwHi = m_id.dwLow =0;
        ShowTip(FALSE);
    }

    void STipCtrl::ShowTip(BOOL bShow)
    {
        if(!bShow)
        {
            ShowWindow(SW_HIDE);
            m_rcTarget.SetRect(0,0,0,0);
            m_strTip=_T("");
        }
        else if(!m_strTip.IsEmpty())
        {
            HDC hdc=::GetDC(NULL);
            CRect rcText(0,0,500,1000);
            HFONT oldFont=(HFONT)SelectObject(hdc,m_font);
            DrawText(hdc,m_strTip,-1,&rcText,DT_CALCRECT|DT_LEFT|DT_WORDBREAK);
            SelectObject(hdc,oldFont);
            ::ReleaseDC(NULL,hdc);
            CRect rcWnd;
            GetWindowRect(&rcWnd);

			//增加对多显示器的支持			
			HMONITOR hMonitor;
			MONITORINFO mi;
			hMonitor = MonitorFromRect(&rcWnd, MONITOR_DEFAULTTONEAREST);	 
			mi.cbSize = sizeof(mi);
			GetMonitorInfo(hMonitor, &mi);	
			int cx=mi.rcMonitor.right;
			int cy=mi.rcMonitor.bottom;
			/*增加部分结束*/


            rcWnd.right=rcWnd.left+rcText.right+2*MARGIN_TIP;
            rcWnd.bottom=rcWnd.top+rcText.bottom+2*MARGIN_TIP;
			//去掉下面这两行
            if(rcWnd.right>cx) rcWnd.OffsetRect(cx-rcWnd.right,0);
            if(rcWnd.bottom>cy) rcWnd.OffsetRect(0,cy-rcWnd.bottom);
            SetWindowPos(HWND_TOPMOST,rcWnd.left,rcWnd.top,rcWnd.Width(),rcWnd.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE| SWP_NOOWNERZORDER);
        }
    }

    void STipCtrl::OnTimer( UINT_PTR idEvent )
    {
        switch(idEvent)
        {
        case TIMERID_DELAY:
            KillTimer(TIMERID_DELAY);       
            ShowTip(TRUE);
            SetTimer(TIMERID_SPAN,m_nShowSpan);
            break;
        case TIMERID_SPAN:
            ShowTip(FALSE);
            KillTimer(TIMERID_SPAN);
            break;
        }
    }

    void STipCtrl::OnPaint( HDC dc )
    {
        PAINTSTRUCT ps;
        dc=::BeginPaint(m_hWnd, &ps);

        CRect rc;
        GetClientRect(&rc);
        HBRUSH br=CreateSolidBrush(GetSysColor(COLOR_INFOBK));
        HGDIOBJ hOld=SelectObject(dc,br);
        Rectangle(dc,rc.left,rc.top,rc.right,rc.bottom);
        SelectObject(dc,hOld);
        DeleteObject(br);

        rc.DeflateRect(MARGIN_TIP,MARGIN_TIP);
        SetBkMode(dc,TRANSPARENT);
        HGDIOBJ hOldFont=SelectObject(dc,m_font);
        ::DrawText(dc,m_strTip,-1,&rc,DT_WORDBREAK);
        SelectObject(dc,hOldFont);

        ::EndPaint(m_hWnd, &ps);
    }

    BOOL STipCtrl::PreTranslateMessage( MSG* pMsg )
    {
        if(IsWindow()) RelayEvent(pMsg);
        return FALSE;
    }

    void STipCtrl::OnFinalMessage( HWND hWnd )
    {
        CSimpleWnd::OnFinalMessage(hWnd);
        delete this;
    }
}//namespace SOUI