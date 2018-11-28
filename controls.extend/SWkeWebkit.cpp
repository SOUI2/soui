#include "StdAfx.h"
#include "SWkeWebkit.h"
#include <Imm.h>
#pragma comment(lib,"imm32.lib")
#pragma comment(lib,"msimg32.lib")
namespace SOUI
{
    //////////////////////////////////////////////////////////////////////////
    // SWkeLoader
    SWkeLoader * SWkeLoader::s_pInst=0;
    
    SWkeLoader* SWkeLoader::GetInstance()
    {
        return s_pInst;
    }

    SWkeLoader::SWkeLoader() :m_hModWke(0)
    {
        SASSERT(!s_pInst);
        s_pInst=this;
    }


    SWkeLoader::~SWkeLoader()
    {
        if(m_hModWke) FreeLibrary(m_hModWke);
    }
    
    BOOL SWkeLoader::Init( LPCTSTR pszDll )
    {
        if(m_hModWke) return TRUE;
        HMODULE m_hModWke=LoadLibrary(pszDll);
        if(!m_hModWke) return FALSE;
        m_funWkeInit = (FunWkeInit)GetProcAddress(m_hModWke,"wkeInit");
        m_funWkeShutdown = (FunWkeShutdown)GetProcAddress(m_hModWke,"wkeShutdown");
        m_funWkeCreateWebView = (FunWkeCreateWebView)GetProcAddress(m_hModWke,"wkeCreateWebView");
        m_funWkeDestroyWebView = (FunWkeDestroyWebView)GetProcAddress(m_hModWke,"wkeDestroyWebView");
        if(!m_funWkeInit 
            || !m_funWkeShutdown
            || !m_funWkeCreateWebView
            || !m_funWkeDestroyWebView )
        {
            FreeLibrary(m_hModWke);
            return FALSE;
        }
        m_funWkeInit();
        return TRUE;
    }



    //////////////////////////////////////////////////////////////////////////
    // SWkeWebkit
    
    SWkeWebkit::SWkeWebkit(void):m_pWebView(NULL)
    {
		m_bFocusable = true;
    }

    SWkeWebkit::~SWkeWebkit(void)
    {
    }

    void SWkeWebkit::OnPaint(IRenderTarget *pRT)
    {
        CRect rcClip;
        pRT->GetClipBox(&rcClip);
        CRect rcClient;
        GetClientRect(&rcClient);
        CRect rcInvalid;
        rcInvalid.IntersectRect(&rcClip,&rcClient);
        HDC hdc=pRT->GetDC();
        if(GetStyle().m_byAlpha!=0xff)
        {
            BLENDFUNCTION bf={AC_SRC_OVER,0,GetStyle().m_byAlpha,AC_SRC_ALPHA };
            AlphaBlend(hdc,rcInvalid.left,rcInvalid.top,rcInvalid.Width(),rcInvalid.Height(),m_pWebView->getViewDC(),rcInvalid.left-rcClient.left,rcInvalid.top-rcClient.top,rcInvalid.Width(),rcInvalid.Height(),bf);
        }else
        {
            BitBlt(hdc,rcInvalid.left,rcInvalid.top,rcInvalid.Width(),rcInvalid.Height(),m_pWebView->getViewDC(),rcInvalid.left-rcClient.left,rcInvalid.top-rcClient.top,SRCCOPY);            
        }
        pRT->ReleaseDC(hdc);
    }

    void SWkeWebkit::OnSize( UINT nType, CSize size )
    {
        __super::OnSize(nType,size);
        m_pWebView->resize(size.cx,size.cy);
        m_pWebView->tick();
    }

    int SWkeWebkit::OnCreate( void * )
    {
        m_pWebView = SWkeLoader::GetInstance()->m_funWkeCreateWebView();
        if(!m_pWebView) return 1;
        m_pWebView->setBufHandler(this);
        m_pWebView->loadURL(m_strUrl);
        SetTimer(TM_TICKER,50); //由于timer不够及时，idle又限制了只在当前的消息循环中有效，使用timer和onidle一起更新浏览器
        return 0;
	}

	void SWkeWebkit::OnDestroy()
	{
		if(m_pWebView) SWkeLoader::GetInstance()->m_funWkeDestroyWebView(m_pWebView);
	}

	LRESULT SWkeWebkit::OnMouseEvent( UINT message, WPARAM wParam,LPARAM lParam)
	{
		if (message == WM_LBUTTONDOWN || message == WM_MBUTTONDOWN || message == WM_RBUTTONDOWN)
		{
			SetFocus();
			SetCapture();
		}
		else if (message == WM_LBUTTONUP || message == WM_MBUTTONUP || message == WM_RBUTTONUP)
		{
			ReleaseCapture();
		}

		CRect rcClient;
		GetClientRect(&rcClient);

		int x = GET_X_LPARAM(lParam)-rcClient.left;
		int y = GET_Y_LPARAM(lParam)-rcClient.top;

		unsigned int flags = 0;

		if (wParam & MK_CONTROL)
			flags |= WKE_CONTROL;
		if (wParam & MK_SHIFT)
			flags |= WKE_SHIFT;

		if (wParam & MK_LBUTTON)
			flags |= WKE_LBUTTON;
		if (wParam & MK_MBUTTON)
			flags |= WKE_MBUTTON;
		if (wParam & MK_RBUTTON)
			flags |= WKE_RBUTTON;

		bool bHandled = m_pWebView->mouseEvent(message, x, y, flags);
		SetMsgHandled(bHandled);
		return 0;
	}

	LRESULT SWkeWebkit::OnKeyDown( UINT uMsg, WPARAM wParam,LPARAM lParam )
	{
		unsigned int flags = 0;
		if (HIWORD(lParam) & KF_REPEAT)
			flags |= WKE_REPEAT;
		if (HIWORD(lParam) & KF_EXTENDED)
			flags |= WKE_EXTENDED;

		SetMsgHandled(m_pWebView->keyDown(wParam, flags, false));
		return 0;
	}

	LRESULT SWkeWebkit::OnKeyUp( UINT uMsg, WPARAM wParam,LPARAM lParam )
	{
		unsigned int flags = 0;
		if (HIWORD(lParam) & KF_REPEAT)
			flags |= WKE_REPEAT;
		if (HIWORD(lParam) & KF_EXTENDED)
			flags |= WKE_EXTENDED;

		SetMsgHandled(m_pWebView->keyUp(wParam, flags, false));
		return 0;
	}

	LRESULT SWkeWebkit::OnMouseWheel( UINT uMsg, WPARAM wParam,LPARAM lParam )
	{
		POINT pt;
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);

		CRect rc;
		GetWindowRect(&rc);
		pt.x -= rc.left;
		pt.y -= rc.top;

		int delta = GET_WHEEL_DELTA_WPARAM(wParam);

		unsigned int flags = 0;

		if (wParam & MK_CONTROL)
			flags |= WKE_CONTROL;
		if (wParam & MK_SHIFT)
			flags |= WKE_SHIFT;

		if (wParam & MK_LBUTTON)
			flags |= WKE_LBUTTON;
		if (wParam & MK_MBUTTON)
			flags |= WKE_MBUTTON;
		if (wParam & MK_RBUTTON)
			flags |= WKE_RBUTTON;

		//flags = wParam;

		BOOL handled = m_pWebView->mouseWheel(pt.x, pt.y, delta, flags);
		SetMsgHandled(handled);

		return handled;
	}

	LRESULT SWkeWebkit::OnChar( UINT uMsg, WPARAM wParam,LPARAM lParam )
	{
		unsigned int charCode = wParam;
		unsigned int flags = 0;
		if (HIWORD(lParam) & KF_REPEAT)
			flags |= WKE_REPEAT;
		if (HIWORD(lParam) & KF_EXTENDED)
			flags |= WKE_EXTENDED;

		//flags = HIWORD(lParam);

		SetMsgHandled(m_pWebView->keyPress(charCode, flags, false));
		return 0;
	}

	LRESULT SWkeWebkit::OnImeStartComposition( UINT uMsg, WPARAM wParam,LPARAM lParam )
	{
		wkeRect caret = m_pWebView->getCaret();

		CRect rcClient;
		GetClientRect(&rcClient);

		CANDIDATEFORM form;
		form.dwIndex = 0;
		form.dwStyle = CFS_EXCLUDE;
		form.ptCurrentPos.x = caret.x + rcClient.left;
		form.ptCurrentPos.y = caret.y + caret.h + rcClient.top;
		form.rcArea.top = caret.y + rcClient.top;
		form.rcArea.bottom = caret.y + caret.h + rcClient.top;
		form.rcArea.left = caret.x + rcClient.left;
		form.rcArea.right = caret.x + caret.w + rcClient.left;
		COMPOSITIONFORM compForm;
		compForm.ptCurrentPos=form.ptCurrentPos;
		compForm.rcArea=form.rcArea;
		compForm.dwStyle=CFS_POINT;

		HWND hWnd=GetContainer()->GetHostHwnd();
		HIMC hIMC = ImmGetContext(hWnd);
		ImmSetCandidateWindow(hIMC, &form);
		ImmSetCompositionWindow(hIMC,&compForm);
		ImmReleaseContext(hWnd, hIMC);
		return 0;
	}

	void SWkeWebkit::OnSetFocus(SWND wndOld)
	{
	    __super::OnSetCursor(wndOld);
		m_pWebView->focus();
	}

	void SWkeWebkit::OnKillFocus(SWND wndFocus)
	{
		m_pWebView->unfocus();
		__super::OnKillFocus(wndFocus);
	}

	void SWkeWebkit::OnTimer( char cTimerID )
	{
		if(cTimerID==TM_TICKER)
		{
			m_pWebView->tick();
		}
	}

	void SWkeWebkit::onBufUpdated( const HDC hdc,int x, int y, int cx, int cy )
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		CRect rcInvalid(CPoint(x,y),CSize(cx,cy));
		rcInvalid.OffsetRect(rcClient.TopLeft());
		InvalidateRect(rcInvalid);
	}

    BOOL SWkeWebkit::OnIdle()
    {
        m_pWebView->tick();
        return TRUE;
    }

	BOOL SWkeWebkit::OnSetCursor( const CPoint &pt )
	{
		return TRUE;
	}

	BOOL SWkeWebkit::OnAttrUrl( SStringW strValue, BOOL bLoading )
	{
		m_strUrl=strValue;
		if(!bLoading) m_pWebView->loadURL(m_strUrl);
		return !bLoading;
	}



}

