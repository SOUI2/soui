#include "StdAfx.h"
#include "SPropertyItem-Option.h"
#include "../SPropertyEmbedWndHelper.hpp"
#include "../SPropertyGrid.h"
#include <helper/SplitString.h>

namespace SOUI
{
    class SPropCombobox: public SComboBox
                   , public IPropInplaceWnd
    {
    public:
        SPropCombobox(IPropertyItem *pOwner):m_pOwner(pOwner)
        {
            SASSERT(m_pOwner);
        }
        
        void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
        {
            if(nChar == VK_RETURN)
            {
                GetParent()->SetFocus();
            }else
            {
                SComboBox::OnKeyDown(nChar,nRepCnt,nFlags); 
            }
        }
        
        SOUI_MSG_MAP_BEGIN()
            MSG_WM_KEYDOWN(OnKeyDown)
        SOUI_MSG_MAP_END()
        
        virtual IPropertyItem* GetOwner(){return m_pOwner;}
        
        virtual void UpdateData()
        {
            int nCurSel=GetCurSel();
            m_pOwner->SetValue(&nCurSel);
        }

    protected:
        CAutoRefPtr<IPropertyItem> m_pOwner;

    };
    
    void SPropertyItemOption::DrawItem( IRenderTarget *pRT,CRect rc )
    {
        SStringT strValue=GetString();
        pRT->DrawText(strValue,strValue.GetLength(),rc,DT_SINGLELINE|DT_VCENTER);
    }
    
    void SPropertyItemOption::OnInplaceActive(bool bActive)
    {
        __super::OnInplaceActive(bActive);
        if(bActive)
        {
            SASSERT(!m_pCombobox);
            m_pCombobox = new TplPropEmbedWnd<SPropCombobox>(this);
            wchar_t szXml[]=L"<combobox dropDown=\"1\" colorBkgnd=\"#ffffff\" drawFocusRect=\"0\">\
                <liststyle colorBorder=\"#000000\" margin-x=\"1\" margin-y=\"1\" colorText=\"#000000\" colorSelText=\"#FFFFFF\" colorItemBkgnd=\"#FFFFFF\" colorItemSelBkgnd=\"#000088\"/>\
                </combobox>";
            pugi::xml_document xmlDoc;
            xmlDoc.load_buffer(szXml,sizeof(szXml));
            pugi::xml_node xmlNode=xmlDoc.first_child();
            xmlNode.append_attribute(L"dropHeght").set_value(m_nDropHeight);
            xmlNode.append_attribute(L"dropDown").set_value(1);
            m_pOwner->OnInplaceActiveWndCreate(this,m_pCombobox,xmlNode);
            for(UINT i=0;i<m_options.GetCount();i++)
            {
                m_pCombobox->InsertItem(i,m_options[i],0,i);
            }
            m_pCombobox->SetCurSel(m_nValue);
        }else
        {
            if(m_pCombobox)
            {
                m_pOwner->OnInplaceActiveWndDestroy(this,m_pCombobox);
                m_pCombobox->Release();
                m_pCombobox = NULL;
            }
        }
    }

    void SPropertyItemOption::SetValue( void *pValue)
    {
        m_nValue = *(int*)pValue;
        OnValueChanged();
    }

    const void * SPropertyItemOption::GetValue() const
    {
        return &m_nValue;
    }

    SStringT SPropertyItemOption::GetString() const
    {
        if(m_nValue<0 || m_nValue>=(int)m_options.GetCount()) return _T("");
        return m_options[m_nValue];
    }

    void SPropertyItemOption::SetString( const SStringT & strValue )
    {
        m_nValue = _ttoi(strValue);
        OnValueChanged();
    }

    HRESULT SPropertyItemOption::OnAttrOptions( const SStringW & strValue,BOOL bLoading )
    {
        SStringT strValueT = S_CW2T(strValue);
        SplitString(strValueT,_T('|'),m_options);
        return S_FALSE;
    }

}
