/**
 * Copyright (C) 2014-2050 359501300
 * All rights reserved.
 *
 * @file       SShellNotifyIcon.h
 * @brief
 * @version    v2.0
 * @author     359501300
 * @date       2017-09-11
 *
 * Describe
 */
#pragma once
#include <ShellAPI.h>
#include "SShellNofityHwnd2.h"
#include "core\SWnd.h"

 /**
  * @class      SShellNotifyIcon
  * @brief      ShellNotifyIcon
  *
  * Describe    ShellNotifyIcon
  */
namespace SOUI
{
	enum MenuType
	{
		menu, menuex, unknow
	};

	class SShellNotifyIcon :public SWindow
	{
		SOUI_CLASS_NAME(SWindow, L"shellnotifyicon")
		friend class CShellNotifyHwnd2;
	public:
		/**
		 * SShellNotifyIcon::SShellNotifyIcon
		 * @brief    构造函数
		 *
		 * Describe  构造函数
		 */
		SShellNotifyIcon();

		/**
		 * SShellNotifyIcon::~SShellNotifyIcon
		 * @brief    析构函数
		 *
		 * Describe  析构函数
		 */
		~SShellNotifyIcon();

		BOOL Show();

		BOOL Hide();
		HRESULT SetTip(SStringW szTip,BOOL bLoading=FALSE);
		BOOL ShowNotify(LPCTSTR szMsg, LPCTSTR szTitle = NULL)
		{
			if (szMsg)
			{
				_tcscpy_s(nid.szInfo, szMsg);
				nid.uFlags |= NIF_INFO;
				nid.uTimeout = 1000;
				//nid.dwInfoFlags = NIIF_USER;
			}
			if (szTitle)
			{
				_tcscpy_s(nid.szInfoTitle, szTitle);
			}
			return Shell_NotifyIcon(NIM_MODIFY, &nid);
		}
		void ShowMenu();
		void StartAni();
		void StopAni();
		bool IsRunAni() { return m_bRunAni; }
		void SetDefIconIdx(int iIdx);
	protected:
		SOUI_MSG_MAP_BEGIN()
			MSG_WM_PAINT_EX(OnPaint)
			MSG_WM_ERASEBKGND_EX(OnEraseBkgnd)
			MSG_WM_NCPAINT_EX(OnPaint)
			SOUI_MSG_MAP_END()
	protected:
		void OnPaint(SOUI::IRenderTarget * pRT) {};
		BOOL OnEraseBkgnd(SOUI::IRenderTarget * pRT) { return TRUE; };
		void NextFrame();
		virtual BOOL CreateChildren(pugi::xml_node xmlNode);
		virtual void OnFinalRelease();
		void Create(HWND hOwner, HICON hIcon, UINT uFlags = (NIF_ICON | NIF_MESSAGE | NIF_TIP), UINT uCallbackMessage = WM_ICONNOTIFY, UINT uId = ID_TASKBARICON);
		void IniNotifyIconData(HWND hOwner, HICON hIcon, UINT flags, UINT callbackmsg, UINT ID, LPCTSTR szTip);
		HRESULT SetMenu(SStringW strValue, BOOL bLoading);
		HRESULT SetIcon(SStringW strValue, BOOL bLoading);

		SOUI_ATTRS_BEGIN()
			ATTR_CUSTOM(L"ico", SetIcon)
			ATTR_CUSTOM(L"tip", SetTip)
			ATTR_CUSTOM(L"menu", SetMenu)
			ATTR_INT(L"deficoidx", m_iDefIcon, FALSE)
			ATTR_INT(L"startframe", m_iStartFrame, FALSE)
			ATTR_INT(L"duration", m_iDuration,FALSE)
		SOUI_ATTRS_END()
	private:
		SArray<HICON> m_ArrIcon;
		SStringT m_strTip;
		SStringT m_strMenu;
		MenuType m_menuType;
		NOTIFYICONDATA nid;
		CShellNotifyHwnd2 *m_MsgOnlyWnd;
		//  默认图标索引			开始帧位置		动画间隔时间
		int m_iDefIcon, m_iCurFrame, m_iStartFrame, m_iDuration;
		bool m_bRunAni;
	};
}