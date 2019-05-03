#pragma once
#include <helper/SMatrix.h>
namespace SOUI
{
class SMatrixWindow : public SWindow
{
SOUI_CLASS_NAME(SMatrixWindow,L"matrixWindow")
public:
    SMatrixWindow(void);
    ~SMatrixWindow(void);
    
protected:
    void OnPaint(IRenderTarget *pRT);
    BOOL OnEraseBkgnd(IRenderTarget *pRT){return TRUE;}

    SOUI_MSG_MAP_BEGIN()
        MSG_WM_PAINT_EX(OnPaint)
        MSG_WM_ERASEBKGND_EX(OnEraseBkgnd)
    SOUI_MSG_MAP_END()
protected:
    HRESULT OnAttrRotate(const SStringW & strValue,BOOL bLoading);
    HRESULT OnAttrScale(const SStringW & strValue,BOOL bLoading);
    HRESULT OnAttrSkew(const SStringW & strValue,BOOL bLoading);
    HRESULT OnAttrTranslate(const SStringW & strValue,BOOL bLoading);
	HRESULT OnAttrPersp(const SStringW & strValue,BOOL bLoading);

    SOUI_ATTRS_BEGIN()
        ATTR_CUSTOM(L"rotate",OnAttrRotate)
        ATTR_CUSTOM(L"scale",OnAttrScale)
        ATTR_CUSTOM(L"skew",OnAttrSkew)
        ATTR_CUSTOM(L"translate",OnAttrTranslate)
		ATTR_CUSTOM(L"perspective",OnAttrPersp)
    SOUI_ATTRS_END()
    
    float m_fRotate;
    float m_fScaleX,m_fScaleY;
    float m_fSkewX, m_fSkewY;
    float m_fTransX, m_fTransY;
	float m_fPerspX, m_fPerspY;
};

}