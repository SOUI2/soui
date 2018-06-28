#include "stdafx.h"
#include "SText.h"

void SText::DrawText(IRenderTarget *pRT,LPCTSTR pszBuf,int cchText,LPRECT pRect,UINT uFormat)
{
    pRT->DrawText(pszBuf,cchText,pRect,uFormat|DT_WORDBREAK|DT_EDITCONTROL);
}
