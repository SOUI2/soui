#include "souistd.h"
#include "Stabctrl2.h"
#include "SAnimImg.h"

namespace SOUI
{
    SPageAniObj::SPageAniObj() :m_aniStatus(STATUS_NOR)
        ,m_iFrame(0)
        ,m_tmWaitFor(0)
        ,m_pAniIn(NULL)
        ,m_pAniOut(NULL)
    {

    }

    BOOL SPageAniObj::Draw(IRenderTarget *pRT,const CRect &rc)
    {
        if(m_aniStatus == STATUS_IN)
        {
            if(m_pAniIn)
            {
                m_pAniIn->Draw(pRT,rc, m_iFrame);
                return TRUE;
            }
        }else if(m_aniStatus == STATUS_OUT)
        {
            if(m_pAniOut) 
            {
                m_pAniOut->Draw(pRT,rc, m_iFrame);
                return TRUE;
            }
        }
        return FALSE;
    }

    BOOL SPageAniObj::OnNextFrame(int nIntervel)
    {
        if(m_aniStatus == STATUS_NOR) return FALSE;
        m_tmWaitFor -= 10;
        if(m_tmWaitFor>0) return FALSE;

        m_iFrame++;
        if(m_aniStatus == STATUS_IN)
        {
            if(!m_pAniIn)
                m_aniStatus = STATUS_NOR;
            else if(m_iFrame>=m_pAniIn->GetStates() - 1)
            {
                m_iFrame = m_pAniIn->GetStates() - 1;
            }
        }else
        {
            if(!m_pAniOut || m_iFrame>=m_pAniOut->GetStates() -1  )
            {
                m_aniStatus = STATUS_NOR;
            }
        }
        m_tmWaitFor = nIntervel;
        return TRUE;
    }

    void SPageAniObj::AnimateOut(int nInterval)
    {
        m_aniStatus = STATUS_OUT;
        m_iFrame = 0;
        m_tmWaitFor = nInterval;
    }

    void SPageAniObj::AnimateIn(int nInterval)
    {
        m_aniStatus = STATUS_IN;
        m_iFrame = 0;
        m_tmWaitFor = nInterval;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

	STabPage2::STabPage2(void)
	{

	}

	STabPage2::~STabPage2(void)
	{

	}


	STabCtrl2::STabCtrl2(void):
	m_pSkinHeadBg(NULL),
        m_iInterval(50)
	{
	}

	STabCtrl2::~STabCtrl2(void)
	{

	}
 
	void STabCtrl2::OnPaint(IRenderTarget *pRT)
	{
		SPainter painter;
		BeforePaint(pRT,painter);

		CRect rcItem,rcItemPrev,rcItem2Draw;
		CRect rcSplit;
		DWORD dwState;
		CRect rcTitle=GetTitleRect();
 

		pRT->PushClipRect(&rcTitle,RGN_AND);
		if(m_pSkinHeadBg)
		{
			m_pSkinHeadBg->Draw(pRT,rcTitle,WndState_Normal);
		}
		for(size_t i=0; i<GetItemCount(); i++)
		{
			dwState=WndState_Normal;
			if(i == m_nCurrentPage) dwState=WndState_PushDown;
			else if(i== m_nHoverTabItem) dwState=WndState_Hover;

			GetItemRect(i,rcItem);
			//画分隔线
			if(i>0 && m_pSkinTabInter && ((i < (m_nCurrentPage)) || (i > (m_nCurrentPage+1))/*选中页左右不用画分隔线*/))
			{ 
				rcSplit=rcItem;
				if(m_nTabAlign==AlignLeft)
				{ 
					rcSplit.top=rcItemPrev.bottom;
					rcSplit.bottom = rcSplit.top + m_nTabInterSize.toPixelSize(GetScale());
				}
				else
				{
					rcSplit.left=rcItemPrev.right;
					rcSplit.right=rcSplit.left + m_nTabInterSize.toPixelSize(GetScale());
				}
				m_pSkinTabInter->Draw(pRT,rcSplit,0);
			}
			rcItemPrev=rcItem;
			if (dwState == WndState_PushDown && m_pSkinTabInter)
			{//如果有分割线，这种状态覆盖掉(模仿360的某个subtab)
				if(m_nTabAlign ==AlignLeft)
				{
					rcItem.top -= m_nTabInterSize.toPixelSize(GetScale());
					rcItem.bottom += m_nTabInterSize.toPixelSize(GetScale());
				}
				else
				{
					rcItem.left -= m_nTabInterSize.toPixelSize(GetScale());
					rcItem.right += m_nTabInterSize.toPixelSize(GetScale());
				}
			}
			DrawItem(pRT,rcItem,i,dwState);
		}
		pRT->PopClip();

		if (m_pSkinFrame)
		{
			CRect rcPage = GetChildrenLayoutRect();
			m_pSkinFrame->Draw(pRT, rcPage, WndState_Normal);
		}

		if(IsFocused() && IsFocusable() && m_bDrawFocusRect)
		{
			CRect rc;
			GetItemRect(m_nCurrentPage,rc);
			rc.DeflateRect(2,2);
			DrawDefFocusRect(pRT,&rc);
		}
		AfterPaint(pRT,painter);
 
		
	}

	void STabCtrl2::DrawItem(IRenderTarget *pRT,const CRect &rcItem,int iItem,DWORD dwState )
	{
		if(rcItem.IsRectEmpty()) return;
		int iState = IIF_STATE3(dwState,WndState_Normal,WndState_Hover,WndState_PushDown);
		if(m_pSkinTab)
			m_pSkinTab->Draw(pRT,rcItem,iState);

		//根据状态从style中获得字体，颜色
		IFontPtr font=m_style.GetTextFont(iState);
		COLORREF crTxt = m_style.GetTextColor(iState);
		CAutoRefPtr<IFont> oldFont;
		if(font) pRT->SelectObject(font,(IRenderObj**)&oldFont);
		COLORREF crOld = 0;
		if(crTxt != CR_INVALID) crOld = pRT->SetTextColor(crTxt);
 
		STabPage2 *pPage = sobj_cast<STabPage2>(m_lstPages.GetAt(iItem));

		CRect rcIcon(rcItem.left+m_ptIcon[0].toPixelSize(GetScale()),
			rcItem.top+m_ptIcon[1].toPixelSize(GetScale()),0,0);
		if(m_pSkinIcon)
		{
			rcIcon.right = rcIcon.left+m_pSkinIcon->GetSkinSize().cx;
			rcIcon.bottom = rcIcon.top+m_pSkinIcon->GetSkinSize().cy;
		}

        if(!pPage->m_aniObj.Draw(pRT,rcIcon) && m_pSkinIcon)
		{
			int iIcon=GetItem(iItem)->GetIconIndex();
			if(iIcon == -1) iIcon = iItem;
			m_pSkinIcon->Draw(pRT,rcIcon,iIcon);
		}

		if(!m_ptText[0].valueEqual(-1.f) && !m_ptText[1].valueEqual(-1.f))
		{//从指定位置开始绘制文字
			pRT->TextOut(rcItem.left+m_ptText[0].toPixelSize(GetScale()),rcItem.top+m_ptText[1].toPixelSize(GetScale()),GetItem(iItem)->GetTitle(),-1);
		}
		else
		{
			CRect rcText=rcItem;
			UINT alignStyle=m_style.GetTextAlign();
			UINT align=alignStyle;
			if(m_ptText[0].valueEqual(-1.f) && !m_ptText[1].valueEqual(-1.f))
			{//指定了Y偏移，X居中
				rcText.top+=m_ptText[1].toPixelSize(GetScale());
				align=alignStyle&(DT_CENTER|DT_RIGHT|DT_SINGLELINE|DT_END_ELLIPSIS);
			}
			else if(!m_ptText[0].valueEqual(-1.f) && m_ptText[1].valueEqual(-1.f))
			{//指定了X偏移，Y居中
				rcText.left+=m_ptText[0].toPixelSize(GetScale());
				align=alignStyle&(DT_VCENTER|DT_BOTTOM|DT_SINGLELINE|DT_END_ELLIPSIS);
			}

			pRT->DrawText(GetItem(iItem)->GetTitle(),-1,&rcText,align);
		}

		//恢复字体，颜色
		if(font) pRT->SelectObject(oldFont);
		if(crTxt!=CR_INVALID) pRT->SetTextColor(crOld);
	}

	void STabCtrl2::OnStateChanged( DWORD dwOldState,DWORD dwNewState )
	{
		InvalidateRect(GetTitleRect());
		__super::OnStateChanged(dwOldState, dwNewState);
	}
  
	void STabCtrl2::OnNextFrame()
	{
		for (int i = 0; i < m_lstPages.GetCount(); i ++)
		{
			STabPage2 *pPage = sobj_cast<STabPage2>(m_lstPages.GetAt(i));
            if(pPage->m_aniObj.OnNextFrame(m_iInterval))
            {
                CRect rc;
                GetItemRect(i,rc);
                InvalidateRect(rc);
            }
		}
	}

	void STabCtrl2::OnDestroy()
	{
        GetContainer()->UnregisterTimelineHandler(this);		
        __super::OnDestroy();        
	}

	int STabCtrl2::OnCreate(void*)
	{
		int iRet = __super::OnCreate(NULL);
		GetContainer()->RegisterTimelineHandler(this);		
		GetEventSet()->subscribeEvent(EventTabItemHover::EventID,Subscriber(&STabCtrl2::OnItemHover,this));
		GetEventSet()->subscribeEvent(EventTabItemLeave::EventID,Subscriber(&STabCtrl2::OnItemLeave,this));
		return iRet;
	}

	bool STabCtrl2::OnItemHover(EventArgs *pEvt)
	{
		bool bRet = true;
		EventTabItemHover *pEvt2 = sobj_cast<EventTabItemHover>(pEvt);
		STabPage2 *pPage = sobj_cast<STabPage2>(m_lstPages.GetAt(pEvt2->iHover));
        pPage->m_aniObj.AnimateIn(m_iInterval);
        CRect rc;
        GetItemRect(pEvt2->iHover,rc);
        InvalidateRect(rc);
		return bRet;
	}

	bool STabCtrl2::OnItemLeave(EventArgs *pEvt)
	{
		bool bRet = true;
		EventTabItemLeave *pEvt2 = sobj_cast<EventTabItemLeave>(pEvt);
		STabPage2 *pPage = sobj_cast<STabPage2>(m_lstPages.GetAt(pEvt2->iLeave));
        pPage->m_aniObj.AnimateOut(m_iInterval);
        CRect rc;
        GetItemRect(pEvt2->iLeave,rc);
        InvalidateRect(rc);

		return bRet;
	}

    STabPage * STabCtrl2::CreatePageFromXml(pugi::xml_node xmlPage)
    {
        if (wcscmp(xmlPage.name(),STabPage2::GetClassName()) != 0) return NULL;
        return (STabPage *)SApplication::getSingleton().CreateWindowByName(STabPage2::GetClassName());
    }
}
 