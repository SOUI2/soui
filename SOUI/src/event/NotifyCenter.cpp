#include "souistd.h"
#include "event/NotifyCenter.h"

namespace SOUI{

template<> SNotifyCenter * SSingleton<SNotifyCenter>::ms_Singleton = 0;

//////////////////////////////////////////////////////////////////////////
class SNotifyReceiver:public CSimpleWnd
{
public:
	enum{
		UM_NOTIFYEVENT = (WM_USER+1000),
		TIMERID_ASYNC = 100,
	};

	SNotifyReceiver(INotifyCallback * pCallback) :m_pCallback(pCallback)
	{
		SASSERT(m_pCallback);
	}

	~SNotifyReceiver()
	{

	}

	LRESULT OnNotifyEvent(UINT uMsg,WPARAM wParam,LPARAM lParam);

	void OnTimer(UINT_PTR uID);

	BEGIN_MSG_MAP_EX(SNotifyReceiver)
		MSG_WM_TIMER(OnTimer)
		MESSAGE_HANDLER_EX(UM_NOTIFYEVENT, OnNotifyEvent)
	END_MSG_MAP()

protected:
	INotifyCallback * m_pCallback;
};


LRESULT SNotifyReceiver::OnNotifyEvent(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (wParam)
	{
#if _MSC_VER >= 1700	//VS2012
	case 1:
		{
			std::function<void(void)>* f = (std::function<void(void)>*)lParam;
			(*f)();
		}
		break;
	case 2:
		{
			std::function<void(void)>* f = (std::function<void(void)>*)lParam;
			(*f)();
			delete f;
		}
		break;
#endif//_MSC_VER >= 1700	//VS2012
	default:
		break;
	}
	return 0;
}

void SNotifyReceiver::OnTimer(UINT_PTR uID)
{
	if(uID==TIMERID_ASYNC)
	{
		m_pCallback->OnFireEvts();
		KillTimer(uID);
	}
}


//////////////////////////////////////////////////////////////////////////
SNotifyCenter::SNotifyCenter(void):m_pReceiver(NULL)
{
	m_dwMainTrdID = GetCurrentThreadId();
	m_pReceiver = new SNotifyReceiver(this);
	m_pReceiver->Create(_T("NotifyReceiver"),WS_POPUP,0,0,0,0,0,HWND_MESSAGE,0);
	SASSERT(m_pReceiver->IsWindow());
}

SNotifyCenter::~SNotifyCenter(void)
{
	m_pReceiver->DestroyWindow();
	delete m_pReceiver;
	m_pReceiver = NULL;
}

void SNotifyCenter::FireEventSync( EventArgs *e )
{
	SASSERT(m_dwMainTrdID == GetCurrentThreadId());
	OnFireEvent(e);
}

//把事件抛到事件队列，不检查事件是否注册，执行事件时再检查。
void SNotifyCenter::FireEventAsync( EventArgs *e )
{
	SAutoLock lock(m_cs);
	e->AddRef();
	if(m_ayncEvent.GetCount()==0)
	{
		m_pReceiver->SetTimer(SNotifyReceiver::TIMERID_ASYNC,20,NULL);
	}
	m_ayncEvent.AddTail(e);
}


void SNotifyCenter::OnFireEvent( EventArgs *e )
{
	if(!GetEventObject(e->GetID())) return;//确保事件是已经注册过的已经事件。

	FireEvent(*e);
	if(!e->bubbleUp) return ;

	SPOSITION pos = m_evtHandlerMap.GetTailPosition();
	while(pos)
	{
		ISlotFunctor * pSlot = m_evtHandlerMap.GetPrev(pos);
		(*pSlot)(e);
		if(!e->bubbleUp) break;
	}
}


void SNotifyCenter::OnFireEvts()
{
	SAutoLock lock(m_cs);
	SPOSITION pos = m_ayncEvent.GetHeadPosition();
	while(pos)
	{
		EventArgs *e = m_ayncEvent.GetNext(pos);
		OnFireEvent(e);
	}
	m_ayncEvent.RemoveAll();
}


bool SNotifyCenter::RegisterEventMap( const ISlotFunctor &slot )
{
	for(SPOSITION pos = m_evtHandlerMap.GetHeadPosition();pos;)
	{
		ISlotFunctor * pSlot = m_evtHandlerMap.GetNext(pos);
		if(pSlot->Equal(slot)) return false;
	}
	m_evtHandlerMap.AddTail(slot.Clone());
	return true;
}

bool SNotifyCenter::UnregisterEventMap( const ISlotFunctor &slot )
{
	for(SPOSITION pos = m_evtHandlerMap.GetHeadPosition();pos;)
	{
		SPOSITION posPrev = pos;
		ISlotFunctor * pSlot = m_evtHandlerMap.GetNext(pos);
		if(pSlot->Equal(slot))
		{
			m_evtHandlerMap.RemoveAt(posPrev);
			delete pSlot;
			return true;
		}
	}
	return false;
}

#if _MSC_VER >= 1700	//VS2012
void SNotifyCenter::RunOnUISync(std::function<void(void)> fn)
{
	m_pReceiver->SendMessage(SNotifyReceiver::UM_NOTIFYEVENT, 1, (LPARAM)&fn);
}
void SNotifyCenter::RunOnUIAsync(std::function<void(void)> fn)
{
	auto f = new std::function<void()>(std::move(fn));
	m_pReceiver->PostMessage(SNotifyReceiver::UM_NOTIFYEVENT, 2, (LPARAM)f);
}
#endif


}