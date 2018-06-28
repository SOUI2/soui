#pragma once

#include "../SPropertyItemBase.h"

namespace SOUI
{
    class SPropertyItemText : public SPropertyItemBase
    {
        SOUI_CLASS_NAME(SPropertyItemText,L"proptext")
    public:
        virtual void DrawItem(IRenderTarget *pRT,CRect rc);
        
        virtual void SetValue(void *pValue);
        virtual const void* GetValue(){return (LPCTSTR)m_strValue;}
        virtual void SetString(const SStringT & strValue);
        virtual SStringT GetString() const {return m_strValue;}
       
        SOUI_ATTRS_BEGIN()
            ATTR_STRINGT(L"value",m_strValue,TRUE)
        SOUI_ATTRS_END()

    protected:
        virtual void OnInplaceActive(bool bActive);
    protected:
        SStringT m_strValue;
        
        SEdit  * m_pEdit;
    public:
        static IPropertyItem * CreatePropItem(SPropertyGrid *pOwner)
        {
            return new SPropertyItemText(pOwner);
        }
    protected:
        SPropertyItemText(SPropertyGrid *pOwner):SPropertyItemBase(pOwner),m_pEdit(NULL)
        {
        }
    };
}
