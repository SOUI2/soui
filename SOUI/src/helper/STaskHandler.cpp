#include <souistd.h>
#include <helper/STaskHandler.h>
#include <algorithm>
#include <cassert>
#include <deque>
#include <limits>

namespace SOUI
{

	STaskHandler::STaskHandler() :
		m_taskListLock(),
		m_runningLock(),
		m_itemsSem(),
		m_items(),
		m_hasRunningItem(false),
		m_runningItem(NULL, 0),
		m_nextTaskID(0)
	{
		m_dwThreadID = GetCurrentThreadId();
		Create(_T("task_handler_wnd"), WS_POPUP, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL);
	}

	STaskHandler::~STaskHandler()
	{
		stop();
	}

	void STaskHandler::start(const char * pszName, Priority priority)
	{
		SAutoLock autoLock(m_runningInfoLock);

		::SetWindowTextA(m_hWnd,pszName);
		SetTimer(100, 10, NULL);
		m_isRunning = true;
	}

	void STaskHandler::stop()
	{
		int taskNum = getTaskCount();

		m_itemsSem.notify();
		KillTimer(100);
		m_isRunning = false;
	}

	bool STaskHandler::isRunning()
	{
		return m_isRunning;
	}

	long STaskHandler::postTask(const IRunnable *runnable, bool waitUntilDone, int priority)
	{
		if (!m_isRunning)
		{
			return -1;
		}
		IRunnable *pCloneRunnable = runnable->clone();
		if (GetCurrentThreadId() == m_dwThreadID && waitUntilDone)
		{
			pCloneRunnable->run();
			delete pCloneRunnable;
			return -1;
		}

		SSemaphore semaphore;
		TaskItem item(pCloneRunnable, priority);

		if (waitUntilDone)
		{
			item.semaphore = &semaphore;
		}

		m_taskListLock.Enter();

		item.taskID = ++m_nextTaskID;
		if (m_nextTaskID > 100000000) m_nextTaskID = 0;

		SPOSITION pos = m_items.GetTailPosition();
		while(pos)
		{
			SPOSITION pos2 = pos;
			const TaskItem & item = m_items.GetPrev(pos);
			if (item.nPriority >= priority)
			{
				m_items.InsertAfter(pos2,item);
				break;
			}
		}
		if(!pos) 
			m_items.AddHead(item);

		m_taskListLock.Leave();
		m_itemsSem.notify();

		if (waitUntilDone)
		{
			int ret = semaphore.wait();

			if (ret == RETURN_ERROR)
			{
			}
		}

		return item.taskID;
	}

	void STaskHandler::OnTimer(UINT_PTR timerId)
	{
		while (true)
		{
			{
				SAutoLock autoLock(m_taskListLock);
				SAutoLock autoRunningLock(m_runningLock);
				SAutoLock autoRuningInfoLock(m_runningInfoLock);
				m_hasRunningItem = false;
				m_runningItem = TaskItem(NULL, 0);
				if (!m_items.IsEmpty())
				{
					m_hasRunningItem = true;
					m_runningItem = m_items.RemoveHead();
				}
			}

			{
				SAutoLock autoRunningLock(m_runningLock);
				if (m_hasRunningItem)
				{
					TaskItem item = m_runningItem;
					item.runnable->run();
					if (item.semaphore)
					{
						//通知一个task执行完毕
						item.semaphore->notify();
					}
					SAutoLock autoRuningInfoLock(m_runningInfoLock);
					m_hasRunningItem = false;
					m_runningItem = TaskItem(NULL, 0);
				}
				else
				{
					break;
				}

			}
		}// end of while
	}

	bool STaskHandler::getName(char * pszBuf, int nBufLen)
	{
		GetWindowTextA(m_hWnd,pszBuf,nBufLen);
		return true;
	}

	void STaskHandler::cancelTasksForObject(void *object)
	{
		if (object == NULL)
		{
			return;
		}

		{
			SAutoLock autoLock(m_taskListLock);
			SPOSITION pos = m_items.GetHeadPosition();
			while(pos)
			{
				SPOSITION prev = pos;
				TaskItem &item = m_items.GetNext(pos);

				if (item.runnable->getObject() == object)
				{
					m_items.RemoveAt(prev);
				}
			}
		}
		{
			SAutoLock autoLock(m_runningLock);
			SAutoLock autoLockRunningInfo(m_runningInfoLock);
			if (m_hasRunningItem)
			{//make sure the running item is not belong to the to be canceled object.
				m_hasRunningItem = false;
				m_runningItem = TaskItem(NULL, 0);
			}
		}
	}

	bool STaskHandler::cancelTask(long taskId)
	{
		SAutoLock autoLock(m_taskListLock);
		SPOSITION pos = m_items.GetHeadPosition();
		while(pos)
		{
			SPOSITION prev = pos;
			TaskItem &item = m_items.GetNext(pos);

			if (item.taskID == taskId)
			{
				m_items.RemoveAt(prev);
				return true;
			}
		}
		return false;
	}

	int STaskHandler::getTaskCount() const
	{
		SAutoLock autoLock(m_taskListLock);
		return (int)m_items.GetCount();
	}

	bool STaskHandler::getRunningTaskInfo(char *buf, int bufLen)
	{
		return false;
	}

}
