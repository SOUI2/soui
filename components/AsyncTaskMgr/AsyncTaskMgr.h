#pragma once

#include <interface/SAsyncTaskMgr-i.h>
#include <windows.h>
#include "thread.h"
#include <helper/SSharedPtr.hpp>
#include <list>
#include <unknown/obj-ref-impl.hpp>

namespace SOUI
{
	class SAsyncTaskMgr : public TObjRefImpl<IAsyncTaskMgr>
	{
	public:
		/**
		* Constructor.
		*/
		SAsyncTaskMgr();

		/**
		* Destructor.
		*/
		virtual ~SAsyncTaskMgr();

		void setName(const char * pszName);

		/**
		* Start RunLoop thread.
		*/
		void start(Priority priority);

		/**
		* Stop RunLoop thread synchronized.
		*/
		void stop();


		/**
		* Remove tasks for a sepcific object from runloop pening task list
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
		* get the total task number in the runloop queue.
		* @return total task number in runloop queue
		*/
		int getTaskCount() const;

		/**
		* get the run loop status.
		* @return the running status
		*/
		bool isRunning();

		long postTask(const IRunnable *runnable, bool waitUntilDone);
	private:
		class TaskItem
		{
		public:
			TaskItem(IRunnable *runnable) : taskID(0), runnable(runnable), semaphore(NULL) {}

			const char *getRunnableInfo()
			{
				return runnable->getClassInfo();
			}

			long taskID;
			SSharedPtr<IRunnable> runnable;
			Semaphore *semaphore;
		};




		void runLoopProc();

		mutable SCriticalSection _lock;
		SCriticalSection _runningLock;
		std::string _name;
		Thread _thread;
		Semaphore _itemsSem;
		std::list<TaskItem> _items;
		bool _hasRunningItem;
		TaskItem _runningItem;
		DWORD _nextTaskID;
	};

    namespace AsyncTask
    {
        SOUI_COM_C BOOL SOUI_COM_API SCreateInstance(IObjRef **ppAsyncTask);
    }
}




