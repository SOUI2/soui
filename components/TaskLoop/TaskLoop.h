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
			_thread.start(&runnable, _name,  (Thread::ThreadPriority)priority);
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

    namespace TASKLOOP
    {
        SOUI_COM_C BOOL SOUI_COM_API SCreateInstance(IObjRef **ppTaskLoop);
    }
}




