#pragma once

#include <interface/STaskLoop-i.h>
#include <helper/SSharedPtr.hpp>
#include <unknown/obj-ref-impl.hpp>
#include <helper/SFunctor.hpp>
#include <core/SimpleWnd.h>
#include <helper/SSemaphore.h>
#include <souicoll.h>
namespace SOUI
{
	class SOUI_EXP STaskHandler : public TObjRefImpl<ITaskLoop>, protected CSimpleWnd
	{
	public:
		/**
		* Constructor.
		*/
		STaskHandler();

		/**
		* Destructor.
		*/
		virtual ~STaskHandler();

		/**
		* Start task mgr thread.
		*/
		void start(const char * pszName, Priority priority);

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

		void OnTimer(UINT_PTR id);

		BEGIN_MSG_MAP_EX(STaskHandler)
			MSG_WM_TIMER(OnTimer)
			CHAIN_MSG_MAP(CSimpleWnd)
		END_MSG_MAP()

		mutable SCriticalSection m_taskListLock;
		SCriticalSection m_runningLock;
		SSemaphore m_itemsSem;
		SList<TaskItem> m_items;

		SCriticalSection m_runningInfoLock;
		bool m_hasRunningItem;
		TaskItem m_runningItem;
		long m_nextTaskID;
		bool    m_isRunning;
		DWORD   m_dwThreadID;
	};
}
