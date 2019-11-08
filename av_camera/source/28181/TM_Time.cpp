#include <stdio.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
/*
struct rtc_time
{
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
};*/

void SetSysTime(struct tm &timeNow)
{
	struct rtc_time rtc_tm;
	char time_set_buf[32] = {0};
	//Rtc ds3231;

	memset(&rtc_tm, 0, sizeof (struct rtc_time));
	rtc_tm.tm_year = timeNow.tm_year;
	rtc_tm.tm_mon = timeNow.tm_mon+1;
	rtc_tm.tm_mday = timeNow.tm_mday;
	rtc_tm.tm_hour = timeNow.tm_hour;
	rtc_tm.tm_min = timeNow.tm_min;
	rtc_tm.tm_sec = timeNow.tm_sec;
	//rtc_tm.tm_wday = timeNow.tm_wday;
	//ds3231.ds3231_set_datetime(&rtc_tm);

	memset(&rtc_tm, 0, sizeof (struct rtc_time));
	//ds3231.ds3231_get_datetime( &rtc_tm);

	fprintf(stderr, "Current RTC date/time is %d-%d-%d, %02d:%02d:%02d. zhou %d.\n",
			rtc_tm.tm_year + 1900,rtc_tm.tm_mon , rtc_tm.tm_mday,
			rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec,rtc_tm.tm_wday);

	memset(&time_set_buf, 0, sizeof(time_set_buf));
	snprintf(time_set_buf, 32,"date %02d%02d%02d%02d%04d.%02d &",
		rtc_tm.tm_mon , rtc_tm.tm_mday, rtc_tm.tm_hour,
		rtc_tm.tm_min,rtc_tm.tm_year + 1900, rtc_tm.tm_sec);

	printf("[test]: *************************cmd_buf = %s\n", time_set_buf);

	//system(time_set_buf); //Уʱ
}

