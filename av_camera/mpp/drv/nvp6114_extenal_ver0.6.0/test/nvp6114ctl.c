/******************************************************************************

                  版权所有 (C), 2001-2011, 郑州天迈科技有限公司

 ******************************************************************************
  文 件 名   : nvp6114ctl.c
  版 本 号   : 初稿
  作    者   : wangdayong
  生成日期   : 2015年1月27日
  最近修改   :
  功能描述   : nvp6114应用层接口文件
  函数列表   :
              set_brightness
              set_contrast
              set_hue
              set_saturation
              set_video_mode
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
#include<stdlib.h>
#include"nvp6114.h"
#define NVP6114DEV  "/dev/nc_vdec"

/*****************************************************************************
 函 数 名  : set_brightness
 功能描述  : 设置ad亮度值
 输入参数  : int chn        通道号  0-7
             signed char bright  亮度值0-100
 输出参数  : 无
 返 回 值  : 当前亮度值
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月27日
    作    者   : wangdayong
    修改内容   : 新生成函数

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
 函 数 名  : set_contrast
 功能描述  : 设置ad对比度
 输入参数  : int chn               通道号 0-7
             signed char contrast  对比度值0-100
 输出参数  : 无
 返 回 值  : 当前对比度值
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月27日
    作    者   : wangdayong
    修改内容   : 新生成函数

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
 函 数 名  : set_hue
 功能描述  : 设置ad饱和度
 输入参数  : int chn          通道号0-7
             signed char hue  饱和度值0-100
 输出参数  : 无
 返 回 值  : 当前亮度值
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月27日
    作    者   : wangdayong
    修改内容   : 新生成函数

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
 函 数 名  : set_saturation
 功能描述  : 设置AD色度值
 输入参数  : int chn      通道号0-7
             signed char saturation  色度值0-100
 输出参数  : 无
 返 回 值  : 当前色度值
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月27日
    作    者   : wangdayong
    修改内容   : 新生成函数

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
 函 数 名  : set_video_mode
 功能描述  : 设置视频模式
 输入参数  : int chip  片选 0或1
             int mode  视频模式NTSC 或PAL
 输出参数  : 无
 返 回 值  : 当前模式
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年1月27日
    作    者   : wangdayong
    修改内容   : 新生成函数

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

