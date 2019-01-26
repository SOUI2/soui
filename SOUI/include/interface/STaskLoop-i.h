#pragma once
#include <unknown/obj-ref-i.h>

namespace SOUI
{
	struct IRunnable
	{
		virtual ~IRunnable() {}
		virtual IRunnable* clone() const = 0;
		virtual void run() = 0;
		virtual void *getObject() = 0;
		virtual const char *getClassInfo() const = 0;
	};

	struct ITaskLoop : public IObjRef
	{
		enum Priority
		{
			High = 1,
			Normal = 0,
			Low = -1,
		};

		virtual bool getName(char *pszBuf, int nBufLen) = 0;

		/**
		* Start a thread to run.
		* @param priority the thread priority
		*/
		virtual void start(const char * pszName,Priority priority) = 0;

		/**
		* Stop thread synchronized.
		*/
		virtual void stop() = 0;

		/**
		* postTask post or send a tasks to this task manager.
		* @param runnable the to be run task object.
		* @param waitUntilDone, true for send and false for post.
		* @param priority, the task priority.
		* @return the task id, can be used by cancelTask.
		*/
		virtual long postTask(const IRunnable *runnable, bool waitUntilDone, int priority=0) = 0;

		/**
		* Remove tasks for a sepcific object from runloop pening task list
		* @param object the specific object wants pending functors to be removed
		*/
		virtual void cancelTasksForObject(void *object) = 0;

		/**
		* Cancel tasks for a specific task ID list
		* @param taskList the task ID list to be canceled
		* @return the removed task list.
		*/
		virtual bool cancelTask(long taskId) = 0;

		/**
		* get the total task number in the runloop queue.
		* @return total task number in runloop queue
		*/
		virtual int getTaskCount() const = 0;

		/**
		* get the run loop status.
		* @return the running status
		*/
		virtual bool isRunning() = 0;


	};
}