#include <Windows.h>
#include "semaphore.h"
#include <cerrno>


namespace SOUI
{
	class SemaphorePrivate
	{
	public:
		HANDLE  _hsem;
	};

Semaphore::Semaphore() :
    _private(*(new SemaphorePrivate()))
{
    _private._hsem = ::CreateSemaphore(NULL, 0, 65535/* we need to discuss this max value*/, NULL);
}

Semaphore::~Semaphore()
{
    ::CloseHandle(_private._hsem);
    delete &_private;
}

int Semaphore::wait()
{
    DWORD ret = ::WaitForSingleObject (_private._hsem, INFINITE);

    if ( ret == WAIT_OBJECT_0)
    {
        return RETURN_OK;
    }

    return RETURN_ERROR; // This is a blocking wait, so any value other than WAIT_OBJECT_0 indicates an error!
}

int Semaphore::wait(unsigned int msec)
{
    DWORD ret = ::WaitForSingleObject (_private._hsem, msec);

    if ( ret == WAIT_OBJECT_0)
    {
        return RETURN_OK;
    }

    // This is a timed wait, so WAIT_TIMEOUT value must be checked!
    if ( ret == WAIT_TIMEOUT  )
    {
        return RETURN_TIMEOUT;
    }

    return RETURN_ERROR;
}

void Semaphore::notify()
{
    LONG previous_count;
    // let's just unblock one waiting thread.
    BOOL ret = ::ReleaseSemaphore(_private._hsem, 1, &previous_count);
}

}
