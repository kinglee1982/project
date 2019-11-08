/******************************************************************************

                  版权所有 (C), 2001-2011, 郑州天迈科技有限公司

 ******************************************************************************
  文 件 名   : commctl.c
  版 本 号   : 初稿
  作    者   : wangdayong
  生成日期   : 2015年1月27日
  最近修改   :
  功能描述   : 外部模块电源及复位控制
  函数列表   :
              COMMCTL_Power
              COMMCTL_Power_3G
              COMMCTL_Power_GsmAudio
              COMMCTL_Power_HDD
              COMMCTL_Power_InAudio
              COMMCTL_Power_Led1
              COMMCTL_Power_Led2
              COMMCTL_Power_Led3
              COMMCTL_Power_Led4
              COMMCTL_Power_OutAudio
              COMMCTL_Power_PCM2407
              COMMCTL_Power_SD
              COMMCTL_Power_SSD
              COMMCTL_Power_Stm32
              COMMCTL_Power_WIFI
              COMMCTL_Reset
              COMMCTL_ResetNvp6114_1
              COMMCTL_ResetNvp6114_2
              COMMCTL_Reset_3G
              COMMCTL_Reset_Eth0
              COMMCTL_Reset_Eth1
              COMMCTL_Reset_UsbHub
              gpio_get_dir
              gpio_get_val
              gpio_set_dir
              gpio_set_val
  修改历史   :
  1.日    期   : 2015年1月27日
    作    者   : wangdayong
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<sys/ioctl.h>
#include<sys/stat.h>

#include"commctl.h"

static int commctlfd = -1;


static int commctl_get_fd(void)
{
	if(-1 == commctlfd)
	{
		//printf("please open device firstly\n");
	}
	return commctlfd;
}

int COMMCTL_init(void)
{
	commctlfd = commctl_get_fd();
	if(-1 == commctlfd)
	{
		commctlfd = open(GPIO_DEV,O_RDWR);
	}
	return commctlfd;
}
int COMMCTL_exit(void)
{
	close(commctlfd);
	commctlfd = -1;
	return commctlfd;
}
/*****************************************************************************
 函 数 名  : gpio_set_dir
 功能描述  : 设置GPIO方向
 输入参数  : int group
             int num
             int dir
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月27日
    作    者   : wangdayong
    修改内容   : 新生成函数

*****************************************************************************/
int gpio_set_dir(int group,int num,int dir)
{
	int fd = -1;
	int ret = -1;
	gpio_groupbit_info gpio;

	memset(&gpio,0,sizeof(gpio));
	fd = commctl_get_fd();
	if(fd < 0)
	{
		printf("open %s fail\n",GPIO_DEV);
		return fd;
	}

	gpio.groupnumber = group;
	gpio.bitnumber	 = num;
	gpio.value		 = dir;

	ret = ioctl(fd,GPIO_SET_DIR,(void*)&gpio);
	if(ret < 0)
	{
		printf("gpio_cfg error\n");
		return ret;
	}

	return 0;
}

/*****************************************************************************
 函 数 名  : gpio_get_dir
 功能描述  : 读取GPIO方向
 输入参数  : int group
             int num
             int * dir
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月27日
    作    者   : wangdayong
    修改内容   : 新生成函数

*****************************************************************************/
int gpio_get_dir(int group,int num,int * dir)
{
	int fd = -1;
	int ret = -1;
	gpio_groupbit_info gpio;

	memset(&gpio,0,sizeof(gpio));
	fd = commctl_get_fd();
	if(fd < 0)
	{
		printf("open %s fail\n",GPIO_DEV);
		return fd;
	}

	gpio.groupnumber = group;
	gpio.bitnumber	 = num;

	ret = ioctl(fd,GPIO_GET_DIR,(void*)&gpio);
	if(ret < 0)
	{
		printf("gpio_cfg error\n");
		return ret;
	}
	*dir = gpio.value;
	close(fd);
	return 0;
}

/*****************************************************************************
 函 数 名  : gpio_set_val
 功能描述  : 设置GPIO状态
 输入参数  : int group
             int num
             int val
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月27日
    作    者   : wangdayong
    修改内容   : 新生成函数

*****************************************************************************/
int gpio_set_val(int group,int num,int val)
{
	int fd = -1;
	int ret = -1;
	gpio_groupbit_info gpio;

	memset(&gpio,0,sizeof(gpio));
	fd = commctl_get_fd();
	if(fd < 0)
	{
		printf("open %s fail\n",GPIO_DEV);
		return fd;
	}

	gpio.groupnumber = group;
	gpio.bitnumber	 = num;
	gpio.value		 = val;

	ret = ioctl(fd,GPIO_WRITE_BIT,(void*)&gpio);
	if(ret < 0)
	{
		printf("gpio_cfg error\n");
		return ret;
	}

	return 0;
}

/*****************************************************************************
 函 数 名  : gpio_get_val
 功能描述  : 读取GPIO状态
 输入参数  : int group
             int num
             int * val
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月27日
    作    者   : wangdayong
    修改内容   : 新生成函数

*****************************************************************************/
int gpio_get_val(int group,int num,int * val)
{
	int fd = -1;
	int ret = -1;
	gpio_groupbit_info gpio;

	memset(&gpio,0,sizeof(gpio));
	fd = commctl_get_fd();
	if(fd < 0)
	{
		printf("open %s fail\n",GPIO_DEV);
		return fd;
	}

	gpio.groupnumber = group;
	gpio.bitnumber	 = num;

	ret = ioctl(fd,GPIO_READ_BIT,(void*)&gpio);
	if(ret < 0)
	{
		printf("gpio_cfg error\n");
		return ret;
	}
	*val = gpio.value;
	return 0;
}

/*****************************************************************************
 函 数 名  : COMMCTL_Reset_3G
 功能描述  : 复位3G模块
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月27日
    作    者   : wangdayong
    修改内容   : 新生成函数

*****************************************************************************/
int COMMCTL_Reset_3G(void)
{
	int ret = -1;
	ret = gpio_set_dir(GPIO13,BIT7,GPIO_DIR_OUT);
	ret = gpio_set_val(GPIO13,BIT7,GPIO_HIGH);
	usleep(SLEEP_TIME);
	ret = gpio_set_val(GPIO13,BIT7,GPIO_LOW);
	usleep(SLEEP_TIME);
	ret = gpio_set_val(GPIO13,BIT7,GPIO_HIGH);
	return ret;
}



/*****************************************************************************
 函 数 名  : COMMCTL_Power_Stm32
 功能描述  : 单片机电源控制函数
 输入参数  : int power
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月27日
    作    者   : wangdayong
    修改内容   : 新生成函数

*****************************************************************************/
int COMMCTL_Power_Stm32(int power)
{
	int ret = -1;
	ret = gpio_set_dir(GPIO0,BIT3,GPIO_DIR_OUT);
	if(POWER_ON == power)
	{
		ret = gpio_set_val(GPIO0,BIT3,GPIO_LOW);
	}
	else
	{
		ret = gpio_set_val(GPIO0,BIT3,GPIO_HIGH);
	}
	return ret;
}

/*****************************************************************************
 函 数 名  : COMMCTL_Power_Led1
 功能描述  : LED1状态控制
 输入参数  : int power
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月27日
    作    者   : wangdayong
    修改内容   : 新生成函数

*****************************************************************************/
int COMMCTL_Power_Led1(int power)
{
	int ret = -1;
	ret = gpio_set_dir(GPIO5,BIT0,GPIO_DIR_OUT);
	if(POWER_OFF == power)
	{
		ret = gpio_set_val(GPIO5,BIT0,GPIO_HIGH);
	}
	else
	{
		ret = gpio_set_val(GPIO5,BIT0,GPIO_LOW);
	}
	return ret;
}
/*****************************************************************************
 函 数 名  : COMMCTL_Power_Led2
 功能描述  : LED2状态控制
 输入参数  : int power
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月27日
    作    者   : wangdayong
    修改内容   : 新生成函数

*****************************************************************************/
int COMMCTL_Power_Led2(int power)
{
	int ret = -1;
	ret = gpio_set_dir(GPIO5,BIT1,GPIO_DIR_OUT);
	if(POWER_OFF == power)
	{
		ret = gpio_set_val(GPIO5,BIT1,GPIO_HIGH);
	}
	else
	{
		ret = gpio_set_val(GPIO5,BIT1,GPIO_LOW);
	}
	return ret;
}

/*****************************************************************************
 函 数 名  : COMMCTL_Power_Led3
 功能描述  : LED3状态控制
 输入参数  : int power
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月27日
    作    者   : wangdayong
    修改内容   : 新生成函数

*****************************************************************************/
int COMMCTL_Power_Led3(int power)
{
	int ret = -1;
	ret = gpio_set_dir(GPIO5,BIT2,GPIO_DIR_OUT);
	if(POWER_OFF == power)
	{
		ret = gpio_set_val(GPIO5,BIT2,GPIO_HIGH);
	}
	else
	{
		ret = gpio_set_val(GPIO5,BIT2,GPIO_LOW);
	}
	return ret;
}

/*****************************************************************************
 函 数 名  : COMMCTL_Power_InAudio
 功能描述  : 内音输出开关
 输入参数  : int power
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月27日
    作    者   : wangdayong
    修改内容   : 新生成函数

*****************************************************************************/
int COMMCTL_Power_InAudio(int power)
{
	int ret = -1;
	ret = gpio_set_dir(GPIO0,BIT6,GPIO_DIR_OUT);
	if(POWER_ON == power)
	{
		ret = gpio_set_val(GPIO0,BIT6,GPIO_HIGH);
	}
	else
	{
		ret = gpio_set_val(GPIO0,BIT6,GPIO_LOW);
	}
	return ret;
}
/*****************************************************************************
 函 数 名  : COMMCTL_Power_OutAudio
 功能描述  : 外音输出开关
 输入参数  : int power
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月27日
    作    者   : wangdayong
    修改内容   : 新生成函数

*****************************************************************************/
int COMMCTL_Power_OutAudio(int power)
{
	int ret = -1;
	ret = gpio_set_dir(GPIO0,BIT7,GPIO_DIR_OUT);
	if(POWER_ON == power)
	{
		ret = gpio_set_val(GPIO0,BIT7,GPIO_HIGH);
	}
	else
	{
		ret = gpio_set_val(GPIO0,BIT7,GPIO_LOW);
	}
	return ret;
}

/*****************************************************************************
 函 数 名  : COMMCTL_Power_TsAudio
 功能描述  : GSM语音开关
 输入参数  : int power
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月27日
    作    者   : wangdayong
    修改内容   : 新生成函数

*****************************************************************************/
int COMMCTL_Power_TsAudio(int power)
{
	int ret = -1;
	ret = gpio_set_dir(GPIO6,BIT3,GPIO_DIR_OUT);
	if(POWER_ON == power)
	{
		ret = gpio_set_val(GPIO6,BIT3,GPIO_HIGH);
	}
	else
	{
		ret = gpio_set_val(GPIO6,BIT3,GPIO_LOW);
	}
	return ret;
}

/*****************************************************************************
 函 数 名  : COMMCTL_Power_WIFI
 功能描述  : WIFI模块电源控制
 输入参数  : int power
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月27日
    作    者   : wangdayong
    修改内容   : 新生成函数

*****************************************************************************/
int COMMCTL_Power_WIFI(int power)
{
	int ret = -1;
	ret = gpio_set_dir(GPIO0,BIT2,GPIO_DIR_OUT);
	if(POWER_ON == power)
	{
		ret = gpio_set_val(GPIO0,BIT2,GPIO_HIGH);
	}
	else
	{
		ret = gpio_set_val(GPIO0,BIT2,GPIO_LOW);
	}
	return ret;
}


/*****************************************************************************
 函 数 名  : COMMCTL_Power_3G
 功能描述  : 3G模块电源控制
 输入参数  : int power
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月27日
    作    者   : wangdayong
    修改内容   : 新生成函数

*****************************************************************************/
int COMMCTL_Power_3G(int power)
{
	int ret = -1;
	ret = gpio_set_dir(GPIO0,BIT1,GPIO_DIR_OUT);
	if(POWER_ON == power)
	{
		ret = gpio_set_val(GPIO0,BIT1,GPIO_HIGH);
	}
	else
	{
		ret = gpio_set_val(GPIO0,BIT1,GPIO_LOW);
	}
	return ret;
}


/*****************************************************************************
 函 数 名  : COMMCTL_Reset
 功能描述  : 模块复位函数
 输入参数  : int item  要复位的模块
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月27日
    作    者   : wangdayong
    修改内容   : 新生成函数

*****************************************************************************/
int COMMCTL_Reset(int item)
{
	switch(item)
	{
		
		case RESET_3G:
			COMMCTL_Reset_3G();
			break;
		default:
			printf("unknown item %d\n",item);
			break;
	}
}


/*****************************************************************************
 函 数 名  : COMMCTL_Power
 功能描述  : 外部模块电源控制
 输入参数  : int item   模块编号
             int power  电源状态 1:上电 0:断电
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月27日
    作    者   : wangdayong
    修改内容   : 新生成函数

*****************************************************************************/
int COMMCTL_Power(int item,int power)
{
	switch(item)
	{
		case POWER_STM32:
			COMMCTL_Power_Stm32(power);
			break;
		
		case POWER_LED1:
			COMMCTL_Power_Led1(power);
			break;
		case POWER_LED2:
			COMMCTL_Power_Led2(power);
			break;
		case POWER_LED3:
			COMMCTL_Power_Led3(power);
			break;
		case POWER_InAudio:
			COMMCTL_Power_InAudio(power);
			break;
		case POWER_OutAudio:
			COMMCTL_Power_OutAudio(power);
			break;
		case POWER_TSAudio:
			COMMCTL_Power_TsAudio(power);
			break;
		case POWER_3G:
			COMMCTL_Power_3G(power);
			break;
		case POWER_WIFI:
			COMMCTL_Power_WIFI(power);
			break;
		default:
			printf("COMMCTL_Power unknown item %d\n",item);
			break;
	}
}

/*****************************************************************************
 函 数 名  : COMMCTL_Switch_Aout
 功能描述  : 切换麦和AOUT
 输入参数  : int audio  audio等于ENMIC或ENAOUT
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年3月12日
    作    者   : wangdayong
    修改内容   : 新生成函数

*****************************************************************************/
int COMMCTL_Switch_Aout(int audio)
{
	int ret = -1;
	ret = gpio_set_dir(GPIO0,BIT4,GPIO_DIR_OUT);
	if(ENAOUT1 == audio)
	{
		ret = gpio_set_val(GPIO0,BIT4,GPIO_HIGH);
	}
	else if(ENAOUT0== audio)
	{
		ret = gpio_set_val(GPIO0,BIT4,GPIO_LOW);
	}
	return ret;
}
int COMMCTL_Switch_Gsm(int audio)
{
	int ret = -1;
	ret = gpio_set_dir(GPIO0,BIT5,GPIO_DIR_OUT);
	if(ENGSM1 == audio)
	{
		ret = gpio_set_val(GPIO0,BIT5,GPIO_HIGH);
	}
	else if(ENGSM0== audio)
	{
		ret = gpio_set_val(GPIO0,BIT5,GPIO_LOW);
	}
	return ret;
}
/*****************************************************************************
 函 数 名  : COMMCTL_SetMicVolume
 功能描述  : 设置麦克音量
 输入参数  : unsigned char volume 音量值
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年3月4日
    作    者   : wangdayong
    修改内容   : 新生成函数

*****************************************************************************/

int COMMCTL_SetMicVolume(unsigned char volume)
{
	int fd = -1;
	fd = open(MCP4531DEV,O_RDWR);
	if(fd < 0)
	{
		printf("open %s error\n",MCP4531DEV);
		return -1;
	}
	if(volume > 127||volume <0)
	{
		printf("COMMCTL_SetMicVolume error:confirm 0=<volume<=127\n");
		return -1;
	}
	ioctl(fd,IOC_MCP4531_SET_MIC_VOLUME,(void *)&volume);
	close(fd);
	return 0;
}

/*****************************************************************************
 函 数 名  : COMMCTL_SetMicVolume
 功能描述  :获取当前麦克音量值
 输入参数  : unsigned char volume 音量值指针
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年3月4日
    作    者   : wangdayong
    修改内容   : 新生成函数

*****************************************************************************/

int COMMCTL_GetMicVolume(unsigned char *volume)
{
	int fd = -1;
	fd = open(MCP4531DEV,O_RDWR);
	if(fd < 0)
	{
		printf("open %s error\n",MCP4531DEV);
		return -1;
	}
	ioctl(fd,IOC_MCP4531_GET_MIC_VOLUME,(void *)volume);
	close(fd);
	return 0;
}

/*****************************************************************************
 函 数 名  : COMMCTL_SetMicVolume
 功能描述  : 设置音频输出音量
 输入参数  : unsigned char volume 音量值
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年3月4日
    作    者   : wangdayong
    修改内容   : 新生成函数

*****************************************************************************/


int COMMCTL_SetAudioVolume(unsigned char volume)
{
	int fd = -1;
	fd = open(MCP4531DEV,O_RDWR);
	if(fd < 0)
	{
		printf("open %s error\n",MCP4531DEV);
		return -1;
	}
	if(volume > 127||volume <0)
	{
		printf("COMMCTL_SetAudioVolume error:confirm 0=<volume<=127\n");
		return -1;
	}
	ioctl(fd,IOC_MCP4531_SET_AUDIO_VOLUME,(void *)&volume);
	close(fd);
	return 0;
}
/*****************************************************************************
 函 数 名  : COMMCTL_SetMicVolume
 功能描述  : 获取当前音量
 输入参数  : unsigned char volume 音量值
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年3月4日
    作    者   : wangdayong
    修改内容   : 新生成函数

*****************************************************************************/

int COMMCTL_GetAudioVolume(unsigned char *volume)
{
	int fd = -1;
	fd = open(MCP4531DEV,O_RDWR);
	if(fd < 0)
	{
		printf("open %s error\n",MCP4531DEV);
		return -1;
	}
	ioctl(fd,IOC_MCP4531_GET_AUDIO_VOLUME,(void *)volume);
	close(fd);
	return 0;
}

