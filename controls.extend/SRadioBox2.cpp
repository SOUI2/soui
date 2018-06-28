#include "stdafx.h"
#include "SRadioBox2.h"

namespace SOUI
{
    SRadioBox2::SRadioBox2(void):m_pIcon(NULL),m_nTextOffset(0)
    {
    }

    SRadioBox2::~SRadioBox2(void)
    {
    }

    void SRadioBox2::OnPaint(IRenderTarget *pRT)
    {
        if(m_pSkin)
        {
            CRect rc;
            GetClientRect(&rc);
            int nState = 0;
            if(GetState() & WndState_Check) nState = 2;
            else if(GetState() & WndState_PushDown) nState = 2;
            else if(GetState() & WndState_Hover) nState = 1;
            
            m_pSkin->Draw(pRT,rc,nState);
        }
        if(m_pIcon)
        {
            CRect rcIcon=GetClientRect();
            rcIcon.OffsetRect(m_ptIcon);
            rcIcon=CRect(rcIcon.TopLeft(),m_pIcon->GetSkinSize());
            m_pIcon->Draw(pRT,rcIcon,IIF_STATE3(GetState(),0,1,2));
        }
        SWindow::OnPaint(pRT);
    }

}
