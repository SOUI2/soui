#include "TaskLoop.h"
#include <unknown/obj-ref-impl.hpp>
#include <algorithm>
#include <cassert>
#include <deque>
#include <limits>

namespace SOUI
{

	STaskLoop::STaskLoop() :
		_lock(),
		_runningLock(),
		_thread(),
		_itemsSem(),
		_items(),
		_hasRunningItem(false),
		_runningItem(0,0),
		_nextTaskID(0)
	{
	}

	STaskLoop::~STaskLoop()
	{
		stop();
	}

	void STaskLoop::start(const char * pszName,Priority priority)
	{
		{
			SAutoLock autoLock(_lock);
			_items.clear();
			if(pszName) _name = pszName;
		}
		_start(this, &STaskLoop::runLoopProc,  priority);
	}

	void STaskLoop::stop()
	{
		int taskNum = getTaskCount();

		_thread.stop();
		_itemsSem.notify();
		_thread.waitForStop();
	}

	bool STaskLoop::isRunning()
	{
		return !_thread.isStopped();
	}

	long STaskLoop::postTask(const IRunnable *runnable, bool waitUntilDone, int priority)
	{
		if (_thread.isStopped())
		{
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
		TaskItem item(pCloneRunnable,priority);

		if (waitUntilDone)
		{
			item.semaphore = &semaphore;
		}

		_lock.Enter();

		item.taskID = _nextTaskID;
		_nextTaskID = (_nextTaskID + 1) & ((std::numeric_limits<long>::max)());
		std::list<TaskItem>::reverse_iterator it= _items.rbegin();
		while(it != _items.rend())
		{
			if(it->nPriority>=priority)
			{
				_items.insert(it.base(),item);
				break;
			}
			it ++;
		}
		if(it==_items.rend())
			_items.push_front(item);

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

	void STaskLoop::runLoopProc()
	{
		while (true)
		{
			if (_thread.isStopping())
			{
				break;
			}

			_itemsSem.wait(INFINITE);

			{
				SAutoLock autoLock(_lock);
				SAutoLock autoRunningLock(_runningLock);

				_hasRunningItem = false;
				_runningItem = TaskItem(0,0);
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
					_runningItem = TaskItem(0,0);
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

	bool STaskLoop::getName(char * pszBuf, int nBufLen)
	{
		SAutoLock autoLock(_lock);
		if (_name.length() >= (size_t)nBufLen)
			return false;
		strcpy_s(pszBuf, nBufLen, _name.c_str());
		return true;
	}

	void STaskLoop::cancelTasksForObject(void *object)
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

	bool STaskLoop::cancelTask(long taskId)
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

	int STaskLoop::getTaskCount() const 
	{
		SAutoLock autoLock(_lock);

		return (int)_items.size();
	}

}

SOUI_COM_C BOOL SOUI_COM_API SOUI::TASKLOOP::SCreateInstance(IObjRef **ppTaskLoop)
{
	*ppTaskLoop = new STaskLoop();
     return TRUE;
}
