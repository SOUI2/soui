#include "stdafx.h"
#include "SButtonEx.h"

namespace SOUI
{
    SButtonEx::SButtonEx(void):m_ptPushOffet(2,2),m_drawMode(HORZ_ICON_TEXT),m_nSepSize(5),m_hIcon(0),m_nIconSize(32),m_iIcon(0),m_bIconVisible(true),m_pIcon(NULL)
    {
        m_bClipClient = TRUE;
    }

    SButtonEx::~SButtonEx(void)
    {
        if(m_hIcon) DestroyIcon(m_hIcon);
    }

    CSize SButtonEx::GetDesiredSize(LPCRECT pRcContainer)
    {
        CAutoRefPtr<IRenderTarget> pRT;
        GETRENDERFACTORY->CreateRenderTarget(&pRT,0,0);
        BeforePaintEx(pRT);
        return GetDesiredSize(pRT,pRcContainer);
    }

    CSize SButtonEx::GetDesiredSize(IRenderTarget *pRT,LPCRECT pRcContainer)
    {
        SASSERT(m_pIcon);
        CSize szIcon = GetIconSize();
        CSize szText;
        pRT->MeasureText(GetWindowText(),GetWindowText().GetLength(),&szText);

        CSize szRet;
        switch(m_drawMode)
        {
        case FREE_DRAW:
            szRet.cx = max(szIcon.cx+m_ptIcon.x, szText.cx+m_ptText.x);
            szRet.cy = max(szIcon.cy+m_ptIcon.y, szText.cy+m_ptText.y);
            break;
        case VERT_ICON_TEXT:
        case VERT_TEXT_ICON:
            szRet.cx = max(szIcon.cx,szText.cx);
            szRet.cy = szIcon.cy + szText.cy + m_nSepSize;
            break;
        case HORZ_ICON_TEXT:
        case HORZ_TEXT_ICON:
            szRet.cy = max(szIcon.cy,szText.cy);
            szRet.cx = szIcon.cx + szText.cx + m_nSepSize;
            break;
        default:
            SASSERT(FALSE);
            break;
        }
		CRect rcPadding = GetStyle().GetPadding();
        szRet.cx += rcPadding.left + rcPadding.right;
        szRet.cy += rcPadding.top + rcPadding.bottom;
        return szRet;
    }

    void SButtonEx::OnPaint(IRenderTarget *pRT)
    {
        SPainter painter;
        BeforePaint(pRT,painter);
        
        CRect rcClient;
        GetClientRect(&rcClient);

        //draw background, copy from SButton
        if(m_pBgSkin)
        {
            if(m_byAlphaAni==0xFF)
            {//不在动画过程中
                m_pBgSkin->Draw(
                    pRT, rcClient,
                    IIF_STATE4(GetState(), 0, 1, 2, 3)
                    );
            }
            else
            {//在动画过程中
                BYTE byNewAlpha=(BYTE)(((UINT)m_byAlphaAni*m_pBgSkin->GetAlpha())>>8);
                if(GetState()&WndState_Hover)
                {
                    //get hover
                    m_pBgSkin->Draw(pRT, rcClient, 0, m_pBgSkin->GetAlpha());
                    m_pBgSkin->Draw(pRT, rcClient, 1, byNewAlpha);
                }
                else
                {
                    //lose hover
                    m_pBgSkin->Draw(pRT, rcClient,0, m_pBgSkin->GetAlpha());
                    m_pBgSkin->Draw(pRT, rcClient, 1, m_pBgSkin->GetAlpha()-byNewAlpha);
                }
            }
        }
        
        if(GetState() & (WndState_PushDown|WndState_Check))
        {
            rcClient.OffsetRect(m_ptPushOffet);
        }
		CRect rcPadding = GetStyle().GetPadding();

        rcClient.DeflateRect(rcPadding);
        
        //draw icon and text
        CPoint ptIcon = rcClient.TopLeft(),ptText=ptIcon;
        CSize szIcon = GetIconSize();
        CSize szDesired = GetDesiredSize(pRT,&rcClient);
        szDesired.cx -= rcPadding.left + rcPadding.right;
        szDesired.cy -= rcPadding.top + rcPadding.bottom;
        
        CSize szText;
        pRT->MeasureText(GetWindowText(),GetWindowText().GetLength(),&szText);
        
        switch(m_drawMode)
        {
        case FREE_DRAW:
            ptIcon += m_ptIcon;
            DrawIcon(pRT,CRect(ptIcon,szIcon));
            ptText += m_ptText;
            pRT->TextOut(ptText.x,ptText.y,GetWindowText(),GetWindowText().GetLength());
            break;
        case VERT_ICON_TEXT:
            ptIcon.x += (rcClient.Width()-szIcon.cx)/2;
            ptIcon.y += (rcClient.Height()-szDesired.cy)/2;
            DrawIcon(pRT,CRect(ptIcon,szIcon));
            ptText.x += (rcClient.Width()-szText.cx)/2;
            ptText.y = ptIcon.y + szIcon.cy + m_nSepSize;
            pRT->TextOut(ptText.x,ptText.y,GetWindowText(),GetWindowText().GetLength());
            break;
        case VERT_TEXT_ICON:
            ptText.x += (rcClient.Width()-szText.cx)/2;
            ptText.y += (rcClient.Height()-szDesired.cy)/2;
            pRT->TextOut(ptText.x,ptText.y,GetWindowText(),GetWindowText().GetLength());
            ptIcon.x += (rcClient.Width()-szIcon.cx)/2;
            ptIcon.y = ptText.y + szText.cy + m_nSepSize;
            DrawIcon(pRT,CRect(ptIcon,szIcon));
            break;
        case HORZ_ICON_TEXT:
            ptIcon.x += (rcClient.Width()-szDesired.cx)/2;
            ptIcon.y += (rcClient.Height()-szIcon.cy)/2;
            DrawIcon(pRT,CRect(ptIcon,szIcon));
            ptText.x = ptIcon.x + szIcon.cx + m_nSepSize;
            ptText.y += (rcClient.Height()-szText.cy)/2;
            pRT->TextOut(ptText.x,ptText.y,GetWindowText(),GetWindowText().GetLength());
            break;
        case HORZ_TEXT_ICON:
            ptText.x += (rcClient.Width()-szDesired.cx)/2;
            ptText.y += (rcClient.Height()-szText.cy)/2;
            pRT->TextOut(ptText.x,ptText.y,GetWindowText(),GetWindowText().GetLength());
            ptIcon.x = ptText.x + szText.cx + m_nSepSize;
            ptIcon.y += (rcClient.Height()-szIcon.cy)/2;
            DrawIcon(pRT,CRect(ptIcon,szIcon));
            break;
        }
        
        AfterPaint(pRT,painter);
    }

    HICON SButtonEx::SetIcon(HICON hIcon,int nIconSize)
    {
        HICON hRet = m_hIcon;
        m_hIcon = hIcon;
        m_nIconSize = nIconSize;
        return hRet;
    }

    CSize SButtonEx::GetIconSize()
    {
        if(m_hIcon) return CSize(m_nIconSize,m_nIconSize);
        else return m_pIcon->GetSkinSize();
    }

    void SButtonEx::DrawIcon(IRenderTarget *pRT,CRect rcIcon)
    {
        if(!m_bIconVisible) return;
        if(m_hIcon)
        {
            pRT->DrawIconEx(rcIcon.left,rcIcon.top,m_hIcon,rcIcon.Width(),rcIcon.Height(),DI_NORMAL);
        }else
        {
            int iState = m_iIcon!=-1?m_iIcon:IIF_STATE4(GetState(),0,1,2,3);
            m_pIcon->Draw(pRT,rcIcon,iState);
        }
    }

    void SButtonEx::SetIconVisible(bool bVisible)
    {
        m_bIconVisible = bVisible;
        Invalidate();
    }

}
