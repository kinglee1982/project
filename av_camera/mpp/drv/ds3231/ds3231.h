#ifndef _DS3231_H_
#define _DS3231_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "strfunc.h"
#include "i2c.h"

#ifndef BCD_TO_BIN
#define BCD2BIN(val)	(((val) & 0x0f) + ((val)>>4)*10)
#define BIN2BCD(val)	((((val)/10)<<4) + (val)%10)

/* backwards compat */
#define BCD_TO_BIN(val) ((val)=BCD2BIN(val))
#define BIN_TO_BCD(val) ((val)=BIN2BCD(val))
#endif
#define  DEVICE_ADDR 0xd0
class  Rtc
{
	public:
		Rtc()
		{
			fd = open("/dev/i2c", O_RDWR);
			if (fd<0)
			{
				printf("Open gpioi2c dev error!\n");
				exit(1);
			}
		};
		~Rtc()
		{
			if(fd > 0)
			{
				close(fd);
			}
		};
		int ds3231_get_datetime( struct rtc_time *dt);
		int ds3231_set_datetime(struct rtc_time *dt);
	private:
		int fd;
		int ds3231_read( unsigned char reg, unsigned char *reg_value);
		int ds3231_write ( unsigned char reg, unsigned char reg_value);


};

#endif
