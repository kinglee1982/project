
/******************************************************************************

                  版权所有 (C), 2001-2011, 郑州天迈科技有限公司

 ******************************************************************************
  文 件 名   : drv_app_init.c
  版 本 号   : 初稿
  作    者   :  
  生成日期   : 2012年5月31日星期四
  最近修改   :
  功能描述   : App模块初始化
  函数列表   :
  修改历史   :
  1.日    期   : 2012年5月31日星期四
    作    者   :  
    修改内容   : 创建文件

******************************************************************************/

#include "av_public_head.h"
#include "av_common_31.h"
#include "av_msgque_proc.h"
#include "av_mem.h"
#include "av_video_proc.h"
#include "av_store.h"
#include "av_init.h"

VIDEO_NORM_E   g_enViNorm   = VIDEO_ENCODING_MODE_PAL;   /* 视频输入格式 */
extern AUDIO_DEV g_iAiDev;  /* 音频输入设备号，IP对讲输入设备号 */
HI_VOID *MP3_PlayThread(HI_VOID * pArg);
HI_VOID *APP_SendAudioAndVideoMsgQueThread(HI_VOID *pArg);
HI_VOID *APP_HddManage(HI_VOID *pArg);
HI_S32 APP_InitStart(HI_VOID);
HI_S32 APP_CameraChnInit(HI_VOID);

/*****************************************************************************
 函 数 名  : APP_CreatThread
 功能描述  : App模块线程创建函数
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  : HI_SUCCESS  执行成功
             HI_FAILURE   执行失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月31日星期四
    作    者   :  
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_CreatThread(HI_VOID)
{
    HI_S32 s32Ret;         /* 返回值 */
    //struct sched_param stMp3PlayThreadParam;
    //pthread_attr_t stMp3PlayThreadAttr;

	pthread_t aiMsgQueRcvTid;		/* AI消息接收线程 */
	pthread_t avMsqQueSendTid;		/* AV消息发送线程 */
	pthread_t mp3PlayTid; 			/* MP3播放线程ID */

    //HI_S32 nSchPolicy;
    //HI_S32 iMaxPolicy = 0;
    //HI_S32 iMinPolicy = 0;

    /* 创建音频消息队列接收线程 */
    s32Ret = pthread_create(&aiMsgQueRcvTid, NULL, APP_RcvAudioMsgQueThread, NULL);
    if (0 > s32Ret)
	{
        AV_PRINT( "#[av]Creat audio msg que receive thread failed!");

        return HI_FAILURE;
	}
    s32Ret = pthread_detach(aiMsgQueRcvTid);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("detach APP_RcvAudioMsgQueThread[tid-%d] failed, err-%s!", (HI_S32)aiMsgQueRcvTid, ERR_STR);
        return HI_FAILURE;
    }
#if 0
    /* 创建视频消息队列接收线程 */
    s32Ret = pthread_create(&aiMsgQueRcvTid, NULL, APP_RcvVideoMsgQueThread, NULL);
	if (0 > s32Ret)
	{
        AV_PRINT( "#[av]Creat video msg que receive thread failed!");

        return HI_FAILURE;
	}
#endif
    s32Ret = pthread_create(&avMsqQueSendTid, NULL, APP_SendAudioAndVideoMsgQueThread, NULL);
	if (0 > s32Ret)
	{
        AV_PRINT( "#[av]Creat audiio and video msg que audio process thread failed!");

        return HI_FAILURE;
	}
    s32Ret = pthread_detach(avMsqQueSendTid);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("detach APP_SendAudioAndVideoMsgQueThread[tid-%d] failed, err-%s!", (HI_S32)avMsqQueSendTid, ERR_STR);
        return HI_FAILURE;
    }
    /* 时间OSD叠加 */
    //s32Ret = pthread_create(&osdTid, NULL, APP_ViTimeOSDThread, NULL);
	//if (0 > s32Ret)
	//{
    //    AV_PRINT( "#[app]Creat VI time osd thread failed!");

    //    return HI_FAILURE;
	//}
#if 0
    /* 创建MP3播放线程 */
    pthread_attr_init(&stMp3PlayThreadAttr);
    pthread_attr_setschedpolicy(&stMp3PlayThreadAttr,SCHED_RR);
    pthread_attr_getschedpolicy(&stMp3PlayThreadAttr, &nSchPolicy);
    if (nSchPolicy == SCHED_RR)
    {
        AV_PRINT( "#[av]nSchPolicy == SCHED_RR!");
        iMaxPolicy = sched_get_priority_max(nSchPolicy);
        iMinPolicy = sched_get_priority_min(nSchPolicy);
        AV_PRINT( "#[av]SCHED_RR sched_get_priority_max = %d, iMinPolicy= %d!",
                    iMaxPolicy, iMinPolicy);
    }
    if (nSchPolicy == SCHED_FIFO)
    {
        AV_PRINT( "#[av]nSchPolicy == SCHED_FIFO!");
        iMaxPolicy = sched_get_priority_max(nSchPolicy);
        iMinPolicy = sched_get_priority_min(nSchPolicy);
        AV_PRINT( "#[av]SCHED_FIFO sched_get_priority_max = %d, iMinPolicy= %d!",
                    iMaxPolicy, iMinPolicy);
    }

    if (nSchPolicy == SCHED_OTHER )
    {
        AV_PRINT( "#[av]nSchPolicy == SCHED_OTHER !");
        iMaxPolicy = sched_get_priority_max(nSchPolicy);
        iMinPolicy = sched_get_priority_min(nSchPolicy);
        AV_PRINT( "#[av]SCHED_FIFO sched_get_priority_max = %d, iMinPolicy= %d!",
                    iMaxPolicy, iMinPolicy);
    }
    pthread_attr_getschedparam(&stMp3PlayThreadAttr,&stMp3PlayThreadParam);
    stMp3PlayThreadParam.sched_priority = 99;
    pthread_attr_setschedparam(&stMp3PlayThreadAttr,&stMp3PlayThreadParam);
    //do not inherit father's attr
    pthread_attr_setinheritsched(&stMp3PlayThreadAttr,PTHREAD_EXPLICIT_SCHED);
    s32Ret = pthread_create(&mp3PlayTid, &stMp3PlayThreadAttr, MP3_PlayThread, NULL);
    if (0 > s32Ret)
	{
        AV_PRINT( "#[av]Creat MP3 play thread failed!");

        return HI_FAILURE;
	}
#endif
    s32Ret = pthread_create(&mp3PlayTid, NULL, MP3_PlayThread, NULL);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("create APP_CreatThread[tid-%d] failed, err-%s!", (HI_S32)mp3PlayTid, ERR_STR);
        return HI_FAILURE;
    }
    s32Ret = pthread_detach(mp3PlayTid);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("detach APP_CreatThread[tid-%d] failed, err-%s!", (HI_S32)mp3PlayTid, ERR_STR);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

