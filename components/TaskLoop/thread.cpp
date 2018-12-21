#include <windows.h>
#include <cassert>
#include "thread.h"
#include <interface\STaskLoop-i.h>
#include <process.h>

namespace SOUI
{
	class ThreadPrivate
	{
	public:
		ThreadPrivate():_hthread(NULL), _threadID(0)
		{

		}
		HANDLE _hthread;
		unsigned int _threadID;
	};

const long Thread::INVALID_THREAD_ID = -1;

Thread::Thread() :
    _stopping(false),
    _runnable(NULL),
    _name(),
    _private(*(new ThreadPrivate())),
    _start(false)
{
}

Thread::~Thread()
{
    waitForStop();
    delete &_private;
}

long Thread::getCurrentThreadID()
{
    DWORD id = ::GetCurrentThreadId();
    return (long) id;
}

bool Thread::start(IRunnable *runnable, const std::string &name, ThreadPriority priority)
{
    assert(runnable);

    {
        SAutoLock autolock (_lock);

        if (_start)
        {
            return false;
        }

        _stopping = false;
        _runnable = runnable->clone();
        _name = name;
        _start = true;
        _priorityLevel = priority;

        _private._hthread = (HANDLE)_beginthreadex(NULL, 0, Thread::threadProcWin32thunk, this, 0, &_private._threadID);

        if (_private._hthread == NULL)
        {
            clear();
            return false;
        }
		setPriority(priority);
    }

    _startSem.wait(100);
    return true;
}

void Thread::stop()
{
    SAutoLock autolock (_lock);

    if (!_start)
    {
        return;
    }

    _stopping = true;
}

void Thread::waitForStop()
{
    // we must release lock before the wait function, to avoid deadlock
    {
        SAutoLock autolock (_lock);

        if (!_start)
        {
            if ( _private._hthread )
            {
                ::CloseHandle( _private._hthread); // resource must be released
                _private._hthread = NULL;
                _private._threadID = 0;
            }

            return;
        }
    }

    DWORD ret = ::WaitForSingleObject(_private._hthread, INFINITE);

    {
        SAutoLock autolock (_lock);

        if ( WAIT_OBJECT_0 == ret )
        {
            ::CloseHandle( _private._hthread);
        }

        _private._hthread = NULL;
        _private._threadID = 0;
    }

    if (WAIT_OBJECT_0  != 0)
    {
    }
}

bool Thread::isStopping()
{
    SAutoLock autolock (_lock);
    return _stopping;
}

bool Thread::isStopped()
{
    SAutoLock autolock (_lock);
    return _stopping || (!_start);
}

std::string Thread::getName()
{
    SAutoLock autolock (_lock);
    return _name;
}

// we have undocumented Win32 APIs to set thread name.
const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
    DWORD dwType; // Must be 0x1000.
    LPCSTR szName; // Pointer to name (in user addr space).
    DWORD dwThreadID; // Thread ID (-1=caller thread).
    DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)
static void SetThreadName(DWORD dwThreadID, const char *threadName)
{
    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = threadName;
    info.dwThreadID = dwThreadID;
    info.dwFlags = 0;
#pragma warning(push)
#pragma warning(disable: 6320 6322)

    __try
    {
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR *)&info);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
    }

#pragma warning(pop)
}

void Thread::setThreadName(const std::string &name)
{
    SetThreadName(::GetCurrentThreadId(), name.c_str());
}

void Thread::setPriority(ThreadPriority priorityLevel)
{
    int priority = 0;

    switch (priorityLevel)
    {
    case High:
        priority = THREAD_PRIORITY_TIME_CRITICAL;
        break;

    case Low:
        priority = THREAD_PRIORITY_IDLE;
        break;

    default:
        priority = THREAD_PRIORITY_NORMAL;
        break;
    }

    BOOL ret = SetThreadPriority(_private._hthread, priority);

    if (!ret)
    {
    }
}

void Thread::clear()
{
    SAutoLock autolock (_lock);
    _stopping = false;
    delete _runnable;
    _runnable = NULL;
    _name = "";
    _start = false;
    _threadID = INVALID_THREAD_ID;
}

//  `stdcall' to 'cdecl' thunk
unsigned WINAPI Thread::threadProcWin32thunk(LPVOID lpParameter )
{
    Thread::threadProc(lpParameter);
    return 0;
}

void *Thread::threadProc(void *args)
{
    Thread *thread = static_cast<Thread *> (args);

    {
        SAutoLock autoLock(thread->_lock);
        thread->_threadID = getCurrentThreadID();
        thread->setThreadName(thread->_name);
        thread->setPriority(thread->_priorityLevel);
    }

    thread->_startSem.notify();
    thread->_runnable->run();

    {
        SAutoLock autoLock(thread->_lock);
        thread->clear();
    }

    return NULL;
}

}
