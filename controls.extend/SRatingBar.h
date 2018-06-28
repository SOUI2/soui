#pragma once

namespace SOUI
{
    class SRatingBar : public SWindow
    {
    SOUI_CLASS_NAME(SRatingBar,L"ratingbar")
    public:
        SRatingBar(void);
        ~SRatingBar(void);
        
        void SetValue(float fValue);
    protected:
        void DrawStars(IRenderTarget *pRT,CRect rc,BOOL bForeground);
        
        virtual CSize GetDesiredSize(LPCRECT pRcContainer);
        
        void OnPaint(IRenderTarget *pRT);
        
        SOUI_MSG_MAP_BEGIN()
            MSG_WM_PAINT_EX(OnPaint)
        SOUI_MSG_MAP_END()
        
        SOUI_ATTRS_BEGIN()
            ATTR_SKIN(L"starSkin",m_pStar,TRUE)
            ATTR_INT(L"starNum",m_nStars,TRUE)
            ATTR_FLOAT(L"value",m_fValue,TRUE)
        SOUI_ATTRS_END()
        
    protected:
        ISkinObj * m_pStar;
        int     m_nStars;
        float   m_fValue;
    };

}
