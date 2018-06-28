/*
基本 是 按 mfc  的 datetime 实现的 只实现了 天数 选择  月 和 年 没做
月 和 年 一般也用的少。 选择也可以。
*/

#pragma once
#include <core/swnd.h>

namespace SOUI
{
	// 按钮 宏定义
#define HIT_NULL				-1				// 无
#define HIT_LEFT				-10			// 上一个月 按钮
#define HIT_RIGHT				-11			// 下一个月 按钮
#define HIT_YEAR				-12			//  年月  还没用到
#define HIT_TODAY				42				//  今天 

class SOUI_EXP SCalendarEx : public SWindow
{
	SOUI_CLASS_NAME(SCalendarEx, L"calendarex")   //
public:
	SCalendarEx(WORD iYeay, WORD iMonth, WORD iDay);
	SCalendarEx();
	~SCalendarEx();
	WORD GetYear();
	WORD GetMonth();
	WORD GetDay();
	void GetDate(WORD &iYear, WORD &iMonth, WORD &iDay);
	BOOL SetDate(WORD iYear, WORD iMonth, WORD iDay, int nBtnType=HIT_NULL, bool bNotify=false);
protected:
	void Init();
	int OnCreate(LPVOID);
	void OnPaint(IRenderTarget *pRT);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint pt);
	void OnMouseLeave();
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	SOUI_ATTRS_BEGIN()
		ATTR_LAYOUTSIZE(L"yearHeight", m_nYearMonthHeight, FALSE)
		ATTR_LAYOUTSIZE(L"weekHeight", m_nWeekHeight, FALSE)
		ATTR_LAYOUTSIZE(L"todayHeight", m_nFooterHeight, FALSE)
		ATTR_COLOR(L"colorSelText", m_crOtherDayText, TRUE)
		ATTR_COLOR(L"colorOtherText", m_crOtherDayText, TRUE)
		ATTR_COLOR(L"colorSelBg", m_crSelDayBack, TRUE)
		ATTR_COLOR(L"colorHoverText", m_crHoverText, TRUE)
		ATTR_SKIN(L"prevSkin", m_pSkinPrev, FALSE)
		ATTR_SKIN(L"nextSkin", m_pSkinNext, FALSE)
		ATTR_SKIN(L"daySkin", m_pSkinDay, FALSE)
	SOUI_ATTRS_END()

	SOUI_MSG_MAP_BEGIN()
		MSG_WM_CREATE(OnCreate)
		MSG_WM_PAINT_EX(OnPaint)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
		MSG_WM_MOUSEWHEEL(OnMouseWheel)
	SOUI_MSG_MAP_END()
protected:
	// 定位 小于0  是 年月 3个按钮 暂时 没完成   0 - 41  天    42 today按钮
	int HitTest(const CPoint& pt);
	void DrawYearMonth(IRenderTarget* pRT, const CRect& rect);
	void DrawWeek(IRenderTarget* pRT, const CRect& rect);
	void DrawDay(IRenderTarget* pRT, CRect& rcDay, int nItem);
	void DrawToday(IRenderTarget* pRT, CRect& rcDay);
	void GetItemRect(int nItem, CRect& rcItem);
	void SetLastMonth();
	void SetNextMonth();
protected:
	SLayoutSize			m_nYearMonthHeight;   //年月
	SLayoutSize			m_nWeekHeight;				//星期高度
	SLayoutSize			m_nFooterHeight;			
	
	COLORREF				m_crSelText;
	COLORREF				m_crOtherDayText;			//其他 天 的 字体颜色 
	COLORREF				m_crSelDayBack;			// 选中 的 天 背颜色
	COLORREF				m_crHoverText;
	
	ISkinObj*				m_pSkinPrev;
	ISkinObj*				m_pSkinNext;
	ISkinObj*				m_pSkinDay;					// 天 皮肤 

	STrText					m_strWeek[7];  /**< 表头文本 */
	
	struct wDayInfo
	{
		WORD			iDay;				// 日历 天 
		int					nType;			// -1 前一个月 0 当月 1 下一个月
	};
	wDayInfo				m_arrDays[42];
	CRect						m_rcDays;
	CRect						m_rcToday;

	int							m_nSelItem;
	int							m_nHoverItem;

	WORD					m_iYear;
	WORD					m_iMonth;
	SYSTEMTIME			m_Today;
};
    //
    // 日期 选择控件
    //
class SOUI_EXP SDateTimePicker : public SWindow, public ISDropDownOwner
{
	SOUI_CLASS_NAME(SDateTimePicker, L"dateTimePicker")   //

public:
	enum EnDateType
	{
		eDT_NULL = 0,
		eDT_Year ,
		eDT_Month ,
		eDT_Day ,
		eDT_Hour ,
		eDT_Minute ,
		eDT_Second ,
	};
	SDateTimePicker();
    ~SDateTimePicker();

public:
	void CloseUp();
	EnDateType HitTest(CPoint pt);
	void SetTime(const SYSTEMTIME& sysTime);
	void GetTime(SYSTEMTIME& sysTime);
	void SetTime(WORD wYear, WORD wMonth, WORD wDay, WORD wHour, WORD wMinute, WORD wSecond);
	SStringT GetWindowText(BOOL bRawText=FALSE);
protected:		// 继承 
	virtual SWindow* GetDropDownOwner();
	virtual void OnCreateDropDown(SDropDownWnd* pDropDown);
	virtual void OnDestroyDropDown(SDropDownWnd* pDropDown);
protected:
	virtual BOOL CreateChildren(pugi::xml_node xmlNode);
	bool OnDateChanged(EventCalendarExChanged* pEvt);
	bool OnDateCmd(EventCmd* pEvt);
	void GetDropBtnRect(LPRECT pBtnRc, LPRECT pSkinRc = NULL);
	SStringT ToFormatText(EnDateType eType, WORD wNum);
	bool CalcPopupRect(int nHeight, CRect& rcPopup);
	void Draw(EnDateType eType, IRenderTarget* pRT, WORD wNum, CRect& rcText);
protected:
	void OnPaint(IRenderTarget* pRT);
	void OnLButtonDown(UINT nFlags, CPoint pt);
	void OnMouseMove(UINT nFlags, CPoint pt);
	void OnMouseLeave();
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void OnKeyDown( TCHAR nChar, UINT nRepCnt, UINT nFlags );
	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnDestroy();
	BOOL IsFocusable();
	void OnSetFocus(SWND wndOld);
	void OnKillFocus(SWND wndFocus);
	void DrawFocus(IRenderTarget *pRT);
	void TimeWheel(bool bUp);
	void CircluNum(bool bUp, WORD& wNum, WORD wMin, WORD wMax);				// 循环 子 
	
	SOUI_ATTRS_BEGIN()
		ATTR_SKIN(L"btnSkin", m_pSkinBtn, FALSE)
		ATTR_BOOL(L"timeEnable", m_bTimeEnable, FALSE)
		ATTR_INT(L"dropWidth", m_nDropWidth, FALSE)
	SOUI_ATTRS_END()

	SOUI_MSG_MAP_BEGIN()
		MSG_WM_PAINT_EX(OnPaint)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)        
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
		MSG_WM_MOUSEWHEEL(OnMouseWheel)
		MSG_WM_KEYDOWN(OnKeyDown) 
		MSG_WM_CHAR(OnChar)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SETFOCUS_EX(OnSetFocus)
		MSG_WM_KILLFOCUS_EX(OnKillFocus)
	SOUI_MSG_MAP_END()

protected:
	
	DWORD								m_dwBtnState;  /**< 按钮状态      */
	ISkinObj*							m_pSkinBtn;    /**< 按钮资源      */
	bool									m_bDropdown;        /**< 是否按下   */
	SDropDownWnd*				m_pDropDownWnd;  /**< DropDown指针 */
	EnDateType						m_eSelDateType;
	int										m_nNumWidth;
	int										m_nNumHeight;
	int										m_nCharWidth;
	COLORREF							m_crSelBg;
	COLORREF							m_crSelText;
	SYSTEMTIME						m_sysTime;
	SStringT								m_sKey;

	int										m_nDropWidth;
	bool									m_bTimeEnable;				// 是否 有 时 分 秒
	SCalendarEx*						m_pCalendar;
	WORD								m_wCharNum;
};
}