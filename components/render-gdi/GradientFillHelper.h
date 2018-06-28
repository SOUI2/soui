#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <windows.h>

namespace SOUI
{

void GradientFillRectV(HDC hdc,const RECT *rcFill, COLORREF crTop, COLORREF crBottom);

void GradientFillRectH(HDC hdc,const RECT *rcFill, COLORREF crLeft, COLORREF crRight);
void GradientFillRect(HDC hdc, const RECT *rcFill, COLORREF cr1, COLORREF cr2,BOOL bVert);
}//namespace SOUI