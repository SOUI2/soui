#pragma once
#include "core/SWnd.h"

namespace SOUI
{
    class SOUI_EXP SSpinButtonCtrl : public SWindow
    {
    SOUI_CLASS_NAME(SSpinButtonCtrl,L"spinButton")
    public:
        SSpinButtonCtrl(void);
        ~SSpinButtonCtrl(void);
        
		void SetValue(int nValue);
		void SetRange(int nMin, int nMax);
		void SetStep(UINT nStep);

		SWindow * GetBuddy();
    protected:
        virtual CSize GetDesiredSize(LPCRECT pRcContainer);
        virtual void UpdateChildrenPosition();
        virtual BOOL CreateChildren(pugi::xml_node xmlNode);
    protected:
        void OnValueChanged(bool bInit=false);
    
        bool OnUpDownClick(EventArgs *pEvt);
    protected:
        HRESULT OnAttrValue(const SStringW& strValue, BOOL bLoading);
        
        SOUI_ATTRS_BEGIN()
            ATTR_INT(L"max",m_nMax,FALSE)
            ATTR_INT(L"min",m_nMin,FALSE)
            ATTR_CUSTOM(L"value",OnAttrValue)
            ATTR_UINT(L"step",m_uStep,FALSE)
            ATTR_INT(L"circle",m_bCircle,FALSE)
            ATTR_STRINGW(L"buddy",m_strBuddy,FALSE)
        SOUI_ATTRS_END()
        
        int m_nMax;
        int m_nMin;
        int m_nValue;
        UINT m_uStep;
        BOOL m_bCircle;
        SStringW m_strBuddy;
        
        SWindow * m_btnUp;
        SWindow * m_btnDown;
    };
}
