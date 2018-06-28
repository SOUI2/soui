#include "StdAfx.h"
#include "SPropertyItem-Size.h"
#include "../SPropertyEmbedWndHelper.hpp"
#include "../SPropertyGrid.h"
#include <helper/SplitString.h>

#define CHILD_WIDTH     1
#define CHILD_HEIGHT    2

namespace SOUI
{
    SPropertyItemSize::SPropertyItemSize( SPropertyGrid *pOwner ) :SPropertyItemText(pOwner),m_bChildChanged(FALSE)
    {
        IPropertyItem *pWidth = SPropertyItemText::CreatePropItem(pOwner);
        pWidth->SetID(CHILD_WIDTH);
        pWidth->SetName(TR(L"width",GetOwner()->GetContainer()->GetTranslatorContext()));
        InsertChild(pWidth);
        pWidth->Release();
        IPropertyItem *pHeight = SPropertyItemText::CreatePropItem(pOwner);
        pHeight->SetID(CHILD_HEIGHT);
        pHeight->SetName(TR(L"height",GetOwner()->GetContainer()->GetTranslatorContext()));
        InsertChild(pHeight);
        pHeight->Release();
        m_szValue.cx=m_szValue.cy=0;
    }

    void SPropertyItemSize::SetValue( void *pValue)
    {
        m_szValue = *(SIZE*)pValue;
        OnValueChanged();
    }

    const void* SPropertyItemSize::GetValue() const
    {
        return &m_szValue;
    }

    void SPropertyItemSize::SetString( const SStringT & strValue )
    {
        SIZE sz;
        if(_stscanf(strValue,_T("%d,%d"),&sz.cx,&sz.cy)==2)
        {
            m_szValue = sz;
            OnValueChanged();
        }
    }

    void SPropertyItemSize::OnChildValueChanged( IPropertyItem *pChild )
    {
        if(pChild->GetID() == CHILD_WIDTH)
        {
            m_szValue.cx=_ttoi(pChild->GetString());
        }else if(pChild->GetID()==CHILD_HEIGHT)
        {
            m_szValue.cy=_ttoi(pChild->GetString());
        }
        m_bChildChanged=TRUE;
        OnValueChanged();
        m_bChildChanged=FALSE;
    }


    void SPropertyItemSize::OnValueChanged()
    {
        if(!m_bChildChanged)
        {
            IPropertyItem *pWid = GetItem(IPropertyItem::GPI_FIRSTCHILD);
            SASSERT(pWid && pWid->GetID()==CHILD_WIDTH);
            SStringT str;
            str.Format(_T("%d"),m_szValue.cx);
            pWid->SetString(str);
            IPropertyItem *pHei = GetItem(IPropertyItem::GPI_LASTCHILD);
            SASSERT(pHei && pHei->GetID()==CHILD_HEIGHT);
            str.Format(_T("%d"),m_szValue.cy);
            pHei->SetString(str);
        }
        __super::OnValueChanged();
    }

    HRESULT SPropertyItemSize::OnAttrValue( const SStringW & strValue,BOOL bLoading )
    {
        SetString(S_CW2T(strValue));
        return S_FALSE;
    }

    HRESULT SPropertyItemSize::OnAttrChildrenNames( const SStringW & strValue,BOOL bLoading )
    {
        SArray<SStringW> strNames;
        SplitString(TR(strValue,GetOwner()->GetContainer()->GetTranslatorContext()),L'|',strNames);
        if(strNames.GetCount()==2)
        {
            GetItem(GPI_FIRSTCHILD)->SetName(strNames[0]);
            GetItem(GPI_LASTCHILD)->SetName(strNames[1]);
        }
        return S_FALSE;
    }
}
