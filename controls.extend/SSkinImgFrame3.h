#pragma once
#include <core/sskin.h>

namespace SOUI
{
    /*usage
    <imgframe2 src="imgx:png_test{0,0,100,100}" xxx/>
    */
    class SSkinImgFrame3 : public SSkinImgFrame
    {
    SOUI_CLASS_NAME(SSkinImgFrame3,L"imgframe3")
    public:
        SSkinImgFrame3(void);
        ~SSkinImgFrame3(void);
        
        virtual SIZE GetSkinSize();
        
        virtual bool SetImage(IBitmap *pImg){return false;}
        
        SOUI_ATTRS_BEGIN()
            ATTR_CUSTOM(L"src",OnAttrSrc)
            /* offcut-left/right/top/bottom 因为组合图往往子图之间有空白 */
            /* 这里定义对应的子图图片切割掉上下左右多少，防止切割出来的图片有无用的边沿   */
            ATTR_RECT(L"offcut",m_offset,FALSE)
        SOUI_ATTRS_END()
        
    protected:
        HRESULT OnAttrSrc(const SStringW & strValue,BOOL bLoading);

        virtual void _Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha);

        CRect m_rcImg;
        CRect m_offset;
        SStringW m_strImgKey;
    };

}
