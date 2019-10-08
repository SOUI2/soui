#pragma once

#include <string>
#include <helper/SCriticalSection.h>
#include <helper/SNoCopyable.hpp>
#include <helper/SFunctor.hpp>
#include <helper/SSemaphore.h>
namespace SOUI
{

class ThreadPrivate;
struct IRunnable;

/**
 * Thread class for creating a thread.
 */
class Thread : public SNoCopyable
{
public:
    enum ThreadPriority
    {
        High = 1,
        Normal = 0,
        Low = -1,
    };

    /**
     * Get current thread ID.
     * @return current thread ID.
     */
    static long getCurrentThreadID();

    /**
     * Set stopping flag to thread. The thread may not be stopped immediately.
     */
    void stop();

    /**
     * Wait for thread stopped synchronized.
     */
    void waitForStop();

    /**
     * Get stopping flag.
     * @return true if stopping flag is set.
     */
    bool isStopping() ;
    /**
      * Get stopped flag.
      * @return true if stopping flag is set or start flag is false.
      */
    bool isStopped() ;

    /**
     * Get thread ID.
     * @return thread ID.
     */
    long getThreadID()
    {
        SAutoLock autoLock(_lock);
        return _threadID;
    }

    /**
     * Get thread name.
     * @return thread name.
     */
    std::string getName() ;

    /**
     * Identify whether thread is started.
     * @return true if started.
     */
    bool isStart()
    {
        SAutoLock autoLock(_lock);
        return _start;
    }

    /**
     * Default constructor.
     */
    Thread();

    /**
     * Destructor.
     */
    virtual ~Thread();

    /**
     * Invalid thread ID.
     */
    static const long INVALID_THREAD_ID;

	bool start(IRunnable *runnable, const std::string &name, ThreadPriority priority = Normal);

private:
    /**
     * For implementation on Windows,
     * we have to use this  Win32 thunk to call the c-style entry point : threadProc
     */
#ifdef WIN32
	static unsigned int WINAPI threadProcWin32thunk(void * lpParameter);
#endif
    static void *threadProc(void *args);

    void clear();
    void setThreadName(const std::string &name);
    void setPriority(ThreadPriority priority);

    // Thread object itself must be thread-safe
    SCriticalSection _lock;
    bool _start;
    bool _stopping;
    IRunnable *_runnable;
    std::string _name;
    long _threadID;
    SSemaphore _startSem;
    ThreadPrivate &_private;
    ThreadPriority _priorityLevel;
};

}
