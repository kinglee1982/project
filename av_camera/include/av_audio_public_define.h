/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : audio_public_define.h
  Version       : Initial Draft
  Author        : wuquanwei
  Created       : 2016/5/24
  Last Modified :
  Description   : 定义音频模块使用的功能选择宏与使用的结构体
  Function List :
  History       :
  1.Date        : 2016/5/24
    Author      : wuquanwei
    Modification: Created file

******************************************************************************/
#ifndef __AUDIO_PUBLIC_DEFINE_H__
#define __AUDIO_PUBLIC_DEFINE_H__

/****************************************hisi head file*/
#include "hi_comm_aio.h"
#include "hi_type.h"
#include "mpi_ai.h"
#include "mpi_ao.h"
#include "mpi_sys.h"
#include "mpi_aenc.h"
#include "mpi_adec.h"
#include "mpi_vb.h"
#include "hi_comm_aenc.h"
#include "hi_comm_adec.h"
#include "hi_comm_vdec.h"
/*******************************************************/


/*******************************************************************************/



#define HI_AI_MIC_CHN 12 /*mic*/
#define HI_AI_MAX_CHN (13)/*(16)*/
#define HI_AENC_MAX_CHN (13)       /*AENC 10 */
#define HI_VI_MAX_CHN (12)           /*最大视频输入通道数 */
#define HI_ENABLE (1)               /*已初始化*/
#define HI_DISABLE (0)              /*未初始化*/

#define HI_MP3_PLAY_PTNUMPERFRM   576  /*每帧采样点数--MP3播放使用*/
#define VOIP_DATA_PER_FRAM      (320) //每帧采样点数默认值--ip对讲是320

#define REC_AUDIO_SAMP_PER_FRAME	(480)  /*录像每帧的采样点*/


#define AENC_BUF_LEN							(1088)		/*单位BYTE*/
#define CHK_VENC_INTERVAL						(10000)		/*检查获取码流间隔，单位SEC*/
#define AENC_WAIT_TIME		(20000)	/* USEC */

#endif
