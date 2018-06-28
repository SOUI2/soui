#include "StdAfx.h"
#include "SDesktopDock.h"
#include <math.h>
#include <ShellAPI.h>
/*
by 多点
@20150510 qq:4361031
*/

//初始化DOCK工具栏小图标的矩形区域，初始图标大小为65×65，工具栏长度：m_DockLen = 650
SDesktopDock::SDesktopDock()
{
	memset(m_pSkin,0,sizeof(m_pSkin));
	m_Width = 50;        //初始宽度
	m_Height = 50;
	m_DockLen = 500;
	m_MaxDisc = 200;
	m_MinDisc = 40;
	m_MaxRate = 2;
	m_MinRate = 1;
	m_gap = 5;         //工具栏距离底边的距离
	m_Init = FALSE;

}
void SDesktopDock::OnPaint(IRenderTarget *pRT)
{
	if (!m_Init)
	{
		m_Init = TRUE;
		CRect rcClient = GetClientRect();
		int cx = rcClient.Width();
		int cy = rcClient.Height();

		double X = (cx - m_DockLen)/2;
		double Y = cy - m_Height - m_gap;
		int i;

		for (i = 0; i < MAX_COUNT; i++)
		{
			m_RTRect[i] = CRect(CPoint(X + m_Width * i+rcClient.left, Y), CSize(m_Width, m_Height));
			m_dRate[i] = 1;
		}
	}
	SPainter painter;
	BeforePaint(pRT,painter);

	GetRTRect();

	for (int i=0; i<MAX_COUNT; i++)
	{
		m_pSkin[i]->Draw(pRT,m_RTRect[i],FALSE);
	}

	AfterPaint(pRT,painter);
}
void SDesktopDock::OnMouseMove(UINT nFlags,CPoint pt)
{
	CRect rc;
	GetClientRect(rc);
	rc.top = rc.bottom - 160;
	static bool OutofRect = true;

	if (rc.PtInRect(pt))
	{
		OutofRect = true;
		m_CPX = pt.x;
		m_CPY = pt.y;
		GetImageRate();
		GetDockLen();
		Invalidate();

		SetCapture();
	}	
	else
	{
		if (!OutofRect)
			return ;

		ReleaseCapture();
		initDockRect();
		Invalidate();
		OutofRect = false;
	}

}

//初始化DOCK工具栏小图标的矩形区域，初始图标大小为65×65，工具栏长度：m_DockLen = 650
void SDesktopDock::initDockRect()
{
	CRect rc;
	GetClientRect(rc);
	int cx = rc.Width();
	int cy = rc.Height();

	//Dock工具栏数据初始化
	m_Width = 50;        //初始宽度
	m_Height = 50;
	m_DockLen = 500;
	m_MaxDisc = 200;
	m_MinDisc = 40;
	m_MaxRate = 2;
	m_MinRate = 1;
	m_gap = 5;         //工具栏距离底边的距离

	double X = (cx - m_DockLen)/2;
	double Y = cy - m_Height - m_gap;
	int i;

	for (i = 0; i < MAX_COUNT; i++)
	{
		m_RTRect[i] = CRect(CPoint(X + m_Width * i+rc.left, Y), CSize(m_Width, m_Height));
		m_dRate[i] = 1;
	}

}

const LPCTSTR adUrls[MAX_COUNT]=
{
_T("http://www.ui520.cn"),
_T("http://www.cnblogs.com/setoutsoft/"),
_T("http://www.b5csgo.com"),
_T("http://love.junzimu.com/"),
_T("http://my.tv.sohu.com/pl/9259542/89333168.shtml"),

_T("http://www.ui520.cn"),
_T("http://www.cnblogs.com/setoutsoft/"),
_T("http://www.b5csgo.com"),
_T("http://love.junzimu.com/"),
_T("http://my.tv.sohu.com/pl/9259542/89333168.shtml"),

};

void SDesktopDock::OnLButtonUp(UINT nFlags, CPoint point)
{
	for (int i=0; i<MAX_COUNT; i++)
	{
		if (m_RTRect[i].PtInRect(point))
		{
			SMessageBox(NULL,SStringT().Format(_T("索引===%d号"),i),_T("多点WIFI"),MB_OK);

			ShellExecute(NULL, _T("open"),adUrls[i], NULL, NULL, SW_SHOWNORMAL);
			return ;
		}
	}
}

//函数功能：
//计算图形缩放比例——m_dRate
//rcOrign:原始矩形，最小的矩形，大小为50×50。
//dRTDisc:即时鼠标坐标和rcOrign中心的距离。
//dRate:动态图形缩放的比例
//MaxDisc:鼠标坐标与图形中心的最远距离，大于这个距离，图形不变。
//MinDisc:鼠标坐标与图形中心的最小距离,小于这个距离，图形不变。
void SDesktopDock::GetImageRate()
{
	int i;
	double dRTDisc;
	CPoint cp;

	double a = (m_MinRate - m_MaxRate)/(m_MaxDisc - m_MinDisc);
	double b = m_MinRate - a * m_MaxDisc;

	for (i = 0; i < MAX_COUNT; i++)
	{
		//取动态矩形中心点
		//cp = CPoint(m_RTRect[i]->X + m_RTRect[i]->Width/2.0, m_RTRect[i]->Y + m_RTRect[i]->Height/2.0);
		cp = CPoint(m_RTRect[i].left + m_RTRect[i].Width()/2.0, m_RTRect[i].top + m_RTRect[i].Height()/2.0);
		//计算鼠标坐标到动态矩形中心的距离	
		dRTDisc = sqrt(double(cp.x-m_CPX)*(cp.x-m_CPX) +double(cp.y-m_CPY)*(cp.y-m_CPY));

		if (dRTDisc >= m_MaxDisc)
		{
			m_dRate[i] = m_MinRate;
			continue;
		}
		if (dRTDisc <= m_MinDisc)
		{
			m_dRate[i] = m_MaxRate;
			continue;
		}		

		//计算图形缩放比例
		m_dRate[i] = a * dRTDisc + b;

		//delete cp;
	}
}

//计算工具栏的动态长度——RTLen.
void SDesktopDock::GetDockLen()
{
	int i;
	m_DockLen = 0;

	for (i = 0; i < MAX_COUNT; i++)
	{
		m_DockLen +=  m_Width * m_dRate[i];
	}
}

void SDesktopDock::GetRTRect()
{
	CRect rc;
	GetClientRect(rc);
	int cx = rc.Width();
	int cy = rc.Height();
	double X = (cx - m_DockLen)/2.0;
	int i;
	double width;

	for (i = 0; i < MAX_COUNT; i++)
	{
		width = m_Width * m_dRate[i];
		if (i > 0)
			X += m_Width * m_dRate[i-1];
		m_RTRect[i].left = X + rc.left;
		m_RTRect[i].top = cy - m_Height * m_dRate[i] - m_gap;
		m_RTRect[i].right = m_RTRect[i].left + width;
		m_RTRect[i].bottom = m_RTRect[i].top + (m_Height * m_dRate[i]);
	}
}