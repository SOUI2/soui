#include "stdafx.h"
#include "ThreadObject.h"
#include <process.h>
#include <assert.h>

CThreadObject::CThreadObject() :m_hThread(0)
{
	m_evtStart = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_evtStop = CreateEvent(NULL, TRUE, FALSE, NULL);
}


CThreadObject::~CThreadObject()
{
	EndThread(1000);
	CloseHandle(m_evtStart);
	CloseHandle(m_evtStop);
}


UINT CThreadObject::ThreadProc()
{
	SetEvent(m_evtStart);	//标记线程启动
	ResetEvent(m_evtStop);  //清除线程结束标志
	UINT uRet = Run();		//执行线程
	ResetEvent(m_evtStart); //清除线程启动标志
	return uRet;
}


UINT __stdcall CThreadObject::StaticTheadProc(LPVOID param)
{
	CThreadObject * _this = (CThreadObject*)param;
	return _this->ThreadProc();
}

BOOL CThreadObject::BeginThread()
{
	if (IsRunning()) return FALSE;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, &CThreadObject::StaticTheadProc, this, 0, NULL);
	if (m_hThread == NULL) return FALSE;
	DWORD dwRet = WaitForSingleObject(m_evtStart, INFINITE);
	return dwRet == WAIT_OBJECT_0;
}

BOOL CThreadObject::EndThread(DWORD dwWaitEndMS)
{
	if (!IsRunning()) return TRUE;
	assert(m_hThread);
	SetEvent(m_evtStop);
	if (WAIT_OBJECT_0 != WaitForSingleObject(m_hThread, dwWaitEndMS))
	{
		TerminateThread(m_hThread, (DWORD)-1);
	}
	CloseHandle(m_hThread);
	m_hThread = 0;
	return TRUE;
}

BOOL CThreadObject::IsRunning() const
{
	return WaitForSingleObject(m_evtStart, 0) == WAIT_OBJECT_0;
}

BOOL CThreadObject::IsStoped() const
{
	return WaitForSingleObject(m_evtStop, 0) == WAIT_OBJECT_0;	
}
