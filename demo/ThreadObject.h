#pragma once

class CThreadObject
{
public:
	CThreadObject();
	~CThreadObject();

	BOOL BeginThread();

	BOOL EndThread(DWORD dwWaitEndMS = INFINITE);

	BOOL IsRunning() const;

	BOOL IsStoped() const;
protected:
	//Work Function.
	virtual UINT Run() = 0;
    
	UINT ThreadProc();
	static UINT __stdcall StaticTheadProc(LPVOID param);

	HANDLE m_evtStart;	//工作线程通知宿主线程“线程启动”事件
	HANDLE m_evtStop;   //宿主线程通知工作线程“线程结束”事件
	HANDLE m_hThread;   //线程句柄
};

