#include <core\SkShader.h>
#include <core\SkDevice.h>
#include <effects\SkDashPathEffect.h>
#include <effects\SkGradientShader.h>
#include <effects\SkBlurMaskFilter.h>
#include "../skia/src/effects/SkBlurMask.h"

#include <gdialpha.h>

#include "drawtext-skia.h"

#include "render-skia.h"
#include "Render-Skia2.h"
#include "trace.h"

#include "skia2rop2.h"
#include "PathEffect-Skia.h"
#include "PathMeasure-Skia.h"

#include <tchar.h>
#include <algorithm>

#define getTotalClip internal_private_getTotalClip
// #include <vld.h>

#ifndef PI	//PI
#define PI         ((float)3.141592654f)
#endif

namespace SOUI
{
	//PS_SOLID
	const float  ps_solid[] ={1.0f,0.0f};
	const float  ps_dash[] ={5.0f,5.0f};
	const float  ps_dot[] ={1.0f,4.0f};
	const float  ps_dashdot[] ={4.0f,1.0f,1.0f,1.0f};
	const float  ps_dashdotdot[] ={4.0f,1.0f,1.0f,1.0f,1.0f,1.0f};

	const struct LineDashEffect
	{
		const float  *fDash;
		int		nCount;
	}LINEDASHEFFECT[] =
	{
		{ps_solid,ARRAYSIZE(ps_solid)},
		{ps_dash,ARRAYSIZE(ps_dash)},
		{ps_dot,ARRAYSIZE(ps_dot)},
		{ps_dashdot,ARRAYSIZE(ps_dashdot)},
		{ps_dashdotdot,ARRAYSIZE(ps_dashdotdot)},
	};

	bool String2Bool(const SStringW & value)
	{
		SASSERT(!value.IsEmpty());
		return !(value == L"false" || value == L"0");
	}

    SkIRect toSkIRect(LPCRECT pRc)
    {
        SkIRect rc={pRc->left,pRc->top,pRc->right,pRc->bottom};
        return rc;
    }

    SkRect toSkRect(LPCRECT pRc)
    {
        SkIRect rc={pRc->left,pRc->top,pRc->right,pRc->bottom};
        return SkRect::Make(rc);
    }
    
	void SkRect2RECT(const SkRect & rcIn, RECT * rcOut)
	{
		rcOut->left = (int)rcIn.fLeft;
		rcOut->top = (int)rcIn.fTop;
		rcOut->right = (int)(rcIn.fRight + 0.5f);
		rcOut->bottom = (int)(rcIn.fBottom + 0.5f);
	}

    void InflateSkRect(SkRect *pRect,SkScalar dx,SkScalar dy)
    {
        pRect->fLeft -= dx;
        pRect->fRight += dx;
        pRect->fTop -= dy;
        pRect->fBottom += dy;
    }
    
    int RectWid(LPCRECT pRc){return pRc->right-pRc->left;}
    int RectHei(LPCRECT pRc){return pRc->bottom-pRc->top;}
    
	class SGetLineDashEffect
	{
	public:
		SGetLineDashEffect(int iStyle):pDashPathEffect(NULL)
		{
			if(iStyle>=PS_SOLID && iStyle<=PS_DASHDOTDOT)
			{
				const LineDashEffect *pEff=&LINEDASHEFFECT[iStyle];
				pDashPathEffect=SkDashPathEffect::Create(pEff->fDash,pEff->nCount,0.0f);
			}
		}
		~SGetLineDashEffect()
		{
			if(pDashPathEffect) pDashPathEffect->unref();
		}

		SkDashPathEffect * Get() const{return pDashPathEffect;}
	private:
		SkDashPathEffect * pDashPathEffect;
	};
	//////////////////////////////////////////////////////////////////////////
	// SRenderFactory_Skia

	BOOL SRenderFactory_Skia::CreateRenderTarget( IRenderTarget ** ppRenderTarget ,int nWid,int nHei)
	{
		*ppRenderTarget = new SRenderTarget_Skia(this, nWid, nHei);
		return TRUE;
	}

    BOOL SRenderFactory_Skia::CreateFont( IFont ** ppFont , const LOGFONT &lf )
    {
        *ppFont = new SFont_Skia(this,&lf);
        return TRUE;
    }

    BOOL SRenderFactory_Skia::CreateBitmap( IBitmap ** ppBitmap )
    {
        *ppBitmap = new SBitmap_Skia(this);
        return TRUE;
    }

    BOOL SRenderFactory_Skia::CreateRegion( IRegion **ppRgn )
    {
        *ppRgn = new SRegion_Skia(this);
        return TRUE;
    }

	BOOL SRenderFactory_Skia::CreatePath(IPath ** ppPath)
	{
		*ppPath = new SPath_Skia(this);
		return TRUE;
	}

	BOOL SRenderFactory_Skia::CreatePathEffect(REFGUID guidEffect,IPathEffect ** ppPathEffect)
	{
		* ppPathEffect = NULL;
		if(guidEffect == __uuidof(ICornerPathEffect))
		{
			*ppPathEffect = (IPathEffect*) new SPathEffect_Corner();
		}else if(guidEffect == __uuidof(IDashPathEffect))
		{
			*ppPathEffect = (IPathEffect *) new SPathEffect_Dash();
		}else if(guidEffect == __uuidof(IDiscretePathEffect))
		{
			*ppPathEffect = (IPathEffect *) new SPathEffect_Discrete();
		}
		return (*ppPathEffect) != NULL;
	}

	BOOL SRenderFactory_Skia::CreatePathMeasure(IPathMeasure ** ppPathMeasure)
	{
		*ppPathMeasure = new SPathMeasure_Skia;
		return TRUE;
	}

    //////////////////////////////////////////////////////////////////////////
	// SRenderTarget_Skia

	SRenderTarget_Skia::SRenderTarget_Skia( IRenderFactory* pRenderFactory ,int nWid,int nHei)
		:m_SkCanvas(NULL)
        ,m_curColor(0xFF000000)//默认黑色
        ,m_hGetDC(0)
        ,m_uGetDCFlag(0)
		,m_bAntiAlias(true)
	{
        m_ptOrg.fX=m_ptOrg.fY=0.0f;
        m_pRenderFactory = pRenderFactory;

        m_SkCanvas = new SkCanvas();

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
		CAutoRefPtr<IPen> pPen;
		CreatePen(PS_SOLID,SColor(0,0,0).toCOLORREF(),1,&pPen);
		SelectObject(pPen);
	}
	
	SRenderTarget_Skia::~SRenderTarget_Skia()
	{
		if(m_SkCanvas) delete m_SkCanvas;
	}

	HRESULT SRenderTarget_Skia::CreateCompatibleRenderTarget( SIZE szTarget,IRenderTarget **ppRenderTarget )
	{
        SRenderTarget_Skia *pRT = new SRenderTarget_Skia(m_pRenderFactory,szTarget.cx,szTarget.cy);
        *ppRenderTarget = pRT;
		return S_OK;
	}

	HRESULT SRenderTarget_Skia::CreatePen( int iStyle,COLORREF cr,int cWidth,IPen ** ppPen )
	{
		*ppPen = new SPen_Skia(m_pRenderFactory,iStyle,cr,cWidth);
		return S_OK;
	}

	HRESULT SRenderTarget_Skia::CreateSolidColorBrush( COLORREF cr,IBrush ** ppBrush )
	{
		*ppBrush = SBrush_Skia::CreateSolidBrush(m_pRenderFactory,cr);
		return S_OK;
	}

	HRESULT SRenderTarget_Skia::CreateBitmapBrush( IBitmap *pBmp,IBrush ** ppBrush )
	{
		SBitmap_Skia *pBmpSkia = (SBitmap_Skia*)pBmp;
		*ppBrush = SBrush_Skia::CreateBitmapBrush(m_pRenderFactory,pBmpSkia->GetSkBitmap());
		return S_OK;
	}

	HRESULT SRenderTarget_Skia::CreateRegion(IRegion ** ppRegion)
	{
		return m_pRenderFactory->CreateRegion(ppRegion)?S_OK:E_OUTOFMEMORY;
	}

	HRESULT SRenderTarget_Skia::Resize( SIZE sz )
	{
    	m_curBmp->Init(sz.cx,sz.cy);
        delete m_SkCanvas;
        m_SkCanvas = new SkCanvas(m_curBmp->GetSkBitmap());
		return S_OK;
	}

	HRESULT SRenderTarget_Skia::PushClipRect( LPCRECT pRect ,UINT mode/*=RGN_AND*/)
	{
        SkRect skrc=toSkRect(pRect);
        skrc.offset(m_ptOrg);
        
        m_SkCanvas->save();

        m_SkCanvas->clipRect(skrc,SRegion_Skia::RGNMODE2SkRgnOP(mode));
	    return S_OK;
	}

	HRESULT SRenderTarget_Skia::PushClipRegion( IRegion *pRegion ,UINT mode/*=RGN_AND*/)
	{
        SRegion_Skia * rgn_skia=(SRegion_Skia*)pRegion;
        SkRegion rgn=rgn_skia->GetRegion();
        rgn.translate((int)m_ptOrg.fX,(int)m_ptOrg.fY);

        m_SkCanvas->save();

        m_SkCanvas->clipRegion(rgn,SRegion_Skia::RGNMODE2SkRgnOP(mode));
        
		return S_OK;
	}

    HRESULT SRenderTarget_Skia::PopClip()
    {
        m_SkCanvas->restore();
        return S_OK;
    }

    HRESULT SRenderTarget_Skia::ExcludeClipRect( LPCRECT pRc )
    {
        SkRect skrc=toSkRect(pRc);
        skrc.offset(m_ptOrg);
        m_SkCanvas->clipRect(skrc,SkRegion::kDifference_Op);
        return S_OK;
    }

    HRESULT SRenderTarget_Skia::IntersectClipRect( LPCRECT pRc )
    {
        SkRect skrc=toSkRect(pRc);
        skrc.offset(m_ptOrg);
        m_SkCanvas->clipRect(skrc,SkRegion::kIntersect_Op);
        return S_OK;
    }

    HRESULT SRenderTarget_Skia::SaveClip( int *pnState )
    {
        int nState=m_SkCanvas->save();
        if(pnState) *pnState=nState;
        return S_OK;
    }

    HRESULT SRenderTarget_Skia::RestoreClip( int nState/*=-1*/ )
    {
        m_SkCanvas->restoreToCount(nState);
        return S_OK;
    }

    HRESULT SRenderTarget_Skia::GetClipRegion( IRegion **ppRegion )
    {
        SRegion_Skia *pRgn=new SRegion_Skia(m_pRenderFactory);
        SkRegion rgn = m_SkCanvas->getTotalClip();
        //需要将rect的viewOrg还原
        rgn.translate((int)-m_ptOrg.fX,(int)-m_ptOrg.fY);
        pRgn->SetRegion(rgn);
        *ppRegion = pRgn;
        return S_OK;
    }

    HRESULT SRenderTarget_Skia::GetClipBox(LPRECT prc)
    {
        SkRect skrc;
        m_SkCanvas->getClipBounds(&skrc);
        //需要将rect的viewOrg还原
        skrc.offset(-m_ptOrg);

        prc->left=(LONG)skrc.fLeft;
        prc->top=(LONG)skrc.fTop;
        prc->right=(LONG)skrc.fRight;
        prc->bottom=(LONG)skrc.fBottom;
        //需要4周缩小一个单位才是和GDI相同的剪裁区
        ::InflateRect(prc,-1,-1);
        return S_OK;
    }
    
	HRESULT SRenderTarget_Skia::BitBlt( LPCRECT pRcDest,IRenderTarget *pRTSour,int xSrc,int ySrc,DWORD dwRop/*=SRCCOPY*/)
	{
        SkPaint paint;
        paint.setStyle(SkPaint::kFill_Style);
        dwRop = dwRop & 0x7fffffff;
        switch(dwRop)
        {
        case SRCCOPY:
            paint.setXfermodeMode(SkXfermode::kSrc_Mode);
            break;
        case DSTINVERT:
            paint.setXfermode(new ProcXfermode(ProcXfermode::Rop2_Invert));
            break;
        case SRCINVERT:
            paint.setXfermode(new ProcXfermode(ProcXfermode::Rop2_Xor));
            break;
        case SRCAND:
            paint.setXfermode(new ProcXfermode(ProcXfermode::Rop2_And));
            break;
        default:
            SASSERT(FALSE);
            break;
        }

        SRenderTarget_Skia *pRtSourSkia=(SRenderTarget_Skia*)pRTSour;
        SkBitmap    bmpSrc=pRtSourSkia->m_curBmp->GetSkBitmap();
        POINT ptSourViewport;
        pRtSourSkia->GetViewportOrg(&ptSourViewport);
        xSrc += ptSourViewport.x;
        ySrc += ptSourViewport.y;
        
        
        SkIRect isrc={xSrc,ySrc,xSrc + pRcDest->right-pRcDest->left,ySrc+pRcDest->bottom-pRcDest->top};
        SkRect skrc=toSkRect(pRcDest);
        skrc.offset(m_ptOrg);
        m_SkCanvas->drawBitmapRect(bmpSrc,&isrc,skrc,&paint);
		return S_OK;
	}

	HRESULT SRenderTarget_Skia::DrawText( LPCTSTR pszText,int cchLen,LPRECT pRc,UINT uFormat)
	{
		if(cchLen<0) cchLen= _tcslen(pszText);
		if(cchLen==0)
        {
            if(uFormat & DT_CALCRECT)
            {
                pRc->right=pRc->left;
                pRc->bottom=pRc->top;
            }
            return S_OK;
        }
		
		SStringW strW=S_CT2W(SStringT(pszText,cchLen));
        SkPaint     txtPaint = m_curFont->GetPaint();
        txtPaint.setColor(m_curColor.toARGB());
        txtPaint.setTypeface(m_curFont->GetFont());
        if(uFormat & DT_CENTER)
            txtPaint.setTextAlign(SkPaint::kCenter_Align);
        else if(uFormat & DT_RIGHT)
            txtPaint.setTextAlign(SkPaint::kRight_Align);

        SkRect skrc=toSkRect(pRc);
        skrc.offset(m_ptOrg);
        skrc=DrawText_Skia(m_SkCanvas,strW,strW.GetLength(),skrc,txtPaint,uFormat);
        if(uFormat & DT_CALCRECT)
        {
            pRc->left=(int)skrc.fLeft;
            pRc->top=(int)skrc.fTop;
            pRc->right=(int)skrc.fRight;
            pRc->bottom=(int)skrc.fBottom;
        }
		return S_OK;
	}

	HRESULT SRenderTarget_Skia::MeasureText( LPCTSTR pszText,int cchLen, SIZE *psz )
	{
        SkPaint     txtPaint = m_curFont->GetPaint();
        txtPaint.setTypeface(m_curFont->GetFont());
        SStringW strW=S_CT2W(SStringT(pszText,cchLen));
        psz->cx = (int)txtPaint.measureText(strW,strW.GetLength()*sizeof(wchar_t));
        
        SkPaint::FontMetrics metrics;
        txtPaint.getFontMetrics(&metrics);
        psz->cy = (int)(metrics.fBottom-metrics.fTop);
		return S_OK;
	}

	HRESULT SRenderTarget_Skia::DrawRectangle(LPCRECT pRect)
	{
		SkPaint paint;
		paint.setColor(SColor(m_curPen->GetColor()).toARGB());
		SGetLineDashEffect skDash(m_curPen->GetStyle());
 		paint.setPathEffect(skDash.Get());
		if(m_bAntiAlias)
		{
			paint.setAntiAlias(true);
			paint.setStrokeWidth((SkScalar)m_curPen->GetWidth()-0.5f);
		}else
		{
			paint.setAntiAlias(false);
			paint.setStrokeWidth((SkScalar)m_curPen->GetWidth());
		}
		paint.setStyle(SkPaint::kStroke_Style);

        SkRect skrc=toSkRect(pRect);
        skrc.offset(m_ptOrg);
        InflateSkRect(&skrc,-0.5f,-0.5f);
		m_SkCanvas->drawRect(skrc,paint);
		return S_OK;
	}

	HRESULT SRenderTarget_Skia::FillRectangle(LPCRECT pRect)
	{
		SkPaint paint;
		
		if(m_curBrush->IsBitmap())
		{
			paint.setFilterBitmap(true);
			paint.setShader(SkShader::CreateBitmapShader(m_curBrush->GetBitmap(),SkShader::kRepeat_TileMode,SkShader::kRepeat_TileMode))->unref();
		}else
		{
			paint.setFilterBitmap(false);
			paint.setColor(SColor(m_curBrush->GetColor()).toARGB());
		}
		paint.setStyle(SkPaint::kFill_Style);

        SkRect skrc=toSkRect(pRect);
        skrc.offset(m_ptOrg);
        InflateSkRect(&skrc,-0.5f,-0.5f);
		m_SkCanvas->drawRect(skrc,paint);
		return S_OK;
	}

    HRESULT SRenderTarget_Skia::DrawRoundRect( LPCRECT pRect,POINT pt )
    {
        SkPaint paint;
        paint.setColor(SColor(m_curPen->GetColor()).toARGB());
        SGetLineDashEffect skDash(m_curPen->GetStyle());
        paint.setPathEffect(skDash.Get());
        paint.setStyle(SkPaint::kStroke_Style);
		if(m_bAntiAlias)
		{
			paint.setAntiAlias(true);
			paint.setStrokeWidth((SkScalar)m_curPen->GetWidth()-0.5f);
		}else
		{
			paint.setAntiAlias(false);
			paint.setStrokeWidth((SkScalar)m_curPen->GetWidth());
		}

        SkRect skrc=toSkRect(pRect);
        InflateSkRect(&skrc,-0.5f,-0.5f);//要缩小0.5显示效果才和GDI一致。
        skrc.offset(m_ptOrg);
        m_SkCanvas->drawRoundRect(skrc,(SkScalar)pt.x,(SkScalar)pt.y,paint);
        return S_OK;
    }

    HRESULT SRenderTarget_Skia::FillRoundRect( LPCRECT pRect,POINT pt )
    {
        SkPaint paint;
        paint.setAntiAlias(m_bAntiAlias);

        if(m_curBrush->IsBitmap())
        {
            paint.setFilterBitmap(true);
            paint.setShader(SkShader::CreateBitmapShader(m_curBrush->GetBitmap(),SkShader::kRepeat_TileMode,SkShader::kRepeat_TileMode))->unref();
        }else
        {
            paint.setFilterBitmap(false);
            paint.setColor(m_curBrush->GetColor());
        }
        paint.setStyle(SkPaint::kFill_Style);

        SkRect skrc=toSkRect(pRect);
        InflateSkRect(&skrc,-0.5f,-0.5f);//要缩小0.5显示效果才和GDI一致。
        skrc.offset(m_ptOrg);

        m_SkCanvas->drawRoundRect(skrc,(SkScalar)pt.x,(SkScalar)pt.y,paint);
        return S_OK;
    }
    
    HRESULT SRenderTarget_Skia::FillSolidRoundRect(LPCRECT pRect,POINT pt,COLORREF cr)
    {
        SkPaint paint;
        paint.setAntiAlias(m_bAntiAlias);

        paint.setFilterBitmap(false);
        paint.setColor(SColor(cr).toARGB());
        paint.setStyle(SkPaint::kFill_Style);

        SkRect skrc=toSkRect(pRect);
        InflateSkRect(&skrc,-0.5f,-0.5f);//要缩小0.5显示效果才和GDI一致。
        skrc.offset(m_ptOrg);

        m_SkCanvas->drawRoundRect(skrc,(SkScalar)pt.x,(SkScalar)pt.y,paint);
        return S_OK;
    }


    HRESULT SRenderTarget_Skia::DrawLines(LPPOINT pPt,size_t nCount)
    {
        SkPoint *pts=new SkPoint[nCount];
        for(size_t i=0; i<nCount; i++ )
        {
            pts[i].fX = (SkScalar)pPt[i].x;
            pts[i].fY = (SkScalar)pPt[i].y;
        }
        SkPoint::Offset(pts,nCount,m_ptOrg);

        SkPaint paint;
		if(m_bAntiAlias)
		{
			paint.setAntiAlias(true);
			paint.setStrokeWidth((SkScalar)m_curPen->GetWidth()-0.5f);
		}else
		{
			paint.setAntiAlias(false);
			paint.setStrokeWidth((SkScalar)m_curPen->GetWidth());
		}

        paint.setColor(SColor(m_curPen->GetColor()).toARGB());
        SGetLineDashEffect skDash(m_curPen->GetStyle());
        paint.setPathEffect(skDash.Get());
        paint.setStyle(SkPaint::kStroke_Style);
        m_SkCanvas->drawPoints(SkCanvas::kPolygon_PointMode,nCount,pts,paint);
        delete []pts;

        return S_OK;
    }

	HRESULT SRenderTarget_Skia::TextOut( int x, int y, LPCTSTR lpszString, int nCount)
	{
		if(nCount<0) nCount= _tcslen(lpszString);
		SStringW strW=S_CT2W(SStringT(lpszString,nCount));
        SkPaint     txtPaint = m_curFont->GetPaint();
        SkPaint::FontMetrics metrics;
        txtPaint.getFontMetrics(&metrics);
        SkScalar fx = m_ptOrg.fX + x;
        SkScalar fy = m_ptOrg.fY + y;
        fy -= metrics.fAscent;

        txtPaint.setColor(m_curColor.toARGB());
        txtPaint.setTypeface(m_curFont->GetFont());
		m_SkCanvas->drawText((LPCWSTR)strW,strW.GetLength()*2,fx,fy,txtPaint);
		return S_OK;
	}

    HRESULT SRenderTarget_Skia::DrawIconEx( int xLeft, int yTop, HICON hIcon, int cxWidth,int cyWidth,UINT diFlags )
    {
        HDC hdc=GetDC(0);
        
        ICONINFO ii={0};
        ::GetIconInfo(hIcon,&ii);
        SASSERT(ii.hbmColor);
        BITMAP bm;
        ::GetObject(ii.hbmColor,sizeof(bm),&bm);

        ALPHAINFO ai;
        RECT rc={xLeft,yTop,xLeft+cxWidth,yTop+cyWidth};
        if(bm.bmBitsPixel!=32)
        {
            CGdiAlpha::AlphaBackup(hdc,&rc,ai);
        }
        BOOL bRet=::DrawIconEx(hdc,xLeft,yTop,hIcon,cxWidth,cyWidth,0,NULL,diFlags);

        if(bm.bmBitsPixel!=32)
        {
            CGdiAlpha::AlphaRestore(ai);
        }
        if(ii.hbmColor) DeleteObject(ii.hbmColor);
        if(ii.hbmMask) DeleteObject(ii.hbmMask);

        ReleaseDC(hdc);
        return bRet?S_OK:S_FALSE;
    }

    HRESULT SRenderTarget_Skia::DrawBitmap(LPCRECT pRcDest,IBitmap *pBitmap,int xSrc,int ySrc,BYTE byAlpha/*=0xFF*/ )
    {
        SBitmap_Skia *pBmp = (SBitmap_Skia*)pBitmap;
        SkBitmap bmp=pBmp->GetSkBitmap();

        SIZE szBmp = pBmp->Size();
        int nWid= (std::min)(pRcDest->right-pRcDest->left,szBmp.cx);
        int nHei= (std::min)(pRcDest->bottom-pRcDest->top,szBmp.cy);

        RECT rcSrc={xSrc,ySrc,xSrc+nWid,ySrc+nHei};
        RECT rcDst={pRcDest->left,pRcDest->top,pRcDest->left+nWid,pRcDest->top+nHei};
        
        SkRect skrcSrc= toSkRect(&rcSrc);
        SkRect skrcDst = toSkRect(&rcDst);
        
        skrcDst.offset(m_ptOrg);

        SkPaint paint;
        paint.setAntiAlias(m_bAntiAlias);
        
        if(byAlpha != 0xFF) paint.setAlpha(byAlpha);
        m_SkCanvas->drawBitmapRectToRect(bmp,&skrcSrc,skrcDst,&paint);
        return S_OK;
    }
    
    HRESULT SRenderTarget_Skia::AlphaBlend( LPCRECT pRcDest,IRenderTarget *pRTSrc,LPCRECT pRcSrc,BYTE byAlpha )
    {
        IBitmap *pBmp=(IBitmap*) pRTSrc->GetCurrentObject(OT_BITMAP);
        if(!pBmp) return S_FALSE;
        RECT rcSrc = *pRcSrc;
        POINT ptSrcOrg;
        pRTSrc->GetViewportOrg(&ptSrcOrg);
        OffsetRect(&rcSrc,ptSrcOrg.x,ptSrcOrg.y);
        return DrawBitmapEx(pRcDest,pBmp,&rcSrc,EM_STRETCH,byAlpha);
    }

    HRESULT SRenderTarget_Skia::DrawBitmapEx( LPCRECT pRcDest,IBitmap *pBitmap,LPCRECT pRcSrc,UINT expendMode, BYTE byAlpha/*=0xFF*/ )
    {
        UINT expendModeLow = LOWORD(expendMode);

        if(expendModeLow == EM_NULL || (RectWid(pRcDest)==RectWid(pRcSrc) && RectHei(pRcDest)==RectHei(pRcSrc)))
            return DrawBitmap(pRcDest,pBitmap,pRcSrc->left,pRcSrc->top,byAlpha);
            
        SBitmap_Skia *pBmp = (SBitmap_Skia*)pBitmap;
        SkBitmap bmp=pBmp->GetSkBitmap();

        RECT rcSour={0,0,bmp.width(),bmp.height()};
        if(!pRcSrc) pRcSrc = &rcSour;
        SkRect rcSrc = toSkRect(pRcSrc);
        SkRect rcDest= toSkRect(pRcDest);
        rcDest.offset(m_ptOrg);

        SkPaint paint;
        paint.setAntiAlias(true);
        if(byAlpha != 0xFF) paint.setAlpha(byAlpha);
        
        SkPaint::FilterLevel fl = (SkPaint::FilterLevel)HIWORD(expendMode);//SkPaint::kNone_FilterLevel;
        paint.setFilterLevel(fl);
                
        if(expendModeLow == EM_STRETCH)
        {
            m_SkCanvas->drawBitmapRectToRect(bmp,&rcSrc,rcDest,&paint);
        }else
        {
            PushClipRect(pRcDest,RGN_AND);
            
            SkIRect rcSrc = toSkIRect(pRcSrc);
            SkRect rcSubDest={0.0f,0.0f,(float)rcSrc.width(),(float)rcSrc.height()};
            for(float y=rcDest.fTop;y<rcDest.fBottom;y+=rcSrc.height())
            {
                rcSubDest.offsetTo(rcDest.fLeft,y);               
                for(float x=rcDest.fLeft;x<rcDest.fRight;x += rcSrc.width())
                {
                    m_SkCanvas->drawBitmapRect(bmp,&rcSrc,rcSubDest,&paint);
                    rcSubDest.offset((float)rcSrc.width(),0.0f);
                }
            }
            
            PopClip();
        }
        return S_OK;

    }


    HRESULT SRenderTarget_Skia::DrawBitmap9Patch( LPCRECT pRcDest,IBitmap *pBitmap,LPCRECT pRcSrc,LPCRECT pRcSourMargin,UINT expendMode,BYTE byAlpha/*=0xFF*/ )
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

	IRenderObj * SRenderTarget_Skia::GetCurrentObject( OBJTYPE uType )
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


    HRESULT SRenderTarget_Skia::SelectDefaultObject(OBJTYPE objType,IRenderObj ** ppOldObj /*= NULL*/)
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

    HRESULT SRenderTarget_Skia::SelectObject( IRenderObj *pObj,IRenderObj ** ppOldObj /*= NULL*/ )
    {
        CAutoRefPtr<IRenderObj> pRet;
        switch(pObj->ObjectType())
        {
        case OT_BITMAP: 
            pRet=m_curBmp;
            m_curBmp=(SBitmap_Skia*)pObj;
            //重新生成clip
            SASSERT(m_SkCanvas);
            delete m_SkCanvas;
            m_SkCanvas = new SkCanvas(m_curBmp->GetSkBitmap());
            break;
        case OT_PEN:
            pRet=m_curPen;
            m_curPen=(SPen_Skia*)pObj;
            break;
        case OT_BRUSH:
            pRet=m_curBrush;
            m_curBrush=(SBrush_Skia*)pObj;
            break;
        case OT_FONT:
            pRet=m_curFont;
            m_curFont=(SFont_Skia*)pObj;
            break;
        }
        if(pRet && ppOldObj)
        {//由调用者调用Release释放该RenderObj
            pRet->AddRef();
            *ppOldObj = pRet;
        }
        return S_OK;
    }

    HRESULT SRenderTarget_Skia::OffsetViewportOrg( int xOff, int yOff, LPPOINT lpPoint )
    {
        if(lpPoint)
        {
            lpPoint->x = (LONG)m_ptOrg.fX;
            lpPoint->y = (LONG)m_ptOrg.fY;
        }
        m_ptOrg.offset((SkScalar)xOff,(SkScalar)yOff);
        return S_OK;
    }

    HRESULT SRenderTarget_Skia::GetViewportOrg( LPPOINT lpPoint )
    {
        if(lpPoint)
        {
            lpPoint->x = (LONG)m_ptOrg.fX;
            lpPoint->y = (LONG)m_ptOrg.fY;
        }
        return S_OK;
    }
    
    HRESULT SRenderTarget_Skia::SetViewportOrg( POINT pt )
    {
        m_ptOrg.fX = SkIntToScalar(pt.x);
        m_ptOrg.fY = SkIntToScalar(pt.y);
        return S_OK;
    }


    HDC SRenderTarget_Skia::GetDC( UINT uFlag )
    {
        if(m_hGetDC) return m_hGetDC;
        
        HBITMAP bmp=m_curBmp->GetGdiBitmap();//bmp可能为NULL
        HDC hdc_desk = ::GetDC(NULL);
        m_hGetDC = CreateCompatibleDC(hdc_desk);
        ::ReleaseDC(NULL,hdc_desk);
        
        ::SelectObject(m_hGetDC,bmp);
        
        if(m_SkCanvas->isClipEmpty())
        {
            ::IntersectClipRect(m_hGetDC,0,0,0,0);
        }else if(m_SkCanvas->isClipRect())
        {
            SkRect rcClip;
            m_SkCanvas->getClipBounds(&rcClip);
            RECT rc = {(int)rcClip.left(),(int)rcClip.top(),(int)rcClip.right(),(int)rcClip.bottom()};
            ::InflateRect(&rc,-1,-1);//注意需要向内缩小一个象素
            ::IntersectClipRect(m_hGetDC,rc.left,rc.top,rc.right,rc.bottom);
        }else
        {
            SkRegion rgn = m_SkCanvas->getTotalClip();
            SkRegion::Iterator it(rgn);
            int nCount=0;
            for(;!it.done();it.next())
            {
                nCount++;
            }
            it.rewind();

            int nSize=sizeof(RGNDATAHEADER)+nCount*sizeof(RECT);
            RGNDATA *rgnData=(RGNDATA*)malloc(nSize);
            memset(rgnData,0,nSize);
            rgnData->rdh.dwSize= sizeof(RGNDATAHEADER);
            rgnData->rdh.iType = RDH_RECTANGLES;
            rgnData->rdh.nCount=nCount;
            rgnData->rdh.rcBound.right=m_curBmp->Width();
            rgnData->rdh.rcBound.bottom=m_curBmp->Height();

            nCount=0;
            LPRECT pRc=(LPRECT)rgnData->Buffer;
            for(;!it.done();it.next())
            {
                SkIRect skrc=it.rect();
                RECT rc = {skrc.fLeft,skrc.fTop,skrc.fRight,skrc.fBottom};
                pRc[nCount++]= rc;
            }

            HRGN hRgn=ExtCreateRegion(NULL,nSize,rgnData);
            free(rgnData);
            ::SelectClipRgn(m_hGetDC,hRgn);
            DeleteObject(hRgn);
        }

        ::SetViewportOrgEx(m_hGetDC,(int)m_ptOrg.fX,(int)m_ptOrg.fY,NULL);

        m_uGetDCFlag = uFlag;
        return m_hGetDC;
    }

    void SRenderTarget_Skia::ReleaseDC( HDC hdc )
    {
        if(hdc == m_hGetDC)
        {
            DeleteDC(hdc);
            m_hGetDC = 0;
            m_uGetDCFlag =0;
        }
    }
    
    HRESULT SRenderTarget_Skia::GradientFillEx( LPCRECT pRect,const POINT* pts,COLORREF *colors,float *pos,int nCount,BYTE byAlpha/*=0xFF*/ )
    {
        SkRect skrc = toSkRect(pRect);
        skrc.offset(m_ptOrg);
        SkPoint *skPts = new SkPoint[nCount];
        SkColor *skColors= new SkColor[nCount];
        for(int i=0;i<nCount;i++)
        {
            skPts[i].iset(pts[i].x,pts[i].y);
            skPts[i].offset(m_ptOrg.x(),m_ptOrg.y());
            skColors[i] = SColor(colors[i],byAlpha).toARGB();
        }
        
        SkShader *pShader = SkGradientShader::CreateLinear(skPts, skColors, pos,nCount,SkShader::kMirror_TileMode);
        SkPaint paint;
        paint.setShader(pShader);
        pShader->unref();

        m_SkCanvas->drawRect(skrc,paint);

        delete []skColors;
        delete []skPts;
        return S_OK;
    }

	static bool fequal(float a,float b)
	{
		return fabs(a-b)< 0.0000001f;
	}

	HRESULT SRenderTarget_Skia::GradientFill2(LPCRECT pRect,GradientType type,COLORREF crStart,COLORREF crCenter,COLORREF crEnd,float fLinearAngle,float fCenterX,float fCenterY,int nRadius,BYTE byAlpha/*=0xff*/)
	{
		SkRect skrc = toSkRect(pRect);
		skrc.offset(m_ptOrg);

		SkColor *skColors= new SkColor[3];

		skColors[0]=SColor(crStart,byAlpha).toARGB();
		skColors[1]=SColor(crCenter,byAlpha).toARGB();
		skColors[2]=SColor(crEnd,byAlpha).toARGB();

		SkShader *pShader = NULL;
		SkScalar wid = skrc.width();
		SkScalar hei = skrc.height();
		SkScalar halfWid = wid/2;
		SkScalar halfHei = hei/2;
		if(type == linear)
		{

			SkPoint *skPts = new SkPoint[3];
			skPts[1].set(halfWid,halfHei);

			if(fequal(fLinearAngle,90.0f) || fequal(fLinearAngle,270.0f))
			{//90度
				skPts[0].set(halfWid,0.0f);
				skPts[2].set(halfWid,hei);
			}else if(fequal(fLinearAngle,0.0f) || fequal(fLinearAngle,180.0f))
			{//水平方向
				skPts[0].set(0.f ,halfHei);
				skPts[2].set(wid,halfHei);
			}else
			{//其它角度

				float angleInRadians = PI*fLinearAngle/180;
				double tanAngle = tan(angleInRadians);

				SkPoint pt1a,pt2a;//与左右两条边相交的位置
				SkPoint pt1b,pt2b;//与上下两条边相关的位置

				pt1a.fX=-halfWid,pt2a.fX=halfWid;
				pt1b.fY=-halfHei,pt2b.fY=halfHei;

				pt1a.fY= (SkScalar)(-halfWid*tanAngle);
				pt2a.fY = -pt1a.fY;

				pt1b.fX = (SkScalar)(-halfHei/tanAngle);
				pt2b.fX = -pt1b.fX;

				if(pt2a.fY > halfHei)
				{//using pt1a,pt2a
					skPts[0] = pt1a;
					skPts[2] = pt2a;
				}else
				{//using pt1b,pt2b
					skPts[0]=pt1b;
					skPts[2]=pt2b;
				}
			}
			pShader = SkGradientShader::CreateLinear(skPts, skColors, NULL,3,SkShader::kRepeat_TileMode);
			delete []skPts;
		}else if(type == radial)
		{
			SkPoint skCenter;
			skCenter.set(halfWid,halfHei);
			pShader = SkGradientShader::CreateRadial(skCenter,SkScalar(nRadius),skColors,NULL,3,SkShader::kRepeat_TileMode);
		}else if(type==sweep)
		{
			SkPoint skCenter;
			skCenter.set(halfWid,halfHei);
			pShader = SkGradientShader::CreateSweep(SkScalar(+fCenterX*wid),SkScalar(fCenterY*hei),skColors,NULL,3);
		}

		delete []skColors;

		if(!pShader)
		{
			return E_INVALIDARG;
		}

		SkPaint paint;
		paint.setShader(pShader);
		pShader->unref();

		SkPoint skOffset = {skrc.left(),skrc.top()};

 		m_SkCanvas->translate(skOffset.x(),skOffset.y());
 		skrc.offset(-skOffset.x(),-skOffset.y());
		m_SkCanvas->drawRect(skrc,paint);
		m_SkCanvas->translate(-skOffset.x(),-skOffset.y());
		return S_OK;
	}

    
    HRESULT SRenderTarget_Skia::GradientFill( LPCRECT pRect,BOOL bVert,COLORREF crBegin,COLORREF crEnd,BYTE byAlpha/*=0xFF*/ )
    {
        SkRect skrc = toSkRect(pRect);
        skrc.offset(m_ptOrg);

        SkPoint pts[2];
        pts[0].set(skrc.left(),skrc.top());

        if (bVert)
        {
            pts[1].set(skrc.left(),skrc.bottom());
        }
        else
        {
            pts[1].set(skrc.right(),skrc.top());
        }

        SColor cr1(crBegin,byAlpha);
        SColor cr2(crEnd,byAlpha);

        const SkColor colors[2] = {cr1.toARGB(),cr2.toARGB()};
        SkShader *pShader = SkGradientShader::CreateLinear(pts, colors, NULL,2,SkShader::kMirror_TileMode);
        SkPaint paint;
        paint.setShader(pShader);
        pShader->unref();

        m_SkCanvas->drawRect(skrc,paint);
        return S_OK;

    }

    HRESULT SRenderTarget_Skia::FillSolidRect( LPCRECT pRect,COLORREF cr )
    {
        SkPaint paint;
        paint.setStyle(SkPaint::kFill_Style);
        paint.setColor(SColor(cr).toARGB());
        paint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
        
        SkRect skrc=toSkRect(pRect);
        skrc.offset(m_ptOrg);
        m_SkCanvas->drawRect(skrc,paint);
        return S_OK;    
    }

    HRESULT SRenderTarget_Skia::ClearRect( LPCRECT pRect,COLORREF cr )
    {
        SkPaint paint;
        paint.setStyle(SkPaint::kFill_Style);
        paint.setColor(SColor(cr).toARGB());
        paint.setXfermodeMode(SkXfermode::kSrc_Mode);

        SkRect skrc=toSkRect(pRect);
        skrc.offset(m_ptOrg);
        m_SkCanvas->drawRect(skrc,paint);
        return S_OK;    
    }

    HRESULT SRenderTarget_Skia::InvertRect(LPCRECT pRect)
    {
        SkPaint paint;
        paint.setStyle(SkPaint::kFill_Style);
        paint.setXfermode(new ProcXfermode(ProcXfermode::Rop2_Invert));
        SkRect skrc = toSkRect(pRect);
        skrc.offset(m_ptOrg);
        m_SkCanvas->drawRect(skrc,paint);
        return S_OK;  
    }

    HRESULT SRenderTarget_Skia::DrawEllipse( LPCRECT pRect )
    {
        SkPaint paint;
        paint.setColor(SColor(m_curPen->GetColor()).toARGB());
        SGetLineDashEffect skDash(m_curPen->GetStyle());
        paint.setPathEffect(skDash.Get());
        paint.setStyle(SkPaint::kStroke_Style);
		if(m_bAntiAlias)
		{
			paint.setAntiAlias(true);
			paint.setStrokeWidth((SkScalar)m_curPen->GetWidth()-0.5f);
		}else
		{
			paint.setAntiAlias(false);
			paint.setStrokeWidth((SkScalar)m_curPen->GetWidth());
		}

        SkRect skrc = toSkRect(pRect);
        skrc.offset(m_ptOrg);
        m_SkCanvas->drawOval(skrc,paint);
        return S_OK;
    }

    HRESULT SRenderTarget_Skia::FillEllipse( LPCRECT pRect )
    {
        SkPaint paint;
        if(m_curBrush->IsBitmap())
        {
            paint.setFilterBitmap(true);
            paint.setShader(SkShader::CreateBitmapShader(m_curBrush->GetBitmap(),SkShader::kRepeat_TileMode,SkShader::kRepeat_TileMode))->unref();
        }else
        {
            paint.setFilterBitmap(false);
            paint.setColor(SColor(m_curBrush->GetColor()).toARGB());
        }
        paint.setStyle(SkPaint::kFill_Style);
        paint.setAntiAlias(m_bAntiAlias);

        SkRect skrc=toSkRect(pRect);
        skrc.offset(m_ptOrg);
        m_SkCanvas->drawOval(skrc,paint);
        return S_OK;
    }

    HRESULT SRenderTarget_Skia::FillSolidEllipse(LPCRECT pRect,COLORREF cr)
    {
        SkPaint paint;
        paint.setFilterBitmap(false);
        paint.setColor(SColor(cr).toARGB());
        paint.setStyle(SkPaint::kFill_Style);

        SkRect skrc=toSkRect(pRect);
        skrc.offset(m_ptOrg);
        m_SkCanvas->drawOval(skrc,paint);
        return S_OK;
    }

    HRESULT SRenderTarget_Skia::DrawArc( LPCRECT pRect,float startAngle,float sweepAngle,bool useCenter )
    {
        SkPaint paint;
        paint.setColor(SColor(m_curPen->GetColor()).toARGB());
        SGetLineDashEffect skDash(m_curPen->GetStyle());
        paint.setPathEffect(skDash.Get());
        paint.setStyle(SkPaint::kStroke_Style);
		if(m_bAntiAlias)
		{
			paint.setAntiAlias(true);
			paint.setStrokeWidth((SkScalar)m_curPen->GetWidth()-0.5f);
		}else
		{
			paint.setAntiAlias(false);
			paint.setStrokeWidth((SkScalar)m_curPen->GetWidth());
		}

        SkRect skrc = toSkRect(pRect);
        skrc.offset(m_ptOrg);
        m_SkCanvas->drawArc(skrc,startAngle,sweepAngle,useCenter,paint);
        return S_OK;
    }

    HRESULT SRenderTarget_Skia::FillArc( LPCRECT pRect,float startAngle,float sweepAngle )
    {
        SkPaint paint;
        if(m_curBrush->IsBitmap())
        {
            paint.setFilterBitmap(true);
            paint.setShader(SkShader::CreateBitmapShader(m_curBrush->GetBitmap(),SkShader::kRepeat_TileMode,SkShader::kRepeat_TileMode))->unref();
        }else
        {
            paint.setFilterBitmap(false);
            paint.setColor(SColor(m_curBrush->GetColor()).toARGB());
        }
        paint.setStyle(SkPaint::kFill_Style);

        SkRect skrc=toSkRect(pRect);
        skrc.offset(m_ptOrg);
        m_SkCanvas->drawArc(skrc,startAngle, sweepAngle,true,paint);
        return S_OK;

    }

    HRESULT SRenderTarget_Skia::QueryInterface( REFGUID iid,IObjRef ** ppObj )
    {
        if(iid == __uuidof(IRenderTarget_Skia2))
        {
            *ppObj = new RenderTarget_Skia2;
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    HRESULT SRenderTarget_Skia::SetTransform(const IxForm * pXForm,IxForm *pOldXFrom)
    {
        SASSERT(pXForm);
        if(pOldXFrom) GetTransform(pOldXFrom);
        SkMatrix m;
        m.setAll(pXForm->eM11,pXForm->eM21,pXForm->eDx,pXForm->eM12,pXForm->eM22,pXForm->eDy,0.0f,0.0f,1.0f);
        m_SkCanvas->setMatrix(m);
        return S_OK;
    }

    HRESULT SRenderTarget_Skia::GetTransform(IxForm * pXForm) const
    {
        SASSERT(pXForm);
        const SkMatrix m = m_SkCanvas->getTotalMatrix();
        pXForm->eM11 = m.getScaleX();
        pXForm->eM21 = m.getSkewX();
        pXForm->eDx  = m.getTranslateX();
        
        pXForm->eM12 = m.getSkewY();
        pXForm->eM22 = m.getScaleY();
        pXForm->eDy  = m.getTranslateY();
        return S_OK;
    }

	COLORREF SRenderTarget_Skia::GetPixel( int x, int y )
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

	COLORREF SRenderTarget_Skia::SetPixel( int x, int y, COLORREF cr )
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

	HRESULT SRenderTarget_Skia::ClipPath(const IPath * path, UINT mode, bool doAntiAlias /*= false*/)
	{
		const SPath_Skia * path2 = (const SPath_Skia *)path;
		m_SkCanvas->clipPath(path2->m_skPath,SRegion_Skia::RGNMODE2SkRgnOP(mode),doAntiAlias);
		return S_OK;
	}

	HRESULT SRenderTarget_Skia::DrawPath(const IPath * path, IPathEffect * pathEffect)
	{
		const SPath_Skia * path2 = (const SPath_Skia *)path;

		SkPaint paint;
		paint.setColor(SColor(m_curPen->GetColor()).toARGB());
		SGetLineDashEffect skDash(m_curPen->GetStyle());
		paint.setPathEffect(skDash.Get());
		paint.setStyle(SkPaint::kStroke_Style);
		if(m_bAntiAlias)
		{
			paint.setAntiAlias(true);
			paint.setStrokeWidth((SkScalar)m_curPen->GetWidth()-0.5f);
		}else
		{
			paint.setAntiAlias(false);
			paint.setStrokeWidth((SkScalar)m_curPen->GetWidth());
		}

		if(pathEffect!=NULL)
		{
			SkPathEffect * skPathEffect = (SkPathEffect*)pathEffect->GetRealPathEffect();
			paint.setPathEffect(skPathEffect);
		}
		SkPath skPath;
		path2->m_skPath.offset(m_ptOrg.fX,m_ptOrg.fY,&skPath);
		m_SkCanvas->drawPath(skPath,paint);

		return S_OK;
	}


    //////////////////////////////////////////////////////////////////////////
	// SBitmap_Skia
    static int s_cBmp = 0;
    SBitmap_Skia::SBitmap_Skia( IRenderFactory *pRenderFac ) :TSkiaRenderObjImpl<IBitmap>(pRenderFac),m_hBmp(0)
    {
//         STRACE(L"bitmap new; objects = %d",++s_cBmp);
    }

    SBitmap_Skia::~SBitmap_Skia()
    {
        m_bitmap.reset();
        if(m_hBmp) DeleteObject(m_hBmp);
//         STRACE(L"bitmap delete objects = %d",--s_cBmp);
    }

    HBITMAP SBitmap_Skia::CreateGDIBitmap( int nWid,int nHei,void ** ppBits )
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
        //LPVOID pBits=NULL;
        HBITMAP hBmp=CreateDIBSection(hdc,&bmi,DIB_RGB_COLORS,ppBits,0,0);
        ReleaseDC(NULL,hdc);
        return hBmp;
    }

	HRESULT SBitmap_Skia::Init( int nWid,int nHei ,const LPVOID pBits/*=NULL*/)
	{
		m_bitmap.reset();
		m_bitmap.setInfo(SkImageInfo::Make(nWid,nHei,kN32_SkColorType,kPremul_SkAlphaType));
        if(m_hBmp) DeleteObject(m_hBmp);
    		
		LPVOID pBmpBits=NULL;
		m_hBmp=CreateGDIBitmap(nWid,nHei,&pBmpBits);
		if(!m_hBmp) return E_OUTOFMEMORY;
        if(pBits)
        {
            memcpy(pBmpBits,pBits,nWid*nHei*4);
        }else
        {
            memset(pBmpBits,0,nWid*nHei*4);
        }
		m_bitmap.setPixels(pBmpBits);
		return S_OK;
	}

    HRESULT SBitmap_Skia::Init( IImgFrame *pFrame )
    {
        UINT uWid=0,uHei =0;
        pFrame->GetSize(&uWid,&uHei);

        if(m_hBmp) DeleteObject(m_hBmp);
        m_bitmap.reset();
        m_bitmap.setInfo(SkImageInfo::Make(uWid, uHei,kN32_SkColorType,kPremul_SkAlphaType));
        void * pBits=NULL;
        m_hBmp = CreateGDIBitmap(uWid,uHei,&pBits);

        if(!m_hBmp) return E_OUTOFMEMORY;
        m_bitmap.setPixels(pBits);

        const int stride = m_bitmap.rowBytes();
        pFrame->CopyPixels(NULL, stride, stride * uHei,
            reinterpret_cast<BYTE*>(m_bitmap.getPixels()));
        return S_OK;
    }

	HRESULT SBitmap_Skia::LoadFromFile( LPCTSTR pszFileName)
	{
	    CAutoRefPtr<IImgX> imgDecoder;
	    GetRenderFactory()->GetImgDecoderFactory()->CreateImgX(&imgDecoder);
		if(imgDecoder->LoadFromFile(S_CT2W(pszFileName))==0) return S_FALSE;
		return ImgFromDecoder(imgDecoder);
	}

	HRESULT SBitmap_Skia::LoadFromMemory(LPBYTE pBuf,size_t szLen)
	{
        CAutoRefPtr<IImgX> imgDecoder;
        GetRenderFactory()->GetImgDecoderFactory()->CreateImgX(&imgDecoder);
		if(imgDecoder->LoadFromMemory(pBuf,szLen)==0) return S_FALSE;
        return ImgFromDecoder(imgDecoder);
	}

    HRESULT SBitmap_Skia::ImgFromDecoder(IImgX *imgDecoder)
    {
        IImgFrame *pFrame=imgDecoder->GetFrame(0);
        UINT uWid=0,uHei =0;
        pFrame->GetSize(&uWid,&uHei);

        if(m_hBmp) DeleteObject(m_hBmp);
        m_bitmap.reset();
        m_bitmap.setInfo(SkImageInfo::Make(uWid, uHei,kN32_SkColorType,kPremul_SkAlphaType));
        void * pBits=NULL;
        m_hBmp = CreateGDIBitmap(uWid,uHei,&pBits);
        
        if(!m_hBmp) return E_OUTOFMEMORY;
        m_bitmap.setPixels(pBits);
        
        const int stride = m_bitmap.rowBytes();
        pFrame->CopyPixels(NULL, stride, stride * uHei,
            reinterpret_cast<BYTE*>(m_bitmap.getPixels()));
        return S_OK;
    }

    UINT SBitmap_Skia::Width()  const
    {
        return m_bitmap.width();
    }

    UINT SBitmap_Skia::Height() const
    {
        return m_bitmap.height();
    }

    SIZE SBitmap_Skia::Size() const
    {
        SIZE sz={m_bitmap.width(),m_bitmap.height()};
        return sz;
    }

    LPVOID SBitmap_Skia::LockPixelBits()
    {
        return m_bitmap.getPixels();
    }

    void SBitmap_Skia::UnlockPixelBits( LPVOID pBuf)
    {
		BITMAP bm;
		GetObject(m_hBmp,sizeof(bm),&bm);
		memcpy(bm.bmBits,pBuf,Width()*Height()*4);
		m_bitmap.setPixels(pBuf);
    }

    const LPVOID SBitmap_Skia::GetPixelBits() const
    {
        return m_bitmap.getPixels();
    }

    //////////////////////////////////////////////////////////////////////////
    static int s_cRgn =0;
	SRegion_Skia::SRegion_Skia( IRenderFactory *pRenderFac )
        :TSkiaRenderObjImpl<IRegion>(pRenderFac)
	{
//         STRACE(L"region new; objects = %d",++s_cRgn);
	}

    SRegion_Skia::~SRegion_Skia()
    {
//         STRACE(L"region delete; objects = %d",--s_cRgn);
    }

	void SRegion_Skia::CombineRect( LPCRECT lprect,int nCombineMode )
	{
        m_rgn.op(toSkIRect(lprect),RGNMODE2SkRgnOP(nCombineMode));
	}

	void SRegion_Skia::CombineRoundRect(LPCRECT lprect, POINT ptRadius, int nCombineMode)
	{
		SkPath skPath;
		skPath.addRoundRect(toSkRect(lprect),SkScalar(ptRadius.x),SkScalar(ptRadius.y));
		SkRegion clip;
		clip.setRect(lprect->left,lprect->top,lprect->right,lprect->bottom);
		SkRegion rgn;
		rgn.setPath(skPath,clip);

		m_rgn.op(rgn,RGNMODE2SkRgnOP(nCombineMode));
	}

	void SRegion_Skia::CombineEllipse(LPCRECT lprect , int nCombineMode)
	{
		SkPath skPath;
		skPath.addOval(toSkRect(lprect));
		SkRegion clip;
		clip.setRect(lprect->left,lprect->top,lprect->right,lprect->bottom);
		SkRegion rgn;
		rgn.setPath(skPath,clip);

		m_rgn.op(rgn,RGNMODE2SkRgnOP(nCombineMode));
	}

    void SRegion_Skia::CombineRgn(const IRegion * pRgnSrc,int nCombineMode)
    {
        const SRegion_Skia * pRgnSrc2 = (const SRegion_Skia*)pRgnSrc;
        m_rgn.op(pRgnSrc2->GetRegion(),RGNMODE2SkRgnOP(nCombineMode));
    }

    void SRegion_Skia::SetRgn(const HRGN hRgn)
    {
        DWORD dwSize = GetRegionData(hRgn,0,NULL);
        RGNDATA *pData = (RGNDATA*)malloc(dwSize);
        GetRegionData(hRgn,dwSize,pData);
        SkIRect *pRcs= new SkIRect[pData->rdh.nCount];
        LPRECT pRcsSrc = (LPRECT)pData->Buffer;
        for(unsigned int i = 0 ;i< pData->rdh.nCount;i++)
        {
            pRcs[i].fLeft = pRcsSrc[i].left;
            pRcs[i].fTop = pRcsSrc[i].top;
            pRcs[i].fRight = pRcsSrc[i].right;
            pRcs[i].fBottom = pRcsSrc[i].bottom;
        }
        m_rgn.setRects(pRcs,pData->rdh.nCount);
        free(pData);
        delete []pRcs;
    }
    
	BOOL SRegion_Skia::PtInRegion( POINT pt )
	{
        return m_rgn.contains(pt.x,pt.y);
	}

	BOOL SRegion_Skia::RectInRegion( LPCRECT lprect )
	{
        SASSERT(lprect);
        return m_rgn.intersects(toSkIRect(lprect));
	}

	void SRegion_Skia::GetRgnBox( LPRECT lprect )
	{
        SASSERT(lprect);
        SkIRect rc=m_rgn.getBounds();
        lprect->left=rc.left();
        lprect->top=rc.top();
        lprect->right=rc.right();
        lprect->bottom=rc.bottom();
	}

	BOOL SRegion_Skia::IsEmpty()
	{
        return m_rgn.isEmpty();
	}

    void SRegion_Skia::Offset( POINT pt )
    {
        m_rgn.translate(pt.x,pt.y);
    }

    SkRegion SRegion_Skia::GetRegion() const
    {
        return m_rgn;
    }

    void SRegion_Skia::SetRegion( const SkRegion & rgn )
    {
        m_rgn=rgn;
    }

    SkRegion::Op SRegion_Skia::RGNMODE2SkRgnOP( UINT mode )
    {
		SkRegion::Op op = SkRegion::kReplace_Op;
        switch(mode)
        {
        case RGN_COPY: op = SkRegion::kReplace_Op;break;
        case RGN_AND: op = SkRegion::kIntersect_Op;break;
        case RGN_OR: op = SkRegion::kUnion_Op;break;
        case RGN_DIFF: op = SkRegion::kDifference_Op;break;
        case RGN_XOR: op = SkRegion::kXOR_Op;break;
        default:SASSERT(FALSE);break;
        }
        return op;
    }

    void SRegion_Skia::Clear()
    {
        m_rgn.setEmpty();
    }


    //////////////////////////////////////////////////////////////////////////
    // SFont_Skia

    SStringT FindPropValue(const SStringT & strProp, const SStringT & strKey)
    {
        SStringT strValue;
        int nPos1 = strProp.Find(strKey);
        if(nPos1!=-1)
        {
            nPos1+=strKey.GetLength();
            if(nPos1<strProp.GetLength() && strProp[nPos1]==_T(':'))
            {
                nPos1++;
                int nPos2=strProp.Find(_T(','),nPos1);
                if(nPos2==-1) nPos2=strProp.GetLength();
                strValue=strProp.Mid(nPos1,nPos2-nPos1);
            }
        }
        return strValue;
    }

    static int s_cFont =0;
    SFont_Skia::SFont_Skia( IRenderFactory * pRenderFac,const LOGFONT * plf) 
        :TSkiaRenderObjImpl<IFont>(pRenderFac)
        ,m_skFont(NULL)
		,m_blurStyle((SkBlurStyle)-1)
		,m_blurRadius(0.0f)
    {
        memcpy(&m_lf,plf,sizeof(LOGFONT));
#ifdef UNICODE
        SStringA strFace=S_CT2A(plf->lfFaceName,CP_UTF8);
#else
		SStringA strFace=S_CT2A(plf->lfFaceName,CP_ACP);
#endif
		BYTE style=SkTypeface::kNormal;
        if(plf->lfItalic) style |= SkTypeface::kItalic;
        if(plf->lfWeight == FW_BOLD) style |= SkTypeface::kBold;

        m_skFont=SkTypeface::CreateFromName(strFace,(SkTypeface::Style)style);

        m_skPaint.setTextSize(SkIntToScalar(abs(plf->lfHeight)));
        m_skPaint.setUnderlineText(!!plf->lfUnderline);
        m_skPaint.setStrikeThruText(!!plf->lfStrikeOut);

        m_skPaint.setTextEncoding(SkPaint::kUTF16_TextEncoding);
        m_skPaint.setAntiAlias(true);
		m_skPaint.setLCDRenderText(true);
		m_skPaint.setStyle(SkPaint::kStrokeAndFill_Style);
//         STRACE(L"font new: objects = %d", ++s_cFont);
    }

    SFont_Skia::~SFont_Skia()
    {
        if(m_skFont) m_skFont->unref();
//         STRACE(L"font delete: objects = %d", --s_cFont);
    }

	HRESULT SFont_Skia::DefAttributeProc(const SStringW & strAttribName,const SStringW & strValue, BOOL bLoading)
	{
		(bLoading);
		if(strAttribName.CompareNoCase(L"antiAlias")==0)
		{
			m_skPaint.setAntiAlias(String2Bool(strValue));
		}else if(strAttribName.CompareNoCase(L"style")==0)
		{
			if(strValue.CompareNoCase(L"strokeAndFill")==0)
				m_skPaint.setStyle(SkPaint::kStrokeAndFill_Style);
			else if(strValue.CompareNoCase(L"fill")!=0)
				m_skPaint.setStyle(SkPaint::kFill_Style);
			else if(strValue.CompareNoCase(L"stroke")==0)
				m_skPaint.setStyle(SkPaint::kStroke_Style);
		}else if(strAttribName.CompareNoCase(L"lcdText")==0)
		{
			m_skPaint.setLCDRenderText(String2Bool(strValue));
		}
		return S_OK;
	}

	void SFont_Skia::OnInitFinished(pugi::xml_node xmlNode)
	{
		(xmlNode);
		if(m_blurStyle != -1 && m_blurRadius > 0.0f)
		{
			m_skPaint.setMaskFilter(SkBlurMaskFilter::Create(m_blurStyle,
				SkBlurMask::ConvertRadiusToSigma(m_blurRadius)))->unref();
		}
	}

	BOOL SFont_Skia::UpdateFont(const LOGFONT *plf)
	{
		if(!m_skFont) return FALSE;

		memcpy(&m_lf,plf,sizeof(LOGFONT));

#ifdef UNICODE
		SStringA strFace=S_CT2A(plf->lfFaceName,CP_UTF8);
#else
		SStringA strFace=S_CT2A(plf->lfFaceName,CP_ACP);
#endif
		BYTE style=SkTypeface::kNormal;
		if(plf->lfItalic) style |= SkTypeface::kItalic;
		if(plf->lfWeight == FW_BOLD) style |= SkTypeface::kBold;

		m_skFont->unref();
		m_skFont=SkTypeface::CreateFromName(strFace,(SkTypeface::Style)style);
		return TRUE;
	}

	namespace RENDER_SKIA
    {
        BOOL SCreateInstance( IObjRef ** ppRenderFactory )
        {
            *ppRenderFactory = new SRenderFactory_Skia;
            return TRUE;
        }
    }


	//////////////////////////////////////////////////////////////////////////
	static int s_cPath =0;
	SPath_Skia::SPath_Skia(IRenderFactory *pRenderFac)
		:TSkiaRenderObjImpl<IPath>(pRenderFac)
	{
//         STRACE(L"path new; objects = %d",++s_cPath);
	}

	SPath_Skia::~SPath_Skia()
	{
//         STRACE(L"path delete; objects = %d",--s_cPath);

	}

	const OBJTYPE SPath_Skia::ObjectType() const
	{
		return OT_PATH;
	}

	IPath::FillType SPath_Skia::getFillType() const
	{
		return (IPath::FillType)m_skPath.getFillType();
	}

	void SPath_Skia::setFillType(IPath::FillType ft)
	{
		m_skPath.setFillType((SkPath::FillType)ft);
	}

	bool SPath_Skia::isInverseFillType() const
	{
		return m_skPath.isInverseFillType();
	}

	void SPath_Skia::toggleInverseFillType()
	{
		m_skPath.toggleInverseFillType();
	}

	IPath::Convexity SPath_Skia::getConvexity() const
	{
		return (IPath::Convexity)m_skPath.getConvexity();
	}

	void SPath_Skia::setConvexity(Convexity c)
	{
		m_skPath.setConvexity((SkPath::Convexity)c);
	}

	bool SPath_Skia::isConvex() const
	{
		return m_skPath.isConvex();
	}

	bool SPath_Skia::isOval(RECT* rect) const
	{
		SkRect skRect;
		bool bRet = m_skPath.isOval(&skRect);
		if(rect && bRet)
		{
			SkRect2RECT(skRect,rect);
		}
		return bRet;
	}

	void SPath_Skia::reset()
	{
		m_skPath.reset();
	}

	void SPath_Skia::rewind()
	{
		m_skPath.rewind();
	}

	bool SPath_Skia::isEmpty() const
	{
		return m_skPath.isEmpty();
	}

	bool SPath_Skia::isFinite() const
	{
		return m_skPath.isFinite();
	}

	bool SPath_Skia::isLine(POINT line[2]) const
	{
		SkPoint skLine[2];
		bool bRet = m_skPath.isLine(skLine);
		if(bRet)
		{
			line[0].x = (int)skLine[0].fX;
			line[0].y = (int)skLine[0].fY;
			line[1].x = (int)skLine[1].fX;
			line[1].x = (int)skLine[1].fY;
		}
		return bRet;
	}

	bool SPath_Skia::isRect(RECT* rect) const
	{
		SkRect skRect;
		bool bRet = m_skPath.isRect(&skRect);
		if(rect && bRet)
		{
			SkRect2RECT(skRect,rect);
		}
		return bRet;
	}

	bool SPath_Skia::isRect(bool* isClosed, Direction* direction) const
	{
		return m_skPath.isRect(isClosed,(SkPath::Direction*)direction);
	}

	int SPath_Skia::countPoints() const
	{
		return m_skPath.countPoints();
	}

	POINT SPath_Skia::getPoint(int index) const
	{
		SkPoint ret = m_skPath.getPoint(index);
		POINT pt = {(int)ret.fX,(int)ret.fY};
		return pt;
	}

	int SPath_Skia::getPoints(POINT points[], int max) const
	{
		SASSERT(points);
		SkPoint *pts = new SkPoint[max];
		int nRet = m_skPath.getPoints(pts,max);
		for(int i=0;i<nRet;i++)
		{
			points[i].x = (int)pts[i].fX;
			points[i].y = (int)pts[i].fY;
		}
		delete []pts;
		return nRet;
	}

	int SPath_Skia::countVerbs() const
	{
		return m_skPath.countVerbs();
	}

	int SPath_Skia::getVerbs(BYTE verbs[], int max) const
	{
		return m_skPath.getVerbs(verbs,max);
	}

	RECT SPath_Skia::getBounds() const
	{
		const SkRect &rc = m_skPath.getBounds();
		RECT rcRet;
		SkRect2RECT(rc,&rcRet);
		return rcRet;
	}

	void SPath_Skia::moveTo(float x, float y)
	{
		m_skPath.moveTo(x,y);
	}

	void SPath_Skia::rMoveTo(float dx, float dy)
	{
		m_skPath.rMoveTo(dx,dy);
	}

	void SPath_Skia::lineTo(float x, float y)
	{
		m_skPath.lineTo(x,y);
	}

	void SPath_Skia::rLineTo(float dx, float dy)
	{
		m_skPath.rLineTo(dx,dy);
	}

	void SPath_Skia::quadTo(float x1, float y1, float x2, float y2)
	{
		m_skPath.quadTo(x1,y1,x2,y2);
	}

	void SPath_Skia::rQuadTo(float dx1, float dy1, float dx2, float dy2)
	{
		m_skPath.rQuadTo(dx1,dy1,dx2,dy2);
	}

	void SPath_Skia::conicTo(float x1, float y1, float x2, float y2, float w)
	{
		m_skPath.conicTo(x1,y1,x2,y2,w);
	}

	void SPath_Skia::rConicTo(float dx1, float dy1, float dx2, float dy2, float w)
	{
		m_skPath.rConicTo(dx1,dy1,dx2,dy2,w);
	}

	void SPath_Skia::cubicTo(float x1, float y1, float x2, float y2, float x3, float y3)
	{
		m_skPath.cubicTo(x1,y1,x2,y2,x3,y3);
	}

	void SPath_Skia::rCubicTo(float dx1, float dy1, float dx2, float dy2, float dx3, float dy3)
	{
		m_skPath.rCubicTo(dx1,dy1,dx2,dy2,dx3,dy3);
	}

	void SPath_Skia::arcTo(const RECT& oval, float startAngle, float sweepAngle, bool forceMoveTo)
	{
		SkRect skOval = toSkRect(&oval);
		m_skPath.arcTo(skOval,startAngle,sweepAngle,forceMoveTo);
	}

	void SPath_Skia::arcTo(float x1, float y1, float x2, float y2, float radius)
	{
		m_skPath.arcTo(x1,y1,x2,y2,radius);
	}

	void SPath_Skia::close()
	{
		m_skPath.close();
	}

	void SPath_Skia::addRect(const RECT& rect, Direction dir /*= kCW_Direction*/)
	{
		SkRect skRc = toSkRect(&rect);
		m_skPath.addRect(skRc,(SkPath::Direction)dir);
	}

	void SPath_Skia::addRect(float left, float top, float right, float bottom, Direction dir /*= kCW_Direction*/)
	{
		m_skPath.addRect(left,top,right,bottom,(SkPath::Direction)dir);
	}

	void SPath_Skia::addOval(const RECT& oval, Direction dir /*= kCW_Direction*/)
	{
		SkRect skRc = toSkRect(&oval);
		m_skPath.addOval(skRc,(SkPath::Direction)dir);
	}

	void SPath_Skia::addCircle(float x, float y, float radius, Direction dir /*= kCW_Direction*/)
	{
		m_skPath.addCircle(x,y,radius,(SkPath::Direction)dir);
	}

	void SPath_Skia::addArc(const RECT& oval, float startAngle, float sweepAngle)
	{
		SkRect skRc = toSkRect(&oval);
		m_skPath.addArc(skRc,startAngle,sweepAngle);
	}

	void SPath_Skia::addRoundRect(const RECT& rect, float rx, float ry, Direction dir /*= kCW_Direction*/)
	{
		SkRect skRc = toSkRect(&rect);
		m_skPath.addRoundRect(skRc,rx,ry,(SkPath::Direction)dir);
	}

	void SPath_Skia::addRoundRect(const RECT& rect, const float radii[], Direction dir /*= kCW_Direction*/)
	{
		SkRect skRc = toSkRect(&rect);
		m_skPath.addRoundRect(skRc,radii,(SkPath::Direction)dir);
	}

	void SPath_Skia::addPoly(const POINT pts[], int count, bool close)
	{
		SkPoint *skPts = new SkPoint[count];
		for(int i=0;i<count;i++)
		{
			skPts[i].fX = (float)pts[i].x;
			skPts[i].fY = (float)pts[i].y;
		}

		m_skPath.addPoly(skPts,count,close);
		delete []skPts;
	}

	void SPath_Skia::addPath(const IPath * src, float dx, float dy, AddPathMode mode /*= kAppend_AddPathMode*/)
	{
		const SPath_Skia *skSrc = (const SPath_Skia*)src;
		m_skPath.addPath(skSrc->m_skPath,dx,dy,(SkPath::AddPathMode)mode);
	}

	void SPath_Skia::reverseAddPath(const IPath* src)
	{
		const SPath_Skia *skSrc = (const SPath_Skia*)src;
		m_skPath.reverseAddPath(skSrc->m_skPath);
	}

	void SPath_Skia::offset(float dx, float dy)
	{
		m_skPath.offset(dx,dy);
	}

	bool SPath_Skia::getLastPt(POINT* lastPt) const
	{
		SkPoint pt;
		bool bRet = m_skPath.getLastPt(&pt);
		if(lastPt)
		{
			lastPt->x = (int)pt.fX;
			lastPt->y = (int)pt.fY;
		}
		return bRet;
	}

	void SPath_Skia::setLastPt(float x, float y)
	{
		m_skPath.setLastPt(x,y);
	}



}//end of namespace SOUI

