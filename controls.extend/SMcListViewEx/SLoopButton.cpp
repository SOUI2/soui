#include "stdafx.h"
#include "SLoopButton.h"

SLoopButton::SLoopButton():m_iSkinStates(1),m_iCurState(0),m_iState(4)
{
	m_evtSet.addEvent(EVENTID(EventBtnSelChanging));
	m_evtSet.addEvent(EVENTID(EventBtnSelChanged));
}

SLoopButton::~SLoopButton()
{
}

void SLoopButton::OnStateChanged(DWORD dwOldState, DWORD dwNewState)
{
	SWindow::OnStateChanged(dwOldState, dwNewState);
	StopCurAnimate();

	if (GetCapture() == m_swnd||m_iSkinStates<2)    //点击中
		return;

	if (m_bAnimate &&
		((dwOldState == WndState_Normal && dwNewState == WndState_Hover)
			|| (dwOldState == WndState_Hover && dwNewState == WndState_Normal)))
	{//启动动画
		m_byAlphaAni = 5;
		GetContainer()->RegisterTimelineHandler(this);
	}
}

void SLoopButton::OnPaint(IRenderTarget * pRT)
{
	if (!m_pBgSkin) return;
	CRect rcClient;
	GetClientRect(&rcClient);

	if (m_byAlphaAni == 0xFF)
	{//不在动画过程中
		m_pBgSkin->Draw(
			pRT, rcClient,
			GetSkinState()
		);
	}
	else
	{//在动画过程中
		BYTE byNewAlpha = (BYTE)(((UINT)m_byAlphaAni*m_pBgSkin->GetAlpha()) >> 8);
		if (GetState()&WndState_Hover)
		{
			//get hover
			m_pBgSkin->Draw(pRT, rcClient, (m_iCurState*m_iSkinStates)+ 0, m_pBgSkin->GetAlpha());
			m_pBgSkin->Draw(pRT, rcClient, (m_iCurState*m_iSkinStates)+1, byNewAlpha);
		}
		else
		{
			//lose hover
			m_pBgSkin->Draw(pRT, rcClient, (m_iCurState*m_iSkinStates)+0, m_pBgSkin->GetAlpha());
			m_pBgSkin->Draw(pRT, rcClient, (m_iCurState*m_iSkinStates)+1, m_pBgSkin->GetAlpha() - byNewAlpha);
		}
	}
	SWindow::OnPaint(pRT);
}

void SLoopButton::OnLButtonUp(UINT nFlags, CPoint pt)
{
	EventBtnSelChanging evt(this);	
	evt.iOldSel =m_iCurState ;
	evt.iNewSel = GetNext();
	FireEvent(evt);
	if (evt.bCancel)
	{
		__super::OnLButtonUp(nFlags, pt);
		return;
	}
	Next();
	if (m_iSkinStates < 2 && m_iState>1)
		Invalidate();
	__super::OnLButtonUp(nFlags, pt);
}
