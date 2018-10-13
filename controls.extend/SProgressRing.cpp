#include "stdafx.h"
#include "SProgressRing.h"
#include <helper/SDIBHelper.h>

namespace SOUI
{
	SProgressRing::SProgressRing(void):m_crBackground(RGBA(0xff,0xff,0xff,0xff)),m_crForeground(RGBA(58,171,249,255)),m_fArcRate(0.2f)
	{
	}

	SProgressRing::~SProgressRing(void)
	{
	}

	void SProgressRing::OnPaint(IRenderTarget *pRT)
	{
		SPainter painter;
		BeforePaint(pRT,painter);

		CRect rcClient = GetClientRect();
		int nAdj = (rcClient.Width()-rcClient.Height())/2;
		if(nAdj>0)
			rcClient.DeflateRect(nAdj,0);
		else
			rcClient.DeflateRect(0,nAdj);
		int radius = rcClient.Width()/2;

		SASSERT(m_fArcRate>0.f && m_fArcRate<1.0f);
		int arcWid = (int)(radius * m_fArcRate);

		int innerRadius = radius -arcWid;

		CAutoRefPtr<IPen> pPenBack,pPenFore,oldPen;
		pRT->CreatePen(PS_SOLID|PS_GEOMETRIC|PS_ENDCAP_FLAT ,m_crBackground,arcWid,&pPenBack);
		pRT->CreatePen(PS_SOLID|PS_GEOMETRIC|PS_ENDCAP_FLAT,m_crForeground,arcWid,&pPenFore);
		pRT->SelectObject(pPenBack,(IRenderObj**)&oldPen);

		CRect rcArc = rcClient;
		rcArc.DeflateRect(arcWid/2,arcWid/2);
		pRT->DrawArc(&rcArc, 0.f, 360.f, false);

		float fStart = -90.f;
		float fEnd = (m_nValue-m_nMinValue)*360.f/(m_nMaxValue-m_nMinValue);
		pRT->SelectObject(pPenFore,NULL);
		pRT->DrawArc(&rcArc, fStart, fEnd, false);
		pRT->SelectObject(oldPen,NULL);

		if (m_bShowPercent)
		{
			SStringT strPercent;
			strPercent.Format(_T("%d%%"), (int)((m_nValue-m_nMinValue) * 100/(m_nMaxValue-m_nMinValue)));
			pRT->DrawText(strPercent, strPercent.GetLength(), rcClient, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		}

		AfterPaint(pRT,painter);
	}

	void SProgressRing::OnColorize(COLORREF cr)
	{
		SDIBHelper::Colorize(m_crForeground,cr);
	}

}
