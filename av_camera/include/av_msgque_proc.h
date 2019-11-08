/******************************************************************************

                  版权所有 (C), 2001-2024, 郑州天迈科技有限公司

 ******************************************************************************
  文 件 名   : av_msgque_proc.h
  版 本 号   : 初稿
  作    者   :  
  生成日期   : 2012年6月1日星期五
  最近修改   :
  功能描述   : drv_app_msgqueproc.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2012年6月1日星期五
    作    者   :  
    修改内容   : 创建文件

******************************************************************************/
#ifndef __AV_MSGQUE_PROC_H__
#define __AV_MSGQUE_PROC_H__


#define  APP_MSG_QUE_PRIORITY     3       /* 优先级种类 */
#define  APP_MSG_QUE_CNT          16      /* 每个消息队列容纳的最大消息条数 */
#define  APP_MSG_QUE_DATA_SIZE    1044    /* 单条消息数据长度 */

typedef struct APP_MSG_QUE_CTLDATA
{
    HI_S32 s32Counter;               /* 消息计数 */
    HI_S32 s32NewestIndex;           /* 最新接收到的消息下标值 */
    HI_S32 s32OldestIndex;           /* 最旧接收到的消息下标值 */
}APP_MSG_CTLDATA_S;

extern HI_S32 APP_MsgQueModeInit(HI_VOID);
extern HI_VOID *APP_RcvAudioMsgQueThread(HI_VOID *pArg);
extern HI_VOID *APP_SendAudioMsgQueThread(HI_VOID *pArg);
extern HI_VOID *APP_RcvVideoMsgQueThread(HI_VOID *pArg);
extern HI_VOID *APP_SendVideoMsgQueThread(HI_VOID *pArg);
extern HI_S32 HI3521_MsgQueModeInit(HI_VOID);
extern HI_S32 HI3521_CreateAiMsgQueRcvThread(HI_VOID);
extern HI_S32 HI3521_CreateViMsgQueRcvThread(HI_VOID);
extern HI_S32 HI3521_CreateAVMsgQueSndThread(HI_VOID);

#define TEST_MSGQ
#ifdef TEST_MSGQ
/*发送消息队列测试接口 */
extern HI_VOID  HI3531_SndMsgQueData(HI_S32 s32MainType , HI_S32 s32MinorType);
#endif

#endif /* __AV_MSGQUE_PROC_H__ */

