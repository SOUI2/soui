
#pragma once
#include "core/SWnd.h"

namespace SOUI
{

class SText : public SStatic
{
    SOUI_CLASS_NAME(SStatic, L"textex")
public:
    virtual void DrawText(IRenderTarget *pRT,LPCTSTR pszBuf,int cchText,LPRECT pRect,UINT uFormat);
};

};