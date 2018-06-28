/**
 * Copyright (C) 2014-2050 SOUI团队
 * All rights reserved.
 *
 * @file       SCalendar.cpp
 * @brief      SCalendarCore以及SCalendar类源文件
 * @version    v1.0
 * @author     soui
 * @date       2014-05-25
 *
 * Describe  时间控件相关函数实现
 */
#include "souistd.h"
#include "control/SCalendar.h"
#include "helper/STime.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SCalendarCore

namespace SOUI{

extern WORD gLunarMonthDay[];
extern BYTE gLunarMonth[];
extern BYTE gLunarHolDay[];

#define REFTYPE_SUJIU 1		//数九
#define REFTYPE_MEIYU 2		//梅雨
#define REFTYPE_SANFU 3		//三伏

#define TITLE_HEIGHT 20.f
#define FOOTER_HEIGHT 20.f

//两个子控件的名字
#define NAME_BTN_TODAY    "btn_today"
#define NAME_LABEL_TODAY "label_today"

WORD const DayOrdinal[13]={0,31,59,90,120,151,181,212,243,273,304,334,365};
WORD const DayOrdinalLeap[13]={0,31,60,91,121,152,182,213,244,274,305,335,366};

BOOL SCalendarCore::IsLeapYear(WORD wYear,BOOL &bLeapYear)
{
	if (wYear<1600||wYear>=7000)//压缩算法规定了的年份区间（提取器只能导出此区间的数据，Lunar.dll支持-6000到20000（不含20000）之间的年份）
	{
		return FALSE;
	}
	if (wYear%4==0&&wYear%100!=0||wYear%400==0)//判断闰年的条件
	{
		bLeapYear=TRUE;
	}else
	{
		bLeapYear=FALSE;
	}
	return TRUE;
}

BOOL SCalendarCore::GetDaysNumInYear(WORD wYear, WORD wMonth, WORD wDay,WORD &wDays)
{
	//从日期算出距离元旦的天数
	if (DateCheck(wYear,wMonth,wDay)==0)//对输入的日期进行检查
	{
		return FALSE;
	}
	BOOL bLeapYear=FALSE;
	if (!IsLeapYear(wYear,bLeapYear))
	{
		return FALSE;
	}
	if (bLeapYear==TRUE)
	{
		wDays=DayOrdinalLeap[wMonth-1]+wDay-1;//元旦为序数0，因此减1
	}else
	{
		wDays=DayOrdinal[wMonth-1]+wDay-1;
	}
	return TRUE;
}

BOOL SCalendarCore::GetDateFromDays(WORD wYear, WORD wDays, WORD &wMonth, WORD &wDay)
{
	//从距离元旦的天数算出日期
	if (wDays<1)
	{
		return FALSE;
	}
	BOOL bLeapYear=FALSE;
	if (!IsLeapYear(wYear,bLeapYear))
	{
		return FALSE;
	}
	if (bLeapYear==TRUE)
	{
		if (wDays>366)//超出了该年的总天数
		{
			return FALSE;
		}
	}else
	{
		if (wDays>365)
		{
			return FALSE;
		}
	}
	wMonth=0;
	int i=0;
	for (i=0;i<12;i++)
	{
		if (bLeapYear==TRUE)
		{
			if (wDays>=DayOrdinalLeap[i]&&wDays<DayOrdinalLeap[i+1])//找出月份
			{
				wMonth=i+1;
				wDay=wDays-DayOrdinalLeap[i];//计算出“日”
				break;
			}
		}else
		{
			if (wDays>=DayOrdinal[i]&&wDays<DayOrdinal[i+1])
			{
				wMonth=i+1;
				wDay=wDays-DayOrdinal[i];
				break;
			}
		}
	}
	return TRUE;
}

BOOL SCalendarCore::DateCheck(WORD wYear,WORD wMonth,WORD wDay)
{
	if (wMonth > 12 || wMonth < 1)
		return FALSE;

	if (wDay < 1 || wDay > 31)
		return FALSE;

	if (wMonth == 4 || wMonth == 6 || wMonth == 9 || wMonth == 11)
	{
		if (wDay > 30)
		{
			return FALSE;
		}
	}
	else if (wMonth == 2)
	{
		BOOL bLeapYear = FALSE;
		IsLeapYear(wYear, bLeapYear);
		if (bLeapYear == FALSE)
		{
			if (wDay > 28)
			{
				return FALSE;
			}
		}else
		{
			if (wDay > 29)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}


short SCalendarCore::GetDayOfWeek(WORD wYear,WORD wMonth,WORD wDay)
{
	WORD uDayOrd=0;
	if (GetDaysNumInYear(wYear,wMonth,wDay,uDayOrd)==0)
	{
		return -1;
	}
	unsigned int DayofWeek=0;
	uDayOrd++;//一年中的第几天，因为GetDaysNumInYear所得到的是索引，因此要加一
	wYear--;
	DayofWeek=(wYear+wYear/4-wYear/100+wYear/400+uDayOrd)%7;//这个只是算星期的通用公式
	return DayofWeek;
}

WORD SCalendarCore::GetDaysOfMonth(WORD wYear,WORD wMonth)
{
	if (wMonth==12)
	{
		return 31;//这里为了简便，判断12月就直接返回
	}
	WORD days1=0,days2=0;
	WORD ret=0;
	ret=GetDaysNumInYear(wYear,wMonth,1,days1);//本月1日在年内的序数
	if (ret==0)
	{
		return ret; 
	}
	wMonth++;
	ret=GetDaysNumInYear(wYear,wMonth,1,days2);//下个月1日的年内序数
	if (ret==0)
	{
		return ret; 
	}
	ret=days2-days1;//下个月1日的序数减本月1日的序数
	return ret;
}

SStringT SCalendarCore::FormatYear(WORD  iYear)
{
    return SStringT().Format(_T("%d"),iYear);
}

SStringT SCalendarCore::FormatMonth(WORD iMonth)
{
    TCHAR szText[][5]={_T("一"),_T("二"),_T("三"),_T("四"),_T("五"),_T("六"),_T("七"),_T("八"),_T("九"),_T("十"),_T("十一"),_T("十二")};
    SASSERT(iMonth<=12 && iMonth>=1);
    return szText[iMonth-1];
}

SStringT SCalendarCore::FormatDay(WORD iDay)
{
    return SStringT().Format(_T("%d"),iDay);
}

//////////////////////////////////////////////////////////////////////////
//    SCalendarCore
//////////////////////////////////////////////////////////////////////////

SCalendar::SCalendar(WORD iYear, WORD iMonth, WORD iDay)
{
    Init();
   if(!SetDate(iYear, iMonth, iDay))
   {
       CTime tm=CTime::GetCurrentTime();
       SetDate(tm.GetYear(),tm.GetMonth(),tm.GetDay());
   }
}

SCalendar::SCalendar()
{
    Init();
    CTime tm=CTime::GetCurrentTime();
    SetDate(tm.GetYear(),tm.GetMonth(),tm.GetDay(),FALSE);
}

void SCalendar::Init()
{
    m_evtSet.addEvent(EVENTID(EventCalendarSelDay));
    m_evtSet.addEvent(EVENTID(EventCalendarSetDate));
	m_nTitleHei.setSize(TITLE_HEIGHT,SLayoutSize::dp);
	m_nFooterHei.setSize(FOOTER_HEIGHT,SLayoutSize::dp);

    m_crWeekend=RGBA(255,0,0,255);
    m_crDay=RGBA(255,0,0,255);
    m_crTitleBack=RGBA(0,255,0,255);
    m_crDayBack=RGBA(0,0,255,255);
    m_pTitleSkin=m_pDaySkin=NULL;

    m_iHoverDay=0;
}

int SCalendar::OnCreate(LPVOID)
{
	__super::OnCreate(NULL);

	TCHAR sztext[][3] = { _T("日"),_T("一"),_T("二"),_T("三"),_T("四"),_T("五"),_T("六") };
	for (int i = 0; i<7; i++)
	{
		m_strTitle[i].SetCtxProvider(this);
		m_strTitle[i].SetText(sztext[i]);
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// SCalendarCore message handlers
void SCalendar::DrawTitle(IRenderTarget *pRT)
{
   CRect rect ;
   GetClientRect(&rect);

   rect.bottom = rect.top + m_nTitleHei.toPixelSize(GetScale());

   if(m_pTitleSkin)
       m_pTitleSkin->Draw(pRT,rect,0);
   else
       pRT->FillSolidRect(&rect,m_crTitleBack);

   int nWid=rect.Width()/7;
   CRect rcItem=rect;
   rcItem.right=rcItem.left+nWid;

   COLORREF crTxt=pRT->GetTextColor();
   for(int i=0; i <7; i++)
   {
       if(i==0 || i==6 )
           pRT->SetTextColor(m_crWeekend);
       else
           pRT->SetTextColor(crTxt);

       pRT->DrawText(m_strTitle[i].GetText(FALSE),m_strTitle[i].GetText(FALSE).GetLength(),rcItem,DT_SINGLELINE|DT_VCENTER|DT_CENTER);
       rcItem.OffsetRect(nWid,0);
   }
   pRT->SetTextColor(crTxt);
}

void SCalendar::DrawDate(IRenderTarget *pRT)
{

    int days=SCalendarCore::GetDaysOfMonth(m_iYear, m_iMonth);

    for(int i=1;i<=days;i++)
    {
        CRect rcDay=GetDayRect(i);
        DrawDay(pRT,rcDay,i);
    }
}


void SCalendar::DrawDay( IRenderTarget *pRT,CRect & rcDay,WORD iDay )
{
    TCHAR text[3];
    _stprintf(text, _T("%2d"), iDay);
    COLORREF crTxt=pRT->GetTextColor();
    if(iDay==m_iDay)
    {
        if(m_pDaySkin) m_pDaySkin->Draw(pRT,rcDay,2);
        else pRT->FillSolidRect(rcDay,m_crDayBack);
        pRT->SetTextColor(m_crDay);
    }else
    {
        if(m_pDaySkin) m_pDaySkin->Draw(pRT,rcDay,iDay==m_iHoverDay?1:0);
        int iweekday=SCalendarCore::GetDayOfWeek(m_iYear,m_iMonth,iDay);
        if(iweekday==0 || iweekday==6)
            pRT->SetTextColor(m_crWeekend);
    }
    pRT->DrawText(text,-1,rcDay,DT_SINGLELINE|DT_VCENTER|DT_CENTER);
    pRT->SetTextColor(crTxt);
}

void SCalendar::RedrawDay(WORD iDay )
{
    CRect rcDay=GetDayRect(iDay);
    IRenderTarget * pRT=GetRenderTarget(&rcDay,OLEDC_PAINTBKGND);
    DrawDay(pRT,rcDay,iDay);
    ReleaseRenderTarget(pRT);
}

void SCalendar::OnPaint(IRenderTarget * pRT) 
{
    SPainter painter;
    BeforePaint(pRT,painter);
    DrawTitle(pRT);
    DrawDate(pRT);
    AfterPaint(pRT,painter);
}

void SCalendar::GetDate(WORD &iYear, WORD &iMonth, WORD &iDay) 
{
    iYear  = m_iYear;
    iMonth = m_iMonth;
    iDay   = m_iDay;
}

BOOL SCalendar::SetDate(WORD iYear, WORD iMonth, WORD iDay,BOOL bNotify)
{
  if(iDay <1 || iDay > SCalendarCore::GetDaysOfMonth(iYear, iMonth))
        iDay = 0;//不选中day

  m_iYear   = iYear;
  m_iMonth  = iMonth;
  m_iDay    = iDay;

  m_iHoverDay    = 0;
  
  Invalidate();
  
  if(bNotify)
  {
      EventCalendarSetDate evt(this);
      FireEvent(evt);
  }
  
  return TRUE;
} 


CRect SCalendar::GetDayRect( WORD iDay )
{
    CRect rcClient;
    GetClientRect(&rcClient);
    rcClient.top+=m_nTitleHei.toPixelSize(GetScale());
    rcClient.bottom-=m_nFooterHei.toPixelSize(GetScale());
    
    int col  = SCalendarCore::GetDayOfWeek(m_iYear, m_iMonth,iDay);//计算出iday是星期几
    int row  = (SCalendarCore::GetDayOfWeek(m_iYear, m_iMonth,1)+iDay-1)/7;//计算出iday是第几周
    
    int nWid=rcClient.Width()/7;
    int nHei=rcClient.Height()/6;

    CRect rc(0,0,nWid,nHei);
    rc.OffsetRect(nWid*col,nHei*row);
    rc.OffsetRect(rcClient.TopLeft());
    return rc;
}

WORD SCalendar::HitTest(CPoint pt)
{
    CRect rcClient;
    GetClientRect(&rcClient);
    rcClient.top+=m_nTitleHei.toPixelSize(GetScale());
    rcClient.bottom-=m_nFooterHei.toPixelSize(GetScale());

    int nWid=rcClient.Width()/7;
    int nHei=rcClient.Height()/6;

    pt-=rcClient.TopLeft();
    if(pt.x<0 || pt.y<0) return 0;

    int iCol  = pt.x/nWid;
    int iRow  = pt.y/nHei;


    WORD startcol ,endrow, endcol;
    startcol = SCalendarCore::GetDayOfWeek(m_iYear, m_iMonth, 1);
    endcol   = SCalendarCore::GetDayOfWeek(m_iYear, m_iMonth, SCalendarCore::GetDaysOfMonth(m_iYear,m_iMonth));

    endrow   = (SCalendarCore::GetDaysOfMonth(m_iYear, m_iMonth) + startcol -1)/7 ;
    if(iRow == 0 && iCol < startcol || iRow == endrow && iCol > endcol ||  iRow > endrow)
        return 0;
    return iRow *7 + iCol + 1 - startcol ;
}

void SCalendar::OnLButtonDown(UINT nFlags, CPoint point) 
{
    __super::OnLButtonDown(nFlags,point);
    WORD day = HitTest(point);
    if(day !=0 && day != m_iDay)
    {
        EventCalendarSelDay evt(this);
        evt.wOldDay=m_iDay;
        evt.wNewDay=day;

        FireEvent(evt);
    }
    m_iDay = day;
    Invalidate();
}

void SCalendar::OnMouseMove( UINT nFlags,CPoint pt )
{
    int iDay=HitTest(pt);
    if(iDay!=m_iHoverDay)
    {
        WORD oldHover=m_iHoverDay;
        m_iHoverDay=iDay;
        if(m_pDaySkin)
        {
            if(oldHover!=0) RedrawDay(oldHover);
            if(m_iHoverDay!=0) RedrawDay(m_iHoverDay);
        }
    }
}

void SCalendar::OnMouseLeave()
{
    if(m_iHoverDay!=0)
    {
        WORD oldHover=m_iHoverDay;
        m_iHoverDay=0;
        if(m_pDaySkin) RedrawDay(oldHover);
    }
}

}//end of namespace
