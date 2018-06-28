#include "stdafx.h"
#include "SQrCtrl.h"


SQrCtrl::SQrCtrl()
{
}


SQrCtrl::~SQrCtrl()
{
}

void SQrCtrl::SetWindowText(LPCTSTR lpszText)
{
	__super::SetWindowText(lpszText);
	CreateQrImg(GetWindowText());
}

BOOL SQrCtrl::OnRelayout(const CRect & rcWnd)
{
	CRect rcOld = GetClientRect();
	BOOL bRet = __super::OnRelayout(rcWnd);
	if(!bRet) return FALSE;
	CRect rcNew = GetClientRect();
	if(rcOld.Size() == rcNew.Size())
		return TRUE;

	CreateQrImg(GetWindowText());
	return TRUE;
}

void SQrCtrl::MakeCacheApha( ISkinObj *pSkin, IBitmap *_pBitCache, IBitmap *_pBitMask )
{
	CAutoRefPtr<IRenderTarget> pRTDst;
	GETRENDERFACTORY->CreateRenderTarget(&pRTDst, 0, 0);
	CAutoRefPtr<IRenderObj> pOldBmp;
	pRTDst->SelectObject(_pBitCache, &pOldBmp);
	CRect rc(CPoint(0, 0), _pBitCache->Size());
	pSkin->Draw(pRTDst, &rc, 0);
	CAutoRefPtr<IBitmap> bmp;
	GETRENDERFACTORY->CreateBitmap(&bmp);
	if (m_QrRect.IsRectEmpty())
	{
		bmp = _pBitMask;
	}
	else
	{
		bmp->Init(m_QrRect.Width(),	m_QrRect.Height());
		pRTDst->SelectObject(bmp, NULL);
		CRect rcSrc(CPoint(0, 0), _pBitMask->Size());
		rc = CRect(CPoint(0, 0), bmp->Size());
		pRTDst->DrawBitmapEx(rc, _pBitMask, rcSrc, MAKELONG(EM_STRETCH, m_fl));
		pRTDst->SelectObject(pOldBmp);
	}


	int nDesLinePixels = _pBitCache->Width();
	int nDesRowPixels = _pBitCache->Height();
	int nline = bmp->Height();
	int nRow = bmp->Width();
	if ((nDesRowPixels < nRow + m_QrRect.top) || (nDesLinePixels < nline + m_QrRect.left))
		return;
	LPDWORD pBitCache = (LPDWORD)_pBitCache->LockPixelBits();
	LPDWORD pBitMask = (LPDWORD)bmp->LockPixelBits();
	LPDWORD pDst = pBitCache;
	LPDWORD pSrc = pBitMask;
	LPDWORD pLine = pDst + m_QrRect.top * nDesLinePixels;
	for (int line = 0; line < nline; line++)
	{
		LPDWORD pLineData = pLine + m_QrRect.left;
		for (int row = 0; row < nRow; row++)
		{
			LPBYTE byAlpha = (LPBYTE)(pSrc + row);
			if (byAlpha[3] == 0)
			{
				LPDWORD pByteAlpha = (pLineData + row);
				*pByteAlpha =RGBA(0,0,0,0);
			}
		}
		pSrc += nRow;
		pLine += nDesLinePixels;
	}
	_pBitCache->UnlockPixelBits(pBitCache);
	bmp->UnlockPixelBits(pBitMask);
}

void SQrCtrl::CreateQrImg( SStringT strContent )
{
	CQR_Encode qrEncode;
	int nLevel = 3;
	int nVersion = 0;
	bool bAutoExtent = true;
	int nMaskingNo = -1;
	SStringA strContentA = S_CT2A(strContent, CP_UTF8);
	if (qrEncode.EncodeData(nLevel, nVersion, bAutoExtent, nMaskingNo, strContentA, strContentA.GetLength()))
	{

		if (m_pSkin == NULL)
		{
			int qrSize = qrEncode.m_nSymbleSize + (QR_MARGIN * 2);
			CAutoRefPtr<IBitmap> bmp;
			GETRENDERFACTORY->CreateBitmap(&bmp);
			bmp->Init(qrSize, qrSize);
			LPDWORD pBits = (LPDWORD)bmp->LockPixelBits();
			memset(pBits, 0xFF, qrSize*qrSize * 4);
			int nLineWid = qrSize;
			LPDWORD pLine = pBits + QR_MARGIN*nLineWid;
			for (int i = 0; i < qrEncode.m_nSymbleSize; i++)
			{
				LPDWORD pLineData = pLine + QR_MARGIN;
				for (int j = 0; j < qrEncode.m_nSymbleSize; j++)
				{
					if (qrEncode.m_byModuleData[i][j])
					{
						pLineData[j] = RGBA(0, 0, 0, 255);
					}
				}
				pLine += qrSize;
			}
			bmp->UnlockPixelBits(pBits);
			SetImage(bmp);
		}
		else
		{
			int qrSize = qrEncode.m_nSymbleSize + 2;
			CAutoRefPtr<IBitmap> bmp;
			GETRENDERFACTORY->CreateBitmap(&bmp);
			SIZE skinSize = m_pSkin->GetSkinSize();
			CRect rcWind = GetWindowRect();

			bmp->Init(rcWind.Width(), rcWind.Height());

			CAutoRefPtr<IBitmap> qrbmp;
			GETRENDERFACTORY->CreateBitmap(&qrbmp);
			qrbmp->Init(qrSize, qrSize);
			LPDWORD pBits = (LPDWORD)qrbmp->LockPixelBits();
			memset(pBits, 0x00, qrSize*qrSize * 4);
			int nLineWid = qrSize;
			LPDWORD pLine = pBits + nLineWid;
			for (int i = 0; i < qrEncode.m_nSymbleSize; i++)
			{
				LPDWORD pLineData = pLine + 1;
				for (int j = 0; j < qrEncode.m_nSymbleSize; j++)
				{
					if (qrEncode.m_byModuleData[i][j])
					{
						pLineData[j] = RGBA(0xFF, 0xFF, 0xFF, 0xFF);
					}
				}
				pLine += qrSize;
			}
			qrbmp->UnlockPixelBits(pBits);
			MakeCacheApha(m_pSkin, bmp, qrbmp);
			SetImage(bmp);
		}
	}
}



