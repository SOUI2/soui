#pragma once

#include <helper/color.h>
#include <unknown/obj-ref-impl.hpp>

#include <string/tstring.h>
#include <string/strcpcvt.h>
#include <interface/render-i.h>
#include <souicoll.h>

namespace SOUI
{


    //////////////////////////////////////////////////////////////////////////
    // SRenderFactory_GDI
    class SRenderFactory_GDI : public TObjRefImpl<IRenderFactory>
    {
    public:
        SRenderFactory_GDI()
        {
        }

        ~SRenderFactory_GDI()
        {
        }
        
        virtual IImgDecoderFactory * GetImgDecoderFactory(){return m_imgDecoderFactory;}
        virtual void SetImgDecoderFactory(IImgDecoderFactory *pImgDecoderFac){ m_imgDecoderFactory=pImgDecoderFac;}
        virtual BOOL CreateRenderTarget(IRenderTarget ** ppRenderTarget,int nWid,int nHei);
        virtual BOOL CreateFont(IFont ** ppFont , const LOGFONT &lf);
        virtual BOOL CreateBitmap(IBitmap ** ppBitmap);
        virtual BOOL CreateRegion(IRegion **ppRgn);

		virtual BOOL CreatePath(IPath ** ppPath);

		virtual BOOL CreatePathEffect(REFGUID guidEffect,IPathEffect ** ppPathEffect);
		
		virtual BOOL CreatePathMeasure(IPathMeasure ** ppPathMeasure);
    protected:
        CAutoRefPtr<IImgDecoderFactory> m_imgDecoderFactory;
    };


    //////////////////////////////////////////////////////////////////////////
    // TGdiRenderObjImpl
    template<class T>
    class TGdiRenderObjImpl : public TObjRefImpl< SObjectImpl<T> >
    {
    public:
        TGdiRenderObjImpl(IRenderFactory * pRenderFac):m_pRenderFactory(pRenderFac)
        {

        }
        
        virtual ~TGdiRenderObjImpl(){}

        virtual IRenderFactory * GetRenderFactory() const
        {
            return m_pRenderFactory;
        }

    protected:
        IRenderFactory *m_pRenderFactory;

    };


    //////////////////////////////////////////////////////////////////////////
    // SPen_GDI
    class SPen_GDI : public TGdiRenderObjImpl<IPen>
    {
		SOUI_CLASS_NAME(SPen_GDI,L"pen")
	public:
        SPen_GDI(IRenderFactory * pRenderFac,int iStyle=PS_SOLID,COLORREF cr=0,int cWidth=1)
            :TGdiRenderObjImpl<IPen>(pRenderFac)
            ,m_nWidth(cWidth),m_style(iStyle),m_cr(cr)
            ,m_hPen(NULL)
        {
			if (iStyle & PS_GEOMETRIC)
			{
				LOGBRUSH lb;
				lb.lbStyle = BS_SOLID;
				lb.lbColor = m_cr & 0x00ffffff;
				lb.lbHatch = 0;
				m_hPen = ::ExtCreatePen(m_style | PS_GEOMETRIC, m_nWidth, &lb, 0, NULL);
			}
			else
			{
				m_hPen = ::CreatePen(m_style , m_nWidth, m_cr & 0x00ffffff);
			}
        }

        ~SPen_GDI()
        {
            DeleteObject(m_hPen);
        }

        int GetWidth(){return m_nWidth;}

        int GetStyle(){return m_style;}

        void SetWidth(int nWid) {m_nWidth=nWid;}

        COLORREF GetColor(){return m_cr;}

        void SetColor(COLORREF cr){m_cr = cr;}
        
        HPEN GetPen(){return m_hPen;}
    protected:

        int		m_style;
        int		m_nWidth;
        COLORREF	m_cr;
        
        HPEN    m_hPen;
    };

    //////////////////////////////////////////////////////////////////////////
    // SFont_GDI
    class SFont_GDI: public TGdiRenderObjImpl<IFont>
    {
		SOUI_CLASS_NAME(SFont_GDI,L"font")
    public:
        SFont_GDI(IRenderFactory * pRenderFac,const LOGFONT * plf)
            :TGdiRenderObjImpl<IFont>(pRenderFac),m_hFont(NULL)
        {
            memcpy(&m_lf,plf,sizeof(LOGFONT));
            m_hFont=CreateFontIndirect(&m_lf);
        }
        ~SFont_GDI()
        {
            DeleteObject(m_hFont);
        }

        virtual const LOGFONT * LogFont() const {return &m_lf;}

        virtual LPCTSTR FamilyName()
        {
            return m_lf.lfFaceName;
        }
        virtual int TextSize(){return m_lf.lfHeight;}
        virtual BOOL IsBold(){ return m_lf.lfWeight == FW_BOLD;}
        virtual BOOL IsUnderline(){return m_lf.lfUnderline;}
        virtual BOOL IsItalic(){return m_lf.lfItalic;}
        virtual BOOL IsStrikeOut(){return m_lf.lfStrikeOut;}

		virtual BOOL UpdateFont(const LOGFONT *pLogFont)
		{
			if(!m_hFont) return FALSE;
			DeleteObject(m_hFont);
			memcpy(&m_lf,pLogFont,sizeof(LOGFONT));
			m_hFont = CreateFontIndirect(&m_lf);
			return TRUE;
		}

        HFONT GetFont(){return m_hFont;}
    protected:
        LOGFONT     m_lf;
        HFONT       m_hFont;
    };

    class SBrush_GDI : public TGdiRenderObjImpl<IBrush>
    {
		SOUI_CLASS_NAME(SBrush_GDI,L"brush")
    public:
        static SBrush_GDI * CreateSolidBrush(IRenderFactory * pRenderFac,COLORREF cr){
            return new SBrush_GDI(pRenderFac,cr);
        }

        static SBrush_GDI * CreateBitmapBrush(IRenderFactory * pRenderFac,HBITMAP hBmp)
        {
            return new SBrush_GDI(pRenderFac,hBmp);
        }


        BOOL IsBitmap(){return m_fBmp;}
        
        HBRUSH GetBrush(){return m_hBrush;}

        COLORREF GetColor() const {return m_cr;}
    protected:
        SBrush_GDI(IRenderFactory * pRenderFac,COLORREF cr)
            :TGdiRenderObjImpl<IBrush>(pRenderFac),m_fBmp(FALSE),m_cr(cr)
        {
            m_hBrush = ::CreateSolidBrush(m_cr&0x00ffffff);
        }
        SBrush_GDI(IRenderFactory * pRenderFac,HBITMAP hBmp)
            :TGdiRenderObjImpl<IBrush>(pRenderFac),m_fBmp(TRUE)
        {
            m_hBrush = ::CreatePatternBrush(hBmp);
        }
        ~SBrush_GDI()
        {
            DeleteObject(m_hBrush);
        }


        HBRUSH   m_hBrush;
        BOOL	 m_fBmp;
        COLORREF    m_cr;
    };

    //////////////////////////////////////////////////////////////////////////
    // SBitmap_GDI
    class SBitmap_GDI : public TGdiRenderObjImpl<IBitmap>
    {
		SOUI_CLASS_NAME(SBitmap_GDI,L"bitmap")
    public:
        SBitmap_GDI(IRenderFactory *pRenderFac)
            :TGdiRenderObjImpl<IBitmap>(pRenderFac),m_hBmp(0)
        {
            m_sz.cx=m_sz.cy=0;
        }
        virtual ~SBitmap_GDI()
        {
            if(m_hBmp) DeleteObject(m_hBmp);
        }
        virtual HRESULT Init(int nWid,int nHei,const LPVOID pBits=NULL);
        virtual HRESULT Init(IImgFrame *pFrame);
		virtual HRESULT LoadFromFile(LPCTSTR pszFileName);
        virtual HRESULT LoadFromMemory(LPBYTE pBuf,size_t szLen);

        virtual UINT Width() const;
        virtual UINT Height() const;
        virtual SIZE Size() const;
        virtual LPVOID  LockPixelBits();
        virtual void    UnlockPixelBits(LPVOID pBuf);
        virtual const LPVOID GetPixelBits() const;
        
        HBITMAP  GetBitmap(){return m_hBmp;}

        static HBITMAP CreateGDIBitmap(int nWid,int nHei,void ** ppBits);
    protected:

        HRESULT ImgFromDecoder(IImgX *imgDecoder);
        SIZE        m_sz;
        HBITMAP     m_hBmp;     //标准的32位位图，和m_bitmap共享内存
    };

    //////////////////////////////////////////////////////////////////////////
    //	SRegion_GDI
    class SRegion_GDI: public TGdiRenderObjImpl<IRegion>
    {
		SOUI_CLASS_NAME(SRegion_GDI,L"region")
		friend class SRenderTarget_GDI;
    public:
        SRegion_GDI(IRenderFactory *pRenderFac);
        ~SRegion_GDI(){
            DeleteObject(m_hRgn);
        }

        virtual void CombineRect(LPCRECT lprect,int nCombineMode);
		virtual void CombineRoundRect(LPCRECT lprect, POINT ptRadius, int nCombineMode);
		virtual void CombineEllipse(LPCRECT lprect , int nCombineMode);

		virtual void CombineRgn(const IRegion * pRgnSrc,int nCombineMode );
        virtual void SetRgn(const HRGN rgn);

        virtual BOOL PtInRegion(POINT pt);
        virtual BOOL RectInRegion(LPCRECT lprect);
        virtual void GetRgnBox(LPRECT lprect);
        virtual BOOL IsEmpty();
        virtual void Offset(POINT pt);
        virtual void Clear();

    protected:
        HRGN GetRegion() const;
        void _CombineRgn(HRGN hRgn,int nCombineMode);



        HRGN    m_hRgn;
    };

    //////////////////////////////////////////////////////////////////////////
    //	SRenderTarget_GDI
    //////////////////////////////////////////////////////////////////////////
    class SRenderTarget_GDI: public TObjRefImpl<IRenderTarget>
    {
    public:
        SRenderTarget_GDI(IRenderFactory* pRenderFactory,int nWid,int nHei);
        ~SRenderTarget_GDI();

        //只支持创建位图表面
        virtual HRESULT CreateCompatibleRenderTarget(SIZE szTarget,IRenderTarget **ppRenderTarget);

        virtual HRESULT CreatePen(int iStyle,COLORREF cr,int cWidth,IPen ** ppPen);
        virtual HRESULT CreateSolidColorBrush(COLORREF cr,IBrush ** ppBrush);
        virtual HRESULT CreateBitmapBrush( IBitmap *pBmp,IBrush ** ppBrush );

        virtual HRESULT Resize(SIZE sz);

        virtual HRESULT OffsetViewportOrg(int xOff, int yOff, LPPOINT lpPoint=NULL);
        virtual HRESULT GetViewportOrg(LPPOINT lpPoint);
        virtual HRESULT SetViewportOrg(POINT pt);

        virtual HRESULT PushClipRect(LPCRECT pRect,UINT mode=RGN_AND);
        virtual HRESULT PushClipRegion(IRegion *pRegion,UINT mode=RGN_AND);
        virtual HRESULT PopClip();

        virtual HRESULT ExcludeClipRect(LPCRECT pRc);
        virtual HRESULT IntersectClipRect(LPCRECT pRc);

        virtual HRESULT SaveClip(int *pnState);
        virtual HRESULT RestoreClip(int nState=-1);

        virtual HRESULT GetClipRegion(IRegion **ppRegion);
        virtual HRESULT GetClipBox(LPRECT prc);

        virtual HRESULT BitBlt(LPCRECT pRcDest,IRenderTarget *pRTSour,int xSrc,int ySrc,DWORD dwRop=SRCCOPY);
        virtual HRESULT AlphaBlend(LPCRECT pRcDest,IRenderTarget *pRTSrc,LPCRECT pRcSrc,BYTE byAlpha);

        virtual HRESULT DrawText( LPCTSTR pszText,int cchLen,LPRECT pRc,UINT uFormat);
        virtual HRESULT MeasureText(LPCTSTR pszText,int cchLen, SIZE *psz );

        virtual HRESULT DrawRectangle(LPCRECT pRect);
        virtual HRESULT FillRectangle(LPCRECT pRect);
        virtual HRESULT FillSolidRect(LPCRECT pRect,COLORREF cr);
        virtual HRESULT ClearRect(LPCRECT pRect,COLORREF cr);
        virtual HRESULT InvertRect(LPCRECT pRect);

        virtual HRESULT DrawEllipse(LPCRECT pRect);
        virtual HRESULT FillEllipse(LPCRECT pRect);
        virtual HRESULT FillSolidEllipse(LPCRECT pRect,COLORREF cr);

        virtual HRESULT DrawArc(LPCRECT pRect,float startAngle,float sweepAngle,bool useCenter);
        virtual HRESULT FillArc(LPCRECT pRect,float startAngle,float sweepAngle);

        virtual HRESULT DrawRoundRect(LPCRECT pRect,POINT pt);
        virtual HRESULT FillRoundRect(LPCRECT pRect,POINT pt);
        virtual HRESULT FillSolidRoundRect(LPCRECT pRect,POINT pt,COLORREF cr);

        virtual HRESULT DrawLines(LPPOINT pPt,size_t nCount);
        virtual HRESULT GradientFill(LPCRECT pRect,BOOL bVert,COLORREF crBegin,COLORREF crEnd,BYTE byAlpha=0xFF);
        virtual HRESULT GradientFillEx( LPCRECT pRect,const POINT* pts,COLORREF *colors,float *pos,int nCount,BYTE byAlpha=0xFF );

        virtual HRESULT TextOut(
            int x,
            int y,
            LPCTSTR lpszString,
            int nCount);

        virtual HRESULT DrawIconEx(int xLeft, int yTop, HICON hIcon, int cxWidth,int cyWidth,UINT diFlags);
        virtual HRESULT DrawBitmap(LPCRECT pRcDest,IBitmap *pBitmap,int xSrc,int ySrc,BYTE byAlpha=0xFF);
        virtual HRESULT DrawBitmapEx(LPCRECT pRcDest,IBitmap *pBitmap,LPCRECT pRcSrc,UINT expendMode, BYTE byAlpha=0xFF);
        virtual HRESULT DrawBitmap9Patch(LPCRECT pRcDest,IBitmap *pBitmap,LPCRECT pRcSrc,LPCRECT pRcSourMargin,UINT expendMode,BYTE byAlpha=0xFF);

        virtual IRenderObj * GetCurrentObject(OBJTYPE uType);
        virtual HRESULT SelectDefaultObject(OBJTYPE objType, IRenderObj ** ppOldObj = NULL);
        virtual HRESULT SelectObject(IRenderObj *pObj,IRenderObj ** ppOldObj = NULL);


        virtual COLORREF GetTextColor()
        {
            return m_curColor.toCOLORREF();
        }

        virtual COLORREF SetTextColor(COLORREF color)
        {
            COLORREF crOld=m_curColor.toCOLORREF();
            m_curColor.setRGB(color);
            ::SetTextColor(m_hdc,color&0x00ffffff);
            return crOld;
        }

        virtual HDC GetDC(UINT uFlag=0);
        virtual void ReleaseDC(HDC hdc);

        virtual HRESULT QueryInterface(REFGUID iid,IObjRef ** ppObj){
			(iid);
			(ppObj);
			return E_NOINTERFACE;
		}

        virtual HRESULT SetTransform(const IxForm * pXForm,IxForm *pOldXFrom=NULL);

        virtual HRESULT GetTransform(IxForm * pXForm) const;

		virtual COLORREF GetPixel( int x, int y );

		virtual COLORREF SetPixel( int x, int y, COLORREF cr );

		virtual HRESULT GradientFill2(LPCRECT pRect,GradientType type,COLORREF crStart,COLORREF crCenter,COLORREF crEnd,float fLinearAngle,float fCenterX,float fCenterY,int nRadius,BYTE byAlpha=0xff);

		virtual HRESULT CreateRegion( IRegion ** ppRegion );

		virtual HRESULT ClipPath(const IPath * path, UINT mode, bool doAntiAlias = false);

		virtual HRESULT DrawPath(const IPath * path,IPathEffect * pathEffect=NULL);

	protected:
        HDC               m_hdc;
        SColor            m_curColor;
        CAutoRefPtr<SBitmap_GDI> m_curBmp;
        CAutoRefPtr<SPen_GDI> m_curPen;
        CAutoRefPtr<SBrush_GDI> m_curBrush;
        CAutoRefPtr<SFont_GDI> m_curFont;
        POINT               m_ptOrg;
        
        //注意保存4个默认的GDI对象
        CAutoRefPtr<IBitmap> m_defBmp;
        CAutoRefPtr<IPen> m_defPen;
        CAutoRefPtr<IBrush> m_defBrush;
        CAutoRefPtr<IFont> m_defFont;
		CAutoRefPtr<IRenderFactory> m_pRenderFactory;
        UINT m_uGetDCFlag;
    };
    
    namespace RENDER_GDI
    {
        SOUI_COM_C BOOL SOUI_COM_API SCreateInstance(IObjRef ** ppRenderFactory);
    }
}


