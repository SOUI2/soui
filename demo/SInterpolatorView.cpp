#include "stdafx.h"
#include "SInterpolatorView.h"

namespace SOUI
{
	SInterpolatorView::SInterpolatorView(void):m_nSpeed(5),m_iFrame(0),m_fTime(0.0f)
	{
	}

	SInterpolatorView::~SInterpolatorView(void)
	{
	}

	void SInterpolatorView::OnNextFrame()
	{
		if(m_interpolator && IsVisible(TRUE))
		{
			m_iFrame++;
			if(m_iFrame%m_nSpeed==0)
			{
				m_fTime += 0.02f;

				if(m_fTime>=1.0f)
				{
					m_fTime = 0.0f;
				}

				float fValue = m_interpolator->getInterpolation(m_fTime);

				int nValue = m_nMinValue + (int)((m_nMaxValue - m_nMinValue)*fValue);
				SetValue(nValue);

				m_iFrame = 0;
			}

		}
	}

	void SInterpolatorView::SetInterpolator(IInterpolator *pInterpolator)
	{
		m_interpolator = pInterpolator;
		m_iFrame = 0;
	}

	int SInterpolatorView::OnCreate(void*)
	{
		int nRet = __super::OnCreate(NULL);
		if(nRet == 0)
		{
			GetContainer()->RegisterTimelineHandler(this);
		}
		return nRet;
	}

	void SInterpolatorView::OnDestroy()
	{
		GetContainer()->RegisterTimelineHandler(this);
		__super::OnDestroy();
	}

	void SInterpolatorView::OnPaint(IRenderTarget * pRT)
	{

		CAutoRefPtr<IPath> path;
		if(GETRENDERFACTORY->CreatePath(&path))
		{
			CRect rc = GetClientRect();
			CAutoRefPtr<IPen> pen,oldPen;
			pRT->CreatePen(PS_DASH,RGBA(255,0,0,255),3,&pen);

			pRT->SelectObject(pen,(IRenderObj**)&oldPen);
			path->moveTo((float)rc.left,(float)rc.top);
			path->quadTo((float)(rc.right+rc.left)/2,(float)rc.bottom + rc.Height(),(float)rc.right,(float)rc.top);
			pRT->DrawPath(path);
			pRT->SelectObject(oldPen);
		}

		__super::OnPaint(pRT);
	}

}
