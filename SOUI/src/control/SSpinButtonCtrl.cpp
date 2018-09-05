#include "include\souistd.h"
#include "control\SSpinButtonCtrl.h"

static const wchar_t * KBTN_UP = L"btn_up";
static const wchar_t * KBTN_DOWN = L"btn_down";

namespace SOUI
{
    SSpinButtonCtrl::SSpinButtonCtrl(void)
    :m_btnUp(NULL)
    ,m_btnDown(NULL)
    ,m_nMin(0)
    ,m_nMax(100)
    ,m_nValue(0)
    ,m_uStep(1)
    ,m_bCircle(TRUE)
    {
        GetEventSet()->addEvent(EVENTID(EventSpinValue2String));
    }

    SSpinButtonCtrl::~SSpinButtonCtrl(void)
    {
    }

	void SSpinButtonCtrl::SetValue(int nValue)
	{
		m_nValue = nValue;
		OnValueChanged();
	}

	void SSpinButtonCtrl::SetRange(int nMin, int nMax)
	{
		m_nMin = nMin;
		m_nMax = nMax;
		if (m_nValue < m_nMin)
			m_nValue = m_nMin;
		if (m_nValue > nMax)
			m_nValue = nMax;
		OnValueChanged();
	}

	void SSpinButtonCtrl::SetStep(UINT nStep)
	{
		m_uStep = nStep;
	}

	SWindow * SSpinButtonCtrl::GetBuddy()
	{
		return GetParent()->FindChildByName(m_strBuddy);
	}

    BOOL SSpinButtonCtrl::CreateChildren(pugi::xml_node xmlNode)
    {
        if(!__super::CreateChildren(xmlNode))
            return FALSE;
        m_btnUp = FindChildByName(KBTN_UP);
        m_btnDown = FindChildByName(KBTN_DOWN);
        
        SASSERT(m_btnUp &&m_btnDown);
        
        if(!m_btnDown || !m_btnUp)
            return FALSE;
        m_btnUp->SetAttribute(L"focusable",L"0");
        m_btnDown->SetAttribute(L"focusable",L"0");
        m_btnUp->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&SSpinButtonCtrl::OnUpDownClick,this));
        m_btnDown->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&SSpinButtonCtrl::OnUpDownClick,this));
        
        OnValueChanged(true);
        return TRUE;        
    }

    CSize SSpinButtonCtrl::GetDesiredSize(LPCRECT pRcContainer)
    {
        CSize szRet;
        if(GetLayoutParam()->IsSpecifiedSize(Horz))
        {
            szRet.cx = GetLayoutParam()->GetSpecifiedSize(Horz).toPixelSize(GetScale());
        }else
        {
            szRet.cx = 16;
        }
        if(GetLayoutParam()->IsSpecifiedSize(Vert))
        {
            szRet.cy = GetLayoutParam()->GetSpecifiedSize(Vert).toPixelSize(GetScale());
        }else
        {
            szRet.cy = pRcContainer->bottom-pRcContainer->top;
        }

        return szRet;
    }

    void SSpinButtonCtrl::UpdateChildrenPosition()
    {
        CRect rcClient = GetClientRect();
        if(rcClient.IsRectEmpty()) return;
        rcClient.bottom = rcClient.top +rcClient.Height()/2;
        m_btnUp->Move(rcClient);
        rcClient.OffsetRect(0,rcClient.Height());
        m_btnDown->Move(rcClient);
    }

    bool SSpinButtonCtrl::OnUpDownClick(EventArgs *pEvt)
    {
        SASSERT(m_uStep < (UINT)abs(m_nMax-m_nMin));
        SASSERT(m_nValue>=m_nMin && m_nValue<=m_nMax);
        
        SWindow * sender = sobj_cast<SWindow>(pEvt->sender);
        SASSERT(sender);
        if(sender == m_btnUp)
        {
            m_nValue += m_uStep;
            if(m_nValue>m_nMax)
            {
                if(m_bCircle) m_nValue = m_nMin;
                else m_nValue = m_nMax;
            }
        }else //if(sender == m_btnDown)
        {
            m_nValue -= m_uStep;
            if(m_nValue<m_nMin)
            {
                if(m_bCircle) m_nValue = m_nMax;
                else m_nValue = m_nMin;
            }
        }
        OnValueChanged();
        return true;
    }

    HRESULT SSpinButtonCtrl::OnAttrValue(const SStringW& strValue, BOOL bLoading)
    {
        m_nValue = _wtoi(strValue);
        if(!bLoading)
        {
            OnValueChanged();
        }
        return S_OK;
    }

    void SSpinButtonCtrl::OnValueChanged(bool bInit)
    {
        SWindow *pBuddy = GetBuddy();
        if(pBuddy) 
        {
            EventSpinValue2String evt(this);
			evt.bInit=bInit;
            evt.nValue = m_nValue;
            evt.strValue = SStringT().Format(_T("%d"),m_nValue);
            FireEvent(evt);
            pBuddy->SetWindowText(evt.strValue);
        }
    }

}
