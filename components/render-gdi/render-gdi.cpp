// render-gdi.cpp : Defines the exported functions for the DLL application.
//

#include "render-gdi.h"
#include "GradientFillHelper.h"
#include <gdialpha.h>
#include <math.h>
#include <trace.h>
#include <tchar.h>
#include <algorithm>

namespace SOUI
{


    //////////////////////////////////////////////////////////////////////////
    // SRenderFactory_GDI
    BOOL SRenderFactory_GDI::CreateRenderTarget( IRenderTarget ** ppRenderTarget ,int nWid,int nHei)
    {
        *ppRenderTarget = new SRenderTarget_GDI(this, nWid, nHei);
        return TRUE;
    }

    BOOL SRenderFactory_GDI::CreateFont( IFont ** ppFont , const LOGFONT &lf)
    {
        *ppFont = new SFont_GDI(this,&lf);
        return TRUE;
    }

    BOOL SRenderFactory_GDI::CreateBitmap( IBitmap ** ppBitmap )
    {
        *ppBitmap = new SBitmap_GDI(this);
        return TRUE;
    }

    BOOL SRenderFactory_GDI::CreateRegion( IRegion **ppRgn )
    {
        *ppRgn = new SRegion_GDI(this);
        return TRUE;
    }

	BOOL SRenderFactory_GDI::CreatePath(IPath ** ppPath)
	{
		return FALSE;
	}

	BOOL SRenderFactory_GDI::CreatePathEffect(REFGUID guidEffect,IPathEffect ** ppPathEffect)
	{
		return FALSE;
	}

	BOOL SRenderFactory_GDI::CreatePathMeasure(IPathMeasure ** ppPathMeasure)
	{
		return FALSE;
	}

    
    //////////////////////////////////////////////////////////////////////////
    //  SBitmap_GDI
    HBITMAP SBitmap_GDI::CreateGDIBitmap( int nWid,int nHei,void ** ppBits )
    {
        BITMAPINFO bmi;
        memset(&bmi, 0, sizeof(bmi));
        bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth       = nWid;
        bmi.bmiHeader.biHeight      = -nHei; // top-down image 
        bmi.bmiHeader.biPlanes      = 1;
        bmi.bmiHeader.biBitCount    = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        bmi.bmiHeader.biSizeImage   = 0;

        HDC hdc=GetDC(NULL);
        HBITMAP hBmp=CreateDIBSection(hdc,&bmi,DIB_RGB_COLORS,ppBits,0,0);
        ReleaseDC(NULL,hdc);
        return hBmp;
    }

    HRESULT SBitmap_GDI::Init( int nWid,int nHei ,const LPVOID pBits/*=NULL*/)
    {
        if(m_hBmp) DeleteObject(m_hBmp);
        LPVOID pBmpBits=NULL;
        m_hBmp = CreateGDIBitmap(nWid,nHei,&pBmpBits);
        if(m_hBmp)
        {
            m_sz.cx=nWid,m_sz.cy=nHei;
            const int stride = m_sz.cx*4;
            if(pBits)
            {
                memcpy(pBmpBits,pBits,stride*m_sz.cy);
            }else
            {
                memset(pBmpBits,0,stride*m_sz.cy);
            }
        }
        return m_hBmp?S_OK:E_OUTOFMEMORY;
    }

    HRESULT SBitmap_GDI::Init(IImgFrame *pFrame )
    {
        UINT nWid,nHei;
        pFrame->GetSize(&nWid,&nHei);

        if(m_hBmp) DeleteObject(m_hBmp);
        void * pBits=NULL;
        m_hBmp = CreateGDIBitmap(nWid,nHei,&pBits);
        if(!m_hBmp) return E_OUTOFMEMORY;

        m_sz.cx=nWid,m_sz.cy=nHei;
        const int stride = m_sz.cx*4;
        pFrame->CopyPixels(NULL, stride, stride * m_sz.cy,
            reinterpret_cast<BYTE*>(pBits));
        return S_OK;
    }

    HRESULT SBitmap_GDI::LoadFromFile( LPCTSTR pszFileName)
    {
        CAutoRefPtr<IImgX> imgDecoder;
        GetRenderFactory()->GetImgDecoderFactory()->CreateImgX(&imgDecoder);
        if(imgDecoder->LoadFromFile(S_CT2W(pszFileName))==0) return S_FALSE;
        return ImgFromDecoder(imgDecoder);
    }

    HRESULT SBitmap_GDI::LoadFromMemory(LPBYTE pBuf,size_t szLen)
    {
        CAutoRefPtr<IImgX> imgDecoder;
        GetRenderFactory()->GetImgDecoderFactory()->CreateImgX(&imgDecoder);
        if(imgDecoder->LoadFromMemory(pBuf,szLen)==0) return S_FALSE;
        return ImgFromDecoder(imgDecoder);
    }

    HRESULT SBitmap_GDI::ImgFromDecoder(IImgX *imgDecoder)
    {
        IImgFrame *pFrame=imgDecoder->GetFrame(0);
        UINT nWid,nHei;
        pFrame->GetSize(&nWid,&nHei);
        m_sz.cx=nWid,m_sz.cy=nHei;
        
        if(m_hBmp) DeleteObject(m_hBmp);
        void * pBits=NULL;
        m_hBmp = CreateGDIBitmap(m_sz.cx,m_sz.cy,&pBits);

        if(!m_hBmp) return E_OUTOFMEMORY;
        const int stride = m_sz.cx*4;
        pFrame->CopyPixels(NULL, stride, stride * m_sz.cy,
            reinterpret_cast<BYTE*>(pBits));

        return S_OK;
    }

    UINT SBitmap_GDI::Width() const
    {
        return m_sz.cx;
    }

    UINT SBitmap_GDI::Height() const
    {
        return m_sz.cy;
    }

    SIZE SBitmap_GDI::Size() const
    {
        return m_sz;
    }

    LPVOID SBitmap_GDI::LockPixelBits()
    {
        BITMAP bm;
        GetObject(m_hBmp,sizeof(bm),&bm);
        return bm.bmBits;
    }

    void SBitmap_GDI::UnlockPixelBits( LPVOID pBuf)
    {

    }

    const LPVOID SBitmap_GDI::GetPixelBits() const
    {
        BITMAP bm;
        GetObject(m_hBmp,sizeof(bm),&bm);
        return bm.bmBits;
    }

    //////////////////////////////////////////////////////////////////////////
    //	SRegion_GDI
    SRegion_GDI::SRegion_GDI( IRenderFactory *pRenderFac )
        :TGdiRenderObjImpl<IRegion>(pRenderFac)
    {
        m_hRgn = ::CreateRectRgn(0,0,0,0);
    }

    /*
    int CombineRgn(  HRGN hrgnDest,      // handle to destination region
    HRGN hrgnSrc1,      // handle to source region
    HRGN hrgnSrc2,      // handle to source region
    int fnCombineMode   // region combining mode);
    
    RGN_AND Creates the intersection of the two combined regions. 
    RGN_COPY Creates a copy of the region identified by hrgnSrc1. 
    RGN_DIFF Combines the parts of hrgnSrc1 that are not part of hrgnSrc2. 
    RGN_OR Creates the union of two combined regions. 
    RGN_XOR Creates the union of two combined regions except for any overlapping areas. 
    
    注意 RGN_DIFF 和 RGN_COPY对于hrgnSrc1的使用差异
    */
    void SRegion_GDI::CombineRect( LPCRECT lprect,int nCombineMode )
    {
        HRGN hRgn=::CreateRectRgnIndirect(lprect);
        _CombineRgn(hRgn,nCombineMode);
        DeleteObject(hRgn);
    }

    void SRegion_GDI::CombineRgn(const IRegion * pRgnSrc,int nCombineMode)
    {
        const SRegion_GDI *pRgnSrcGdi = (const SRegion_GDI*)pRgnSrc;
        HRGN hRgn = pRgnSrcGdi->GetRegion();
        _CombineRgn(hRgn,nCombineMode);
    }


    void SRegion_GDI::_CombineRgn(HRGN hRgn,int nCombineMode)
    {
        if(nCombineMode == RGN_DIFF)
        {
            ::CombineRgn(m_hRgn,m_hRgn,hRgn,RGN_DIFF);
        }else
        {
            ::CombineRgn(m_hRgn,hRgn,m_hRgn,nCombineMode);
        }

    }

    BOOL SRegion_GDI::PtInRegion( POINT pt )
    {
        return ::PtInRegion(m_hRgn,pt.x,pt.y);
    }

    BOOL SRegion_GDI::RectInRegion( LPCRECT lprect )
    {
        SASSERT(lprect);
        return ::RectInRegion(m_hRgn,lprect);
    }

    void SRegion_GDI::GetRgnBox( LPRECT lprect )
    {
        SASSERT(lprect);
        ::GetRgnBox(m_hRgn,lprect);
    }

    BOOL SRegion_GDI::IsEmpty()
    {
        RECT rcBox;
        GetRgnBox(&rcBox);
        return (rcBox.left == rcBox.right) || (rcBox.top== rcBox.bottom);
    }

    void SRegion_GDI::Offset( POINT pt )
    {
        ::OffsetRgn(m_hRgn,pt.x,pt.y);
    }

    HRGN SRegion_GDI::GetRegion() const
    {
        return m_hRgn;
    }

    void SRegion_GDI::SetRgn( const HRGN  rgn )
    {
        ::CombineRgn(m_hRgn,rgn,NULL,RGN_COPY);
    }

    void SRegion_GDI::Clear()
    {
        ::SetRectRgn(m_hRgn,0,0,0,0);
    }

	void SRegion_GDI::CombineRoundRect(LPCRECT lprect, POINT ptRadius, int nCombineMode)
	{
		HRGN hRgn = ::CreateRoundRectRgn(lprect->left,lprect->top,lprect->right,lprect->bottom,ptRadius.x,ptRadius.y);
		::CombineRgn(m_hRgn,hRgn,NULL,nCombineMode);
		DeleteObject(hRgn);
	}

	void SRegion_GDI::CombineEllipse(LPCRECT lprect , int nCombineMode)
	{
		HRGN hRgn = ::CreateEllipticRgnIndirect(lprect);
		::CombineRgn(m_hRgn,hRgn,NULL,nCombineMode);
		DeleteObject(hRgn);
	}

    //////////////////////////////////////////////////////////////////////////
    //  DCBuffer
    //////////////////////////////////////////////////////////////////////////
    class DCBuffer
    {
    public:
        DCBuffer(HDC hdc,LPCRECT pRect,BYTE byAlpha,BOOL bCopyBits=TRUE)
            :m_hdc(hdc)
            ,m_byAlpha(byAlpha)
            ,m_pRc(pRect)
            ,m_bCopyBits(bCopyBits)
        {
            m_nWid = pRect->right-pRect->left;
            m_nHei = pRect->bottom-pRect->top;
            m_hBmp = SBitmap_GDI::CreateGDIBitmap(m_nWid,m_nHei,(void**)&m_pBits);
            m_hMemDC = ::CreateCompatibleDC(hdc);
            ::SetBkMode(m_hMemDC,TRANSPARENT);
            ::SelectObject(m_hMemDC,m_hBmp);
            ::SetViewportOrgEx(m_hMemDC,-pRect->left,-pRect->top,NULL);
            //从原DC中获得画笔，画刷，字体，颜色等
            m_hCurPen = ::SelectObject(hdc,GetStockObject(BLACK_PEN));
            m_hCurBrush = ::SelectObject(hdc,GetStockObject(BLACK_BRUSH));
            m_hCurFont = ::SelectObject(hdc,GetStockObject(DEFAULT_GUI_FONT));
            COLORREF crCur = ::GetTextColor(hdc);

            //将画笔，画刷，字体设置到memdc里
            ::SelectObject(m_hMemDC,m_hCurPen);
            ::SelectObject(m_hMemDC,m_hCurBrush);
            ::SelectObject(m_hMemDC,m_hCurFont);
            ::SetTextColor(m_hMemDC,crCur);

            if(m_bCopyBits) ::BitBlt(m_hMemDC,pRect->left,pRect->top,m_nWid,m_nHei,m_hdc,pRect->left,pRect->top,SRCCOPY);
            //将alpha全部强制修改为0xFF。
            BYTE * p= m_pBits+3;
            for(int i=0;i<m_nHei;i++)for(int j=0;j<m_nWid;j++,p+=4) *p=0xFF;
        }

        ~DCBuffer()
        {
            //将alpha为0xFF的改为0,同时将rgb值也清0(XP系统下不清0会导致背景变成白色）,为0的改为0xFF
            BYTE * p= m_pBits+3;
            for(int i=0;i<m_nHei;i++)for(int j=0;j<m_nWid;j++,p+=4)
			{
				if(*p==0) *p=0xff;
				else
				{
					memset(p-3,0,4);
				}
			}
            BLENDFUNCTION bf={AC_SRC_OVER,0,m_byAlpha,AC_SRC_ALPHA };
            BOOL bRet=::AlphaBlend(m_hdc,m_pRc->left,m_pRc->top,m_nWid,m_nHei,m_hMemDC,m_pRc->left,m_pRc->top,m_nWid,m_nHei,bf);
			(void)bRet;
            ::DeleteDC(m_hMemDC);
            ::DeleteObject(m_hBmp);
            
            //恢复原DC的画笔，画刷，字体
            ::SelectObject(m_hdc,m_hCurPen);
            ::SelectObject(m_hdc,m_hCurBrush);
            ::SelectObject(m_hdc,m_hCurFont);
        }

        operator HDC()
        {
            return m_hMemDC;
        }

    protected:
        HDC m_hdc;
        HDC m_hMemDC;
        HBITMAP m_hBmp; 
        LPBYTE  m_pBits;
        BYTE    m_byAlpha;
        LPCRECT m_pRc;
        int     m_nWid,m_nHei;
        BOOL    m_bCopyBits;

        HGDIOBJ m_hCurPen;
        HGDIOBJ m_hCurBrush;
        HGDIOBJ m_hCurFont;
    };

    //////////////////////////////////////////////////////////////////////////
    //	SRenderTarget_GDI
    //////////////////////////////////////////////////////////////////////////
    
    SRenderTarget_GDI::SRenderTarget_GDI( IRenderFactory* pRenderFactory ,int nWid,int nHei)
        :m_hdc(NULL)
        ,m_curColor(0xFF000000)//默认黑色
        ,m_uGetDCFlag(0)
    {
		m_pRenderFactory = pRenderFactory;

        m_ptOrg.x=m_ptOrg.y=0;
                
        HDC hdc=::GetDC(NULL);
        m_hdc = CreateCompatibleDC(hdc);
        ::ReleaseDC(NULL,hdc);
        ::SetBkMode(m_hdc,TRANSPARENT);
        ::SetGraphicsMode(m_hdc,GM_ADVANCED);
        CreatePen(PS_SOLID,SColor(0,0,0).toCOLORREF(),1,&m_defPen);
        SelectObject(m_defPen);

        CreateSolidColorBrush(SColor(0,0,0).toCOLORREF(),&m_defBrush);
        SelectObject(m_defBrush);

        LOGFONT lf={0};
        lf.lfHeight=20;
        _tcscpy(lf.lfFaceName,_T("宋体"));
        pRenderFactory->CreateFont(&m_defFont,lf);
        SelectObject(m_defFont);

        pRenderFactory->CreateBitmap(&m_defBmp);
        m_defBmp->Init(nWid,nHei);
        SelectObject(m_defBmp);
    }

    SRenderTarget_GDI::~SRenderTarget_GDI()
    {
        DeleteDC(m_hdc);
    }

    HRESULT SRenderTarget_GDI::CreateCompatibleRenderTarget( SIZE szTarget,IRenderTarget **ppRenderTarget )
    {
        SRenderTarget_GDI *pRT = new SRenderTarget_GDI(m_pRenderFactory,szTarget.cx,szTarget.cy);
        *ppRenderTarget = pRT;
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::CreatePen( int iStyle,COLORREF cr,int cWidth,IPen ** ppPen )
    {
		*ppPen = new SPen_GDI(m_pRenderFactory,iStyle,cr,cWidth);
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::CreateSolidColorBrush( COLORREF cr,IBrush ** ppBrush )
    {
        *ppBrush = SBrush_GDI::CreateSolidBrush(m_pRenderFactory,cr);
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::CreateBitmapBrush( IBitmap *pBmp,IBrush ** ppBrush )
    {
        SBitmap_GDI *pBmpSkia = (SBitmap_GDI*)pBmp;
        *ppBrush = SBrush_GDI::CreateBitmapBrush(m_pRenderFactory,pBmpSkia->GetBitmap());
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::Resize( SIZE sz )
    {
        HBITMAP hBmp=CreateCompatibleBitmap(m_hdc,0,0);
        ::SelectObject(m_hdc,hBmp);
        m_curBmp->Init(sz.cx,sz.cy);
        ::SelectObject(m_hdc,m_curBmp->GetBitmap());
        ::DeleteObject(hBmp);
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::PushClipRect( LPCRECT pRect ,UINT mode/*=RGN_AND*/)
    {
        RECT rc=*pRect;
        ::OffsetRect(&rc,m_ptOrg.x,m_ptOrg.y);
        HRGN hRgn=::CreateRectRgnIndirect(&rc);
        ::SaveDC(m_hdc);
        ::ExtSelectClipRgn(m_hdc,hRgn,mode);
        ::DeleteObject(hRgn);
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::PopClip()
    {
        ::RestoreDC(m_hdc,-1);
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::ExcludeClipRect( LPCRECT pRc )
    {
        ::ExcludeClipRect(m_hdc,pRc->left,pRc->top,pRc->right,pRc->bottom);
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::IntersectClipRect( LPCRECT pRc )
    {
        ::IntersectClipRect(m_hdc,pRc->left,pRc->top,pRc->right,pRc->bottom);
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::SaveClip( int *pnState )
    {
        int nState=::SaveDC(m_hdc);
        if(pnState) *pnState=nState;
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::RestoreClip( int nState/*=-1*/ )
    {
        ::RestoreDC(m_hdc,nState);
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::PushClipRegion( IRegion *pRegion ,UINT mode/*=RGN_AND*/)
    {
        SRegion_GDI *pRgnGDI=(SRegion_GDI*)pRegion;
        HRGN hRgn=::CreateRectRgn(0,0,0,0);
        ::CombineRgn(hRgn,pRgnGDI->GetRegion(),NULL,RGN_COPY);
        ::OffsetRgn(hRgn,m_ptOrg.x,m_ptOrg.y);
        ::SaveDC(m_hdc);
        ::ExtSelectClipRgn(m_hdc,hRgn,mode);
        ::DeleteObject(hRgn);
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::GetClipRegion( IRegion **ppRegion )
    {
        SRegion_GDI *pRgn=new SRegion_GDI(m_pRenderFactory);
        ::GetClipRgn(m_hdc,pRgn->GetRegion());
        POINT pt={-m_ptOrg.x,-m_ptOrg.y};
        pRgn->Offset(pt);
        *ppRegion = pRgn;
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::GetClipBox(LPRECT prc)
    {
        ::GetClipBox(m_hdc,prc);
        return S_OK;
    }


    HRESULT SRenderTarget_GDI::BitBlt( LPCRECT pRcDest,IRenderTarget *pRTSour,int xSrc,int ySrc,DWORD dwRop/*=SRCCOPY*/)
    {
        SRenderTarget_GDI *pRTSrc_GDI=(SRenderTarget_GDI*)pRTSour;
        ALPHAINFO ai;
        if(dwRop!=SRCCOPY)
            CGdiAlpha::AlphaBackup(m_hdc,pRcDest,ai);
        ::BitBlt(m_hdc,pRcDest->left,pRcDest->top,pRcDest->right-pRcDest->left,pRcDest->bottom-pRcDest->top,pRTSrc_GDI->m_hdc,xSrc,ySrc,dwRop);
        if(dwRop!=SRCCOPY)
            CGdiAlpha::AlphaRestore(ai);
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::DrawText( LPCTSTR pszText,int cchLen,LPRECT pRc,UINT uFormat)
    {
        if(uFormat & DT_CALCRECT)
        {
            int nRet = ::DrawText(m_hdc,pszText,cchLen,pRc,uFormat);
			if(!nRet)
			{
				pRc->right = pRc->left;
				pRc->bottom = pRc->top;
			}
            return S_OK;
        }
        
        if(cchLen == 0) return S_OK;

        {
            DCBuffer dcBuf(m_hdc,pRc,m_curColor.a);
            ::DrawText(dcBuf,pszText,cchLen,pRc,uFormat);
        }

        return S_OK;
    }

    HRESULT SRenderTarget_GDI::MeasureText( LPCTSTR pszText,int cchLen, SIZE *psz )
    {
        if(cchLen<0) cchLen = _tcslen(pszText);
        ::GetTextExtentPoint32(m_hdc,pszText,cchLen,psz);
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::DrawRectangle(LPCRECT pRect)
    {
		if(!m_curPen) return E_INVALIDARG;

		RECT rcBuf = *pRect;
		if (m_curPen->GetWidth() == 1)
		{
			rcBuf.right--;
			rcBuf.bottom--;
		}
		else
		{
			::InflateRect(&rcBuf, -m_curPen->GetWidth() / 2, -m_curPen->GetWidth() / 2);
		}
        ALPHAINFO ai;
        CGdiAlpha::AlphaBackup(m_hdc, pRect,ai);
        HGDIOBJ oldBr=::SelectObject(m_hdc,GetStockObject(NULL_BRUSH));
        ::Rectangle(m_hdc, rcBuf.left, rcBuf.top, rcBuf.right, rcBuf.bottom);
        CGdiAlpha::AlphaRestore(ai);
        ::SelectObject(m_hdc,oldBr);
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::FillRectangle(LPCRECT pRect)
    {
        BYTE byAlpha=0xFF;
        if(!m_curBrush->IsBitmap()) byAlpha = GetAValue(m_curBrush->GetColor());

        DCBuffer dcBuf(m_hdc,pRect,byAlpha,FALSE);
        
        HGDIOBJ oldPen=::SelectObject(dcBuf,GetStockObject(NULL_PEN));
        ::Rectangle(dcBuf,pRect->left,pRect->top,pRect->right,pRect->bottom);
        ::SelectObject(dcBuf,oldPen);
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::DrawRoundRect( LPCRECT pRect,POINT pt )
    {
        DCBuffer dcBuf(m_hdc,pRect,GetAValue(m_curPen->GetColor()));
        HGDIOBJ oldBr=::SelectObject(dcBuf,GetStockObject(NULL_BRUSH));
        ::RoundRect(dcBuf,pRect->left,pRect->top,pRect->right,pRect->bottom,pt.x*2,pt.y*2);
        ::SelectObject(dcBuf,oldBr);
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::FillRoundRect( LPCRECT pRect,POINT pt )
    {
        BYTE byAlpha=0xFF;
        if(!m_curBrush->IsBitmap()) byAlpha = GetAValue(m_curBrush->GetColor());
        DCBuffer dcBuf(m_hdc,pRect,byAlpha);

        HGDIOBJ oldPen=::SelectObject(dcBuf,GetStockObject(NULL_PEN));
        ::RoundRect(dcBuf,pRect->left,pRect->top,pRect->right,pRect->bottom,pt.x,pt.y);
        ::SelectObject(dcBuf,oldPen);
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::FillSolidRoundRect(LPCRECT pRect,POINT pt,COLORREF cr)
    {
        DCBuffer dcBuf(m_hdc,pRect,GetAValue(cr),FALSE);
        HBRUSH br=::CreateSolidBrush(cr&0x00ffffff);
		HGDIOBJ oldObj=::SelectObject(dcBuf,br);
		HGDIOBJ oldPen = ::SelectObject(dcBuf, GetStockObject(NULL_PEN));
		::RoundRect(dcBuf,pRect->left,pRect->top,pRect->right,pRect->bottom,pt.x*2,pt.y*2);
		::SelectObject(dcBuf, oldPen);
		::SelectObject(dcBuf,oldObj);
        ::DeleteObject(br);
        return S_OK;    
    }

    HRESULT SRenderTarget_GDI::DrawLines(LPPOINT pPt,size_t nCount)
    {
        RECT rc={100000,100000,0,0};
        for(size_t i=0;i<nCount;i++)
        {
            if(pPt[i].x<rc.left) rc.left=pPt[i].x;
            if(pPt[i].x>rc.right) rc.right=pPt[i].x;
            if(pPt[i].y<rc.top) rc.top=pPt[i].y;
            if(pPt[i].y>rc.bottom) rc.bottom=pPt[i].y;
        }
        rc.left -= 1;
        rc.top -=1;
        int nPenWidth = 1;
        if(m_curPen) nPenWidth = m_curPen->GetWidth();
        rc.bottom+=nPenWidth;
        rc.right+=nPenWidth;

        DCBuffer dcBuf(m_hdc,&rc,GetAValue(m_curPen->GetColor()));
        ::Polyline(dcBuf,pPt,(int)nCount);
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::TextOut( int x, int y, LPCTSTR lpszString, int nCount)
    {
        if(nCount<0) nCount = _tcslen(lpszString);
        SIZE sz;
        MeasureText(lpszString,nCount,&sz);
        RECT rc={x,y,x+sz.cx,y+sz.cy};
        return DrawText(lpszString,nCount,&rc,DT_LEFT|DT_SINGLELINE);
    }

    HRESULT SRenderTarget_GDI::DrawIconEx( int xLeft, int yTop, HICON hIcon, int cxWidth,int cyWidth,UINT diFlags )
    {
        ICONINFO ii={0};
        ::GetIconInfo(hIcon,&ii);
        SASSERT(ii.hbmColor);
        BITMAP bm;
        ::GetObject(ii.hbmColor,sizeof(bm),&bm);
        
        ALPHAINFO ai;
        RECT rc={xLeft,yTop,xLeft+cxWidth,yTop+cyWidth};
        if(bm.bmBitsPixel!=32)
        {
            CGdiAlpha::AlphaBackup(m_hdc,&rc,ai);
        }
        BOOL bRet=::DrawIconEx(m_hdc,xLeft,yTop,hIcon,cxWidth,cyWidth,0,NULL,diFlags);
        
        if(bm.bmBitsPixel!=32)
        {
            CGdiAlpha::AlphaRestore(ai);
        }
        
        if(ii.hbmColor) DeleteObject(ii.hbmColor);
        if(ii.hbmMask) DeleteObject(ii.hbmMask);
        
        return bRet?S_OK:S_FALSE;
    }

    HRESULT SRenderTarget_GDI::DrawBitmap(LPCRECT pRcDest,IBitmap *pBitmap,int xSrc,int ySrc,BYTE byAlpha/*=0xFF*/ )
    {
        SBitmap_GDI *pBmp = (SBitmap_GDI*)pBitmap;
        HBITMAP bmp=pBmp->GetBitmap();
        HDC hmemdc=CreateCompatibleDC(m_hdc);
        ::SelectObject(hmemdc,bmp);
        SIZE szBmp = pBmp->Size();
        
        BLENDFUNCTION bf={ AC_SRC_OVER,0,byAlpha,AC_SRC_ALPHA};
        int nWid= (std::min)(pRcDest->right-pRcDest->left,szBmp.cx);
        int nHei= (std::min)(pRcDest->bottom-pRcDest->top,szBmp.cy);
        
        BOOL bOK=::AlphaBlend(m_hdc,pRcDest->left,pRcDest->top,nWid,nHei,
                   hmemdc,xSrc,ySrc,nWid,nHei,bf);
        DeleteDC(hmemdc);
        
        return bOK?S_OK:E_FAIL;
    }

    HRESULT SRenderTarget_GDI::AlphaBlend( LPCRECT pRcDest,IRenderTarget *pRTSrc,LPCRECT pRcSrc,BYTE byAlpha )
    {
        BLENDFUNCTION bf={ AC_SRC_OVER,0,byAlpha,AC_SRC_ALPHA};
        SRenderTarget_GDI *pRTGdiSrc=(SRenderTarget_GDI*)pRTSrc;
        BOOL bOK=::AlphaBlend(m_hdc,pRcDest->left,pRcDest->top,pRcDest->right-pRcDest->left,pRcDest->bottom-pRcDest->top,
                              pRTGdiSrc->m_hdc,pRcSrc->left,pRcSrc->top,pRcSrc->right-pRcSrc->left,pRcSrc->bottom-pRcSrc->top,
                              bf);
        return bOK?S_OK:E_FAIL;
    }

    HRESULT SRenderTarget_GDI::DrawBitmapEx( LPCRECT pRcDest,IBitmap *pBitmap,LPCRECT pRcSrc,UINT expendMode, BYTE byAlpha/*=0xFF*/ )
    {
        UINT expandModeLow = LOWORD(expendMode);
        
        if(expandModeLow == EM_NULL)
            return DrawBitmap(pRcDest,pBitmap,pRcSrc->left,pRcSrc->top,byAlpha);

        SBitmap_GDI *pBmp = (SBitmap_GDI*)pBitmap;
        HBITMAP bmp=pBmp->GetBitmap();
        HDC hmemdc=CreateCompatibleDC(m_hdc);
        ::SelectObject(hmemdc,bmp);

        BLENDFUNCTION bf={ AC_SRC_OVER,0,byAlpha,AC_SRC_ALPHA};
        if(expandModeLow == EM_STRETCH)
        {
            ::AlphaBlend(m_hdc,pRcDest->left,pRcDest->top,pRcDest->right-pRcDest->left,pRcDest->bottom-pRcDest->top,
                hmemdc,pRcSrc->left,pRcSrc->top,pRcSrc->right-pRcSrc->left,pRcSrc->bottom-pRcSrc->top,bf);
        }else
        {
            ::SaveDC(m_hdc);
            ::IntersectClipRect(m_hdc,pRcDest->left,pRcDest->top,pRcDest->right,pRcDest->bottom);
            int nWid=pRcSrc->right-pRcSrc->left;
            int nHei=pRcSrc->bottom-pRcSrc->top;
            for(int y=pRcDest->top ;y<pRcDest->bottom;y+=nHei)
            {
                for(int x=pRcDest->left; x<pRcDest->right; x+=nWid)
                {
                    ::AlphaBlend(m_hdc,x,y,nWid,nHei,
                        hmemdc,pRcSrc->left,pRcSrc->top,nWid,nHei,
                        bf);                    
                }
            }
            ::RestoreDC(m_hdc,-1);
        }
        DeleteDC(hmemdc);
        return S_OK;

    }


    HRESULT SRenderTarget_GDI::DrawBitmap9Patch( LPCRECT pRcDest,IBitmap *pBitmap,LPCRECT pRcSrc,LPCRECT pRcSourMargin,UINT expendMode,BYTE byAlpha/*=0xFF*/ )
    {
        int xDest[4] = {pRcDest->left,pRcDest->left+pRcSourMargin->left,pRcDest->right-pRcSourMargin->right,pRcDest->right};
        int xSrc[4] = {pRcSrc->left,pRcSrc->left+pRcSourMargin->left,pRcSrc->right-pRcSourMargin->right,pRcSrc->right};
        int yDest[4] = {pRcDest->top,pRcDest->top+pRcSourMargin->top,pRcDest->bottom-pRcSourMargin->bottom,pRcDest->bottom};
        int ySrc[4] = {pRcSrc->top,pRcSrc->top+pRcSourMargin->top,pRcSrc->bottom-pRcSourMargin->bottom,pRcSrc->bottom};

        //首先保证九宫分割正常
        if(!(xSrc[0] <= xSrc[1] && xSrc[1] <= xSrc[2] && xSrc[2] <= xSrc[3])) return S_FALSE;
        if(!(ySrc[0] <= ySrc[1] && ySrc[1] <= ySrc[2] && ySrc[2] <= ySrc[3])) return S_FALSE;

        //调整目标位置
        int nDestWid=pRcDest->right-pRcDest->left;
        int nDestHei=pRcDest->bottom-pRcDest->top;

        if((pRcSourMargin->left + pRcSourMargin->right) > nDestWid)
        {//边缘宽度大于目标宽度的处理
            if(pRcSourMargin->left >= nDestWid)
            {//只绘制左边部分
                xSrc[1] = xSrc[2] = xSrc[3] = xSrc[0]+nDestWid;
                xDest[1] = xDest[2] = xDest[3] = xDest[0]+nDestWid;
            }else if(pRcSourMargin->right >= nDestWid)
            {//只绘制右边部分
                xSrc[0] = xSrc[1] = xSrc[2] = xSrc[3]-nDestWid;
                xDest[0] = xDest[1] = xDest[2] = xDest[3]-nDestWid;
            }else
            {//先绘制左边部分，剩余的用右边填充
                int nRemain=xDest[3]-xDest[1];
                xSrc[2] = xSrc[3]-nRemain;
                xDest[2] = xDest[3]-nRemain;
            }
        }

        if(pRcSourMargin->top + pRcSourMargin->bottom > nDestHei)
        {
            if(pRcSourMargin->top >= nDestHei)
            {//只绘制上边部分
                ySrc[1] = ySrc[2] = ySrc[3] = ySrc[0]+nDestHei;
                yDest[1] = yDest[2] = yDest[3] = yDest[0]+nDestHei;
            }else if(pRcSourMargin->bottom >= nDestHei)
            {//只绘制下边部分
                ySrc[0] = ySrc[1] = ySrc[2] = ySrc[3]-nDestHei;
                yDest[0] = yDest[1] = yDest[2] = yDest[3]-nDestHei;
            }else
            {//先绘制左边部分，剩余的用右边填充
                int nRemain=yDest[3]-yDest[1];
                ySrc[2] = ySrc[3]-nRemain;
                yDest[2] = yDest[3]-nRemain;
            }
        }

        //定义绘制模式
        UINT mode[3][3]={
            {EM_NULL,expendMode,EM_NULL},
            {expendMode,expendMode,expendMode},
            {EM_NULL,expendMode,EM_NULL}
        };

        for(int y=0;y<3;y++)
        {
            if(ySrc[y] == ySrc[y+1]) continue;
            for(int x=0;x<3;x++)
            {
                if(xSrc[x] == xSrc[x+1]) continue;
                RECT rcSrc = {xSrc[x],ySrc[y],xSrc[x+1],ySrc[y+1]};
                RECT rcDest ={xDest[x],yDest[y],xDest[x+1],yDest[y+1]};
                DrawBitmapEx(&rcDest,pBitmap,&rcSrc,mode[y][x],byAlpha);
            }
        }

        return S_OK;
    }

    IRenderObj * SRenderTarget_GDI::GetCurrentObject( OBJTYPE uType )
    {
        IRenderObj *pRet=NULL;
        switch(uType)
        {
        case OT_BITMAP: 
            pRet=m_curBmp;
            break;
        case OT_PEN:
            pRet=m_curPen;
            break;
        case OT_BRUSH:
            pRet=m_curBrush;
            break;
        case OT_FONT:
            pRet=m_curFont;
            break;
        }
        return pRet;
    }

    HRESULT SRenderTarget_GDI::SelectDefaultObject(OBJTYPE objType, IRenderObj ** ppOldObj)
    {
        IRenderObj *pDefObj = NULL;
        switch(objType)
        {
        case OT_BITMAP: pDefObj = m_defBmp;break;
        case OT_PEN: pDefObj = m_defPen;break;
        case OT_BRUSH: pDefObj = m_defBrush;break;
        case OT_FONT: pDefObj = m_defFont;break;
        default:return E_INVALIDARG;
        }
        if(pDefObj == GetCurrentObject(objType)) 
            return S_FALSE;
        return SelectObject(pDefObj,ppOldObj);
    }

    HRESULT SRenderTarget_GDI::SelectObject( IRenderObj *pObj,IRenderObj ** ppOldObj /*= NULL*/ )
    {
        CAutoRefPtr<IRenderObj> pRet;
        switch(pObj->ObjectType())
        {
        case OT_BITMAP: 
            pRet=m_curBmp;
            m_curBmp=(SBitmap_GDI*)pObj;
            ::SelectObject(m_hdc,m_curBmp->GetBitmap());
            break;
        case OT_PEN:
            pRet=m_curPen;
            m_curPen=(SPen_GDI*)pObj;
            ::SelectObject(m_hdc,m_curPen->GetPen());
            break;
        case OT_BRUSH:
            pRet=m_curBrush;
            m_curBrush=(SBrush_GDI*)pObj;
            ::SelectObject(m_hdc,m_curBrush->GetBrush());
            break;
        case OT_FONT:
            pRet=m_curFont;
            m_curFont=(SFont_GDI*)pObj;
            ::SelectObject(m_hdc,m_curFont->GetFont());
            break;
        }
        if(pRet && ppOldObj)
        {//由调用者调用Release释放该RenderObj
            pRet->AddRef();
            *ppOldObj = pRet;
        }
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::OffsetViewportOrg( int xOff, int yOff, LPPOINT lpPoint )
    {
        if(lpPoint)
        {
            *lpPoint=m_ptOrg;
        }
        m_ptOrg.x+=xOff;
        m_ptOrg.y+=yOff;
        ::SetViewportOrgEx(m_hdc,m_ptOrg.x,m_ptOrg.y,NULL);
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::GetViewportOrg( LPPOINT lpPoint )
    {
        if(lpPoint)
        {
            *lpPoint=m_ptOrg;
        }
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::SetViewportOrg( POINT pt )
    {
        m_ptOrg = pt;
        ::SetViewportOrgEx(m_hdc,m_ptOrg.x,m_ptOrg.y,NULL);
        return S_OK;
    }

    HDC SRenderTarget_GDI::GetDC( UINT uFlag )
    {
        m_uGetDCFlag = uFlag;
        return m_hdc;
    }

    void SRenderTarget_GDI::ReleaseDC( HDC hdc )
    {
        if(hdc == m_hdc)
        {
            m_uGetDCFlag =0;
        }
    }

    HRESULT SRenderTarget_GDI::GradientFill( LPCRECT pRect,BOOL bVert,COLORREF crBegin,COLORREF crEnd,BYTE byAlpha/*=0xFF*/ )
    {
        if(byAlpha!=0xFF)
        {
            DCBuffer dcBuf(m_hdc,pRect,byAlpha,FALSE);
            GradientFillRect(dcBuf,pRect,crBegin,crEnd,bVert);
        }else
        {
            GradientFillRect(m_hdc,pRect,crBegin,crEnd,bVert);
        }
        return S_OK;
    }
    
    HRESULT SRenderTarget_GDI::GradientFillEx( LPCRECT pRect,const POINT* pts,COLORREF *colors,float *pos,int nCount,BYTE byAlpha/*=0xFF */ )
    {
		(pRect);
		(pts);
		(colors);
		(pos);
		(nCount);
		(byAlpha);
        return E_NOTIMPL;
    }

    //通过一个内存位图来填充位置的alpha值
    HRESULT SRenderTarget_GDI::FillSolidRect( LPCRECT pRect,COLORREF cr )
    {
        DCBuffer dcBuf(m_hdc,pRect,GetAValue(cr),FALSE);
        HBRUSH br=::CreateSolidBrush(cr&0x00ffffff);
        ::FillRect(dcBuf,pRect,br);
        ::DeleteObject(br);
        return S_OK;    
    }

    HRESULT SRenderTarget_GDI::ClearRect( LPCRECT pRect,COLORREF cr )
    {
        int nWid=pRect->right-pRect->left;
        int nHei=pRect->bottom-pRect->top;
		if(nWid == 0 || nHei == 0)
			return S_OK;

        LPDWORD pBits;
        HBITMAP hBmp=SBitmap_GDI::CreateGDIBitmap(nWid,nHei,(void**)&pBits);
        HDC hMemDC=CreateCompatibleDC(m_hdc);
        ::SelectObject(hMemDC,hBmp);

        SColor color(cr);
        DWORD dwColor=color.toARGB();

		for(int i=0;i<nHei;i++)for(int j=0;j<nWid;j++) 
			*pBits++ = dwColor;

        ::BitBlt(m_hdc,pRect->left,pRect->top,nWid,nHei,hMemDC,0,0,SRCCOPY);
        ::DeleteDC(hMemDC);
        ::DeleteObject(hBmp);
        return S_OK;    
    }

    HRESULT SRenderTarget_GDI::InvertRect(LPCRECT pRect)
    {
        return ::InvertRect(m_hdc,pRect)?S_OK:E_UNEXPECTED;
    }

    HRESULT SRenderTarget_GDI::DrawEllipse( LPCRECT pRect )
    {
		if(!m_curPen) return E_INVALIDARG;
		RECT rcBuf = *pRect;
		::InflateRect(&rcBuf,m_curPen->GetWidth()/2,m_curPen->GetWidth()/2);
		DCBuffer dcBuf(m_hdc,&rcBuf,GetAValue(m_curPen->GetColor()));

        ::Ellipse(dcBuf,pRect->left,pRect->top,pRect->right,pRect->bottom);
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::FillEllipse( LPCRECT pRect )
    {
        BYTE byAlpha=0xFF;
        if(!m_curBrush->IsBitmap()) byAlpha = GetAValue(m_curBrush->GetColor());
        DCBuffer dcBuf(m_hdc,pRect,byAlpha);
        ::Ellipse(dcBuf,pRect->left,pRect->top,pRect->right,pRect->bottom);
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::FillSolidEllipse(LPCRECT pRect,COLORREF cr)
    {
        DCBuffer dcBuf(m_hdc,pRect,GetAValue(cr),FALSE);
        HBRUSH br=::CreateSolidBrush(cr&0x00ffffff);
        ::Ellipse(dcBuf,pRect->left,pRect->top,pRect->right,pRect->bottom);
        ::DeleteObject(br);
        return S_OK;    
    }

    const float PI = 3.1415926f;

    HRESULT SRenderTarget_GDI::DrawArc( LPCRECT pRect,float startAngle,float sweepAngle,bool useCenter )
    {
		if(!m_curPen) return E_INVALIDARG;

		RECT rcBuf = *pRect;
		::InflateRect(&rcBuf,m_curPen->GetWidth()/2,m_curPen->GetWidth()/2);
        DCBuffer dcBuf(m_hdc,&rcBuf,GetAValue(m_curPen->GetColor()));

        HGDIOBJ oldBr=::SelectObject(dcBuf,GetStockObject(NULL_BRUSH));
        POINT ptCenter = {(pRect->left+pRect->right)/2,(pRect->top+pRect->bottom)/2};
        int   a=ptCenter.x-pRect->left,b=ptCenter.y-pRect->top;
        POINT pt1,pt2;
        float startAngle2 =startAngle*PI/180.0f;
        float endAngle2 = (startAngle+sweepAngle)*PI/180.0f;
        pt1.x=ptCenter.x+(int)(a*cos(startAngle2));
        pt1.y=ptCenter.y+(int)(b*sin(startAngle2));
        pt2.x=ptCenter.x+(int)(a*cos(endAngle2));
        pt2.y=ptCenter.y+(int)(b*sin(endAngle2));
		if (useCenter)
			::Pie(dcBuf, pRect->left, pRect->top, pRect->right, pRect->bottom, pt2.x, pt2.y, pt1.x, pt1.y);
        else
			::Arc(dcBuf,pRect->left,pRect->top,pRect->right,pRect->bottom,pt2.x,pt2.y,pt1.x,pt1.y);
        ::SelectObject(dcBuf,oldBr);
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::FillArc( LPCRECT pRect,float startAngle,float sweepAngle )
    {
        BYTE byAlpha=0xFF;
        if(!m_curBrush->IsBitmap()) byAlpha = GetAValue(m_curBrush->GetColor());
        DCBuffer dcBuf(m_hdc,pRect,byAlpha);

        HGDIOBJ oldPen=::SelectObject(dcBuf,GetStockObject(NULL_PEN));
        POINT ptCenter = {(pRect->left+pRect->right)/2,(pRect->top+pRect->bottom)/2};
        int   a=ptCenter.x-pRect->left,b=ptCenter.y-pRect->top;
        POINT pt1,pt2;
        float startAngle2 =startAngle*PI/180.0f;
        float endAngle2 = (startAngle+sweepAngle)*PI/180.0f;
        pt1.x=ptCenter.x+(int)(a*cos(startAngle2));
        pt1.y=ptCenter.y+(int)(b*sin(startAngle2));
        pt2.x=ptCenter.x+(int)(a*cos(endAngle2));
        pt2.y=ptCenter.y+(int)(b*sin(endAngle2));
        ::Pie(dcBuf,pRect->left,pRect->top,pRect->right,pRect->bottom,pt2.x,pt2.y,pt1.x,pt1.y);
        ::SelectObject(dcBuf,oldPen);
        return S_OK;
    }

    HRESULT SRenderTarget_GDI::SetTransform(const IxForm * pXForm,IxForm *pOldXFrom/*=NULL*/)
    {
        const XFORM * pXFormGDI = (const XFORM*)pXForm;
        if(pOldXFrom)
        {
            GetTransform(pOldXFrom);
        }
        return ::SetWorldTransform(m_hdc,pXFormGDI)?S_OK:E_FAIL;
    }

    HRESULT SRenderTarget_GDI::GetTransform(IxForm * pXForm) const
    {
        XFORM * pXFormGDI = (XFORM*)pXForm;
        return ::GetWorldTransform(m_hdc,pXFormGDI)?S_OK:E_FAIL;
    }

	COLORREF SRenderTarget_GDI::GetPixel( int x, int y )
	{
		if(!m_curBmp) return CR_INVALID;
		const COLORREF *pBits = (const COLORREF*)m_curBmp->GetPixelBits();
		POINT pt;
		GetViewportOrg(&pt);
		x += pt.x;
		y += pt.y;
		if(x<0 || x >= (int)m_curBmp->Width() || y<0 || y>= (int)m_curBmp->Height())
			return CR_INVALID;

		return pBits[y*m_curBmp->Width()+x];
	}

	COLORREF SRenderTarget_GDI::SetPixel( int x, int y, COLORREF cr )
	{
		if(!m_curBmp) return CR_INVALID;
		COLORREF *pBits = (COLORREF*)m_curBmp->LockPixelBits();
		POINT pt;
		GetViewportOrg(&pt);
		x += pt.x;
		y += pt.y;
		
		if(x >= (int)m_curBmp->Width() || y>= (int)m_curBmp->Height())
			return CR_INVALID;

		COLORREF crRet = pBits[y*m_curBmp->Width()+x];
	
		pBits[y*m_curBmp->Width()+x] = cr;
		
		m_curBmp->UnlockPixelBits(pBits);

		return crRet;
	}

	HRESULT SRenderTarget_GDI::GradientFill2(LPCRECT pRect,GradientType type,COLORREF crStart,COLORREF crCenter,COLORREF crEnd,float fLinearAngle,float fCenterX,float fCenterY,int nRadius,BYTE byAlpha/*=0xff*/)
	{
		(pRect), (type), (crStart), (crCenter), (crEnd), (fLinearAngle), (fCenterX), (fCenterY), (nRadius), (byAlpha);
		return E_NOTIMPL;
	}

	HRESULT SRenderTarget_GDI::CreateRegion(IRegion ** ppRegion)
	{
		return m_pRenderFactory->CreateRegion(ppRegion)?S_OK:E_OUTOFMEMORY;
	}

	HRESULT SRenderTarget_GDI::ClipPath(const IPath * path, UINT mode, bool doAntiAlias /*= false*/)
	{
		return E_NOTIMPL;
	}

	HRESULT SRenderTarget_GDI::DrawPath(const IPath * path,IPathEffect * pathEffect)
	{
		return E_NOTIMPL;
	}


    //////////////////////////////////////////////////////////////////////////
    namespace RENDER_GDI
    {
        BOOL SCreateInstance(IObjRef ** ppRenderFactory)
        {
            *ppRenderFactory = new SRenderFactory_GDI;
            return TRUE;
        }
    }

}


