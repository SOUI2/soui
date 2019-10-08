#include "include\souistd.h"
#include "helper\SMenuEx.h"
#include "helper\SplitString.h"
#include <layout/SLinearLayout.h>
namespace SOUI
{
#define TIMERID_POPSUBMENU  100
#define TIME_PUPSUBMENU     500

#define WIDTH_MENU_INIT     10000
#define WIDTH_MENU_MAX      2000
#define WIDTH_MENU_MIN      100.f


#define Y_MIDFLAG L"-1000px"
#define Y_IMIDFLAG (-1000)


	//////////////////////////////////////////////////////////////////////////
	class SMenuExRoot : public SWindow
	{
		SOUI_CLASS_NAME(SMenuExRoot, L"menuRoot")
		friend class SMenuEx;
		friend class SMenuExItem;
	protected:
		ISkinObj * m_pItemSkin;
		ISkinObj * m_pIconSkin;
		ISkinObj * m_pCheckSkin;
		ISkinObj * m_pArrowSkin;
		ISkinObj * m_pSepSkin;

		SMenuEx  * m_pMenuEx;

		SLayoutSize  m_nItemHei;
		SLayoutSize  m_nIconBarWidth;
		SLayoutSize  m_nTextOffset;
		SLayoutSize  m_iconX, m_iconY;
		SLayoutSize  m_nMinWidth;
		SLayoutSize  m_nSubMenuOffset;

		DWORD	  m_dwContextHelpId;
		
		void Copy(SMenuExRoot* pNewMenuExRoot)
		{
			pNewMenuExRoot->m_pItemSkin = m_pItemSkin;
			pNewMenuExRoot->m_pIconSkin = m_pIconSkin;
			pNewMenuExRoot->m_pCheckSkin = m_pCheckSkin;
			pNewMenuExRoot->m_pArrowSkin = m_pArrowSkin;
			pNewMenuExRoot->m_pSepSkin = m_pSepSkin;
			pNewMenuExRoot->m_nItemHei = m_nItemHei;
			pNewMenuExRoot->m_nIconBarWidth = m_nIconBarWidth;
			pNewMenuExRoot->m_nTextOffset = m_nTextOffset;
			pNewMenuExRoot->m_iconX = m_iconX;
			pNewMenuExRoot->m_iconY = m_iconY;
			pNewMenuExRoot->m_nMinWidth = m_nMinWidth;
			pNewMenuExRoot->m_nSubMenuOffset = m_nSubMenuOffset;
			pNewMenuExRoot->m_style = m_style;		// 设置了 些 margin 之类的 属性 也要 copy
		}

		HRESULT OnAttrIconPos(const SStringW & strValue, BOOL bLoading);

		SOUI_ATTRS_BEGIN()
			ATTR_SKIN(L"itemSkin", m_pItemSkin, FALSE)
			ATTR_SKIN(L"checkSkin", m_pCheckSkin, FALSE)
			ATTR_SKIN(L"sepSkin", m_pSepSkin, FALSE)
			ATTR_SKIN(L"arrowSkin", m_pArrowSkin, FALSE)
			ATTR_LAYOUTSIZE(L"itemHeight", m_nItemHei, FALSE)
			ATTR_CUSTOM(L"iconPos", OnAttrIconPos)
			ATTR_SKIN(L"iconSkin", m_pIconSkin, FALSE)
			//ATTR_LAYOUTSIZE4(L"margin", m_rcMargin,FALSE)
			ATTR_LAYOUTSIZE(L"iconBarWidth", m_nIconBarWidth, FALSE)
			ATTR_LAYOUTSIZE(L"textOffset", m_nTextOffset, FALSE)
			ATTR_LAYOUTSIZE(L"minWidth", m_nMinWidth, FALSE)
			ATTR_LAYOUTSIZE(L"subMenuOffset", m_nSubMenuOffset, FALSE)
			ATTR_DWORD(L"contextHelpId", m_dwContextHelpId, FALSE)
			SOUI_ATTRS_END()

	public:
		SMenuExRoot(SMenuEx * pMenuEx)
			:m_pItemSkin(GETBUILTINSKIN(SKIN_SYS_MENU_SKIN))
			, m_pSepSkin(GETBUILTINSKIN(SKIN_SYS_MENU_SEP))
			, m_pCheckSkin(GETBUILTINSKIN(SKIN_SYS_MENU_CHECK))
			, m_pArrowSkin(GETBUILTINSKIN(SKIN_SYS_MENU_ARROW))
			, m_pIconSkin(NULL)
			, m_pMenuEx(pMenuEx)
			, m_dwContextHelpId(0)
		{
			m_nItemHei.setSize(26.f, SLayoutSize::dp);
			m_nIconBarWidth.setSize(24.f, SLayoutSize::dp);
			m_nMinWidth.setSize(WIDTH_MENU_MIN, SLayoutSize::dp);
			OnAttrLayout(SVBox::GetClassName(), TRUE);//set layout to vbox
		}
		
		SMenuExItem * GetNextMenuItem(SMenuExItem *pItem, BOOL bForword, int nCount = 0);

		CSize CalcMenuSize()
		{
			CRect rcContainer(0, 0, WIDTH_MENU_INIT, WIDTH_MENU_INIT);
			return GetDesiredSize(rcContainer);
		}

		//CRect GetMargin() const;

		virtual CSize GetDesiredSize(LPCRECT pRcContainer)
		{
			CSize szRet;
			SWindow *pItem = GetWindow(GSW_FIRSTCHILD);
			while (pItem)
			{
				CSize szItem = pItem->GetDesiredSize(pRcContainer);
				if (szItem.cx < WIDTH_MENU_MAX)
					szRet.cx = (std::max)(szRet.cx, szItem.cx);
				szRet.cy += szItem.cy;
				pItem = pItem->GetWindow(GSW_NEXTSIBLING);
			}
			CRect rcMargin = m_style.GetMargin();
			szRet.cx += rcMargin.left + rcMargin.right;
			szRet.cy += rcMargin.top + rcMargin.bottom;

			if (szRet.cx > m_nMaxWidth.toPixelSize(GetScale()) && !m_nMaxWidth.isWrapContent())
				szRet.cx = m_nMaxWidth.toPixelSize(GetScale());
			if (szRet.cx < m_nMinWidth.toPixelSize(GetScale()))
				szRet.cx = m_nMinWidth.toPixelSize(GetScale());
			return szRet;
		}

		virtual BOOL InitFromXml(pugi::xml_node xmlNode)
		{
			//找到根节点，获取在根节点上配置的全局菜单对象属性
			pugi::xml_node xmlRoot = xmlNode.root().first_child();
			if (xmlNode != xmlRoot)
			{
				SObject::InitFromXml(xmlRoot);
			}

			BOOL bRet = __super::InitFromXml(xmlNode);

			SetWindowText(_T(""));//防止子菜单显示父级菜单项的文本。
			return bRet;
		}

		virtual BOOL CreateChildren(pugi::xml_node xmlNode)
		{
			pugi::xml_node xmlItem = xmlNode.first_child();
			while (xmlItem)
			{
				SWindow *pMenuItem = CreateMenuItem(xmlItem.name());
				if (pMenuItem)
				{
					InsertChild(pMenuItem);
					pMenuItem->InitFromXml(xmlItem);
				}
				xmlItem = xmlItem.next_sibling();
			}
			return TRUE;
		}

		virtual CRect GetChildrenLayoutRect()
		{
			CRect rcClient = GetClientRect();
			return rcClient;
		}

		virtual void UpdateChildrenPosition()
		{
			CRect rcClient = GetChildrenLayoutRect();

			SWindow *pChild = GetWindow(GSW_FIRSTCHILD);
			CRect rcItem = rcClient;
			rcItem.bottom = rcItem.top;
			while (pChild)
			{
				CSize szItem = pChild->GetDesiredSize(rcClient);
				rcItem.top = rcItem.bottom;
				rcItem.bottom += szItem.cy;
				pChild->Move(rcItem);
				pChild = pChild->GetWindow(GSW_NEXTSIBLING);
			}
		}

		virtual void OnScaleChanged(int nScale)
		{
			__super::OnScaleChanged(nScale);
			GetScaleSkin(m_pItemSkin, nScale);
			GetScaleSkin(m_pIconSkin, nScale);
			GetScaleSkin(m_pCheckSkin, nScale);
			GetScaleSkin(m_pArrowSkin, nScale);
			GetScaleSkin(m_pSepSkin, nScale);
		}

		SWindow * CreateMenuItem(const SStringW & strItemName);
	};


	HRESULT SMenuExRoot::OnAttrIconPos(const SStringW & strValue, BOOL bLoading)
	{
		SStringWList values;
		SplitString(strValue, L',', values);
		if (1 == values.GetCount())
		{
			//只设置X时，让Y方向自动居中
			m_iconX = GETLAYOUTSIZE(values[0]);
			m_iconY.parseString(Y_MIDFLAG);
			return S_OK;
		}
		else if (2 != values.GetCount())
			return E_INVALIDARG;
		m_iconX = GETLAYOUTSIZE(values[0]);
		m_iconY = GETLAYOUTSIZE(values[1]);
		return S_OK;
	}

	SMenuExItem * SMenuExRoot::GetNextMenuItem(SMenuExItem *pItem, BOOL bForword, int nCount)
	{
		if ((UINT)nCount == GetChildrenCount()) return NULL;

		SMenuExItem *pRet = NULL;
		if (pItem)
		{
			SASSERT(pItem->GetParent() == this);
			pRet = (SMenuExItem *)pItem->GetWindow(bForword ? GSW_NEXTSIBLING : GSW_PREVSIBLING);
		}
		if (!pRet)
		{
			pRet = (SMenuExItem *)GetWindow(bForword ? GSW_FIRSTCHILD : GSW_LASTCHILD);
		}

		if (!pRet->IsDisabled(TRUE))
			return pRet;
		else
			return GetNextMenuItem(pRet, bForword, nCount + 1);
	}


	//////////////////////////////////////////////////////////////////////////
	// SMenuExItem

	SMenuExItem::~SMenuExItem()
	{
		if (m_pSubMenu)
		{
			delete m_pSubMenu;
		}
	}

	SMenuExItem::SMenuExItem(SMenuEx *pOwnerMenu, ISkinObj *pItemSkin) :m_pSubMenu(NULL)
		, m_pOwnerMenu(pOwnerMenu)
		, m_iIcon(-1)
		, m_bCheck(FALSE)
		, m_bRadio(FALSE)
		, m_cHotKey(0)
	{
		m_pBgSkin = pItemSkin;
		m_style.m_bTrackMouseEvent = TRUE;
		m_style.SetAttribute(L"align", L"left", TRUE);
	}

	void SMenuExItem::OnPaint(IRenderTarget *pRT)
	{
		__super::OnPaint(pRT);

		CRect rc = GetClientRect();
		SMenuExRoot * pMenuRoot = sobj_cast<SMenuExRoot>(GetRoot()->GetWindow(GSW_FIRSTCHILD));
		SASSERT(pMenuRoot);
		rc.right = rc.left + pMenuRoot->m_nIconBarWidth.toPixelSize(GetScale());
		rc.left += pMenuRoot->m_iconX.toPixelSize(GetScale());
		int icoY = pMenuRoot->m_iconY.toPixelSize(GetScale());

		if (m_bCheck || m_bRadio)
		{
			SASSERT(pMenuRoot->m_pCheckSkin);
			int nState = 0;
			if (m_bRadio)
			{
				nState = m_bCheck ? 1 : 2;
			}
			if (icoY == Y_IMIDFLAG)
			{
				icoY = (rc.Height() - pMenuRoot->m_pCheckSkin->GetSkinSize().cy) / 2;
			}
			rc.top += icoY;
			CRect rcIcon(rc.TopLeft(), pMenuRoot->m_pCheckSkin->GetSkinSize());
			pMenuRoot->m_pCheckSkin->Draw(pRT, rcIcon, nState);
		}
		else if (pMenuRoot->m_pIconSkin)
		{
			if (icoY == Y_IMIDFLAG)
			{
				icoY =(rc.Height()- pMenuRoot->m_pIconSkin->GetSkinSize().cy) / 2;
			}
			rc.top += icoY;
			CRect rcIcon(rc.TopLeft(), pMenuRoot->m_pIconSkin->GetSkinSize());
			pMenuRoot->m_pIconSkin->Draw(pRT, rcIcon, m_iIcon);
		}

		if (m_pSubMenu)
		{
			CRect rcArrow = GetClientRect();
			CSize szArrow = pMenuRoot->m_pArrowSkin->GetSkinSize();
			rcArrow.left = rcArrow.right - szArrow.cx;
			rcArrow.DeflateRect(0, (rcArrow.Height() - szArrow.cy) / 2);

			pMenuRoot->m_pArrowSkin->Draw(pRT, rcArrow, (GetState()&WndState_Hover) ? 1 : 0);
		}
	}

	BOOL SMenuExItem::OnEraseBkgnd(IRenderTarget *pRT)
	{
		if (!m_pBgSkin) return FALSE;
		int nState = 0;

		if (GetState()&WndState_Disable)
		{
			nState = 2;
		}
		else if (GetState()&WndState_Check || GetState()&WndState_PushDown || GetState()&WndState_Hover)
		{
			nState = 1;
		}
		if (nState >= m_pBgSkin->GetStates()) nState = 0;
		m_pBgSkin->Draw(pRT, GetClientRect(), nState);

		return TRUE;
	}

	void SMenuExItem::GetTextRect(LPRECT pRect)
	{
		GetClientRect(pRect);
		SMenuExRoot * pMenuRoot = sobj_cast<SMenuExRoot>(GetRoot()->GetWindow(GSW_FIRSTCHILD));
		SASSERT(pMenuRoot);
		int nScale = GetScale();
		pRect->left += pMenuRoot->m_nIconBarWidth.toPixelSize(nScale) + pMenuRoot->m_nTextOffset.toPixelSize(nScale);
		if (m_pSubMenu) pRect->right -= pMenuRoot->m_pArrowSkin->GetSkinSize().cx;
	}

	CSize SMenuExItem::GetDesiredSize(LPCRECT pRcContainer)
	{
		if (!IsVisible())
			return CSize();
		CSize szRet = __super::GetDesiredSize(pRcContainer);

		SMenuExRoot * pMenuRoot = sobj_cast<SMenuExRoot>(GetRoot()->GetWindow(GSW_FIRSTCHILD));
		SASSERT(pMenuRoot);
		if (!GetLayoutParam()->IsSpecifiedSize(Horz))
		{
			szRet.cx += pMenuRoot->m_nIconBarWidth.toPixelSize(GetScale()) + pMenuRoot->m_nTextOffset.toPixelSize(GetScale());
			if (m_pSubMenu) szRet.cx += pMenuRoot->m_pArrowSkin->GetSkinSize().cx;//加上子菜单箭头宽度
		}
		if (!GetLayoutParam()->IsSpecifiedSize(Vert))
		{
			szRet.cy = pMenuRoot->m_nItemHei.toPixelSize(GetScale());
		}
		return szRet;
	}

	BOOL SMenuExItem::CreateChildren(pugi::xml_node xmlNode)
	{
		__super::CreateChildren(xmlNode);
		pugi::xml_node xmlChild = xmlNode.child(SMenuExItem::GetClassName());
		if (xmlChild)
		{//有子菜单
			m_pSubMenu = new SMenuEx(this);
			m_pSubMenu->LoadMenu(xmlNode,m_pOwnerMenu->GetParentWnd());
		}
		return TRUE;
	}

	void SMenuExItem::OnSubMenuHided(BOOL bUncheckItem)
	{
		m_pOwnerMenu->OnSubMenuHided(bUncheckItem);
	}

	WCHAR SMenuExItem::GetHotKey() const
	{
		return m_cHotKey;
	}

	void SMenuExItem::ShowSubMenu(BOOL bCheckFirstItem)
	{
		if (!m_pSubMenu) return;
		m_pOwnerMenu->PopupSubMenu(this, bCheckFirstItem);
	}

	void SMenuExItem::HideSubMenu()
	{
		if (!m_pSubMenu) return;
		m_pSubMenu->HideMenu(FALSE);
	}

	SMenuEx * SMenuExItem::GetOwnerMenu()
	{
		return m_pOwnerMenu;
	}

	SMenuEx * SMenuExItem::GetSubMenu()
	{
		return m_pSubMenu;
	}

	//////////////////////////////////////////////////////////////////////////


	class SMenuExSep : public SMenuExItem
	{
		SOUI_CLASS_NAME(SMenuExSep, L"sep")
	public:
		SMenuExSep(SMenuEx *pOwnerMenu, ISkinObj *pItemSkin)
			:SMenuExItem(pOwnerMenu, pItemSkin)
		{
			m_dwState |= WndState_Disable;
			m_bDisable = TRUE;
		}

		virtual CSize GetDesiredSize(LPCRECT pRcContainer)
		{
			SMenuExRoot * pMenuRoot = sobj_cast<SMenuExRoot>(GetRoot()->GetWindow(GSW_FIRSTCHILD));
			(void)pMenuRoot;
			SASSERT(pMenuRoot);
			CSize szRet;
			szRet.cx = WIDTH_MENU_INIT;
			if (!GetLayoutParam()->IsSpecifiedSize(Vert))
			{
				if (m_pBgSkin)
				{
					szRet.cy = m_pBgSkin->GetSkinSize().cy;
					if (szRet.cy == 0)//纯色皮肤没有size
						szRet.cy = 1;
				}
				else
					szRet.cy = 0;
			}
			else
			{
				szRet.cy = GetLayoutParam()->GetSpecifiedSize(Vert).toPixelSize(GetScale());
			}
			szRet.cy += m_style.GetMargin().top+m_style.GetMargin().bottom;
			return szRet;
		}

	protected:

		BOOL OnEraseBkgnd(IRenderTarget *pRT)
		{
			if (!m_pBgSkin) return FALSE;
			m_pBgSkin->Draw(pRT, GetClientRect(), 0);
			return TRUE;
		}
		void OnPaint(SOUI::IRenderTarget * pRT) { (void)pRT; }
		SOUI_MSG_MAP_BEGIN()
			MSG_WM_PAINT_EX(OnPaint)
			MSG_WM_ERASEBKGND_EX(OnEraseBkgnd)
		SOUI_MSG_MAP_END()
	};

	//////////////////////////////////////////////////////////////////////////
	SWindow * SMenuExRoot::CreateMenuItem(const SStringW & strItemName)
	{
		SWindow *pMenuItem = NULL;
		if (strItemName == SMenuExItem::GetClassName())
		{
			pMenuItem = new SMenuExItem(m_pMenuEx, m_pItemSkin);
		}
		else if (strItemName == SMenuExSep::GetClassName())
		{
			pMenuItem = new SMenuExSep(m_pMenuEx, m_pSepSkin);
		}
		//从style里初始化MenuItem
		if (pMenuItem)
			SApplication::getSingleton().SetSwndDefAttr(pMenuItem);

		return pMenuItem;
	}
	//////////////////////////////////////////////////////////////////////////
	class SMenuExRunData : public TObjRefImpl<IObjRef>
	{
		friend class SMenuEx;
	public:
		SMenuExRunData(HWND hOwner, int nScale) :m_hOwner(hOwner), m_bExit(FALSE), m_nCmdID(-1), m_nScale(nScale)
		{

		}

		BOOL IsMenuWnd(HWND hWnd)
		{
			SPOSITION pos = m_lstMenuEx.GetTailPosition();
			while (pos)
			{
				if (m_lstMenuEx.GetPrev(pos)->m_hWnd == hWnd) return TRUE;
			}
			return FALSE;
		}

		void PushMenuEx(SMenuEx * pMenu)
		{
			m_lstMenuEx.AddTail(pMenu);
		}

		SMenuEx * GetMenuEx()
		{
			if (m_lstMenuEx.IsEmpty()) return 0;
			return m_lstMenuEx.GetTail();
		}

		SMenuEx * PopMenuEx()
		{
			SASSERT(!m_lstMenuEx.IsEmpty());
			SMenuEx *pMenuEx = m_lstMenuEx.RemoveTail();
			return pMenuEx;
		}

		SMenuEx * SMenuExFromHwnd(HWND hWnd)
		{
			SPOSITION pos = m_lstMenuEx.GetTailPosition();
			while (pos)
			{
				SMenuEx * pMenuEx = m_lstMenuEx.GetPrev(pos);
				if (pMenuEx->m_hWnd == hWnd) return pMenuEx;
			}
			return NULL;
		}

		BOOL IsMenuExited()
		{
			return m_bExit;
		}

		void ExitMenu(int nCmdID)
		{
			m_bExit = TRUE;
			m_nCmdID = nCmdID;
		}

		int GetCmdID() { return m_nCmdID; }

		HWND GetOwner() { return m_hOwner; }

		int GetScale() const { return m_nScale; }
	protected:
		SList<SMenuEx*> m_lstMenuEx;

		BOOL m_bExit;
		int  m_nCmdID;
		HWND m_hOwner;
		int	 m_nScale;
	};

	static SMenuExRunData * s_MenuData = NULL;

	//////////////////////////////////////////////////////////////////////////
	SMenuEx::SMenuEx(void)
		:m_pParent(NULL)
		, m_pHoverItem(NULL)
		, m_pCheckItem(NULL)
		, m_bMenuInitialized(FALSE)
	{
		m_hostAttr.SetTranslucent(true);
	}

	SMenuEx::SMenuEx(SMenuExItem *pParent) 
		:m_pParent(pParent)
		, m_pHoverItem(NULL)
		, m_pCheckItem(NULL)
		, m_bMenuInitialized(FALSE)
		, m_hParent(NULL)
	{

	}

	SMenuEx::~SMenuEx(void)
	{
		if (IsWindow())
			DestroyWindow();
	}

	BOOL SMenuEx::LoadMenu(LPCTSTR pszMenu,HWND hParent)
	{
		SStringTList strMenu;
		if (1 == SplitString<SStringT, TCHAR>(pszMenu, _T(':'), strMenu))
			strMenu.InsertAt(0, _T("SMENUEX"));

		pugi::xml_document xmlMenu;
		BOOL bLoad = LOADXML(xmlMenu, strMenu[1], strMenu[0]);
		if (!bLoad) return FALSE;
		return LoadMenu(xmlMenu.first_child(),hParent);
	}

	BOOL SMenuEx::LoadMenu(pugi::xml_node xmlNode,HWND hParent)
	{
		if (IsWindow()) return FALSE;
		if (xmlNode.name() != SStringW(SMenuExRoot::GetClassName())
			&& xmlNode.name() != SStringW(SMenuExItem::GetClassName()))
			return FALSE;

		HWND hWnd = Create(hParent, WS_POPUP, WS_EX_TOOLWINDOW | WS_EX_TOPMOST, 0, 0, 0, 0);
		pugi::xml_document souiXml;
		pugi::xml_node root = souiXml.append_child(L"SOUI");
		root.append_attribute(L"translucent").set_value(1);
		if (m_pParent == NULL)
		{
			root.append_attribute(L"trCtx").set_value(xmlNode.attribute(L"trCtx").value());
		}
		m_hParent = hParent;
		_InitFromXml(root, 0, 0);

		if (!hWnd) return FALSE;


		SMenuExRoot *pMenuRoot = new SMenuExRoot(this);
		InsertChild(pMenuRoot);

		pMenuRoot->InitFromXml(xmlNode);

		pMenuRoot->GetLayoutParam()->SetWrapContent(Both);

		return TRUE;
	}

	SMenuExItem * SMenuEx::GetMenuItem(int nID)
	{
		return FindChildByID2<SMenuExItem>(nID);
	}

	SMenuEx * SMenuEx::GetSubMenu(int nID)
	{
		SMenuExItem * pItem = GetMenuItem(nID);
		if (!pItem) return NULL;
		return pItem->GetSubMenu();
	}

	UINT SMenuEx::TrackPopupMenu(UINT flag, int x, int y, HWND hOwner, int nScale)
	{
		if (!IsWindow()) return (UINT)-1;
		if (!s_MenuData)
			s_MenuData = new SMenuExRunData(hOwner, nScale);
		else
			s_MenuData->AddRef();

		HWND hActive = hOwner;
		if (!hOwner || !::IsWindowEnabled(hOwner)) hActive = ::GetActiveWindow();

		HWND hRoot = hActive;
		while (::GetParent(hRoot))
		{
			hRoot = ::GetParent(hRoot);
		}
		SetForegroundWindow(hRoot);

		ShowMenu(flag, x, y);
		RunMenu(hRoot);
		HideMenu(FALSE);

		if (hActive)
		{
			CPoint pt;
			GetCursorPos(&pt);
			::ScreenToClient(hActive, &pt);
			::PostMessage(hActive, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
		}

		int nRet = s_MenuData->GetCmdID();
		if (0 == s_MenuData->Release())
		{
			s_MenuData = NULL;
		}
		if (flag & TPM_RETURNCMD)
		{
			return nRet;
		}
		else
		{
			::SendMessage(hOwner, WM_COMMAND, MAKEWPARAM(nRet, 0), 0);
			return TRUE;
		}

	}

	void SMenuEx::ShowMenu(UINT uFlag, int x, int y)
	{
		SMenuExRoot *pMenuRoot = sobj_cast<SMenuExRoot>(GetRoot()->GetWindow(GSW_FIRSTCHILD));

		SendInitPopupMenu2Owner(0);
		SASSERT(pMenuRoot);
		pMenuRoot->SDispatchMessage(UM_SETSCALE, GetScale(), 0);

		CSize szMenu = pMenuRoot->CalcMenuSize();

		pMenuRoot->Move(CRect(CPoint(), szMenu));
		if (uFlag&TPM_CENTERALIGN)
		{
			x -= szMenu.cx / 2;
		}
		else if (uFlag & TPM_RIGHTALIGN)
		{
			x -= szMenu.cx;
		}

		if (uFlag & TPM_VCENTERALIGN)
		{
			y -= szMenu.cy / 2;
		}
		else if (uFlag&TPM_BOTTOMALIGN)
		{
			y -= szMenu.cy;
		}

		HMONITOR hMor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY);
		MONITORINFO mi = { sizeof(MONITORINFO),0 };
		GetMonitorInfo(hMor, &mi);
		CRect rcMenu(CPoint(x, y), szMenu);
		CRect rcInter;
		rcInter.IntersectRect(&rcMenu, &mi.rcMonitor);
		int subMenuOffset = m_pParent ? pMenuRoot->m_nSubMenuOffset.toPixelSize(GetScale()) : 0;
		if (rcInter != rcMenu)
		{
			if (m_pParent)
			{
				SMenuEx *pParent = m_pParent->GetOwnerMenu();
				CRect rcParent = pParent->GetWindowRect();
				pParent->ClientToScreen(&rcParent);
				if (rcMenu.right > mi.rcMonitor.right)
				{
					rcMenu.MoveToX(x - szMenu.cx - rcParent.Width() - subMenuOffset);
				}
				else {
					rcMenu.MoveToX(x + subMenuOffset);
				}

				int xOffset = 0, yOffset = 0;
				if (rcMenu.left < mi.rcMonitor.left) xOffset = mi.rcMonitor.left - rcMenu.left;
				else if (rcMenu.right > mi.rcMonitor.right) xOffset = mi.rcMonitor.right - rcMenu.right;
				if (rcMenu.top < mi.rcMonitor.top) yOffset = mi.rcMonitor.top - rcMenu.top;
				else if (rcMenu.bottom > mi.rcMonitor.bottom) yOffset = mi.rcMonitor.bottom - rcMenu.bottom;

				rcMenu.OffsetRect(xOffset, yOffset);
			}
			else {

				if (rcMenu.right > mi.rcMonitor.right)
				{
					rcMenu.MoveToX(x - szMenu.cx - subMenuOffset);
				}
				else {
					rcMenu.MoveToX(x + subMenuOffset);
				}

				if (rcMenu.top < mi.rcMonitor.top)
				{
					rcMenu.MoveToY(y + szMenu.cy);
				}
				if (rcMenu.bottom > mi.rcMonitor.bottom)
				{
					rcMenu.MoveToY(y - szMenu.cy);
				}
			}


		}
		else {

			rcMenu.MoveToX(x + subMenuOffset);
		}

		SetWindowPos(HWND_TOPMOST, rcMenu.left, rcMenu.top, szMenu.cx, szMenu.cy, SWP_SHOWWINDOW | SWP_NOACTIVATE);
		s_MenuData->PushMenuEx(this);
	}

	void SMenuEx::HideMenu(BOOL bUncheckParentItem)
	{
		if (!CSimpleWnd::IsWindowVisible()) return;
		HideSubMenu();
		ShowWindow(SW_HIDE);
		if (m_pCheckItem)
		{
			m_pCheckItem->SetCheck(FALSE);
			m_pCheckItem = NULL;
		}
		s_MenuData->PopMenuEx();
		if (m_pParent)
		{
			m_pParent->OnSubMenuHided(bUncheckParentItem);
		}
	}

	void SMenuEx::HideSubMenu()
	{
		if (m_pCheckItem) m_pCheckItem->HideSubMenu();
	}

	int SMenuEx::OnMouseActivate(HWND wndTopLevel, UINT nHitTest, UINT message)
	{
		return MA_NOACTIVATE;
	}


	void SMenuEx::RunMenu(HWND hRoot)
	{
		SASSERT(s_MenuData);


		BOOL bMsgQuit(FALSE);
		HWND hCurMenu(NULL);

		for (;;)
		{

			if (s_MenuData->IsMenuExited())
			{
				break;
			}

			if (GetForegroundWindow() != hRoot)
			{
				break;
			}
			MSG msg = { 0 };

			if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
			{//获取消息，不从消息队列中移除。
				if (msg.message == WM_KEYDOWN
					|| msg.message == WM_SYSKEYDOWN
					|| msg.message == WM_KEYUP
					|| msg.message == WM_SYSKEYUP
					|| msg.message == WM_CHAR
					|| msg.message == WM_IME_CHAR)
				{
					msg.hwnd = s_MenuData->GetMenuEx()->m_hWnd;
				}
				else if (msg.message == WM_LBUTTONDOWN
					|| msg.message == WM_RBUTTONDOWN
					|| msg.message == WM_NCLBUTTONDOWN
					|| msg.message == WM_NCRBUTTONDOWN
					|| msg.message == WM_LBUTTONDBLCLK
					)
				{
					//click on other window
					if (!s_MenuData->IsMenuWnd(msg.hwnd))
					{
						s_MenuData->ExitMenu(0);
						break;
					}
					else
					{
						SMenuEx *pMenu = s_MenuData->SMenuExFromHwnd(msg.hwnd);
						pMenu->HideSubMenu();
					}
				}
				else if (msg.message == WM_QUIT)
				{
					bMsgQuit = TRUE;
				}

				//移除消息队列中当前的消息。
				MSG msgT;
				::GetMessage(&msgT, 0, 0, 0);

				//拦截非菜单窗口的MouseMove消息
				if (msg.message == WM_MOUSEMOVE)
				{
					if (msg.hwnd != hCurMenu)
					{
						if (hCurMenu)
						{
							::SendMessage(hCurMenu, WM_MOUSELEAVE, 0, 0);
						}
						hCurMenu = msg.hwnd;
					}

					SMenuEx *pMenu = s_MenuData->SMenuExFromHwnd(msg.hwnd);
					if (!pMenu)
					{
						hCurMenu = NULL;
					}

				}

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				MsgWaitForMultipleObjects(0, 0, 0, 10, QS_ALLINPUT);
			}

			if (bMsgQuit)
			{
				PostQuitMessage((int)msg.wParam);
				break;
			}
		}

	}

	BOOL SMenuEx::_HandleEvent(EventArgs *pEvt)
	{
		if (pEvt->sender->IsClass(SMenuExItem::GetClassName()))
		{
			SMenuExItem *pMenuItem = sobj_cast<SMenuExItem>(pEvt->sender);
			if (pEvt->GetID() == EventSwndMouseHover::EventID)
			{
				if (pMenuItem->GetSubMenu() != NULL)
				{
					CSimpleWnd::SetTimer(TIMERID_POPSUBMENU, TIME_PUPSUBMENU);
					m_pHoverItem = pMenuItem;
				}
				HideSubMenu();
				return FALSE;
			}
			else if (pEvt->GetID() == EventSwndMouseLeave::EventID)
			{
				if (pMenuItem->GetSubMenu() != NULL)
				{
					CSimpleWnd::KillTimer(TIMERID_POPSUBMENU);
					m_pHoverItem = NULL;
				}
				return FALSE;
			}

			if (pEvt->GetID() != EventCmd::EventID) return FALSE;
			SASSERT(pMenuItem);
			if (pMenuItem->GetSubMenu())
			{
				PopupSubMenu(pMenuItem, FALSE);
				return FALSE;
			}
			else if (pMenuItem->GetID() == 0)
			{
				return FALSE;
			}
			s_MenuData->ExitMenu(pMenuItem->GetID());
			return TRUE;
		}
		else if (s_MenuData && ::IsWindow(s_MenuData->GetOwner()))
		{
			return (BOOL)::SendMessage(s_MenuData->GetOwner(), UM_MENUEVENT, 0, (LPARAM)pEvt);
		}
		else
		{
			return FALSE;
		}
	}

	void SMenuEx::OnTimer(UINT_PTR timeID)
	{
		if (timeID == TIMERID_POPSUBMENU)
		{
			PopupSubMenu(m_pHoverItem, FALSE);
		}
		else
		{
			SetMsgHandled(FALSE);
		}
	}

	void SMenuEx::OnSubMenuHided(BOOL bUncheckItem)
	{
		SASSERT(m_pCheckItem);
		if (!bUncheckItem)
		{
			m_pCheckItem->SetCheck(FALSE);
			m_pCheckItem = NULL;
		}
	}

	void SMenuEx::PopupSubMenu(SMenuExItem * pItem, BOOL bCheckFirstItem)
	{
		CSimpleWnd::KillTimer(TIMERID_POPSUBMENU);

		SMenuEx * pSubMenu = pItem->GetSubMenu();
		SASSERT(pSubMenu);
		if (!pSubMenu->m_bMenuInitialized)
		{
			int idx = 0;
			SWindow *pPrev = pItem->GetWindow(GSW_PREVSIBLING);
			while (pPrev)
			{
				idx++;
				pPrev = pPrev->GetWindow(GSW_PREVSIBLING);
			}
			pSubMenu->SendInitPopupMenu2Owner(idx);
		}

		CRect rcWnd = GetRoot()->GetWindowRect();
		CRect rcItem = pItem->GetWindowRect();
		rcItem.left = rcWnd.left, rcItem.right = rcWnd.right;
		ClientToScreen(&rcItem);

		m_pCheckItem = pItem;
		m_pCheckItem->SetCheck(TRUE);
		pSubMenu->SetWindowLongPtr(GWLP_HWNDPARENT, (ULONG_PTR)m_hWnd);
		pSubMenu->ShowMenu(0, rcItem.right, rcItem.top);
		if (bCheckFirstItem)
		{
			SMenuExRoot *pMenuRoot = sobj_cast<SMenuExRoot>(pSubMenu->GetRoot()->GetWindow(GSW_FIRSTCHILD));
			SASSERT(pMenuRoot);
			SMenuExItem *pFirstItem = pMenuRoot->GetNextMenuItem(NULL, TRUE);
			if (pFirstItem)
			{
				pSubMenu->m_pCheckItem = pFirstItem;
				pFirstItem->SetCheck(TRUE);
			}
		}
	}

	void SMenuEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		SMenuExRoot *pMenuRoot = sobj_cast<SMenuExRoot>(GetRoot()->GetWindow(GSW_FIRSTCHILD));
		SASSERT(pMenuRoot);
		switch (nChar)
		{
		case VK_UP:
		case VK_DOWN:
			if (m_pCheckItem)
				m_pCheckItem->SetCheck(FALSE);
			m_pCheckItem = pMenuRoot->GetNextMenuItem(m_pCheckItem, nChar == VK_DOWN);
			if (m_pCheckItem)
			{
				m_pCheckItem->SetCheck(TRUE);
				m_pCheckItem->Invalidate();
			}
			break;
		case VK_ESCAPE:
		case VK_LEFT:
			if (m_pParent)
			{
				HideMenu(TRUE);
			}
			else
			{
				s_MenuData->ExitMenu(0);
			}
			break;
		case VK_RIGHT:
			if (m_pCheckItem)
			{
				m_pCheckItem->ShowSubMenu(TRUE);
			}
			break;
		case VK_RETURN:
			if (m_pCheckItem) m_pCheckItem->FireCommand();
			break;
			break;
		default:
			if (isprint(nChar))
			{
				nChar = tolower(nChar);
				SMenuExItem *pMenuItem = (SMenuExItem*)pMenuRoot->GetWindow(GSW_FIRSTCHILD);
				while (pMenuItem)
				{
					if ((UINT)tolower(pMenuItem->GetHotKey()) == nChar)
					{
						pMenuItem->FireCommand();
						return;
					}
					pMenuItem = (SMenuExItem*)pMenuItem->GetWindow(GSW_NEXTSIBLING);
				}
			}
			else
			{
				SetMsgHandled(FALSE);
			}
			break;
		}
	}

	const SStringW & SMenuEx::GetTranslatorContext() const
	{
		if (m_pParent != NULL)
		{
			return m_pParent->GetTrCtx();
		}
		else
		{
			return __super::GetTranslatorContext();
		}
	}

	int SMenuEx::GetScale() const
	{
		if (!s_MenuData) return 100;
		return s_MenuData->GetScale();
	}

	void SMenuEx::ExitPopupMenu(int nCmdId/*=0*/)
	{
		if (!s_MenuData) return;
		s_MenuData->ExitMenu(nCmdId);
		::PostMessage(s_MenuData->GetOwner(), WM_NULL, 0, 0);
	}
	
	SWindow * SMenuEx::FindItem(UINT uPos, UINT uFlag)
	{
		SMenuExRoot *pMenuRoot = sobj_cast<SMenuExRoot>(GetRoot()->GetWindow(GSW_FIRSTCHILD));
		SASSERT(pMenuRoot);
		SWindow * pItemRef = NULL;

		if (uFlag & MF_BYPOSITION)
		{
			if (uPos != -1)
			{
				UINT i = 0;
				SWindow *p = pMenuRoot->GetWindow(GSW_FIRSTCHILD);
				while (i < uPos && p)
				{
					i++;
					p = p->GetWindow(GSW_NEXTSIBLING);
				}
				pItemRef = p;
			}
		}
		else//MF_BYCOMMAND
		{
			pItemRef = pMenuRoot->FindChildByID2<SMenuExItem>(uPos);
		}
		return pItemRef;
	}

	BOOL SMenuEx::InsertMenu(UINT uPos, UINT uFlag, int nId, LPCTSTR lpNewItem)
	{
		SMenuExRoot *pMenuRoot = sobj_cast<SMenuExRoot>(GetRoot()->GetWindow(GSW_FIRSTCHILD));
		SASSERT(pMenuRoot);
		SWindow * pItemRef = FindItem(uPos,uFlag);

		if (uFlag & MF_BYCOMMAND && !pItemRef)
			return FALSE;
        
		SMenuExItem *pMenuItem = (SMenuExItem*)pMenuRoot->CreateMenuItem((uFlag & MF_SEPARATOR) ? SMenuExSep::GetClassName() : SMenuExItem::GetClassName());

        if (pItemRef)
		{
			SWindow *pRefPrev = pItemRef->GetWindow(GSW_PREVSIBLING);
			if (pRefPrev)
			{
				pRefPrev->GetParent()->InsertChild(pMenuItem, pRefPrev);
			}
			else
			{
				pItemRef->GetParent()->InsertChild(pMenuItem, ICWND_FIRST);
			}
		}
		else
		{
			pMenuRoot->InsertChild(pMenuItem);
		}
		if (!(uFlag & MF_SEPARATOR))
		{
			pMenuItem->SetWindowText(lpNewItem);
			if (uFlag & MF_POPUP)
			{
				pMenuItem->m_pSubMenu = new SMenuEx(pMenuItem);
				pMenuItem->m_pSubMenu->IniNullMenu(pMenuRoot);
			}

            pMenuItem->SetAttribute(L"ID", SStringW().Format(L"%d",nId));
			if (uFlag & MF_CHECKED)
			{
				pMenuItem->SetAttribute(L"check", L"1");
			}
		}

		return TRUE;
	}

	BOOL SMenuEx::IniNullMenu(SMenuExRoot *ParentRoot)
	{
		HWND hWnd = Create(NULL, WS_POPUP, WS_EX_TOOLWINDOW | WS_EX_TOPMOST, 0, 0, 0, 0);
		pugi::xml_document souiXml;
		pugi::xml_node root = souiXml.append_child(L"SOUI");
		root.append_attribute(L"translucent").set_value(1);		
		_InitFromXml(root, 0, 0);
		if (!hWnd) return FALSE;
		SMenuExRoot *pMenuRoot = new SMenuExRoot(this);		
		if (ParentRoot)
		{
			//拷贝属性
			ParentRoot->Copy(pMenuRoot);
		}
		InsertChild(pMenuRoot);
		pMenuRoot->SSendMessage(WM_CREATE);
		pMenuRoot->GetLayoutParam()->SetWrapContent(Both);
		return TRUE;
	}

	BOOL SMenuEx::DeleteMenu(UINT uPos, UINT uFlag)
	{
		SMenuExRoot *pMenuRoot = sobj_cast<SMenuExRoot>(GetRoot()->GetWindow(GSW_FIRSTCHILD));
		SASSERT(pMenuRoot);
		SWindow * pItemRef = FindItem(uPos, uFlag);
		if (!pItemRef) return FALSE;
		pMenuRoot->DestroyChild(pItemRef);
		return TRUE;
	}

	BOOL SMenuEx::CheckMenuItem(UINT uPos, UINT uFlag)
	{
		SMenuExRoot *pMenuRoot = sobj_cast<SMenuExRoot>(GetRoot()->GetWindow(GSW_FIRSTCHILD));
		SASSERT(pMenuRoot);
		SWindow * pItemRef = FindItem(uPos, uFlag);
		if (!pItemRef) return FALSE;

		if (uFlag & MF_CHECKED)
		{
			pItemRef->SetAttribute(L"check", L"1");
		}
		else
		{
			pItemRef->SetAttribute(L"check", L"0");
		}
		return TRUE;
	}

	BOOL SMenuEx::CheckMenuRadioltem(UINT idFirst, UINT idLast, UINT idCheck, UINT uFlags)
	{
		SMenuExRoot *pMenuRoot = sobj_cast<SMenuExRoot>(GetRoot()->GetWindow(GSW_FIRSTCHILD));
		SASSERT(pMenuRoot);
		SWindow * pItemRefFirst = FindItem(idFirst, uFlags);
		SWindow * pItemRefLast = FindItem(idLast, uFlags);
		SWindow * pItemRefCheck = FindItem(idCheck, uFlags);

		if (!pItemRefFirst || !pItemRefLast || !pItemRefCheck)
			return FALSE;
		
		int idxFirst = -1;
		int idxLast = -1 ;
		int idxCheck = -1;

		SWindow *pChild = pMenuRoot->GetWindow(GSW_FIRSTCHILD);
		int i = 0;
		while (pChild)
		{
			if (pChild == pItemRefFirst)
				idxFirst = i;
			else if (pChild == pItemRefCheck)
				idxCheck = i;
			else if (pChild == pItemRefLast)
				idxLast = i;
			pChild = pChild->GetWindow(GSW_NEXTSIBLING);
			i++;
		}
		if (idxFirst == -1 || idxLast == -1 || idxCheck == -1)
			return FALSE;
		if (idxFirst < idxLast)
		{
			SWindow *t = pItemRefFirst;
			pItemRefFirst = pItemRefLast;
			pItemRefLast = t;
			int tIdx = idxFirst;
			idxFirst = idxLast;
			idxLast = tIdx;
		}

		if (idxFirst > idxCheck || idxLast < idxCheck)
			return FALSE;
		
		pChild = pItemRefFirst;
		for (;;)
		{
			pChild->SetAttribute(L"radio", L"1");
			if (pChild == pItemRefCheck)
			{
				pChild->SetAttribute(L"check", L"1");
			}
			else
			{
				pChild->SetAttribute(L"check", L"0");
			}
			if (pChild == pItemRefLast) break;
			else pChild = pChild->GetWindow(GSW_NEXTSIBLING);
		}
		return TRUE;
	}

	void SMenuEx::SendInitPopupMenu2Owner(int idx)
	{
		if (m_bMenuInitialized) return;

		if (::IsWindow(s_MenuData->GetOwner()))
		{
			::SendMessage(s_MenuData->GetOwner(), WM_INITMENUPOPUP, (WPARAM)this, (LPARAM)idx);
		}
		m_bMenuInitialized = TRUE;
	}

	DWORD SMenuEx::GetContextHelpId() const
	{
		SMenuExRoot *pMenuRoot = sobj_cast<SMenuExRoot>(GetRoot()->GetWindow(GSW_FIRSTCHILD));
		SASSERT(pMenuRoot);
		return pMenuRoot->m_dwContextHelpId;
	}

	void SMenuEx::SetContextHelpId(DWORD dwId)
	{
		SMenuExRoot *pMenuRoot = sobj_cast<SMenuExRoot>(GetRoot()->GetWindow(GSW_FIRSTCHILD));
		SASSERT(pMenuRoot);
		pMenuRoot->m_dwContextHelpId = dwId;
	}

}
