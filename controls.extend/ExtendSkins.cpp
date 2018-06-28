#include "stdafx.h"
#include "souistd.h"
#include "core/Sskin.h"
#include "ExtendSkins.h"

//////////////////////////////////////////////////////////////////////////
// SColorMask
SColorMask::SColorMask()
{ 
    m_crStates[0]=(RGB(0xEE, 0xEE, 0xEE));
    m_crStates[1]=(RGB(0xD6, 0xD6, 0xD6));
    m_crStates[2]=(RGB(0xEE, 0xEE, 0xEE));
    m_crStates[3]=(RGB(0xE0, 0xE0, 0xE0));
}

void SColorMask::_Draw(IRenderTarget *pRT, LPCRECT prcDraw, DWORD dwState, BYTE byAlpha)
{
    CRect rcDraw = *prcDraw;

    //STRACE(_T("skin btn state:%d"), dwState);
    if (dwState >= 0 && dwState < STATE_COUNT && m_bmpSkin)
    {
        SColor cr(m_crStates[dwState]);
        cr.updateAlpha(byAlpha);

        int nWidthPerState = m_size.cx / STATE_COUNT;
        CRect rcSrc(CPoint(dwState * nWidthPerState, 0), CSize(nWidthPerState, m_size.cy));
        pRT->DrawBitmapEx(rcDraw, m_bmpSkin, &rcSrc, EM_STRETCH, byAlpha);
    }
}

void SColorMask::SetColors(COLORREF cr[4])
{
    memcpy(m_crStates, cr, 4*sizeof(COLORREF));
}

HRESULT SColorMask::OnAttrMask(const SStringW & strValue, BOOL bLoading)
{
    SStringW strChannel = strValue.Right(2);
    m_iMaskChannel = -1;
    if(strChannel == L".a")
        m_iMaskChannel = 3;
    else if(strChannel == L".r")
        m_iMaskChannel =0;
    else if(strChannel == L".g")
        m_iMaskChannel = 1;
    else if(strChannel == L".b")
        m_iMaskChannel = 2;

    IBitmap *pImg = NULL;
    if(m_iMaskChannel==-1)
    {//use alpha channel as default
        m_iMaskChannel = 0;
        pImg = LOADIMAGE2(strValue);
    }else
    {
        pImg = LOADIMAGE2(strValue.Left(strValue.GetLength()-2));
    }
    if(!pImg)
    {
        return E_FAIL;
    }
    m_bmpMask = pImg;
    pImg->Release();

    MakeCacheApha();

    return S_OK;
}

void SColorMask::MakeCacheApha()
{
    int nWidthPerSize  = m_size.cx / STATE_COUNT;

    m_bmpSkin = NULL;
    GETRENDERFACTORY->CreateBitmap(&m_bmpSkin);
    m_bmpSkin->Init(m_size.cx, m_size.cy);

    // 创建画布并把skin选进画布
    CAutoRefPtr<IRenderTarget> pRTDst;
    GETRENDERFACTORY->CreateRenderTarget(&pRTDst,0,0);
    CAutoRefPtr<IRenderObj> pOldBmp;
    pRTDst->SelectObject(m_bmpSkin,&pOldBmp);
    for (int i = 0; i < STATE_COUNT; ++i)
    {
        CRect rc(CPoint(i * nWidthPerSize, 0), m_size);
        SColor cr(m_crStates[i]);
        pRTDst->FillSolidRect(&rc, cr.toCOLORREF());
    }
    pRTDst->SelectObject(pOldBmp);

    //从mask的指定channel中获得alpha通道，与cache按位运算
    LPBYTE pBitCache = (LPBYTE)m_bmpSkin->LockPixelBits();
    LPBYTE pBitMask = (LPBYTE)m_bmpMask->LockPixelBits();

    int cx = m_bmpMask->Width() < m_bmpSkin->Width() ? m_bmpMask->Width() : m_bmpSkin->Width();
    int cy = m_bmpMask->Height() < m_bmpSkin->Height() ? m_bmpMask->Height() : m_bmpSkin->Height();

    for (int i = 0; i < STATE_COUNT; ++i)
    {    
        for (int y = 0; y < cy; ++y)
        {
            LPBYTE pDst = pBitCache + (y * m_bmpSkin->Width()  + i * nWidthPerSize) * 4;
            LPBYTE pSrc = pBitMask + y * m_bmpMask->Width() * 4 + m_iMaskChannel;

            for (int x = 0; x < cx; ++x)
            {
                BYTE byAlpha = *pSrc;
                pSrc += 4;

                *pDst++ = ((*pDst) * byAlpha)>>8;//做premutiply
                *pDst++ = ((*pDst) * byAlpha)>>8;//做premutiply
                *pDst++ = ((*pDst) * byAlpha)>>8;//做premutiply
                *pDst++ = byAlpha;
            }
        }
    }

    m_bmpSkin->UnlockPixelBits(pBitCache);
    m_bmpMask->UnlockPixelBits(pBitMask);
}
