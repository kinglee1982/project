#include <stdio.h>
#include <ctype.h>
#include<unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<linux/rtc.h>
//#include<linux/bcd.h>
#include <string.h> 

#include "strfunc.h"
#include "i2c.h"
#include "ds3231.h"


using namespace std;

int Rtc::ds3231_read( unsigned char reg, unsigned char *reg_value)
{
	int ret;
	int value;

	value = ((DEVICE_ADDR&0xff)<<24) | ((reg&0xff)<<16);        
	ret = ioctl(fd, I2C_READ, &value);        
	*reg_value = (unsigned char)(value&0xff);        
	// printf("0x%2x\n", *reg_value);
	return 0;
}


int Rtc::ds3231_write ( unsigned char reg,unsigned char  reg_value)
{
	int ret;
	int value;
    
    value = ((DEVICE_ADDR&0xff)<<24) | ((reg&0xff)<<16) | (reg_value&0xffff);
	ret = ioctl(fd, I2C_WRITE, &value);
	
	return 0;
}

/*
 * DS3231时间获取函数
 */
int Rtc::ds3231_get_datetime( struct rtc_time *dt)
{
	unsigned char buf[7];
	unsigned char val;
	unsigned char i = 0;

	int err = 0;

	if (!dt) {
		printf("%s: EINVAL: dt=NULL\n", __FUNCTION__);
		return -22;/* Invalid argument */
	}

	/* 读取寄存器的值*/
	for (i=0; i<7; i++)
	{
		err = ds3231_read(i, &buf[i]);
		if (err != 0)
			break;
	}
	/*
	   printf( "%s: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
	   __FUNCTION__,  buf[0], buf[1], buf[2], buf[3],
	   buf[4], buf[5], buf[6]);
	   */

	if (err == 0) {
		dt->tm_sec = BCD2BIN(buf[0]);
		dt->tm_min = BCD2BIN(buf[1]);
		val = buf[2] & 0x3f;       //RTC 模式是24小时制
		dt->tm_hour = BCD2BIN(val);
		dt->tm_wday = BCD2BIN(buf[3]);
		dt->tm_mday = BCD2BIN(buf[4]);
		val = buf[5] & 0x7f;
		dt->tm_mon = BCD2BIN(val);
		dt->tm_year = BCD2BIN(buf[6]);
		if (buf[5] & 0x80)
			dt->tm_year += 100;
		/*
		   printf("%s: secs=%d, mins=%d, "
		   "hours=%d, mday=%d, mon=%d, year=%d, wday=%d\n",
		   __FUNCTION__, dt->tm_sec, dt->tm_min,
		   dt->tm_hour, dt->tm_mday,
		   dt->tm_mon, dt->tm_year, dt->tm_wday);
		   */
		return 0;
	}

	printf("error reading data! %d\n", err);
	return -5;/*i/o error*/
}


/*
 * DS3231时间设置函数
 */
int Rtc::ds3231_set_datetime(struct rtc_time *dt)
{
	unsigned char buf[7];
	unsigned char val;
	unsigned char i = 0;

	if (!dt) {
		printf("%s: EINVAL: dt=NULL\n", __FUNCTION__);
		return -22;/* Invalid argument */
	}
	/*
	   printf("%s: secs=%d, mins=%d, hours=%d, "
	   "mday=%d, mon=%d, year=%d, wday=%d\n", __FUNCTION__,
	   dt->tm_sec, dt->tm_min, dt->tm_hour,
	   dt->tm_mday, dt->tm_mon, dt->tm_year, dt->tm_wday);
	   */
	buf[0] = BIN2BCD(dt->tm_sec);
	buf[1] = BIN2BCD(dt->tm_min);
	buf[2] = BIN2BCD(dt->tm_hour);
	buf[3] = BIN2BCD(dt->tm_wday);
	buf[4] = BIN2BCD(dt->tm_mday);
	buf[5] = BIN2BCD(dt->tm_mon);
	val = dt->tm_year;
	if (val >= 100) {
		val -= 100;
		buf[5] |= (1 << 7);
	}
	buf[6] = BIN2BCD(val);
	/*
	   printf( "%s: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
	   __FUNCTION__,  buf[0], buf[1], buf[2], buf[3],
	   buf[4], buf[5], buf[6]);
	   */
	for (i=0; i<7; i++)
	{
		ds3231_write(i, buf[i]);
	}
	return 0;
}

 
