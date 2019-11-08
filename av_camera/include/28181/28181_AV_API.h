/******************************************************************************

  Copyright (C), 2001-2011, zhengzhou Tiamaes Technology Co., Ltd.

 ******************************************************************************
  File Name     : 28181_AV_API.h
  Version       : Initial Draft
  Author        : mabuyun
  Created       : 2015/6/8
  Last Modified :
  Description   : 28181_AV_API.c header file
  Function List :
  History       :
  1.Date        : 2015/6/8
    Author      : mabuyun
    Modification: Created file

******************************************************************************/

#ifndef __28181_AV_API_H__
#define __28181_AV_API_H__

#include "av_public_head.h"
#include "av_common_31.h"
#include <pthread.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define TM_APP_RESTM_FREE        0
#define TM_APP_RESTM_USE         1

#define TM_APP_AUDIO_MIC_INDEX          24

#define BZQ_OPEN_VOICE_CHN   1
#define BZQ_CLOSE_VOICE_CHN  0
#define BZQ_VOIP_RING_CMD    2

typedef enum Pthread_Task_Status
{
    TASK_IDLE_E = 0,
    TASK_START_E,       /* 任务处于启动阶段 */
    TASK_WORK_E,
    TASK_STOP_E,
    TASK_INVALID_E
}PTHREAD_TASK_STATUS_E;


#define VOIPMAXBUFSIZE  50
#define AUDIOFREAMSIZE  480

typedef struct{
    PTHREAD_TASK_STATUS_E ucRun;   //线程运行状态
    unsigned char ucMicVolume;     //麦克音量
    char rdIndex;         //最新音频数据读下标
    char wrIndex;         //最新音频数据写下标
    char buf[VOIPMAXBUFSIZE][AUDIOFREAMSIZE];              //数据
}TM_APP_VoipCache_st;


extern void init_28181();
extern HI_S32 AVSndBZQCtrlVOIP(HI_U8 pktCmd );
extern HI_S32 SndUIOpenVOIP(HI_VOID);
extern HI_S32 SndUIStopVOIP(HI_VOID);
extern unsigned long long mygetLocalTime();
extern void SetVoipAdecStop();
extern void CpyVoipDataToBuf(HI_U8* buf,int len);
extern HI_S32 VoipAdecEntry();
extern void SetVoipMicVolume(HI_U8 u8Volume);
void initPhoneMutex();
void LockPhoneMutex();
void UnlockPhoneMutex();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __28181_AV_API_H__ */
