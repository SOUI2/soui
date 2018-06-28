//////////////////////////////////////////////////////////////////////////
//  Class Name: SShellNotifyIcon
// Description:show ShellNotifyIcon
//     Creator: 359501300
//     Version: 2017.9.11 - 2.0 - Create
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SShellNotifyIcon.h"
#include "helper\smenuex.h"
#include "helper\SMenu.h"
#pragma warning(disable:4482) //warning C4482: 使用了非标准扩展: 限定名中使用了枚举“SOUI::MenuType”

namespace SOUI
{
	SShellNotifyIcon::SShellNotifyIcon() :m_menuType(unknow), m_MsgOnlyWnd(NULL), m_iDefIcon(0), m_iStartFrame(1), m_bRunAni(false), m_iDuration(200)
	{
		memset(&nid, 0, sizeof(nid));
	}

	SShellNotifyIcon::~SShellNotifyIcon() {}

	void SShellNotifyIcon::Create(HWND hOwner, HICON hIcon, UINT uFlags, UINT uCallbackMessage, UINT uId)
	{
		IniNotifyIconData(hOwner, hIcon, uFlags, uCallbackMessage, uId, m_strTip);
	}

	void SShellNotifyIcon::IniNotifyIconData(HWND hOwner, HICON hIcon, UINT flags, UINT callbackmsg, UINT ID, LPCTSTR szTip)
	{	
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hOwner;
		nid.uID = ID;
		nid.uFlags = flags;//NIF_ICON | NIF_MESSAGE | NIF_TIP;
		nid.uCallbackMessage = callbackmsg;
		nid.hIcon = hIcon;
		_tcscpy_s(nid.szTip, szTip);
	}

	void SShellNotifyIcon::ShowMenu()
	{
		switch (m_menuType)
		{
		case MenuType::menu:
		{
			SMenu tmenu;
			if (tmenu.LoadMenu(m_strMenu))
			{
				POINT pt;
				GetCursorPos(&pt);
				SetForegroundWindow(m_MsgOnlyWnd->m_hWnd);
				tmenu.TrackPopupMenu(0, pt.x, pt.y, m_MsgOnlyWnd->m_hWnd);
			}
		}
		break;
		case MenuType::menuex:
		{
			SMenuEx tmenuex;
			if (tmenuex.LoadMenu(m_strMenu))
			{
				POINT pt;
				GetCursorPos(&pt);
				SetForegroundWindow(m_MsgOnlyWnd->m_hWnd);
				tmenuex.TrackPopupMenu(0, pt.x, pt.y, m_MsgOnlyWnd->m_hWnd);
			}
		}
		break;
		}
	}

	void SShellNotifyIcon::StartAni()
	{
		SASSERT(m_MsgOnlyWnd);

		if (m_MsgOnlyWnd)
		{
			if (m_ArrIcon.GetCount() - (m_iStartFrame + 1) > 2)//至少要两帧吧。。。
			{
				nid.uFlags |= NIF_ICON;
				nid.uFlags &= ~NIF_INFO;
				m_iCurFrame = m_iStartFrame;
				m_MsgOnlyWnd->SetTimer(ANI_TIMER_ID, m_iDuration);
				m_bRunAni = true;
			}
		}
	}

	void SShellNotifyIcon::StopAni()
	{
		SASSERT(m_MsgOnlyWnd);
		m_iCurFrame = 0;
		if (m_MsgOnlyWnd&&m_bRunAni)
		{
			m_bRunAni = false;
			m_MsgOnlyWnd->KillTimer(ANI_TIMER_ID);
		}
		nid.hIcon = m_ArrIcon[m_iDefIcon];
		Shell_NotifyIcon(NIM_MODIFY, &nid);
	}

	void SShellNotifyIcon::SetDefIconIdx(int iIdx)
	{
		if (iIdx >= m_ArrIcon.GetCount())
			return;
		m_iDefIcon = iIdx;
		if (!m_bRunAni)
		{
			nid.uFlags |= NIF_ICON;
			nid.uFlags &= ~NIF_INFO;
			nid.hIcon = m_ArrIcon[m_iDefIcon];
			Shell_NotifyIcon(NIM_MODIFY, &nid);
		}
	}

	void SShellNotifyIcon::NextFrame()
	{
		if (m_iCurFrame >= m_ArrIcon.GetCount())
			m_iCurFrame = m_iStartFrame;
		nid.hIcon = m_ArrIcon[m_iCurFrame++];
		Shell_NotifyIcon(NIM_MODIFY, &nid);
	}

	HRESULT SShellNotifyIcon::SetMenu(SStringW strValue, BOOL bLoading)
	{
		pugi::xml_document xmlDoc;
		if (SApplication::getSingleton().LoadXmlDocment(xmlDoc, S_CW2T(strValue)))
		{
			if (wcscmp(xmlDoc.first_child().name(), L"menu") == 0)
				m_menuType = MenuType::menu;
			else if ((wcscmp(xmlDoc.first_child().name(),L"menuRoot") == 0) || (wcscmp(xmlDoc.first_child().name(), L"menuItem") == 0))
				m_menuType = MenuType::menuex;
			else m_menuType = MenuType::unknow;

			if (MenuType::unknow != m_menuType)
				m_strMenu = S_CW2T(strValue);
			return S_OK;
		}
		else return S_FALSE;
	}
	
	HRESULT SShellNotifyIcon::SetIcon(SStringW strValue, BOOL bLoading)
	{
		for (size_t i = 0; i < m_ArrIcon.GetCount(); i++)
		{
			DestroyIcon(m_ArrIcon[i]);
		}
		m_ArrIcon.RemoveAll();
		SStringWList icoList;
		size_t icoCount = SplitString(strValue, L';', icoList);
		SASSERT(icoCount > 0);
		for (size_t i = 0; i < icoCount; i++)
		{
			HICON ico = LOADICON2(icoList[i]);
			if (ico)
				m_ArrIcon.Add(ico);
		}
		return m_ArrIcon.GetCount() == 0 ? S_FALSE : S_OK;
	}

	BOOL SShellNotifyIcon::Show()
	{
		//未初使化NotifyIconData
		if (nid.cbSize != sizeof(NOTIFYICONDATA))
			return FALSE;
		return Shell_NotifyIcon(NIM_ADD, &nid);
	}

	BOOL SShellNotifyIcon::Hide()
	{
		return Shell_NotifyIcon(NIM_DELETE, &nid);
	}

	HRESULT SShellNotifyIcon::SetTip(SStringW szTip, BOOL bLoading)
	{	
		if (!szTip.IsEmpty())
		{
			m_strTip = S_CW2T(szTip);
			if (!bLoading)
			{
				nid.uFlags |= NIF_TIP;
				nid.uFlags &= ~NIF_INFO;
				_tcscpy_s(nid.szTip, m_strTip);
				Shell_NotifyIcon(NIM_MODIFY, &nid);
			}
			return S_OK;
		}
		return S_FALSE;
	}

	BOOL SShellNotifyIcon::CreateChildren(pugi::xml_node xmlNode)
	{
		SHostWnd *pHostWnd = (SHostWnd*)(GetTopLevelParent()->GetContainer());
		SASSERT(pHostWnd);
		m_MsgOnlyWnd = new CShellNotifyHwnd2(pHostWnd, this);
		SASSERT(IsWindow(m_MsgOnlyWnd->m_hWnd));
		SASSERT(m_iDefIcon < m_ArrIcon.GetCount());
		//SASSERT(m_ArrIcon.GetCount()>0);
		if (m_iDefIcon >= m_ArrIcon.GetCount())
			m_iDefIcon = 0;
		Create(m_MsgOnlyWnd->m_hWnd, m_ArrIcon.GetCount() == 0 ? GETRESPROVIDER->LoadIcon(_T("ICON_LOGO"), 16) : m_ArrIcon[m_iDefIcon]);
		return Show();
	}

	void SShellNotifyIcon::OnFinalRelease()
	{
		if (m_MsgOnlyWnd)
		{
			Hide();
			for (size_t i = 0; i < m_ArrIcon.GetCount(); i++)
			{
				DestroyIcon(m_ArrIcon[i]);
			}
			m_ArrIcon.RemoveAll();
			m_MsgOnlyWnd->PostMessage(WM_CLOSE);
		}
	}


}