#include "StdAfx.h"
#include "SRealWndHandler_Scintilla.h"
#include "ScintillaWnd.h"

namespace SOUI
{
	SRealWndHandler_Scintilla::SRealWndHandler_Scintilla(void)
	{
	}

	SRealWndHandler_Scintilla::~SRealWndHandler_Scintilla(void)
	{
	}

	HWND SRealWndHandler_Scintilla::OnRealWndCreate( SRealWnd *pRealWnd )
	{
		if(pRealWnd->GetRealWndParam().m_strClassName.CompareNoCase(CScintillaWnd::GetScintillaWndClass())==0)
		{
			CScintillaWnd *pWnd=new CScintillaWnd;
			BOOL bOK=pWnd->Create(pRealWnd->GetRealWndParam().m_strWindowName,WS_CHILD,CRect(0,0,0,0),pRealWnd->GetContainer()->GetHostHwnd(),pRealWnd->GetID(),SApplication::getSingleton().GetInstance());
			if(!bOK)
			{
				SASSERT(FALSE);
				delete pWnd;
				return 0;
			}
			pRealWnd->SetUserData((ULONG_PTR)pWnd);
			return pWnd->m_hWnd;
		}else
		{
			return 0;
		}
	}

	void SRealWndHandler_Scintilla::OnRealWndDestroy( SRealWnd *pRealWnd )
	{
		if(pRealWnd->GetRealWndParam().m_strClassName.CompareNoCase(CScintillaWnd::GetScintillaWndClass())==0)
		{
			CScintillaWnd *pWnd=(CScintillaWnd *)pRealWnd->GetUserData();
			if(pWnd)
			{
				pWnd->DestroyWindow();
				delete pWnd;
			}
		}
	}

	//不处理，返回FALSE
	BOOL SRealWndHandler_Scintilla::OnRealWndSize( SRealWnd *pRealWnd )
	{
		return FALSE;
	}

	//不处理，返回FALSE
	BOOL SRealWndHandler_Scintilla::OnRealWndInit( SRealWnd *pRealWnd )
	{
		return FALSE;
	}

	BOOL SRealWndHandler_Scintilla::OnRealWndPosition(SRealWnd *pRealWnd, const CRect &rcWnd)
	{
		HWND hRealWnd = pRealWnd->GetRealHwnd(TRUE);
		if(IsWindow(hRealWnd))
		{
			::SetWindowPos(hRealWnd, 0, rcWnd.left,rcWnd.top, rcWnd.Width(), rcWnd.Height(), SWP_NOZORDER);
			::UpdateWindow(hRealWnd);
		}
		return TRUE;
	}

}
