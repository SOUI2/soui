#include "stdafx.h"
#include "STabCtrlHeaderBinder.h"
using namespace SOUI;

STabCtrlHeaderBinder::STabCtrlHeaderBinder(STabCtrl * _host) :m_pTabHost(_host)
{
	if (m_pTabHost)
	{
		m_pTabHost->GetEventSet()->subscribeEvent<STabCtrlHeaderBinder,EventTabSelChanged>(&STabCtrlHeaderBinder::TabSelChanged,this);
	}
}

STabCtrlHeaderBinder::~STabCtrlHeaderBinder()
{
}

void STabCtrlHeaderBinder::Bind(SWindow * _pWnd, int _idx, DWORD _evtcode)
{
	if (_pWnd->GetEventSet()->subscribeEvent(_evtcode, Subscriber(&STabCtrlHeaderBinder::EvtHander, this)))
		m_lstPages[_pWnd] = _idx;
}

bool STabCtrlHeaderBinder::EvtHander(EventArgs *e)
{
	if (SMap<SWindow*,int>::CPair *pair=m_lstPages.Lookup((SWindow*)e->sender))
	{
		m_pTabHost->GetEventSet()->setMutedState(true);
		m_pTabHost->SetCurSel(pair->m_value);
		m_pTabHost->GetEventSet()->setMutedState(false);
	}	
	return false;
}
bool STabCtrlHeaderBinder::TabSelChanged(EventTabSelChanged *evt_sel)
{
	SPOSITION pos=m_lstPages.GetStartPosition();
	SMap<SWindow*, int>::CPair* pair;
	while (pos)
	{
		pair= m_lstPages.GetNext(pos);
		if (pair->m_value == (int)evt_sel->uNewSel)
		{			
			//pair->m_key->ModifyState(WndState_PushDown| WndState_Check,0,TRUE);
			pair->m_key->SetCheck(TRUE);
		}
		else if (pair->m_value == (int)evt_sel->uOldSel)
		{
			//pair->m_key->ModifyState(0, WndState_PushDown| WndState_Check, TRUE);
			pair->m_key->SetCheck(FALSE);
		}
	}
	return false;
}