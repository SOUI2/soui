#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <windows.h>

#define GetAValue(rgb)      (LOBYTE((rgb)>>24))
#define RGBA(r,g,b,a)       (RGB(r,g,b)|(a<<24))
#define CR_INVALID          0x00FFFFFF

//以ARGB格式存储颜色值
class SColor
{
public:
	SColor(BYTE r,BYTE g,BYTE b,BYTE a=0xFF):b(b),g(g),r(r),a(a)
	{

	}

	SColor(COLORREF cr,BYTE a)
	{
	    r=GetRValue(cr);
	    g=GetGValue(cr);
	    b=GetBValue(cr);
        this->a = a;
	}

    SColor(COLORREF cr)
    {
        r=GetRValue(cr);
        g=GetGValue(cr);
        b=GetBValue(cr);
        a=GetAValue(cr);
    }
  
    const COLORREF toCOLORREF() const
    {
        return RGB(r,g,b)|(a<<24);
    }

    const DWORD toARGB() const
    {
        DWORD crRet;
        memcpy(&crRet,this,4);
        return crRet;
    }
        
    void setRGB(COLORREF cr)
    {
        r=GetRValue(cr);
        g=GetGValue(cr);
        b=GetBValue(cr);
        a=GetAValue(cr);
    }
    
    void setRGB(COLORREF cr,BYTE a)
    {
        r=GetRValue(cr);
        g=GetGValue(cr);
        b=GetBValue(cr);
        this->a=a;
    }
    
    void updateAlpha(BYTE alpha)
    {
		if(alpha != 0xFF)
			a = (a * alpha)>>8;
    }
    
    DWORD b:8;
    DWORD g:8;
    DWORD r:8;
    DWORD a:8;
};