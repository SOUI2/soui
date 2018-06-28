#pragma once

namespace SOUI
{
    class SImageMaskWnd : public SImageWnd
    {
    SOUI_CLASS_NAME(SImageMaskWnd,L"imageMask")
    public:
        SImageMaskWnd(void);
        ~SImageMaskWnd(void);

    protected:
        void OnPaint(IRenderTarget *pRT);
        
        SOUI_MSG_MAP_BEGIN()
            MSG_WM_PAINT_EX(OnPaint)
        SOUI_MSG_MAP_END()
        
        HRESULT OnAttrMask(const SStringW & strValue,BOOL bLoading);
        HRESULT OnAttrImage(const SStringW & strValue,BOOL bLoading);
        
        void MakeCacheApha(ISkinObj *pSkin);
        
        SOUI_ATTRS_BEGIN()
            ATTR_CUSTOM(L"mask", OnAttrMask)//image.a
            ATTR_CUSTOM(L"skin", OnAttrImage)
        SOUI_ATTRS_END()
        
        SStringW                m_strSkin;
        CAutoRefPtr<IBitmap>    m_bmpCache;
        CAutoRefPtr<IBitmap>    m_bmpMask;
        int                     m_iMaskChannel;
    };
}
