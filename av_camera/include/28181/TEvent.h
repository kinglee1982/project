

#ifndef TEVENT_H_
#define TEVENT_H_
/////////////////////////////////////////////////////////////////////////////
#include <assert.h>

#ifdef WIN32
#else
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#endif //WIN32



namespace comn
{

#ifdef WIN32
class Event
{
public:
    typedef HANDLE EventType;
    typedef HANDLE EventHandle;

    Event()
    {
        open();
    }

    ~Event()
    {
        close();
    }

    bool post()
    {
        return ( 0 != ::SetEvent( m_object ) );
    }

    // return true if event is set
    bool wait()
    {
        return timedwait( INFINITE );
    }

    // return true if event is set
    bool timedwait( int millisec )
    {
        bool ok = false;
        DWORD ret = ::WaitForSingleObject( m_object, millisec );
        if ( WAIT_OBJECT_0 == ret )
        {
            ::ResetEvent( m_object );
            ok = true;
        }
        return ok;
    }

    // return true if event is set
    bool trywait()
    {
        return timedwait( 0 );
    }

    EventHandle getHandle()
    {
        return m_object;
    }

    void reset()
    {
        ::ResetEvent( m_object );
    }

protected:
    bool open()
    {
        m_object = CreateEventA( NULL, TRUE, FALSE, NULL );
        return ( NULL != m_object );
    }

    void close()
    {
        ::CloseHandle( m_object );
        m_object = NULL;
    }

    EventType m_object;
};

#else
/////////////////////////////////////////////////////////////////////////////


class ConditionEvent
{
public:
    ConditionEvent()
    {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutex_init(&m_mutex, &attr);
        pthread_mutexattr_destroy(&attr);
        pthread_cond_init(&m_cond, NULL);
        m_signaled = false;
    }

    ~ConditionEvent()
    {
        pthread_cond_destroy(&m_cond);
        pthread_mutex_destroy(&m_mutex);
    }

    bool post()
    {
        pthread_mutex_lock(&m_mutex);
        m_signaled = true;
        pthread_cond_signal(&m_cond);
        pthread_mutex_unlock(&m_mutex);
        return true;
    }

    bool wait()
    {
        return timedwait(-1);
    }

    bool trywait()
    {
        return timedwait(0);
    }

    bool timedwait(int millisec)
    {
        bool ok = false;

        pthread_mutex_lock(&m_mutex);

        struct timespec spec;
        while (!m_signaled)
        {
            if (millisec >= 0)
            {
                getTimeout(&spec, millisec);
                int rc = pthread_cond_timedwait(&m_cond, &m_mutex, (const struct timespec *)&spec);
                if (rc == ETIMEDOUT)
                {
                    break;
                }
            }
            else
            {
                pthread_cond_wait(&m_cond, &m_mutex);
            }
        }

        ok = m_signaled;

        if (m_signaled)
        {
            m_signaled = false;
        }

        pthread_mutex_unlock(&m_mutex);

        return ok;
    }

    void reset()
    {
		pthread_mutex_lock(&m_mutex);
        m_signaled = false;
		pthread_mutex_unlock(&m_mutex);
    }

    static void getTimeout(struct timespec *abstime, long milliseconds)
    {
		struct timeval tv;
		gettimeofday(&tv, NULL);

		abstime->tv_sec  = tv.tv_sec + milliseconds / 1000;
		abstime->tv_nsec = tv.tv_usec*1000 + (milliseconds % 1000)*1000000;
		if (abstime->tv_nsec >= 1000000000)
		{
			abstime->tv_nsec -= 1000000000;
			abstime->tv_sec++;
		}
    }

private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
    bool m_signaled;
};



typedef ConditionEvent Event;
////////////////////////////////////////////////////////////////////////////

#endif //WIN32

} // end of namespace

////////////////////////////////////////////////////////////////////////////
#endif /* TEVENT_H_ */
