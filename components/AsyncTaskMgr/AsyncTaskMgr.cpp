#include "AsyncTaskMgr.h"
#include <unknown/obj-ref-impl.hpp>
#include <algorithm>
#include <cassert>
#include <deque>
#include <limits>

namespace SOUI
{

	SAsyncTaskMgr::SAsyncTaskMgr() :
		_lock(),
		_runningLock(),
		_thread(),
		_itemsSem(),
		_items(),
		_hasRunningItem(false),
		_runningItem(0),
		_nextTaskID(0)
	{
	}

	SAsyncTaskMgr::~SAsyncTaskMgr()
	{
		stop();
	}

	void SAsyncTaskMgr::start(const char * pszName,Priority priority)
	{
		{
			SAutoLock autoLock(_lock);
			_items.clear();
			_name = pszName;
		}
		_start(this, &SAsyncTaskMgr::runLoopProc,  priority);
	}

	void SAsyncTaskMgr::stop()
	{
		int taskNum = getTaskCount();

		_thread.stop();
		_itemsSem.notify();
		_thread.waitForStop();
	}

	bool SAsyncTaskMgr::isRunning()
	{
		return !_thread.isStopped();
	}

	long SAsyncTaskMgr::postTask(const IRunnable *runnable, bool waitUntilDone)
	{
		if (_thread.isStopped())
		{
			delete runnable;
			return -1;
		}
		IRunnable *pCloneRunnable = runnable->clone();
		if (Thread::getCurrentThreadID() == _thread.getThreadID() && waitUntilDone)
		{
			pCloneRunnable->run();
			delete pCloneRunnable;
			return -1;
		}

		Semaphore semaphore;
		TaskItem item(pCloneRunnable);

		if (waitUntilDone)
		{
			item.semaphore = &semaphore;
		}

		_lock.Enter();

		item.taskID = _nextTaskID;
		_nextTaskID = (_nextTaskID + 1) & ((std::numeric_limits<long>::max)());
		_items.push_back(item);

		size_t totalSize = _items.size();
		_lock.Leave();
		_itemsSem.notify();

		if (waitUntilDone)
		{
			int ret = semaphore.wait();

			if (ret == RETURN_ERROR)
			{
			}
		}

		return item.taskID;
	}

	void SAsyncTaskMgr::runLoopProc()
	{
		while (true)
		{
			if (_thread.isStopping())
			{
				break;
			}

			_itemsSem.wait(10);

			{
				SAutoLock autoLock(_lock);
				SAutoLock autoRunningLock(_runningLock);

				_hasRunningItem = false;
				_runningItem = TaskItem(0);
				if (!_items.empty())
				{
					_hasRunningItem = true;
					_runningItem = _items.front();
					_items.pop_front();
				}
			}


			{
				//执行一个task
				SAutoLock autoRunningLock(_runningLock);

				if (_hasRunningItem)
				{
					TaskItem item = _runningItem;
					item.runnable->run();

					if (item.semaphore)
					{
						//通知一个task执行完毕
						item.semaphore->notify();
					}

					_hasRunningItem = false;
					_runningItem = TaskItem(0);
				}
			}
		}// end of while

		SAutoLock autoLock(_lock);

		size_t itemsSize = _items.size();
		while (itemsSize > 0)
		{
			TaskItem item = _items.front();
			_items.pop_front();
			itemsSize--;

			if (item.semaphore)
			{
				item.semaphore->notify();
			}
		}

		_items.clear();
	}

	void SAsyncTaskMgr::cancelTasksForObject(void *object)
	{
		if (object == NULL)
		{
			return;
		}

		{
			SAutoLock autoLock(_lock);
			std::list<TaskItem>::iterator iter = _items.begin();

			while (iter != _items.end())
			{
				TaskItem &item = *iter;

				if (item.runnable->getObject() == object)
				{
					iter = _items.erase(iter);
				}
				else
				{
					++iter;
				}
			}
		}

		{
			if (Thread::getCurrentThreadID() != _thread.getThreadID())
			{
				_runningLock.Enter();
			}

			if (Thread::getCurrentThreadID() != _thread.getThreadID())
			{
				_runningLock.Leave();
			}
		}

		{
			if (Thread::getCurrentThreadID() != _thread.getThreadID())
			{
				_runningLock.Enter();
			}


			if (Thread::getCurrentThreadID() != _thread.getThreadID())
			{
				_runningLock.Leave();
			}
		}
	}

	bool SAsyncTaskMgr::cancelTask(long taskId)
	{
		SAutoLock autoLock(_lock);
		std::list<TaskItem>::iterator itemIt = _items.begin();

		while (itemIt != _items.end())
		{
			if (itemIt->taskID == taskId)
			{
				itemIt = _items.erase(itemIt);
				return true;
			}
			else
			{
				++itemIt;
			}
		}
		return false;
	}

	int SAsyncTaskMgr::getTaskCount() const 
	{
		SAutoLock autoLock(_lock);

		return (int)_items.size();
	}

}

SOUI_COM_C BOOL SOUI_COM_API SOUI::AsyncTask::SCreateInstance(IObjRef **ppAsyncTask)
{
	*ppAsyncTask = new SAsyncTaskMgr();
     return TRUE;
}
