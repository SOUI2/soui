#include "souistd.h"
#include "helper/SDIBHelper.h"

#define RGB2GRAY(r,g,b) (((b)*117 + (g)*601 + (r)*306) >> 10)

namespace SOUI
{
    struct DIBINFO{
        LPBYTE  pBits;
        UINT     nWid;
		UINT     nHei;
    };

	// ------------------------------------------------------------
	// 有太多的算法需要用某种方式(map)变换位图的每个像素的颜色，比如
	// 彩色转换为灰度图，gamma校正，颜色空间转换,hsl调整.所以写一个模板做为参数调用的通用算法
	// ------------------------------------------------------------
	template <class Mode, class Param>
	bool ColorTransform(DIBINFO* pDib, Mode mode, const Param &param)
	{
		if (NULL == pDib||NULL == pDib->pBits)
		{
			return false;
		}
        
		SMap<DWORD, DWORD> cache;
        int nPixels = pDib->nWid * pDib->nHei;
        LPBYTE pBit = pDib->pBits;
        for(int i=0;i<nPixels;i++, pBit+=4)
        {
			DWORD crFrom = *(DWORD*)pBit;
			SMap<DWORD, DWORD>::CPair *p = cache.Lookup(crFrom);
			if (p)
			{
				memcpy(pBit, &p->m_value, 4);
			}
			else
			{
				mode(pBit, param);
				cache[crFrom] = *(DWORD*)pBit;
			}
        }

		return true;
	}


	// 灰度 = 0.299 * red + 0.587 * green + 0.114 * blue 
	static void GrayMode(BYTE *pColor,const int &)
	{
        pColor[0] = pColor[1] = pColor[2] = RGB2GRAY(pColor[0],pColor[1],pColor[2]);
	}
    
    struct COLORIZEPARAM{
        BYTE hue;
        BYTE sat;
        int  a0;//[0-256]
        int  a1;//[0-256]
    };

    static void FillColorizeParam(COLORIZEPARAM &param ,BYTE hue,BYTE sat,float fBlend)
    {
        param.hue = hue;
        param.sat = sat;
        SASSERT(fBlend>=0.0f && fBlend <=1.0f);
        param.a0 = (int)(fBlend*256);
        param.a1 = 256 - param.a0;
    }

    ////////////////////////////////////////////////////////////////////////////////
    #define  HSLMAX   255	/* H,L, and S vary over 0-HSLMAX */
    #define  RGBMAX   255   /* R,G, and B vary over 0-RGBMAX */
        /* HSLMAX BEST IF DIVISIBLE BY 6 */
        /* RGBMAX, HSLMAX must each fit in a BYTE. */
        /* Hue is undefined if Saturation is 0 (grey-scale) */
        /* This value determines where the Hue scrollbar is */
        /* initially set for achromatic colors */
    #define HSLUNDEFINED (HSLMAX*2/3)
    ////////////////////////////////////////////////////////////////////////////////
    static RGBQUAD RGBtoHSL(RGBQUAD lRGBColor)
    {
        BYTE R,G,B;					/* input RGB values */
        BYTE H,L,S;					/* output HSL values */
        BYTE cMax,cMin;				/* max and min RGB values */
        WORD Rdelta,Gdelta,Bdelta;	/* intermediate value: % of spread from max*/

        R = lRGBColor.rgbRed;	/* get R, G, and B out of DWORD */
        G = lRGBColor.rgbGreen;
        B = lRGBColor.rgbBlue;

        cMax = (std::max)( (std::max)(R,G), B);	/* calculate lightness */
        cMin = (std::min)( (std::min)(R,G), B);
        L = (BYTE)((((cMax+cMin)*HSLMAX)+RGBMAX)/(2*RGBMAX));

        if (cMax==cMin){			/* r=g=b --> achromatic case */
            S = 0;					/* saturation */
            H = HSLUNDEFINED;		/* hue */
        } else {					/* chromatic case */
            if (L <= (HSLMAX/2))	/* saturation */
                S = (BYTE)((((cMax-cMin)*HSLMAX)+((cMax+cMin)/2))/(cMax+cMin));
            else
                S = (BYTE)((((cMax-cMin)*HSLMAX)+((2*RGBMAX-cMax-cMin)/2))/(2*RGBMAX-cMax-cMin));
            /* hue */
            Rdelta = (WORD)((((cMax-R)*(HSLMAX/6)) + ((cMax-cMin)/2) ) / (cMax-cMin));
            Gdelta = (WORD)((((cMax-G)*(HSLMAX/6)) + ((cMax-cMin)/2) ) / (cMax-cMin));
            Bdelta = (WORD)((((cMax-B)*(HSLMAX/6)) + ((cMax-cMin)/2) ) / (cMax-cMin));

            if (R == cMax)
                H = (BYTE)(Bdelta - Gdelta);
            else if (G == cMax)
                H = (BYTE)((HSLMAX/3) + Rdelta - Bdelta);
            else /* B == cMax */
                H = (BYTE)(((2*HSLMAX)/3) + Gdelta - Rdelta);

            //		if (H < 0) H += HSLMAX;     //always false
            if (H > HSLMAX) H -= HSLMAX;
        }
        RGBQUAD hsl={L,S,H,0};
        return hsl;
    }

    ////////////////////////////////////////////////////////////////////////////////
    static RGBQUAD RGBtoRGBQUAD(COLORREF cr)
    {
        RGBQUAD c;
        c.rgbRed = GetRValue(cr);	/* get R, G, and B out of DWORD */
        c.rgbGreen = GetGValue(cr);
        c.rgbBlue = GetBValue(cr);
        c.rgbReserved=GetAValue(cr);
        return c;
    }
    ////////////////////////////////////////////////////////////////////////////////
    static COLORREF RGBQUADtoRGB (RGBQUAD c)
    {
        return RGBA(c.rgbRed,c.rgbGreen,c.rgbBlue,c.rgbReserved);
    }

    ////////////////////////////////////////////////////////////////////////////////
    static float HueToRGB(float n1,float n2, float hue)
    {
        //<F. Livraghi> fixed implementation for HSL2RGB routine
        float rValue;

        if (hue > 360)
            hue = hue - 360;
        else if (hue < 0)
            hue = hue + 360;

        if (hue < 60)
            rValue = n1 + (n2-n1)*hue/60.0f;
        else if (hue < 180)
            rValue = n2;
        else if (hue < 240)
            rValue = n1+(n2-n1)*(240-hue)/60;
        else
            rValue = n1;

        return rValue;
    }
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    static RGBQUAD HSLtoRGB(RGBQUAD lHSLColor)
    { 
        //<F. Livraghi> fixed implementation for HSL2RGB routine
        float h,s,l;
        float m1,m2;
        BYTE r,g,b;

        h = (float)lHSLColor.rgbRed * 360.0f/255.0f;
        s = (float)lHSLColor.rgbGreen/255.0f;
        l = (float)lHSLColor.rgbBlue/255.0f;

        if (l <= 0.5)	m2 = l * (1+s);
        else			m2 = l + s - l*s;

        m1 = 2 * l - m2;

        if (s == 0) {
            r=g=b=(BYTE)(l*255.0f);
        } else {
            r = (BYTE)(HueToRGB(m1,m2,h+120) * 255.0f);
            g = (BYTE)(HueToRGB(m1,m2,h) * 255.0f);
            b = (BYTE)(HueToRGB(m1,m2,h-120) * 255.0f);
        }

        RGBQUAD rgb = {b,g,r,0};
        return rgb;
    }

    static void ColorizeMode(BYTE *pArgb, const COLORIZEPARAM & param)
    {
        BYTE blue=pArgb[0],green=pArgb[1],red = pArgb[2],alpha=pArgb[3];
        
        if(alpha == 0) return;
        if(alpha!=255)
        {
            red = (red * 255)/alpha;
            green = (green * 255)/alpha;
            blue = (blue * 255)/alpha;
        }
        RGBQUAD pixel = {blue,green, red};

        if (param.a0 == 256){
            RGBQUAD color = RGBtoHSL(pixel);
            color.rgbRed= param.hue;
            color.rgbGreen=param.sat;
            pixel = HSLtoRGB(color);
            red = pixel.rgbRed;
            green = pixel.rgbGreen;
            blue = pixel.rgbBlue;
        } else {
            RGBQUAD color = pixel;
            RGBQUAD hsl;
            hsl.rgbRed= param.hue;
            hsl.rgbGreen=param.sat;
            hsl.rgbBlue = (BYTE)RGB2GRAY(color.rgbRed,color.rgbGreen,color.rgbBlue);
            hsl = HSLtoRGB(hsl);
            red = (BYTE)((hsl.rgbRed * param.a0 + color.rgbRed * param.a1)>>8);
            blue = (BYTE)((hsl.rgbBlue * param.a0 + color.rgbBlue * param.a1)>>8);
            green= (BYTE)((hsl.rgbGreen * param.a0 + color.rgbGreen * param.a1)>>8);
        }
        if(alpha!=255)
        {
            red = (red *alpha)/255;
            green = (green *alpha)/255;
            blue = (blue *alpha)/255;
        }
        pArgb[0] = blue;
        pArgb[1] = green;
        pArgb[2] = red;
    }

    bool SDIBHelper::Colorize(IBitmap * pBmp, COLORREF crRef)
    {
        RGBQUAD color = RGBtoRGBQUAD(crRef);
        RGBQUAD hsl = RGBtoHSL(color);
        COLORIZEPARAM param;
		float fBlend = 0.8f;
		BYTE byAlpha = GetAValue(crRef);
		if(byAlpha != 0) fBlend = byAlpha*1.0f/255;

        FillColorizeParam(param,hsl.rgbRed,hsl.rgbGreen,fBlend);

        DIBINFO di={(LPBYTE)pBmp->LockPixelBits(),pBmp->Width(),pBmp->Height()};

        bool bRet = ColorTransform(&di, ColorizeMode,param);

        pBmp->UnlockPixelBits(di.pBits);
        return bRet;
    }

    bool SDIBHelper::Colorize(COLORREF & crTarget,COLORREF crRef)
    {
        RGBQUAD color = RGBtoRGBQUAD(crRef);
        RGBQUAD hsl = RGBtoHSL(color);
        COLORIZEPARAM param;

		float fBlend = 0.8f;
		BYTE byAlpha = GetAValue(crRef);
		if(byAlpha != 0) fBlend = byAlpha*1.0f/255;

        FillColorizeParam(param,hsl.rgbRed,hsl.rgbGreen,fBlend);
        

		RGBQUAD argbTarget = RGBtoRGBQUAD(crTarget);
        ColorizeMode((BYTE*)&argbTarget,param);
		crTarget = RGBQUADtoRGB(argbTarget);
        return true;
    }   

	bool SDIBHelper::GrayImage(IBitmap * pBmp)
	{
	    DIBINFO di={(LPBYTE)pBmp->LockPixelBits(),pBmp->Width(),pBmp->Height()};
		bool bRet =ColorTransform(&di, GrayMode,0);
		pBmp->UnlockPixelBits(di.pBits);
		return bRet;
	}
	

    static COLORREF CalcAvarageRectColor(const DIBINFO &di, RECT rc)
    {
        LPBYTE pLine= di.pBits + di.nWid * rc.top *4;
        if(rc.right > (int)di.nWid) rc.right=di.nWid;
        if(rc.bottom > (int)di.nHei) rc.bottom = di.nHei;
        int nWid = rc.right-rc.left;
        int nHei = rc.bottom -rc.top;
        
        int r=0,g=0,b=0;
        for(int y=0;y<nHei;y++)
        {
            LPBYTE p = pLine + rc.left * 4;
            for(int x=0;x<nWid;x++)
            {
                b += *p++;
                g += *p++;
                r += *p++;
                p++;//skip alpha
            }
            pLine += di.nWid*4;
        }
        int nPixels = (nWid*nHei);
        r /= nPixels;
        g /= nPixels;
        b /= nPixels;
        return RGB(r,g,b);
    }
    
    static int RgbCmp(const void *p1,const void *p2)
    {
        const BYTE * cr1 = (const BYTE*)p1;
        const BYTE * cr2 = (const BYTE*)p2;
        int deltaR = ((int)cr2[0] - (int)cr1[0]);
        int deltaG = ((int)cr2[1] - (int)cr1[1]);
        int deltaB = ((int)cr2[2] - (int)cr1[2]);
        return deltaR + deltaG + deltaB;
    }
    
    COLORREF SDIBHelper::CalcAvarageColor(IBitmap *pBmp,int nPercent,int nBlockSize/*=5*/)
    {
        DIBINFO di={(LPBYTE)pBmp->LockPixelBits(),pBmp->Width(),pBmp->Height()};
        
        int xBlocks = (di.nWid + nBlockSize -1)/nBlockSize;
        int yBlocks = (di.nHei + nBlockSize -1)/nBlockSize;
        
        int nBlocks = xBlocks*yBlocks;
        COLORREF *pAvgColors = new COLORREF[nBlocks];
        
        CRect rcBlock(0,0,nBlockSize,nBlockSize);
        int iBlock = 0;
        for(int y=0;y<yBlocks;y++)
        {
            for(int x=0;x<xBlocks;x++)
            {
                pAvgColors[iBlock++] = CalcAvarageRectColor(di,rcBlock);
                rcBlock.OffsetRect(nBlockSize,0);
            }
            rcBlock.MoveToX(0);
            rcBlock.OffsetRect(0,nBlockSize);
        }
        //RGB排序
        qsort(pAvgColors,nBlocks,sizeof(COLORREF),RgbCmp);
        
        int nThrows = nBlocks * (100-nPercent)/200;//一端丢弃数量
        int iBegin = nThrows;
        int iEnd = nBlocks - nThrows;
        
        int r = 0, g= 0, b=0;
        for(int i= iBegin;i<iEnd;i++)
        {
            BYTE *p = (BYTE*)(pAvgColors+i);
            r += p[0];
            g += p[1];
            b += p[2];
        }
        
        r /= (iEnd - iBegin);
        g /= (iEnd - iBegin);
        b /= (iEnd - iBegin);
                        
        delete []pAvgColors;
        pBmp->UnlockPixelBits(di.pBits);
        
        return RGB(r,g,b);
    }

}//namespace SOUI