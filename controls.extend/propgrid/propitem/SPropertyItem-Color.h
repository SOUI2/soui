#pragma once

#include "SPropertyItem-Text.h"

namespace SOUI
{
    class SPropertyItemColor : public SPropertyItemText
    {
        friend class SPropColorEdit;
        SOUI_CLASS_NAME(SPropertyItemColor,L"propcolor")
    public:
        virtual BOOL HasButton() const {return TRUE;}
        virtual void DrawItem(IRenderTarget *pRT,CRect rc);
        virtual void AdjustInplaceActiveWndRect(CRect & rc);
        
        virtual void SetValue(void *pValue);
        virtual const void* GetValue();
        virtual void SetString(const SStringT & strValue);
        
        virtual SStringT GetString() const {
            SStringT str;
            int r,g,b,a;
            r = GetRValue(m_crValue);
            g = GetGValue(m_crValue);
            b = GetBValue(m_crValue);
            a = GetAValue(m_crValue);
            str.Format(m_strFormat,r,g,b,a);
            return str;
        }
        
        SOUI_ATTRS_BEGIN()
            ATTR_STRINGT(L"format",m_strFormat,TRUE)
            ATTR_COLOR(L"value",m_crValue,TRUE)
        SOUI_ATTRS_END()

    protected:
        virtual void OnInplaceActive(bool bActive);
        virtual void OnButtonClick();
    protected:
        SStringT m_strFormat;
        COLORREF    m_crValue;
        
    public:
        static IPropertyItem * CreatePropItem(SPropertyGrid *pOwner)
        {
            return new SPropertyItemColor(pOwner);
        }
    protected:
        bool OnReNotify(EventArgs *pEvt);

        SPropertyItemColor(SPropertyGrid *pOwner):SPropertyItemText(pOwner)
        {
            m_strFormat = _T("RGB(%d,%d,%d,%d)");
        }
    };
}
