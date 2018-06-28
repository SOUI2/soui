#include "include\souistd.h"
#include "control\SSpinButton.h"
namespace SOUI
{

	SSpinButton::SSpinButton() 
		: m_nMinValue(0)
        , m_nMaxValue(100)
		, m_pBuddy(NULL)
		, m_pSpinSkin(NULL)
		, m_nHover(-1)
		, m_uStep(1)
		, m_bCircle(true)
		, m_iNextTime(0)
    {
		m_bFocusable = TRUE;
		m_pSpinSkin = GETSKIN(L"_skin.sys.btn.spin", 100);
    }

	SSpinButton::~SSpinButton()
    {

    }

	void SSpinButton::SetBuddy(SWindow* pBuddy)
	{
		m_pBuddy = pBuddy;
	}

	LRESULT SSpinButton::OnCreate(LPVOID)
    {
        if (0 != __super::OnCreate(NULL)) 
			return 1;
      
        return 0;
    }
	  
	void SSpinButton::OnShowWindow(BOOL bShow, UINT nStatus)
	{
		__super::OnShowWindow(bShow, nStatus);
		if (!bShow)
		{
			if (IsPlaying()) 
				GetContainer()->UnregisterTimelineHandler(this);
		}
		else
		{
			if (IsPlaying()) 
				GetContainer()->RegisterTimelineHandler(this);
			
		}
	}

	void SSpinButton::OnDestroy()
	{
		Stop();
	}

	void SSpinButton::OnPaint(IRenderTarget *pRT)
	{
		__super::OnPaint(pRT);

		if (NULL == m_pSpinSkin)
			return;
		
		CRect rcClient;
		GetClientRect(&rcClient);

		SIZE si = m_pSpinSkin->GetSkinSize();

		int nSepX = (rcClient.right - rcClient.left - si.cx) / 2;
		int nSepY = (rcClient.bottom - rcClient.top - si.cy) / 4;
		// ¼ÆËã
		CRect rc1(rcClient);
		rc1.left += nSepX;
		rc1.top += nSepY;
		rc1.right = rc1.left + si.cx;
		rc1.bottom = rc1.top + si.cy;

		CRect rc2(rcClient);
		rc2.left += nSepX;
		rc2.bottom -= nSepY;		
		rc2.right = rc2.left + si.cx;
		rc2.top = rc2.bottom - si.cy;

		int nState1 = 0;
		int nState2 = 0;
		if (1 == m_nHover)
		{
			nState1 = IIF_STATE4(GetState(), 0, 1, 2, 0);
		}
		else if(2 == m_nHover)
		{
			nState2 = IIF_STATE4(GetState(), 0, 1, 2, 0);
		}
			
		m_pSpinSkin->Draw(pRT, rc1, nState1);
		m_pSpinSkin->Draw(pRT, rc2, nState2+3);
		
	}

	int SSpinButton::HitTest(CPoint pt)
	{
		CRect rcClient;
		GetClientRect(&rcClient);

		// ¼ÆËã
		CRect rc1(rcClient);
		int nHover = (pt.y > (rcClient.bottom - rcClient.top) / 2 + rc1.top) ? 2 : 1;
		
		return nHover;
	}

	void SSpinButton::OnNextFrame()
	{
		if(m_bFirst)
		{
			if (m_iNextTime < 50)
			{
				++m_iNextTime;
				return;
			}
			m_iNextTime = 0;
			m_bFirst = false;
		}

		if (m_iNextTime > 5) 
			m_iNextTime = 0;

		if (m_iNextTime == 0 && IsPlaying())
			ChangeValue(m_nHover);

		++m_iNextTime;
	}

	void SSpinButton::OnLButtonDown(UINT nFlags, CPoint pt)
    {
		if (m_bFocusable) SetFocus();
		SetCapture();
		ModifyState(WndState_PushDown, 0, TRUE);

		m_nPush = HitTest(pt);
		ChangeValue(m_nPush);
		Start();
    }

    void SSpinButton::OnLButtonUp(UINT nFlags, CPoint pt)
    {
		SWindow::OnLButtonUp(nFlags, pt);
		
		Invalidate();
		KillFocus();
		Stop();
		m_nPush = -1;
    }
	   

	void SSpinButton::OnMouseMove(UINT nFlags, CPoint point)
	{
		int nHover = HitTest(point);

		if (nHover != m_nHover)
		{
			m_nHover = nHover;
			Invalidate();
		}
	}

	void SSpinButton::OnMouseLeave()
	{
		SWindow::OnMouseLeave();
		m_nHover = -1;
		Invalidate();
	}

	HRESULT SSpinButton::OnAttrBuddy(const SStringW& strValue, BOOL bLoading)
	{
		SWindow* pParent = this;
		while (true)
		{
			pParent = pParent->GetParent();
			if (NULL == pParent)
				break;

			m_pBuddy = pParent->FindChildByName(strValue);
			if(NULL != m_pBuddy)
				break;
		}
		
		return S_OK;
	}

	void SSpinButton::Start()
	{
		m_bFirst = true;
		m_iNextTime = 0;
		if (IsVisible(TRUE)) 
			GetContainer()->RegisterTimelineHandler(this);
	}

	void SSpinButton::Stop()
	{
		if (IsVisible(TRUE)) 
			GetContainer()->UnregisterTimelineHandler(this);
	}

	bool SSpinButton::IsPlaying()
	{
		if (m_dwState && WndState_PushDown && m_nPush == m_nHover)
			return true;

		return false;
	}

	void SSpinButton::ChangeValue(int nHover)
	{
		if (NULL == m_pBuddy)
		{
			EventSpinValue2String evt(this);
			evt.nValue = nHover;
			evt.strValue;
			FireEvent(evt);
			return;
		}

		SStringT sBuddyValue = m_pBuddy->GetWindowText(TRUE);
		int nValue = _ttoi(sBuddyValue);
		if (1 == nHover)
		{
			nValue += m_uStep;
			if (nValue > m_nMaxValue)
			{
				if (m_bCircle)
					nValue = m_nMinValue;
				else
					nValue = m_nMaxValue;
			}
		}
		else if (2 == nHover)
		{
			nValue -= m_uStep;
			if (nValue < m_nMinValue)
			{
				if (m_bCircle)
					nValue = m_nMaxValue;
				else
					nValue = m_nMinValue;
			}
		}

		EventSpinValue2String evt(this);
		evt.nValue = nValue;
		evt.strValue.Format(_T("%d"), nValue);
		FireEvent(evt);
		if (!evt.bubbleUp) return;

		m_pBuddy->SetWindowText(evt.strValue);
	}

}//namespace SOUI
