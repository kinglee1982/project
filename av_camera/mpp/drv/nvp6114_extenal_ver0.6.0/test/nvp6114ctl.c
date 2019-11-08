/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ֣�������Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : nvp6114ctl.c
  �� �� ��   : ����
  ��    ��   : wangdayong
  ��������   : 2015��1��27��
  ����޸�   :
  ��������   : nvp6114Ӧ�ò�ӿ��ļ�
  �����б�   :
              set_brightness
              set_contrast
              set_hue
              set_saturation
              set_video_mode
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
#include<stdlib.h>
#include"nvp6114.h"
#define NVP6114DEV  "/dev/nc_vdec"

/*****************************************************************************
 �� �� ��  : set_brightness
 ��������  : ����ad����ֵ
 �������  : int chn        ͨ����  0-7
             signed char bright  ����ֵ0-100
 �������  : ��
 �� �� ֵ  : ��ǰ����ֵ
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��27��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

*****************************************************************************/
int set_brightness(int chn,signed char bright)
{
	int fd = -1;
	int ret = -1;
	int temp = 0;
	int cmd = IOC_VDEC_SET_BRIGHTNESS;
	vdec_video_adjust adjust;
	fd = open(NVP6114DEV,O_RDWR);
	if(fd < 0)
	{
		printf("open %s error\n",NVP6114DEV);
		return -1;
	}
	adjust.ch    = chn;
	temp = bright*255/100-128;
	adjust.value = temp;
	ret = ioctl(fd,cmd,(void*)&adjust);
	if(ret < 0)
	{
		printf("set brightness fail\n");
		return -1;
	}

	return bright;
}

/*****************************************************************************
 �� �� ��  : set_contrast
 ��������  : ����ad�Աȶ�
 �������  : int chn               ͨ���� 0-7
             signed char contrast  �Աȶ�ֵ0-100
 �������  : ��
 �� �� ֵ  : ��ǰ�Աȶ�ֵ
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��27��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

*****************************************************************************/
int set_contrast(int chn,signed char contrast)
{
	int fd = -1;
	int ret = -1;
	int temp = 0;
	int cmd = IOC_VDEC_SET_CONTRAST;
	vdec_video_adjust adjust;
	fd = open(NVP6114DEV,O_RDWR);
	if(fd < 0)
	{
		printf("open %s error\n",NVP6114DEV);
		return -1;
	}
	adjust.ch    = chn;
	temp = contrast*255/100-128;
	adjust.value = temp;
	ret = ioctl(fd,cmd,(void*)&adjust);
	if(ret < 0)
	{
		printf("set brightness fail\n");
		return -1;
	}

	return contrast;
}


/*****************************************************************************
 �� �� ��  : set_hue
 ��������  : ����ad���Ͷ�
 �������  : int chn          ͨ����0-7
             signed char hue  ���Ͷ�ֵ0-100
 �������  : ��
 �� �� ֵ  : ��ǰ����ֵ
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��27��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

*****************************************************************************/
int set_hue(int chn,signed char hue)
{
	int fd = -1;
	int ret = -1;
	int temp = 0;
	int cmd = IOC_VDEC_SET_HUE;
	vdec_video_adjust adjust;
	fd = open(NVP6114DEV,O_RDWR);
	if(fd < 0)
	{
		printf("open %s error\n",NVP6114DEV);
		return -1;
	}
	adjust.ch    = chn;
	temp = hue*255/100;
	adjust.value = temp;
	ret = ioctl(fd,cmd,(void*)&adjust);
	if(ret < 0)
	{
		printf("set hue fail\n");
		return -1;
	}

	return hue;
}

/*****************************************************************************
 �� �� ��  : set_saturation
 ��������  : ����ADɫ��ֵ
 �������  : int chn      ͨ����0-7
             signed char saturation  ɫ��ֵ0-100
 �������  : ��
 �� �� ֵ  : ��ǰɫ��ֵ
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��27��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

*****************************************************************************/
int set_saturation(int chn,signed char saturation)
{
	int fd = -1;
	int ret = -1;
	int temp = 0;
	int cmd = IOC_VDEC_SET_HUE;
	vdec_video_adjust adjust;
	fd = open(NVP6114DEV,O_RDWR);
	if(fd < 0)
	{
		printf("open %s error\n",NVP6114DEV);
		return -1;
	}
	adjust.ch    = chn;
	temp = saturation*255/100;
	adjust.value = temp;
	ret = ioctl(fd,cmd,(void*)&adjust);
	if(ret < 0)
	{
		printf("set saturation fail\n");
		return -1;
	}

	return saturation;
}


/*****************************************************************************
 �� �� ��  : set_video_mode
 ��������  : ������Ƶģʽ
 �������  : int chip  Ƭѡ 0��1
             int mode  ��ƵģʽNTSC ��PAL
 �������  : ��
 �� �� ֵ  : ��ǰģʽ
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��27��
    ��    ��   : wangdayong
    �޸�����   : �����ɺ���

*****************************************************************************/
int set_video_mode(int chip,int mode)
{
	int fd = -1;
	int ret = -1;
	int temp = 0;
	int cmd = IOC_VDEC_SET_VIDEO_MODE;
	vdec_video_mode adjust;
	fd = open(NVP6114DEV,O_RDWR);
	if(fd < 0)
	{
		printf("open %s error\n",NVP6114DEV);
		return -1;
	}
	adjust.chip    = chip;
	adjust.mode = mode;
	ret = ioctl(fd,cmd,(void*)&adjust);
	if(ret < 0)
	{
		printf("set mode fail\n");
		return -1;
	}

	return mode;
}

