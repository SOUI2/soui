#include "souistd.h"
#include "control/SDateTimePicker.h"
#include "helper/STime.h"

namespace SOUI
{

	SCalendarEx::SCalendarEx(WORD iYeay, WORD iMonth, WORD iDay)
	{
		Init();
		if (!SetDate(iYeay, iMonth, iDay))
		{
			//CTime tm = CTime::GetCurrentTime();
			SetDate(m_Today.wYear, m_Today.wMonth, m_Today.wDay);
		}
	}

	SCalendarEx::SCalendarEx()
	{
		Init();
		CTime tm = CTime::GetCurrentTime();
		SetDate(tm.GetYear(), tm.GetMonth(), tm.GetDay());
	}

	SCalendarEx::~SCalendarEx()
	{

	}

	void SCalendarEx::Init()
	{
		m_pSkinPrev = GETBUILTINSKIN(SKIN_SYS_BTN_PREV);
		m_pSkinNext = GETBUILTINSKIN(SKIN_SYS_BTN_NEXT);

		m_bFocusable = TRUE;
		GetLocalTime(&m_Today);

		m_evtSet.addEvent(EVENTID(EventCalendarExChanged));

		m_nYearMonthHeight.setSize(30.f, SLayoutSize::dp);
		m_nWeekHeight.setSize(20.f, SLayoutSize::dp);
		m_nFooterHeight.setSize(20.f, SLayoutSize::dp);

		m_crSelText = RGBA(0xFF, 0xFF, 0xFF, 255);
		m_crSelDayBack = RGBA(0x0, 0x7A, 0xCC, 255);
		m_crOtherDayText = RGBA(0xA0, 0xA0, 0xA0, 255);

		// 设置 默认 的 背景色  和 悬浮 字体颜色
		GetStyle().m_crBg = RGBA(255, 255, 255, 255);
		//GetStyle().SetTextColor(1, RGBA(0, 0x7A, 0xCC, 255));
		m_crHoverText = RGBA(0, 0x7A, 0xCC, 255);

		m_pSkinDay = NULL;

	}

	WORD SCalendarEx::GetYear()
	{
		return m_iYear;
	}

	WORD SCalendarEx::GetMonth()
	{
		return m_iMonth;
	}

	WORD SCalendarEx::GetDay()
	{
		if (m_nSelItem < 0 || m_nSelItem >= 42)
			return 1;

		return m_arrDays[m_nSelItem].iDay;
	}

	void SCalendarEx::GetDate(WORD &iYear, WORD &iMonth, WORD &iDay)
	{
		iYear = m_iYear;
		iMonth = m_iMonth;
		iDay = GetDay();
	}

	BOOL SCalendarEx::SetDate(WORD iYear, WORD iMonth, WORD iDay, int nBtnType, bool bNotify)
	{
		short iWeek = SCalendarCore::GetDayOfWeek(iYear, iMonth, 1);		// 计算出 当月 1号 是星期几
		if (iWeek < 0) return FALSE;

		WORD nDayCount = SCalendarCore::GetDaysOfMonth(iYear, iMonth);
				
		if (iWeek > 0)				// 如果 不是星期天 先计算 上个月 的最后几天
		{
			WORD nFrontDayCount = 0;
			if (1 == iMonth)
				nFrontDayCount = SCalendarCore::GetDaysOfMonth(iYear - 1, 12);
			else
				nFrontDayCount = SCalendarCore::GetDaysOfMonth(iYear, iMonth - 1);
			
			
			for (int i=0; i<iWeek; ++i)
			{
				m_arrDays[i].nType = -1;
				m_arrDays[i].iDay = nFrontDayCount - iWeek + i + 1;
			}
		}

		
		// 计算 当月 的 天数 位置
		for (int i=0; i<nDayCount; ++i)
		{
			m_arrDays[iWeek + i].nType = 0;
			m_arrDays[iWeek + i].iDay = i + 1;
		}

		int nNextDay = 1;
		for (int i=iWeek+nDayCount; i<42; ++i)
		{
			m_arrDays[i].nType = 1;
			m_arrDays[i].iDay = nNextDay++;
		}
	
		m_iYear = iYear;
		m_iMonth = iMonth;
	
		m_nHoverItem = HIT_NULL;
		m_nSelItem = iWeek % 7 + iDay - 1;
		Invalidate();

		if (bNotify)
		{
			EventCalendarExChanged evt(this);
			evt.iNewDay = iDay;
			evt.iNewMonth = m_iMonth;
			evt.iNewYear = m_iYear;
			evt.nBtnType = nBtnType;
			FireEvent(evt);
		}
		
		return TRUE;
	}

	void SCalendarEx::SetLastMonth()
	{
		if (1 == m_iMonth)
		{
			m_iMonth = 12;
			--m_iYear;
		}
		else
			--m_iMonth;

		SetDate(m_iYear, m_iMonth, 1, HIT_LEFT, true);
	}

	void SCalendarEx::SetNextMonth()
	{
		if (12 == m_iMonth)
		{
			m_iMonth = 1;
			++m_iYear;
		}
		else
			++m_iMonth;

		SetDate(m_iYear, m_iMonth, 1, HIT_RIGHT, true);
	}
	
	int SCalendarEx::OnCreate(LPVOID)
	{
		__super::OnCreate(NULL);
		
		TCHAR sztext[][3] = { _T("日"),_T("一"),_T("二"),_T("三"),_T("四"),_T("五"),_T("六") };
		for (int i = 0; i < 7; i++)
		{
			m_strWeek[i].SetCtxProvider(this);
			m_strWeek[i].SetText(sztext[i]);
		}

		return 0;
	}

	void SCalendarEx::DrawYearMonth(IRenderTarget* pRT, const CRect& rect)
	{
		int nHeight = m_nYearMonthHeight.toPixelSize(GetScale());

		// 绘制 上一个 按钮
		DWORD dwPrevState = 0;
		if (HIT_LEFT == m_nHoverItem)
		{
			dwPrevState = 1;
		}
				
		if (m_pSkinPrev)
		{
			SIZE si = m_pSkinPrev->GetSkinSize();
			int n = (nHeight - si.cy) / 2;
			CRect rcItem = rect;
			rcItem.left += n;
			rcItem.right = rcItem.left + si.cx;
			rcItem.top += n;
			rcItem.bottom = rcItem.top + si.cy;
			m_pSkinPrev->Draw(pRT, rcItem, dwPrevState);
		}
		
		// 绘制 年月
		COLORREF crText = pRT->GetTextColor();
		if (HIT_YEAR == m_nHoverItem)
		{
			pRT->SetTextColor(m_crHoverText);
		}
		else
			pRT->SetTextColor(crText);

		SStringT szYearMonth;
		szYearMonth.Format(_T("%04d年%d月"), m_iYear, m_iMonth);
		CRect rc = rect;
		pRT->DrawText(szYearMonth, -1, rc, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

		pRT->SetTextColor(crText);

		// 绘制 下一个 按钮
		DWORD dwNextState = 0;
		if (HIT_RIGHT == m_nHoverItem)
		{
			dwNextState = 1;
		}
				
		if (m_pSkinNext)
		{
			SIZE si = m_pSkinNext->GetSkinSize();
			int n = (nHeight - si.cy) / 2;
			CRect rcItem = rect;
			rcItem.right -= n;
			rcItem.left = rcItem.right - si.cx;
			rcItem.top += n;
			rcItem.bottom = rcItem.top + si.cy;
			
			m_pSkinNext->Draw(pRT, rcItem, dwNextState);
		}
	}

	void SCalendarEx::DrawWeek(IRenderTarget* pRT, const CRect& rect)
	{
		int nWid = rect.Width() / 7;
		CRect rcItem = rect;
		rcItem.right = rcItem.left + nWid;

		COLORREF crTxt = pRT->GetTextColor();
		for (int i = 0; i < 7; i++)
		{
			pRT->SetTextColor(crTxt);

			pRT->DrawText(m_strWeek[i].GetText(FALSE), m_strWeek[i].GetText(FALSE).GetLength(), rcItem, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			rcItem.OffsetRect(nWid, 0);
		}
		pRT->SetTextColor(crTxt);
	}

	void SCalendarEx::DrawDay(IRenderTarget* pRT, CRect& rcDay, int nItem)
	{
		if (nItem < 0 || nItem >= 42)
			return;

		wDayInfo& dayInfo = m_arrDays[nItem];

		COLORREF crText = pRT->GetTextColor();
		CPoint ptRound(1, 1);
		
		// 
		
		if (0 != dayInfo.nType)
		{
			pRT->SetTextColor(m_crOtherDayText);
		}
		else if(m_Today.wDay == dayInfo.iDay && m_Today.wYear == m_iYear && m_Today.wMonth == m_iMonth)
		{
			// today 框 就用 textcolor
			pRT->DrawRoundRect(rcDay, ptRound);
		}
		DWORD dwState = 0;
		if (m_nSelItem == nItem)
		{
			if (CR_INVALID != m_crSelDayBack)
				pRT->FillSolidRoundRect(rcDay, ptRound, m_crSelDayBack);

			if(CR_INVALID != m_crSelText)
				pRT->SetTextColor(m_crSelText);

			dwState = 2;
		}
		else if (m_nHoverItem == nItem)				// 
		{
			pRT->SetTextColor(m_crHoverText);
			dwState = 1;
		}
		
		if (NULL != m_pSkinDay)
			m_pSkinDay->Draw(pRT, rcDay, dwState);

		SStringT sDay;
		sDay.Format(_T("%d"), dayInfo.iDay);
		pRT->DrawText(sDay, -1, rcDay, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		
		pRT->SetTextColor(crText);
	}

	void SCalendarEx::DrawToday(IRenderTarget* pRT, CRect& rcDay)
	{
		COLORREF crText = pRT->GetTextColor();
		if (HIT_TODAY == m_nHoverItem)
		{
			pRT->SetTextColor(m_crHoverText);
		}
	
		SStringT szToday;
		szToday.Format(_T("今天:%04d/%d/%d"), m_Today.wYear, m_Today.wMonth, m_Today.wDay);
		//CRect rc = rect;
		m_rcToday = rcDay;
		// 计算 文本 
		pRT->DrawText(szToday, -1, m_rcToday, DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_CALCRECT);
		m_rcToday.bottom = m_rcToday.top + rcDay.Height();	//高度 不变
		m_rcToday.OffsetRect((rcDay.Width() - m_rcToday.Width()) / 2, 0);
		pRT->DrawText(szToday, -1, m_rcToday, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		pRT->SetTextColor(crText);
	}

	void SCalendarEx::OnPaint(IRenderTarget * pRT)
	{
		// 字体 都用 默认 的 
		SPainter painter;
		BeforePaint(pRT, painter);

		CRect rcClient;
		GetClientRect(&rcClient);

		CRect rcYear(rcClient);			// 年月 区域
		rcYear.bottom = rcYear.top + m_nYearMonthHeight.toPixelSize(GetScale());
		DrawYearMonth(pRT, rcYear);

		CRect rcWeek(rcClient);		// 星期 区域
		rcWeek.top = rcYear.bottom;
		rcWeek.bottom = rcWeek.top + m_nWeekHeight.toPixelSize(GetScale());
		DrawWeek(pRT, rcWeek);
		
		// 计算 天 
		m_rcDays = rcClient;
		m_rcDays.top = rcWeek.bottom;
		m_rcDays.bottom = rcClient.bottom - m_nFooterHeight.toPixelSize(GetScale());
		//m_rcDays.bottom;

		CRect rcItem;
		for (int i=0; i<42; ++i)
		{
			GetItemRect(i, rcItem);
			DrawDay(pRT, rcItem, i);
		}
		
		CRect rcToday(rcClient);
		rcToday.top = m_rcDays.bottom;
		DrawToday(pRT, rcToday);

		AfterPaint(pRT, painter);
	}

	void SCalendarEx::OnLButtonDown(UINT nFlags, CPoint point)
	{
		__super::OnLButtonDown(nFlags, point);
		int nItem = HitTest(point);
		if (nItem < 0)
		{
			if (HIT_LEFT == nItem)
				SetLastMonth();
			else if (HIT_RIGHT == nItem)
				SetNextMonth();
			
			return;
		}
		else if (HIT_TODAY == nItem)
		{
			SetDate(m_Today.wYear, m_Today.wMonth, m_Today.wDay, HIT_TODAY, true);
		}
		else if (m_nSelItem != nItem)
		{
			wDayInfo& dayInfo = m_arrDays[nItem];
			
			m_nSelItem = nItem;
			EventCalendarExChanged evt(this);
			evt.nBtnType = nItem;		// 天数 就是 0-41
			evt.iNewDay = dayInfo.iDay;
			if (dayInfo.nType < 0)		// 点击 上一个月 的天
			{
				if (1 == m_iMonth)
				{
					m_iMonth = 12;
					--m_iYear;
				}
				else
					--m_iMonth;
			}
			else if (dayInfo.nType > 0)	//点击 下一个月 的 天
			{
				if (12 == m_iMonth)
				{
					m_iMonth = 1;
					++m_iYear;
				}
				else
					++m_iMonth;
			}

			evt.iNewMonth = m_iMonth;
			evt.iNewYear = m_iYear;
			FireEvent(evt);
		}
		Invalidate();
	}

	void SCalendarEx::OnLButtonUp(UINT nFlags, CPoint point)
	{
		int nItem = HitTest(point);
		if (nItem < 0 || nItem >= 42)					// 只有 点击 天  才 发送 cmd 事件 
		{
			GetEventSet()->setMutedState(true);
		
		}
		
		__super::OnLButtonUp(nFlags, point);
		if (nItem < 0 || nItem >= 42)
		{
			GetEventSet()->setMutedState(false);
		}
	}

	void SCalendarEx::OnMouseMove(UINT nFlags, CPoint pt)
	{
		int nItem = HitTest(pt);
		m_nHoverItem = nItem;
		Invalidate();
	}

	void SCalendarEx::OnMouseLeave()
	{
		m_nHoverItem = HIT_NULL;
		Invalidate();		
	}

	BOOL SCalendarEx::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
	{
		if (zDelta > 0)			// 上滚  上一个月
		{
			SetLastMonth();
		}
		else							// 下滚   下一个 月
		{
			SetNextMonth();
		
		}
		
		
		return TRUE;
	}

	int SCalendarEx::HitTest(const CPoint& pt)
	{
		if (pt.y < m_rcDays.top)				//  小于  表示  在 天  的上面
		{
			if (pt.y > (m_rcDays.top - m_nWeekHeight.toPixelSize(GetScale())))
				return HIT_NULL;

			int nYearHei = m_nYearMonthHeight.toPixelSize(GetScale());
			if ((pt.x - m_rcDays.left) < nYearHei)
				return HIT_LEFT;
			else if ((m_rcDays.right - pt.x) < nYearHei)
				return HIT_RIGHT;
			
			return HIT_YEAR;
		}
		else if (pt.y > m_rcDays.bottom)
		{
			if(m_rcToday.PtInRect(pt))
				return HIT_TODAY;	
			
			return 43;
		}

		int nDay_X = pt.x - m_rcDays.left;
		int nDay_Y = pt.y - m_rcDays.top;
		int nWid = m_rcDays.Width() / 7;
		int nHei = m_rcDays.Height() / 6;

		int nRow = nDay_X / nWid;
		if (nRow > 6)			// 处理 边缘 
			nRow = 6;
		
		return nRow + (nDay_Y / nHei ) * 7;
	}

	void SCalendarEx::GetItemRect(int nItem, CRect& rcItem)
	{
		int nItemWidth = m_rcDays.Width() / 7;
		int nItemHeight = m_rcDays.Height() / 6;

		
		rcItem.left = (nItem % 7) * nItemWidth;
		rcItem.right = rcItem.left + nItemWidth;

		
		rcItem.top = (nItem / 7) * nItemHeight;
		rcItem.bottom = rcItem.top + nItemHeight;
		rcItem.OffsetRect(m_rcDays.TopLeft());
	}
	



	

	//////////////////////////////////////////////////////////////////////////
	SDateTimePicker::SDateTimePicker()
		: m_pDropDownWnd(NULL)
		, m_pSkinBtn(GETBUILTINSKIN(SKIN_SYS_DROPBTN))
		, m_eSelDateType(eDT_NULL)
		, m_nNumWidth(0)
		, m_nCharWidth(0)
		, m_pCalendar(NULL)
		, m_nDropWidth(220)
		, m_bTimeEnable(true)
		, m_wCharNum(0)
	{
		m_evtSet.addEvent(EVENTID(EventDateTimeChanged));
		m_pNcSkin = GETBUILTINSKIN(SKIN_SYS_BORDER);
		m_style.SetAttribute(L"margin", L"1", TRUE);
		m_style.SetAttribute(L"align",L"left",TRUE);
		m_style.SetAttribute(L"valign",L"middle",TRUE);

		m_bFocusable = TRUE;

		GetLocalTime(&m_sysTime);

		m_crSelBg = RGBA(0,120,215,255);
		m_crSelText = RGBA(255,255,255,255);
	}

	SDateTimePicker::~SDateTimePicker()
	{
		if(NULL != m_pCalendar)
		{
			m_pCalendar->SSendMessage(WM_DESTROY);
			delete m_pCalendar;
		}
	}
	
	BOOL SDateTimePicker::CreateChildren(pugi::xml_node xmlNode)
	{
		m_pCalendar = (SCalendarEx*)SApplication::getSingleton().CreateWindowByName(SCalendarEx::GetClassName());
		if (NULL == m_pCalendar)
			return FALSE;
		m_pCalendar->SetContainer(GetContainer());

		pugi::xml_node xmlCal = xmlNode.child(L"calstyle");
		if(xmlCal)
			m_pCalendar->InitFromXml(xmlCal);

		m_pCalendar->SetAttribute(L"pos", L"0,0,-0,-0", TRUE);
		m_pCalendar->SetOwner(this);    //chain notify message to combobox
		//m_pCalendar->SetID(IDC_DROPDOWN_LIST);

		m_pCalendar->GetEventSet()->subscribeEvent(&SDateTimePicker::OnDateChanged, this);
		m_pCalendar->GetEventSet()->subscribeEvent(&SDateTimePicker::OnDateCmd, this);
		return TRUE;
	}

	bool SDateTimePicker::OnDateChanged(EventCalendarExChanged* pEvt)
	{
		if (NULL == pEvt) return true;
				
		m_sysTime.wDay = pEvt->iNewDay;
		m_sysTime.wMonth = pEvt->iNewMonth;
		m_sysTime.wYear = pEvt->iNewYear;

		Invalidate();

		EventDateTimeChanged e(this);
		e.newTime = m_sysTime;
		FireEvent(e);

		return true;
	}

	bool SDateTimePicker::OnDateCmd(EventCmd* pEvt)
	{
		CloseUp();
		return true;
	}

	SWindow* SDateTimePicker::GetDropDownOwner()
	{
		return this;
	}

	void SDateTimePicker::OnCreateDropDown(SDropDownWnd* pDropDown)
	{
		m_pCalendar->SetDate(m_sysTime.wYear, m_sysTime.wMonth, m_sysTime.wDay);
		//pDropDown->SetAttribute(L"sendWheel2Hover", L"1", TRUE);
		pDropDown->InsertChild(m_pCalendar);
		pDropDown->UpdateChildrenPosition();
		
		m_pCalendar->SetVisible(TRUE);
		m_pCalendar->SetFocus();
		
		m_dwBtnState = WndState_PushDown;
		CRect rcBtn;
		GetDropBtnRect(&rcBtn);
		InvalidateRect(rcBtn); 
	}

	void SDateTimePicker::OnDestroyDropDown(SDropDownWnd* pDropDown)
	{
		pDropDown->RemoveChild(m_pCalendar);
		
		m_pCalendar->SetVisible(FALSE);
		m_pCalendar->SetContainer(GetContainer());

		m_dwBtnState = WndState_Normal;
		m_pDropDownWnd = NULL;
	}

	void SDateTimePicker::GetDropBtnRect(LPRECT pBtnRc, LPRECT pSkinRc)
	{
		SIZE szBtn = m_pSkinBtn->GetSkinSize();
		GetClientRect(pBtnRc);
		pBtnRc->left = pBtnRc->right - (pBtnRc->bottom - pBtnRc->top);			// 取 右边的正方形 为  按钮 区域

		if(NULL == pSkinRc)
			return ;

		int n = (pBtnRc->bottom - pBtnRc->top - szBtn.cy) / 2;			// 按钮皮肤 则 要居中
		
		pSkinRc->right = pBtnRc->right - n;
		pSkinRc->left = pSkinRc->right - szBtn.cx;

		pSkinRc->top = pBtnRc->top + n;
		pSkinRc->bottom = pSkinRc->top + szBtn.cy;
	}

	SStringT SDateTimePicker::ToFormatText(EnDateType eType, WORD wNum)
	{
		SStringT szNum;
		if (eType == m_eSelDateType)						// 选中项
		{
			if (eDT_Year == eType)
			{
				if(m_wCharNum < 10)
					szNum.Format(_T("%d"), wNum);
				else if (m_wCharNum < 100)
					szNum.Format(_T("%02d"), wNum);
				else if (m_wCharNum < 1000)
					szNum.Format(_T("%03d"), wNum);
				else
					szNum.Format(_T("%04d"), wNum);
			}
			else
			{
				if (m_wCharNum > 0)
					szNum.Format(_T("%d"), wNum);
				else
					szNum.Format(_T("%02d"), wNum);
			}
		}
		else
		{
			if (eDT_Year == eType)
			{
				szNum.Format(_T("%04d"), wNum);
			}
			else
			{
				szNum.Format(_T("%02d"), wNum);
			}
		}
				
		return szNum;
	}

	void SDateTimePicker::Draw(EnDateType eType, IRenderTarget* pRT, WORD wNum, CRect& rcText)
	{
		SStringT szNum = ToFormatText(eType, wNum);
		
		if(eType != m_eSelDateType)
		{
			pRT->DrawText(szNum, szNum.GetLength(), rcText, GetTextAlign());
			return ;
		}

		// 选中 的 填充 字体颜色 变 白
		pRT->FillSolidRect(rcText, m_crSelBg);

		COLORREF old = pRT->SetTextColor(m_crSelText);
		pRT->DrawText(szNum, szNum.GetLength(), rcText, GetTextAlign());
		pRT->SetTextColor(old);
	}

	void SDateTimePicker::OnPaint(IRenderTarget* pRT)
	{
		//__super::OnPaint(pRT);
		SPainter painter;

		BeforePaint(pRT, painter);
		
		CRect rcText;
		GetTextRect(rcText);
		if(0 == m_nNumWidth)
		{
			SIZE sz;
			pRT->MeasureText(_T("5"), 1, &sz);
			m_nNumWidth = sz.cx;
			m_nNumHeight = sz.cy;

			pRT->MeasureText(_T("-"), 1, &sz);
			m_nCharWidth = sz.cx;

		}
		rcText.OffsetRect(0, (rcText.Height() - m_nNumHeight)/2 + 1);
		rcText.bottom = rcText.top + m_nNumHeight;
		

		//SStringT strText;
		
		// year
		rcText.right = rcText.left + m_nNumWidth * 4;
		//strText.Format(_T("%d"), m_sysTime.wYear);
		Draw(eDT_Year, pRT, m_sysTime.wYear, rcText);
		
		
		rcText.left = rcText.right;
		rcText.right = rcText.left + m_nCharWidth;
		pRT->DrawText(_T("-"), -1, rcText, GetTextAlign());
		
		//month
		rcText.left = rcText.right;
		rcText.right = rcText.left + m_nNumWidth * 2;
		//strText.Format(_T("%02d"), m_sysTime.wMonth);
		Draw(eDT_Month, pRT, m_sysTime.wMonth, rcText);
		
		rcText.left = rcText.right;
		rcText.right = rcText.left + m_nCharWidth;
		pRT->DrawText(_T("-"), -1, rcText, GetTextAlign());
		
		//day
		rcText.left = rcText.right;
		rcText.right = rcText.left + m_nNumWidth * 2;
		//strText.Format(_T("%02d"), m_sysTime.wDay);
		Draw(eDT_Day, pRT, m_sysTime.wDay, rcText);
	
		//rcText.left = rcText.right;
		//rcText.right = rcText.left + m_nCharWidth;
		//pRT->DrawText(L" ", -1, rcText, GetTextAlign());
		if (m_bTimeEnable)
		{
			// hour
			rcText.left = rcText.right + m_nCharWidth;
			rcText.right = rcText.left + m_nNumWidth * 2;
			//strText.Format(_T("%02d"), m_sysTime.wHour);
			Draw(eDT_Hour, pRT, m_sysTime.wHour, rcText);

			rcText.left = rcText.right;
			rcText.right = rcText.left + m_nCharWidth;
			pRT->DrawText(_T(":"), -1, rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			// minute
			rcText.left = rcText.right;
			rcText.right = rcText.left + m_nNumWidth * 2;
			//strText.Format(_T("%02d"), m_sysTime.wMinute);
			Draw(eDT_Minute, pRT, m_sysTime.wMinute, rcText);

			rcText.left = rcText.right;
			rcText.right = rcText.left + m_nCharWidth;
			pRT->DrawText(_T(":"), -1, rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			// second
			rcText.left = rcText.right;
			rcText.right = rcText.left + m_nNumWidth * 2;
			//strText.Format(_T("%02d"), m_sysTime.wSecond);
			Draw(eDT_Second, pRT, m_sysTime.wSecond, rcText);
		}
		
	
		
		AfterPaint(pRT, painter);

		CRect rcBtn;
		CRect rcSkin;
		GetDropBtnRect(&rcBtn, &rcSkin);
		if(WndState_Hover == m_dwBtnState)
		{
			rcBtn += CRect(1,1,1,1);
			m_pNcSkin->Draw(pRT, rcBtn, 1);
		}
		int nState = IIF_STATE3(m_dwBtnState, WndState_Normal, WndState_Hover, WndState_PushDown);
		m_pSkinBtn->Draw(pRT, rcSkin, nState);
	}

	bool SDateTimePicker::CalcPopupRect(int nWidth, CRect& rcPopup)
	{
		CRect rcWnd = GetWindowRect();
		GetContainer()->FrameToHost(rcWnd);

		ClientToScreen(GetContainer()->GetHostHwnd(), (LPPOINT)&rcWnd);
		ClientToScreen(GetContainer()->GetHostHwnd(), ((LPPOINT)&rcWnd)+1);

		HMONITOR hMonitor = ::MonitorFromWindow(GetContainer()->GetHostHwnd(), MONITOR_DEFAULTTONULL);
		CRect rcMonitor;
		if (hMonitor)
		{
			MONITORINFO mi = {sizeof(MONITORINFO)};
			::GetMonitorInfo(hMonitor, &mi);
			rcMonitor = mi.rcMonitor;
		}
		else
		{
			rcMonitor.right = GetSystemMetrics(   SM_CXSCREEN   );   
			rcMonitor.bottom = GetSystemMetrics(   SM_CYSCREEN   );
		}

		int nHeight = nWidth * 4 / 5;

		if(rcWnd.bottom+nHeight <= rcMonitor.bottom)
		{
			rcPopup.SetRect(rcWnd.right-nWidth, rcWnd.bottom, rcWnd.right, rcWnd.bottom+nHeight);
			return true;
		}
		

		rcPopup.SetRect(rcWnd.right- nWidth, rcWnd.top-nHeight, rcWnd.right, rcWnd.top);
		return false;		
	}

	SDateTimePicker::EnDateType SDateTimePicker::HitTest(CPoint pt)
	{
		CRect rcText;
		GetTextRect(rcText);
		int nWidth = pt.x - rcText.left;

		EnDateType eSelType = eDT_NULL;

		if (nWidth < (4 * m_nNumWidth + m_nCharWidth))
		{
			eSelType = eDT_Year;			// year
		}
		else if (nWidth < (6 * m_nNumWidth + 2 * m_nCharWidth))
		{
			eSelType = eDT_Month;			// month
		}
		else if (!m_bTimeEnable || nWidth < (8 * m_nNumWidth + 3 * m_nCharWidth))
		{
			eSelType = eDT_Day;			// day
		}
		else if (nWidth < (10 * m_nNumWidth + 4 * m_nCharWidth))
		{
			eSelType = eDT_Hour;			// hour
		}
		else if (nWidth < (12 * m_nNumWidth + 5 * m_nCharWidth))
		{
			eSelType = eDT_Minute;			// minute
		}
		else
		{
			eSelType = eDT_Second;			// second
		}

		return eSelType;
	}

	void SDateTimePicker::SetTime(const SYSTEMTIME& sysTime)
	{
		m_sysTime = sysTime;
		Invalidate();
	}

	void SDateTimePicker::SetTime(WORD wYear, WORD wMonth, WORD wDay, WORD wHour, WORD wMinute, WORD wSecond)
	{
		m_sysTime.wYear = wYear;
		m_sysTime.wMonth = wMonth;
		m_sysTime.wDay = wDay;
		m_sysTime.wHour = wHour;
		m_sysTime.wMinute = wMinute;
		m_sysTime.wSecond = wSecond;
		Invalidate();
	}

	void SDateTimePicker::GetTime(SYSTEMTIME& sysTime)
	{
		sysTime = m_sysTime;
	}

	SStringT SDateTimePicker::GetWindowText(BOOL bRawText)
	{
		(bRawText);
		SStringT szText;
		szText.Format(_T("%04d-%02d-%02d"), m_sysTime.wYear, m_sysTime.wMonth, m_sysTime.wDay);
		if(m_bTimeEnable)
			szText.AppendFormat(_T(" %02d:%02d:%02d"), m_sysTime.wHour, m_sysTime.wMinute, m_sysTime.wSecond);

		return szText;
	}

	void SDateTimePicker::OnLButtonDown(UINT nFlags, CPoint pt)
	{
		SetFocus();
		if(WndState_PushDown == m_dwBtnState) return;

		m_wCharNum = 0;

		CRect rcBtn;
		GetDropBtnRect(&rcBtn);
		if(!rcBtn.PtInRect(pt))				// 没有点击 按钮 
		{
			__super::OnLButtonDown(nFlags, pt);

			EnDateType  eSelType = HitTest(pt);
			if(m_eSelDateType != eSelType)
			{
				m_eSelDateType = eSelType;
				Invalidate();
			}

			return ;
		}

		m_eSelDateType = eDT_NULL;
		Invalidate();

		if(NULL != m_pDropDownWnd)
			return ;

		m_pDropDownWnd = new SDropDownWnd_ComboBox(this);		// 直接 用这个 处理 wheel 滚动
		CRect rcPopup;
		SLayoutSize nWid;
		nWid.setSize((float)m_nDropWidth, SLayoutSize::dp);
		bool bDown = CalcPopupRect(nWid.toPixelSize(GetScale()), rcPopup);
		m_pDropDownWnd->Create(rcPopup, 0);

		//if(m_nAnimTime>0)
		//	m_pDropDownWnd->AnimateHostWindow(m_nAnimTime,AW_SLIDE|(bDown?AW_VER_POSITIVE:AW_VER_NEGATIVE));
		//else
			m_pDropDownWnd->SetWindowPos(HWND_TOP,0,0,0,0,SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);

		m_pDropDownWnd->CSimpleWnd::SetCapture();

	}

	void SDateTimePicker::OnMouseMove(UINT nFlags, CPoint pt)
	{
		if(WndState_PushDown == m_dwBtnState)
			return;

		__super::OnMouseHover(nFlags, pt);
		
		CRect rcBtn;
		GetDropBtnRect(&rcBtn);
		
		if(rcBtn.PtInRect(pt))
		{
			m_dwBtnState = WndState_Hover;
			InvalidateRect(rcBtn);
		}
		else if(WndState_Hover == m_dwBtnState)
		{
			m_dwBtnState = WndState_Normal;
			InvalidateRect(rcBtn);
		}
	}

	void SDateTimePicker::OnMouseLeave()
	{
		if(WndState_PushDown == m_dwBtnState) 
			return;

		if(GetState() & WndState_Hover) 
			__super::OnMouseLeave();

		if(WndState_Hover == m_dwBtnState)
		{
			m_dwBtnState = WndState_Normal;
			
			CRect rcBtn;
			GetDropBtnRect(&rcBtn);
			InvalidateRect(rcBtn);
		}
	}

	BOOL SDateTimePicker::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
	{
		TimeWheel(zDelta > 0);		
		return TRUE;
	}

	void SDateTimePicker::TimeWheel(bool bUp)
	{
		switch (m_eSelDateType)
		{
		case eDT_Year:
			bUp ? ++m_sysTime.wYear : --m_sysTime.wYear;
			break;
		case eDT_Month:
		{
			CircluNum(bUp, m_sysTime.wMonth, 1, 12);
			break;
		}
		case eDT_Day:
		{
			int nDays = SCalendarCore::GetDaysOfMonth(m_sysTime.wYear, m_sysTime.wMonth);
			CircluNum(bUp, m_sysTime.wDay, 1, nDays);
			break;
		}
		case eDT_Hour:
		{
			CircluNum(bUp, m_sysTime.wHour, 0, 23);
			break;
		}
		case eDT_Minute:
		{
			CircluNum(bUp, m_sysTime.wMinute, 0, 59);
			break;
		}
		case eDT_Second:
		{
			CircluNum(bUp, m_sysTime.wSecond, 0, 59);
			break;
		}
		default:
			break;
		}

		if (!SCalendarCore::DateCheck(m_sysTime.wYear, m_sysTime.wMonth, m_sysTime.wDay))
		{
			WORD nDays = SCalendarCore::GetDaysOfMonth(m_sysTime.wYear, m_sysTime.wMonth);
			m_sysTime.wDay = nDays;
		}

		m_wCharNum = 0;
		Invalidate();
	}
	
	void SDateTimePicker::CircluNum(bool bUp, WORD& wNum, WORD wMin/*=1*/, WORD wMax/*=0*/)
	{
		if (bUp)
		{
			if (wNum >= wMax)
				wNum = wMin;
			else
				++wNum;
		}
		else
		{
			if (wNum <= wMin)
				wNum = wMax;
			else
				--wNum;
		}
	}
	
	void SDateTimePicker::OnKeyDown(TCHAR nChar, UINT nRepCnt, UINT nFlags)
	{
		if (37 == nChar)				// <-
		{
			if (eDT_Year == m_eSelDateType)
				return;

			m_eSelDateType = static_cast<EnDateType>((int)m_eSelDateType - 1);
			m_wCharNum = 0;
			Invalidate();
		}
		else if (39 == nChar)
		{
			if (eDT_Second == m_eSelDateType)
				return;

			m_eSelDateType = static_cast<EnDateType>((int)m_eSelDateType + 1);
			m_wCharNum = 0;
			Invalidate();
		}
		else if (38 == nChar)
			TimeWheel(true);
		else if(40 == nChar)
			TimeWheel(false);		
	}

	void SDateTimePicker::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		if(nChar > '9' || nChar < '0')
			return ;

		if(eDT_NULL == m_eSelDateType)
			return ;
		// 这个 不写了   谁帮我写了吧
		
		int nNum = nChar - 48;
		if (m_wCharNum > 0)
		{
			m_wCharNum *= 10;
			m_wCharNum += nNum;
		}
		else
		{
			m_wCharNum = nNum;
		}

		//SYSTEMTIME oldTime = m_sysTime;

		switch(m_eSelDateType)
		{
		case eDT_Year :
			if (m_wCharNum < 10)
			{
				m_sysTime.wYear = m_sysTime.wYear / 10 * 10 + m_wCharNum;
			}
			else if(m_wCharNum < 100)
			{
				m_sysTime.wYear = m_sysTime.wYear / 100 * 100 + m_wCharNum;
			}
			else if(m_wCharNum < 1000)
			{
				m_sysTime.wYear = m_sysTime.wYear / 100 * 100 + m_wCharNum;
			}
			else if(m_wCharNum < 1601 || m_wCharNum > 9999)
			{
				return;
			}
			else
				m_sysTime.wYear = m_wCharNum;
			break;
		case eDT_Month :
			if (m_wCharNum <= 0)
			{
				return;
			}
			else if (m_wCharNum > 12)
				m_wCharNum = nNum;
			else if (!SCalendarCore::DateCheck(m_sysTime.wYear, m_wCharNum, m_sysTime.wDay))
				m_sysTime.wDay = SCalendarCore::GetDaysOfMonth(m_sysTime.wYear, m_wCharNum);
			m_sysTime.wMonth = m_wCharNum;
			break;
		case eDT_Day :
			if (m_wCharNum <= 0)
			{
				return;
			}
			if (!SCalendarCore::DateCheck(m_sysTime.wYear, m_sysTime.wMonth, m_wCharNum))
				m_wCharNum = nNum;
			m_sysTime.wDay = m_wCharNum;
			break;
		case eDT_Hour :
			if (m_wCharNum > 23 || m_wCharNum < 0)
				m_wCharNum = nNum;
			m_sysTime.wHour = m_wCharNum;
			break;
		case eDT_Minute :
			if (m_wCharNum > 59 || m_wCharNum < 0)
				m_wCharNum = nNum;
			m_sysTime.wMinute = m_wCharNum;
			break;
		case eDT_Second :
			if (m_wCharNum > 59 || m_wCharNum < 0)
				m_wCharNum = nNum;
			m_sysTime.wSecond = m_wCharNum;
			break;
		default:
		break;
		}
		
		if (eDT_NULL != m_eSelDateType)
			Invalidate();
	}

	void SDateTimePicker::OnDestroy()
	{
		CloseUp();
		__super::OnDestroy();
	}

	BOOL SDateTimePicker::IsFocusable()
	{
		if (m_bDropdown && m_bFocusable)
			return TRUE;
		return FALSE;
	}

	void SDateTimePicker::OnSetFocus(SWND wndOld)
	{
		__super::OnSetFocus(wndOld);
	}

	void SDateTimePicker::OnKillFocus(SWND wndFocus)
	{
		__super::OnKillFocus(wndFocus);
		CloseUp();
		return;
		m_wCharNum = 0;
		if (eDT_NULL != m_eSelDateType)
		{
			m_eSelDateType = eDT_NULL;
			Invalidate();
		}
		
	}

	void SDateTimePicker::DrawFocus(IRenderTarget *pRT)
	{

	}

	

	void SDateTimePicker::CloseUp()
	{
		if(NULL != m_pDropDownWnd)
		{
			m_pDropDownWnd->EndDropDown(IDCANCEL);
		}
	}

} // SOUI
