#include "GradientFillHelper.h"
#include <Wingdi.h>
#pragma comment(lib,"msimg32.lib")

namespace SOUI
{

void GradientFillRect(HDC hdc, const RECT *rcFill, COLORREF cr1, COLORREF cr2,BOOL bVert)
{
	HDC hdc1=hdc;

	TRIVERTEX        vert[2] ;
	vert [0] .x      = rcFill->left;
	vert [0] .y      = rcFill->top;
	vert [0] .Red    = GetRValue(cr1)<<8;
	vert [0] .Green  = GetGValue(cr1)<<8;
	vert [0] .Blue   = GetBValue(cr1)<<8;
	vert [0] .Alpha  = 0xff00;

	vert [1] .x      = rcFill->right;
	vert [1] .y      = rcFill->bottom; 
	vert [1] .Red    = GetRValue(cr2)<<8;
	vert [1] .Green  = GetGValue(cr2)<<8;
	vert [1] .Blue   = GetBValue(cr2)<<8;
	vert [1] .Alpha  = 0xff00;

	GRADIENT_RECT    gRect={0,1};
	GradientFill(hdc1,vert,2,&gRect,1,bVert?GRADIENT_FILL_RECT_V:GRADIENT_FILL_RECT_H);
}

void GradientFillRectH(HDC hdc, const RECT *rcFill, COLORREF crLeft, COLORREF crRight)
{
    GradientFillRect(hdc, rcFill, crLeft,crRight,FALSE);
}

void GradientFillRectV( HDC hdc,const RECT *rcFill, COLORREF crTop, COLORREF crBottom)
{
	GradientFillRect(hdc, rcFill, crTop,crBottom,TRUE);
}

}//namespace DuiEngine