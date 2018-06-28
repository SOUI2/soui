#pragma once

#include "core/SSkinObjBase.h"

namespace SOUI
{
 
class SColorMask : public SSkinObjBase
{
    #define STATE_COUNT 4
    SOUI_CLASS_NAME(SColorMask, L"colormask")

    enum{
        ST_NORMAL=0,
        ST_HOVER,
        ST_PUSHDOWN,
        ST_DISABLE,
    };

public:
    SColorMask();
    virtual BOOL IgnoreState()  { return FALSE;     }
    virtual int  GetStates()    { return STATE_COUNT;}
    virtual SIZE GetSkinSize()  { return m_size;    }

    void SetColors(COLORREF cr[4]);

protected:
    virtual void _Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha);
    HRESULT      OnAttrMask(const SStringW & strValue,BOOL bLoading);
    void         MakeCacheApha();

    COLORREF    m_crStates[STATE_COUNT];
    SIZE        m_size;

    SOUI_ATTRS_BEGIN()
        ATTR_COLOR(L"colorNormal", m_crStates[0], TRUE)   
        ATTR_COLOR(L"colorHover", m_crStates[1], TRUE)    
        ATTR_COLOR(L"colorDown", m_crStates[2], TRUE)     
        ATTR_COLOR(L"colorDisable", m_crStates[3], TRUE)  
        ATTR_INT(L"width", m_size.cx, 0)
        ATTR_INT(L"height", m_size.cy, 0)
        ATTR_CUSTOM(L"mask", OnAttrMask)
        ATTR_RECT(L"margin" ,m_rcMargin,FALSE)          //æ≈π¨∏Ò4÷‹
    SOUI_ATTRS_END()

    //CAutoRefPtr<IBitmap> *  m_pBmpCaches;

    SStringW                m_strSkin;
    CAutoRefPtr<IBitmap>    m_bmpSkin;
    CAutoRefPtr<IBitmap>    m_bmpMask;
    CRect                   m_rcMargin;
    int                     m_iMaskChannel;
};

};