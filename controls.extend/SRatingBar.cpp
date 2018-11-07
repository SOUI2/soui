#include "stdafx.h"
#include "SRatingBar.h"

namespace SOUI
{
    SRatingBar::SRatingBar(void):m_pStar(NULL),m_nStars(5),m_fValue(0.0f)
    {
    }

    SRatingBar::~SRatingBar(void)
    {
    }

    void SRatingBar::OnPaint(IRenderTarget *pRT)
    {
        CRect rcClient = GetClientRect();
        int nWid = (int)(rcClient.Width()*m_fValue/m_nStars);
        CRect rcFore = rcClient;
        rcFore.right = rcFore.left + nWid;
        pRT->PushClipRect(rcFore);
        DrawStars(pRT,rcClient,TRUE);
        pRT->PopClip();
        CRect rcBack = rcClient;
        rcBack.left = rcFore.right;
        pRT->PushClipRect(rcBack);
        DrawStars(pRT,rcClient,FALSE);
        pRT->PopClip();
    }

    CSize SRatingBar::GetDesiredSize(LPCRECT pRcContainer)
    {
		if (!m_pStar)
			return CSize(16,16);
        CSize szStar = m_pStar->GetSkinSize();
        szStar.cx *= m_nStars;
        return szStar;
    }

    void SRatingBar::DrawStars(IRenderTarget *pRT,CRect rc,BOOL bForeground)
    {
		if (!m_pStar)
			return;

        CSize szStar = rc.Size();
        szStar.cx/=m_nStars;
        CRect rcStar(rc.TopLeft(),szStar);
        
        for(int i=0;i<m_nStars;i++)
        {
            m_pStar->Draw(pRT,rcStar,bForeground?0:1);
            rcStar.OffsetRect(szStar.cx,0);
        }
    }

    void SRatingBar::SetValue(float fValue)
    {
        m_fValue = fValue;
        if(m_fValue>(float)m_nStars) m_fValue = (float)m_nStars;
        Invalidate();
    }

}
