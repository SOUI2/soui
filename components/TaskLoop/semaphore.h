#pragma once

#include <helper/SNoCopyable.hpp>

namespace SOUI
{
	enum
	{
		RETURN_OK = 0,
		RETURN_TIMEOUT = 1,
		RETURN_ERROR = -1,
	};

class SemaphorePrivate;
/**
 * Semaphore class for thread synchronization.
 */
class Semaphore : public SNoCopyable
{
public:
    /**
     * Wait for notified permanently.
     * @return RETURN_OK if notified. RETURN_ERROR if error occurs.
     */
    int wait();

    /**
     * Wait for notified during a time.
     * @param msec waiting time in milliseconds.
     * @return RETURN_OK if notified. RETURN_TIMEOUT if time out. RETURN_ERROR
     * if error occurs.
     */
    int wait(unsigned int msec);

    /**
     * Notify object. This function could be called before wait, and the notify
     * count will increase.
     */
    void notify();

    /**
     * Default constructor.
     */
    Semaphore();

    /**
     * Destructor.
     */
    virtual ~Semaphore();

private:
    SemaphorePrivate &_private;
};

}
