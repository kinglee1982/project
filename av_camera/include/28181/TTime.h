
#ifndef TTIME_H_
#define TTIME_H_

#include <ctime>
#include <string>
#include <stdio.h>

#ifdef WIN32

#else
#include <sys/time.h>
#endif //WIN32
////////////////////////////////////////////////////////////////////////////
namespace comn
{

class Time
{
public:
    int m_second;
    int m_millisecond;

    Time()
    {
        m_second = 0;
        m_millisecond = 0;
        getLocalTime();
    }

	static std::string toString(long long timestamp)
	{
		time_t theTime = timestamp/1000;
		struct tm sNow;
#ifdef WIN32
		 localtime_s(&sNow,&theTime);
#else
		localtime_r(&theTime,&sNow);
#endif

		char buf[100] =
		{ 0 };
		sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d.%d", sNow.tm_year + 1900,
			sNow.tm_mon+1, sNow.tm_mday, sNow.tm_hour, sNow.tm_min,
			sNow.tm_sec, (int)timestamp%1000);
		return buf;
	}

    int getSecond() const
    {
        return m_second;
    }

    int getMillisecond() const
    {
        return m_millisecond;
    }

    /// reset
    void reset()
    {
        getLocalTime();
    }

    /// get elapse now
    int getElapse() const
    {
        return Time().getElapse(*this);
    }

    /// get elapse millisecond
    int getElapse(const Time& tmPrev) const
    {
        int sec = (m_second - tmPrev.m_second);
        int millisec = (m_millisecond - tmPrev.m_millisecond);
        return (sec * 1000 + millisec);
    }

    /// get date string
    std::string toString() const
    {
        time_t theTime = m_second;
        struct tm* pNow = localtime(&theTime);
        char buf[100] =
        { 0 };
        sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d.%d", pNow->tm_year,
                pNow->tm_mon, pNow->tm_mday, pNow->tm_hour, pNow->tm_min,
                pNow->tm_sec, m_millisecond);
        return buf;
    }

    /// get string with millisecond
    std::string getMillisecondString() const
    {
        time_t theTime = m_second;
        struct tm* pNow = localtime(&theTime);
        char buf[100] =
        { 0 };
        sprintf(buf, "%02d:%02d:%02d.%d", pNow->tm_hour, pNow->tm_min,
                pNow->tm_sec, m_millisecond);
        return buf;
    }

    /// get local time
    void getLocalTime()
    {
        struct timeval tp;
        mygettimeofday(&tp, NULL);
        m_second = tp.tv_sec;
        m_millisecond = tp.tv_usec / 1000;
    }

#ifdef WIN32
    /*
     * Number of micro-seconds between the beginning of the Windows epoch
     * (Jan. 1, 1601) and the Unix epoch (Jan. 1, 1970).
     *
     * This assumes all Win32 compilers have 64-bit support.
     */
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS) || defined(__WATCOMC__)
#define DELTA_EPOCH_IN_USEC  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_USEC  11644473600000000ULL
#endif
    typedef unsigned __int64 u_int64_t;

    static u_int64_t filetime_to_unix_epoch (const FILETIME *ft)
    {
        u_int64_t res = (u_int64_t) ft->dwHighDateTime << 32;

        res |= ft->dwLowDateTime;
        res /= 10; /* from 100 nano-sec periods to usec */
        res -= DELTA_EPOCH_IN_USEC; /* from Win epoch to Unix epoch */
        return (res);
    }

    int mygettimeofday (struct timeval *tv, void *tz)
    {
		(void)tz;
        FILETIME ft;
        u_int64_t tim;

        if (!tv)
        {
            //errno = EINVAL;
            return (-1);
        }
        GetSystemTimeAsFileTime (&ft);
        tim = filetime_to_unix_epoch (&ft);
        tv->tv_sec = (long) (tim / 1000000L);
        tv->tv_usec = (long) (tim % 1000000L);
        return (0);
    }
#else
    int mygettimeofday(struct timeval *tv, struct timezone *tz)
    {
        return gettimeofday(tv, tz);
    }
#endif //win32

};

} // end of namespace
////////////////////////////////////////////////////////////////////////////
#endif /* TTIME_H_ */
