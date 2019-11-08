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

#include"ds3231.h"
#if 0
struct rtc_time {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
};

#endif
using namespace std;
int main(int argc , char* argv[])
{
	struct rtc_time rtc_tm;
	char time_set_buf[32] = {0};
	Rtc ds3231;
	
	if(1 == argc)
	{
		//如果不传参数，就是读取ds3231里的值，并且设置系统时间
		memset(&rtc_tm, 0, sizeof (struct rtc_time));
		if (ds3231.ds3231_get_datetime(&rtc_tm))
		{
			printf("#[rtc]: rtc get date time error!\n");
			system("echo get time error! >> /mnt/nand/rtc_error_log &");
			system("date >> /mnt/nand/rtc_error_log &");
			return -EFAULT;
		}
		
		fprintf(stderr, "Current RTC date/time is %d-%d-%d, %02d:%02d:%02d. zhou %d.\n",  
				rtc_tm.tm_year + 1900,rtc_tm.tm_mon , rtc_tm.tm_mday,  
				rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec,rtc_tm.tm_wday); 
		
		memset(&time_set_buf, 0, sizeof(time_set_buf));
		snprintf(time_set_buf, 32,"date %02d%02d%02d%02d%04d.%02d &",\
				rtc_tm.tm_mon , rtc_tm.tm_mday, rtc_tm.tm_hour, \
				rtc_tm.tm_min,rtc_tm.tm_year + 1900, rtc_tm.tm_sec); 
				
		printf("[test]: *************************cmd_buf = %s\n", time_set_buf);
		system(time_set_buf); //校时

		
		
		//下面获取一下系统时间，与rtc时间做一个比较
		usleep(1000*1000);
		time_t timep;  
		struct tm *p_tm;  
		timep = time(NULL);  
		p_tm = localtime(&timep); /*获取本地时区时间*/  
		char cmd_buf[128] = {0};
		
		if ( p_tm->tm_year !=  rtc_tm.tm_year) //由于他们都是+1900，所以这里不再加1900
		{
		    //system("date");
			printf("#[rtc]: in rtc, systime is not same as rtctime!\n");
			printf("system time year: %d, rtc time year: %d \n", (p_tm->tm_year+1900),( rtc_tm.tm_year +1900));
			
			snprintf(cmd_buf, sizeof(cmd_buf),"echo rtc set sys time: %s >> /mnt/nand/rtc_error_log &", time_set_buf);
		    char *p =  index(cmd_buf, '&');
			p[0] = 0x20;
			system(cmd_buf);
			
			snprintf(cmd_buf, sizeof(cmd_buf),"echo read system time year: %d, rtc time year: %d >> /mnt/nand/rtc_error_log &", (p_tm->tm_year+1900),( rtc_tm.tm_year +1900));
			system(cmd_buf);
			

			system("date >> /mnt/nand/rtc_error_log &");			
		}
	}
	else if(2 == argc)  //只读取-不设置系统时间
	{
		//如果不传参数，就是读取ds3231里的值，并且设置系统时间
		memset(&rtc_tm, 0, sizeof (struct rtc_time));
		if (ds3231.ds3231_get_datetime(&rtc_tm))
		{
			printf("#[rtc]: rtc get date time error!\n");
			system("echo get time error! >> /mnt/nand/rtc_error_log &");
			system("date >> /mnt/nand/rtc_error_log &");
			return -EFAULT;
		}
		
		fprintf(stderr, "Current RTC date/time is %d-%d-%d, %02d:%02d:%02d. zhou %d.\n",  
				rtc_tm.tm_year + 1900,rtc_tm.tm_mon , rtc_tm.tm_mday,  
				rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec,rtc_tm.tm_wday); 
	}
	else if(8 == argc)
	{
	//如果传参数，就是设置ds3231里的值,并且设置
		memset(&rtc_tm, 0, sizeof (struct rtc_time));
		rtc_tm.tm_year=atoi(argv[1])-1900;
		rtc_tm.tm_mon=atoi(argv[2]);
		rtc_tm.tm_mday=atoi(argv[3]);	
		rtc_tm.tm_hour=atoi(argv[4]);
		rtc_tm.tm_min=atoi(argv[5]);
		rtc_tm.tm_sec=atoi(argv[6]);
		rtc_tm.tm_wday=atoi(argv[7]);
		ds3231.ds3231_set_datetime(&rtc_tm);
		//sleep(1);
		fprintf(stderr, "##########Current RTC date/time is %d-%d-%d, %02d:%02d:%02d. zhou %d.\n",  
				rtc_tm.tm_year + 1900,rtc_tm.tm_mon , rtc_tm.tm_mday,  
				rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec,rtc_tm.tm_wday); 
		memset(&rtc_tm, 0, sizeof (struct rtc_time));
		if (ds3231.ds3231_get_datetime( &rtc_tm))
			return -EFAULT;
	fprintf(stderr, "$$$$$$$$Current RTC date/time is %d-%d-%d, %02d:%02d:%02d. zhou %d.\n",  
			rtc_tm.tm_year + 1900,rtc_tm.tm_mon , rtc_tm.tm_mday,   
			rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec,rtc_tm.tm_wday);
			
	memset(&time_set_buf, 0, sizeof(time_set_buf));
	snprintf(time_set_buf, 32,"date %02d%02d%02d%02d%04d.%02d &",\
			rtc_tm.tm_mon , rtc_tm.tm_mday, rtc_tm.tm_hour, \
			rtc_tm.tm_min,rtc_tm.tm_year + 1900, rtc_tm.tm_sec); 

	printf("[test]: *************************cmd_buf = %s\n", time_set_buf);
	system(time_set_buf); //校时 
	}
	else
	{
		printf("usage: %s <tm_year> <tm_mon> <tm_mday> <tm_hour> <tm_min> <tm_sec> <tm_wday>.\
				sample: %s 2013 11 12 13 45 35\n", argv[0], argv[0]);
		return -1;
	}	
	return 0;
}
