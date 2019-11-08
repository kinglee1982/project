/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ֣�������Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : commctl.c
  �� �� ��   : ����
  ��    ��   : wangdayong
  ��������   : 2015��1��27��
  ����޸�   :
  ��������   : �ⲿģ���Դ����λ����
  �����б�   :
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
  �޸���ʷ   :
  1.��    ��   : 2015��1��27��
    ��    ��   : wangdayong
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ⲿ����˵��                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ⲿ����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ȫ�ֱ���                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �궨��                                       *
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
 �� �� ��  : gpio_set_dir
 ��������  : ����GPIO����
 �������  : int group
             int num
             int dir
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��27��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

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
 �� �� ��  : gpio_get_dir
 ��������  : ��ȡGPIO����
 �������  : int group
             int num
             int * dir
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��27��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

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
 �� �� ��  : gpio_set_val
 ��������  : ����GPIO״̬
 �������  : int group
             int num
             int val
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��27��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

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
 �� �� ��  : gpio_get_val
 ��������  : ��ȡGPIO״̬
 �������  : int group
             int num
             int * val
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��27��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

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
 �� �� ��  : COMMCTL_Reset_3G
 ��������  : ��λ3Gģ��
 �������  : void
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��27��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

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
 �� �� ��  : COMMCTL_Power_Stm32
 ��������  : ��Ƭ����Դ���ƺ���
 �������  : int power
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��27��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

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
 �� �� ��  : COMMCTL_Power_Led1
 ��������  : LED1״̬����
 �������  : int power
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��27��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

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
 �� �� ��  : COMMCTL_Power_Led2
 ��������  : LED2״̬����
 �������  : int power
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��27��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

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
 �� �� ��  : COMMCTL_Power_Led3
 ��������  : LED3״̬����
 �������  : int power
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��27��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

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
 �� �� ��  : COMMCTL_Power_InAudio
 ��������  : �����������
 �������  : int power
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��27��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

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
 �� �� ��  : COMMCTL_Power_OutAudio
 ��������  : �����������
 �������  : int power
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��27��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

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
 �� �� ��  : COMMCTL_Power_TsAudio
 ��������  : GSM��������
 �������  : int power
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��27��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

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
 �� �� ��  : COMMCTL_Power_WIFI
 ��������  : WIFIģ���Դ����
 �������  : int power
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��27��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

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
 �� �� ��  : COMMCTL_Power_3G
 ��������  : 3Gģ���Դ����
 �������  : int power
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��27��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

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
 �� �� ��  : COMMCTL_Reset
 ��������  : ģ�鸴λ����
 �������  : int item  Ҫ��λ��ģ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��27��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

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
 �� �� ��  : COMMCTL_Power
 ��������  : �ⲿģ���Դ����
 �������  : int item   ģ����
             int power  ��Դ״̬ 1:�ϵ� 0:�ϵ�
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��27��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

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
 �� �� ��  : COMMCTL_Switch_Aout
 ��������  : �л����AOUT
 �������  : int audio  audio����ENMIC��ENAOUT
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��12��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

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
 �� �� ��  : COMMCTL_SetMicVolume
 ��������  : �����������
 �������  : unsigned char volume ����ֵ
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��4��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

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
 �� �� ��  : COMMCTL_SetMicVolume
 ��������  :��ȡ��ǰ�������ֵ
 �������  : unsigned char volume ����ֵָ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��4��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

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
 �� �� ��  : COMMCTL_SetMicVolume
 ��������  : ������Ƶ�������
 �������  : unsigned char volume ����ֵ
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��4��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

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
 �� �� ��  : COMMCTL_SetMicVolume
 ��������  : ��ȡ��ǰ����
 �������  : unsigned char volume ����ֵ
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��4��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

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

