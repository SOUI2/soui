#include "StdAfx.h"
#include "SPropertyItem-Color.h"
#include "../SPropertyEmbedWndHelper.hpp"
#include "../SPropertyGrid.h"
#include <commdlg.h>

const int KColorWidth   = 50;
const int KTransGridSize    =5;
namespace SOUI
{
    void SPropertyItemColor::DrawItem( IRenderTarget *pRT,CRect rc )
    {
        CRect rcColor = rc;
        rcColor.right = rcColor.left + KColorWidth;
        rcColor.DeflateRect(2,2);
        
        //画一个代表透明的网格背景
        pRT->PushClipRect(&rcColor);
        bool bDrawY=true;
        for(int y=rcColor.top;y<rcColor.bottom;y+=KTransGridSize)
        {
            bool bDraw= bDrawY;
            for(int x=rcColor.left;x<rcColor.right;x+=KTransGridSize)
            {
                if(bDraw) pRT->FillSolidRect(CRect(CPoint(x,y),CSize(KTransGridSize,KTransGridSize)),RGBA(0xcc,0xcc,0xcc,0xff));
                bDraw=!bDraw;
            }
            bDrawY=!bDrawY;
        }
        pRT->PopClip();
        
        pRT->FillSolidRect(&rcColor,m_crValue);
        pRT->DrawRectangle(&rcColor);
        CRect rcValue = rc;
        rcValue.left += KColorWidth;
        SStringT strValue = GetString();
        pRT->DrawText(strValue,strValue.GetLength(),&rcValue,DT_SINGLELINE|DT_VCENTER);
    }
    
    void SPropertyItemColor::OnInplaceActive(bool bActive)
    {
        SPropertyItemText::OnInplaceActive(bActive);
        if(bActive)
        {
            LRESULT lr=m_pEdit->SSendMessage(EM_SETEVENTMASK,0,ENM_CHANGE);
            m_pEdit->GetEventSet()->subscribeEvent(EventRENotify::EventID,Subscriber(&SPropertyItemColor::OnReNotify,this));
        }
    }

    bool SPropertyItemColor::OnReNotify(EventArgs *pEvt)
    {
        EventRENotify *pReEvt = (EventRENotify*)pEvt;
        if(pReEvt->iNotify == EN_CHANGE)
        {
            SStringT strValue=m_pEdit->GetWindowText();
            int r,g,b,a;
            int nGet=_stscanf(strValue,m_strFormat,&r,&g,&b,&a);
            if(nGet==4)
            {
                m_crValue = RGBA(r,g,b,a);
                CRect rcColor;
                m_pEdit->GetWindowRect(&rcColor);
                rcColor.right=rcColor.left;
                rcColor.left -= KColorWidth;
                m_pOwner->InvalidateRect(rcColor);
            }
        }
        return true;
    }
    
    void SPropertyItemColor::SetValue( void *pValue)
    {
        m_crValue = *(COLORREF*)pValue;
        OnValueChanged();
    }

    const void* SPropertyItemColor::GetValue()
    {
        return &m_crValue;
    }

    void SPropertyItemColor::SetString( const SStringT & strValue )
    {
        int r,g,b,a;
        if(_stscanf(strValue,m_strFormat,&r,&g,&b,&a)==4)
        {
            m_crValue = RGBA(r,g,b,a);
            OnValueChanged();
        }
    }

    void SPropertyItemColor::OnButtonClick()
    {
        CHOOSECOLOR cc;                 // common dialog box structure 
        static COLORREF acrCustClr[16]; // array of custom colors 

        // Initialize CHOOSECOLOR 
        ZeroMemory(&cc, sizeof(cc));
        cc.lStructSize = sizeof(cc);
        cc.hwndOwner = GetOwner()->GetContainer()->GetHostHwnd();
        cc.lpCustColors = (LPDWORD) acrCustClr;
        cc.rgbResult = m_crValue;
        cc.Flags = CC_FULLOPEN | CC_RGBINIT;
        
        if (ChooseColor(&cc))
        {
            m_crValue = cc.rgbResult|0xff000000;
            OnValueChanged();
            CRect rc=GetOwner()->GetItemRect(this);
            GetOwner()->InvalidateRect(&rc);
        }
    }

    void SPropertyItemColor::AdjustInplaceActiveWndRect( CRect & rc )
    {
        __super::AdjustInplaceActiveWndRect(rc);
        rc.left += KColorWidth;
    }

}
