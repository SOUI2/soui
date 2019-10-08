#pragma once

#include <interface/STaskLoop-i.h>
#include <windows.h>
#include "thread.h"
#include <helper/SSharedPtr.hpp>
#include <list>
#include <unknown/obj-ref-impl.hpp>
#include <helper/SFunctor.hpp>

namespace SOUI
{
	class STaskLoop : public TObjRefImpl<ITaskLoop>
	{
	public:
		/**
		* Constructor.
		*/
		STaskLoop();

		/**
		* Destructor.
		*/
		virtual ~STaskLoop();

		/**
		* Start task mgr thread.
		*/
		void start(const char * pszName, Priority priority);

		template<typename TClass,typename Fun>
		void _start(TClass *obj, Fun fun, Priority priority)
		{
			SFunctor0<TClass,Fun>  runnable(this, &STaskLoop::runLoopProc);
			m_thread.start(&runnable, m_strName,  (Thread::ThreadPriority)priority);
		}
		/**
		* Stop task mgr synchronized.
		*/
		void stop();


		/**
		* Remove tasks for a sepcific object from task mgr pening task list
		* @param object the specific object wants pending functors to be removed
		*/
		void cancelTasksForObject(void *object);

		/**
		* Cancel tasks for a specific task ID list
		* @param taskList the task ID list to be canceled
		* @return the removed task list.
		*/
		bool cancelTask(long taskId);


		/**
		* get the total task number in the task mgr queue.
		* @return total task number in task mgr queue
		*/
		int getTaskCount() const;

		/**
		* get the run loop status.
		* @return the running status
		*/
		bool isRunning();

		long postTask(const IRunnable *runnable, bool waitUntilDone, int priority);

		bool getName(char * pszBuf, int nBufLen);

		bool getRunningTaskInfo(char *buf, int bufLen);

	private:
		class TaskItem
		{
		public:
			TaskItem(IRunnable *runnable_, int nPriority_) 
				: taskID(0)
				, runnable(runnable_)
				, nPriority(nPriority_)
				, semaphore(NULL) 
			{}

			const char *getRunnableInfo()
			{
				return runnable->getClassInfo();
			}

			long taskID;
			SSharedPtr<IRunnable> runnable;
			SSemaphore *semaphore;
			int  nPriority;
		};




		void runLoopProc();

		mutable SCriticalSection m_taskListLock;
		SCriticalSection m_runningLock;
		std::string m_strName;
		Thread m_thread;
		SSemaphore m_itemsSem;
		std::list<TaskItem> m_items;

		SCriticalSection m_runningInfoLock;
		bool m_hasRunningItem;
		std::string m_runingItemInfo;
		TaskItem m_runningItem;
		long m_nextTaskID;

	};

    namespace TASKLOOP
    {
        SOUI_COM_C BOOL SOUI_COM_API SCreateInstance(IObjRef **ppTaskLoop);
    }
}




