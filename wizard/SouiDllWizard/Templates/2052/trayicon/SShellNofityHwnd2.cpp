#include "stdafx.h"
#include "SShellNofityHwnd2.h"

namespace SOUI
{
	CShellNotifyHwnd2::CShellNotifyHwnd2(SHostWnd * pMainWnd, SShellNotifyIcon* shellnotifyicon) :m_pMainWnd(pMainWnd),m_ShellNotifyIcon(shellnotifyicon)
	{
		MsgTaskbarCreated = RegisterWindowMessage(L"TaskbarCreated");
		Create(L"shell_nofity_msg_windows", 0, 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr);
	}

	//托盘通知消息处理函数

	LRESULT CShellNotifyHwnd2::OnIconNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL)
	{
		LRESULT bRet = S_FALSE;
		switch (lParam)
		{
		case  WM_RBUTTONDOWN:
		{
			m_ShellNotifyIcon->ShowMenu();
			bRet = S_OK;
		}break;
		case WM_LBUTTONDOWN:
		{
			if (m_pMainWnd->IsWindowVisible())
				m_pMainWnd->ShowWindow(SW_HIDE);
			else
			{
				m_pMainWnd->ShowWindow(SW_SHOW);
				SetForegroundWindow(m_pMainWnd->m_hWnd);
			}
			bRet = S_OK;
		}break;
		}
		return bRet;
	}

	LRESULT CShellNotifyHwnd2::OnTaskbarCreated(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL)
	{
		return m_ShellNotifyIcon->Show();
	}

	void CShellNotifyHwnd2::OnFinalMessage(HWND hWnd)
	{
		__super::OnFinalMessage(hWnd);
		delete this;
	}

	void CShellNotifyHwnd2::OnTimer(UINT_PTR nIDEvent)
	{
		switch (nIDEvent)
		{
		case ANI_TIMER_ID:
		{
			m_ShellNotifyIcon->NextFrame();
		}break;
		default:
			break;
		}
	}
}