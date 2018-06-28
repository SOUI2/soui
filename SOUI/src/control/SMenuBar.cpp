#include "souistd.h"
#include "control/SMenuBar.h"

#define TIMER_POP	10

namespace SOUI
{
	static const wchar_t XmlBtnStyle[] = L"btnStyle";
	static const wchar_t XmlMenus[] = L"menus";

	class SMenuItem : 
		public SButton
	{
		SOUI_CLASS_NAME(SMenuItem, L"menuItem")
		friend class SMenuBar;
	public:
		SMenuItem(SMenuBar *pHostMenu, const SStringT &strResName);
		~SMenuItem();

		void SetData(ULONG_PTR data) { m_data = data; }
		ULONG_PTR GetData() { return m_data; }

	protected:
		BOOL IsLoad() { return m_bIsLoad; }
		UINT PopMenu();

		virtual void OnFinalRelease() { delete this; }
		void OnMouseMove(UINT nFlags, CPoint pt);
		void OnLButtonUp(UINT nFlags, CPoint pt);
		void OnLButtonDown(UINT nFlags, CPoint pt);

		void OnTimer(char timerID);

		SOUI_MSG_MAP_BEGIN()
			MSG_WM_MOUSEMOVE(OnMouseMove)
			MSG_WM_LBUTTONDOWN(OnLButtonDown)
			MSG_WM_LBUTTONUP(OnLButtonUp)
			MSG_WM_TIMER_EX(OnTimer)
		SOUI_MSG_MAP_END()

		SOUI_ATTRS_BEGIN()
			ATTR_STRINGT(L"menuName", m_strResName, TRUE)
		SOUI_ATTRS_END()

		ULONG_PTR m_data;
		SMenuBar* m_pHostMenu;
		SStringT m_strResName;
		BOOL m_bIsLoad;
		SMenuEx m_Menu;
	};

	SMenuItem::SMenuItem(SMenuBar *pHostMenu, const SStringT & strResName):
		m_data(0),
		m_pHostMenu(pHostMenu),
		m_strResName(strResName),
		m_bIsLoad(FALSE)
	{
		SetAttribute(L"drawFocusRect", L"0");
		m_bIsLoad = m_Menu.LoadMenu(strResName);
	}

	SMenuItem::~SMenuItem()
	{
	}

	UINT SMenuItem::PopMenu()
	{
		m_pHostMenu->m_bIsShow = TRUE;
		m_pHostMenu->m_pNowMenu = this;

		SetCheck(TRUE);

		CRect rcHost;
		::GetWindowRect(m_pHostMenu->m_hWnd, rcHost);
		CRect rcMenu = GetClientRect();
		int iRet = 0;
		iRet = m_Menu.TrackPopupMenu(TPM_RETURNCMD,
			rcHost.left + rcMenu.left, rcHost.top + rcMenu.bottom + 2, m_pHostMenu->m_hWnd);

		SetCheck(FALSE);
		m_pHostMenu->m_bIsShow = FALSE;
		if (m_pHostMenu->m_pNowMenu != this || iRet == 0)
		{
			m_pHostMenu->m_pNowMenu = NULL;
			return iRet;
		}
		m_pHostMenu->m_pNowMenu = NULL;

		// 把选择事件发送过去
		EventSelectMenu evt(m_pHostMenu);
		evt.m_id = iRet;
		evt.m_pMenu = &m_Menu;
		FireEvent(evt);

		return iRet;
	}

	void SMenuItem::OnMouseMove(UINT nFlags, CPoint pt)
	{
		CRect rcWnd = GetWindowRect();
		if (m_pHostMenu->m_bIsShow && m_pHostMenu->m_pNowMenu != this)
		{
			m_pHostMenu->m_pNowMenu = this;
			SMenuEx::ExitPopupMenu();
			SetTimer(TIMER_POP, 10);
		}
	}

	void SMenuItem::OnLButtonUp(UINT nFlags, CPoint pt)
	{
		SButton::OnLButtonUp(nFlags, pt);
	}

	void SMenuItem::OnLButtonDown(UINT nFlags, CPoint pt)
	{
		SButton::OnLButtonDown(nFlags, pt);
		BringWindowToTop();
		if (!::IsWindow(m_pHostMenu->m_hWnd))
			return;
		PopMenu();
	}

	void SMenuItem::OnTimer(char timerID)
	{
		if (timerID == TIMER_POP)
		{
			if (!m_pHostMenu->m_bIsShow)
			{
				PopMenu();
				KillTimer(timerID);
			}
		}
	}

	SMenuBar::SMenuBar() :
		m_bIsShow(FALSE),
		m_hWnd(NULL),
		m_pNowMenu(NULL)
	{
		m_evtSet.addEvent(EVENTID(EventSelectMenu));
	}

	SMenuBar::~SMenuBar()
	{
	}
	BOOL SMenuBar::Init(SHostWnd * pHostWnd)
	{
		if (::IsWindow(pHostWnd->m_hWnd))
		{
			m_hWnd = pHostWnd->m_hWnd;
			return TRUE;
		}
		return FALSE;
	}
	BOOL SMenuBar::Insert(LPCTSTR pszTitle, LPCTSTR pszResName, int iPos)
	{
		if (!pszResName)
			return FALSE;

		SMenuItem *pNewMenu = new SMenuItem(this, pszResName);
		SASSERT(pNewMenu);
		if (!pNewMenu->IsLoad())
		{
			pNewMenu->DestroyWindow();
			delete pNewMenu;
			return FALSE;
		}
		InsertChild(pNewMenu);

		pugi::xml_node xmlBtnStyle = m_xmlStyle.child(XmlBtnStyle);
		if (xmlBtnStyle)
			pNewMenu->InitFromXml(xmlBtnStyle);

		if(pszTitle)
			pNewMenu->SetWindowText(pszTitle);

		if (iPos < 0) iPos = (int)m_lstMenuItem.GetCount();
		m_lstMenuItem.InsertAt(iPos, pNewMenu);

		UpdateChildrenPosition();
		return TRUE;
	}
	BOOL SMenuBar::CreateChildren(pugi::xml_node xmlNode)
	{
		pugi::xml_node xmlBtnStyle = xmlNode.child(XmlBtnStyle);
		if (xmlBtnStyle)
		{
			m_xmlStyle.append_copy(xmlBtnStyle);
		}
		pugi::xml_node xmlTMenus = xmlNode.child(XmlMenus);
		if (xmlTMenus)
		{
			for (pugi::xml_node xmlChild = xmlTMenus.first_child(); xmlChild; xmlChild = xmlChild.next_sibling())
			{
				if (wcscmp(xmlChild.name(), SMenuItem::GetClassName()) != 0)
					continue;
				Insert(S_CW2T(xmlChild.first_child().value()),
					S_CW2T(xmlChild.attribute(L"menuName").value()));
			}
		}

		return TRUE;
	}
	void SMenuBar::UpdateChildrenPosition()
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		for (size_t i = 0; i < m_lstMenuItem.GetCount(); i++)
		{
			CRect rcLeft;
			if (i > 0)
				m_lstMenuItem[i - 1]->GetWindowRect(&rcLeft);
			else
			{
				rcLeft = rcClient;
				rcLeft.right = rcLeft.left;
			}
			CRect rcInit;
			rcInit.top = rcLeft.top;
			rcInit.left = rcLeft.right + 1;
			rcInit.right = rcLeft.right + 50;
			rcInit.bottom = rcInit.top + 20;
			m_lstMenuItem[i]->Move(rcInit);
		}
	}
}