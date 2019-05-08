#include "stdafx.h"
#include "SRoundImage.h"

namespace SOUI
{
	SRoundImage::SRoundImage()
	{
	}


	SRoundImage::~SRoundImage()
	{
	}

	CAutoRefPtr<IBitmap> SRoundImage::GetRoundImage() const
	{
		if (!m_pSkin) return NULL;
		CAutoRefPtr<IRenderTarget> pRT; 
		CRect rc = GetClientRect();
		GETRENDERFACTORY->CreateRenderTarget(&pRT, rc.Width(), rc.Height());
		pRT->ClearRect(&rc, 0);
		CAutoRefPtr<IPen> pen;
		pRT->CreatePen(PS_SOLID, RGBA(0xBA,0xB3,0x99,0xFF), 1, &pen);
		pRT->SelectObject(pen);
		pRT->DrawEllipse(&rc);
		pRT->SetRopMode(kSrcIn_Mode);
		m_pSkin->Draw(pRT, rc, 0);
		return (IBitmap*)pRT->GetCurrentObject(OT_BITMAP);
	}

	void SRoundImage::OnPaint(IRenderTarget * pRT)
	{
		if (!m_img) return;
		CRect rc = GetClientRect();
		pRT->DrawBitmap(&rc, m_img, 0, 0);
	}

	void SRoundImage::OnSize(UINT nType, CSize size)
	{
		__super::OnSize(nType, size);
		m_img = GetRoundImage();
	}

}
