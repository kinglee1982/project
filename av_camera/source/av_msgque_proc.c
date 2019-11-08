/******************************************************************************

                  版权所有 (C), 2001-2011, 郑州天迈科技有限公司

 ******************************************************************************
  文 件 名   : drv_app_msgqueproc.c
  版 本 号   : 初稿
  作    者   :
  生成日期   : 2012年5月31日星期四
  最近修改   :
  功能描述   : 消息队列处理文件
  函数列表   :
  修改历史   :
  1.日    期   : 2012年5月31日星期四
    作    者   :
    修改内容   : 创建文件

******************************************************************************/
#include "av_public_head.h"
#include "av_common_31.h"
#include "mp3dec.h"
#include "av_public_ctrl_cmd.h"
#include "av_mp3_play.h"
#include "av_msgque_proc.h"
#include "av_video_proc.h"

HI_S32 g_iAudioMsgQueID;                   /* 音频消息队列ID值 */
HI_S32 g_iVideoMsgQueID;                   /* 视频消息队列ID值 */
HI_S32 g_iParaSetMsgQueID;                 /* 参数设置消息队列ID值 */
HI_S32 g_iDataSyncMsgQueID;                /* 进程间数据同步消息队列 */

static MSG_QUE_BUF_DATA_T g_stMsgQueAudioMsg[APP_MSG_QUE_PRIORITY][APP_MSG_QUE_CNT];

APP_MSG_CTLDATA_S g_stAudioMsgQueCtlMsg[APP_MSG_QUE_PRIORITY];  /* 音频消息队列控制队列本地缓冲区数据 */

static MSG_QUE_BUF_DATA_T g_stMsgQueVideoMsg[APP_MSG_QUE_PRIORITY][APP_MSG_QUE_CNT];

APP_MSG_CTLDATA_S g_stVideoMsgQueCtlMsg[APP_MSG_QUE_PRIORITY];  /* 视频消息队列控制队列本地缓冲区数据 */

APP_MSG_CTLDATA_S g_stParaSetMsgQueCtlMsg[APP_MSG_QUE_PRIORITY];  /* 参数设置消息队列控制队列本地缓冲区数据 */

pthread_mutex_t g_AudioMutexLock; /* 音频数据控制互斥锁 */
pthread_mutex_t g_VideoMutexLock; /* 视频数据控制互斥锁 */
pthread_mutex_t g_ParaSetMutexLock; /* 参数设置数据控制互斥锁 */
pthread_rwlock_t g_RecMemRWLock;  /* 录像缓冲区读写锁 */

/*****************************************************************************
 函 数 名  : APP_MSG_QUE_AddData
 功能描述  : 将获取的消息队列加入到本地消息缓冲区中
 输入参数  : const HI_CHAR *pcData
             APP_MSG_QUE_BUFFERDATA_t *pstBufferData
             APP_MSG_QUE_CTLDATA_t *cpBuffer

 输出参数  : 无
 返 回 值  : HI_SUCCESS
             HI_FAILURE
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月24日星期四
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_MSG_QUE_AddData(const HI_CHAR *pcData,
                                          MSG_QUE_BUF_DATA_T *ppstBufferData,
                                          APP_MSG_CTLDATA_S *pstCtlData
                                         )
{
    HI_S32 iMsgPrioity = 0;
    /* 从消息队列中读取的消息 */
    MSG_QUE_BUF_DATA_T *pstMsgQueBuffer = NULL;
    /* 消息队列控制消息 */
    APP_MSG_CTLDATA_S   *pstMsgQueCtlData = NULL;
    HI_S32 iMemoryMove = 0;

    if  ((NULL == pcData) || (NULL == ppstBufferData) || (NULL == pstCtlData))
    {
		AV_PRINT("para is invalid.");
        return HI_FAILURE;
    }

    pstMsgQueBuffer = (MSG_QUE_BUF_DATA_T *)pcData;
    iMsgPrioity = (pstMsgQueBuffer->s32Prio >> 4);

    if  ((0 > iMsgPrioity) || (iMsgPrioity > 2))
    {
		AV_PRINT( "#[app]Msg que prioity error!iMsgPrioity = %d", iMsgPrioity);

        return HI_FAILURE;
    }

    pstMsgQueCtlData = pstCtlData + iMsgPrioity;

    if ((0 > pstMsgQueCtlData->s32Counter) ||
        (APP_MSG_QUE_CNT < pstMsgQueCtlData->s32Counter) ||
        (0 > pstMsgQueCtlData->s32NewestIndex) ||
        (APP_MSG_QUE_CNT <= pstMsgQueCtlData->s32NewestIndex) ||
        (0 > pstMsgQueCtlData->s32OldestIndex) ||
        (APP_MSG_QUE_CNT <= pstMsgQueCtlData->s32OldestIndex))
    {
		AV_PRINT( "#[app]Msg que ctl data error, counter=%d, newestindex=%d, oldestindex=%d",
                   pstMsgQueCtlData->s32Counter, pstMsgQueCtlData->s32NewestIndex,
                   pstMsgQueCtlData->s32OldestIndex);

        return HI_FAILURE;
    }

    if (APP_MSG_QUE_CNT > pstMsgQueCtlData->s32Counter)
    {
        pstMsgQueCtlData->s32Counter = pstMsgQueCtlData->s32Counter + 1;
    }
    else
    {
        pstMsgQueCtlData->s32Counter = APP_MSG_QUE_CNT;
    }

    iMemoryMove = (iMsgPrioity * APP_MSG_QUE_CNT ) + pstMsgQueCtlData->s32NewestIndex;
    memcpy((HI_CHAR *)((HI_U32)ppstBufferData+ APP_MSG_QUE_DATA_SIZE*iMemoryMove),
           pstMsgQueBuffer,
           APP_MSG_QUE_DATA_SIZE);
    /* 应该放置的拷贝的消息，放入数据后，进行更新下标值 */
    pstMsgQueCtlData->s32NewestIndex = (pstMsgQueCtlData->s32NewestIndex + 1) % APP_MSG_QUE_CNT;

    return HI_SUCCESS;

}

/*****************************************************************************
 函 数 名  : APP_ReadLocalBuffer
 功能描述  : 更新当前正在运行的消息
 输入参数  : APP_MSG_QUE_BUFFERDATA_t *pstBufferData      本地消息命令缓冲区
             APP_MSG_QUE_CTLDATA_t *pstCtlData            本地消息缓冲区控制数据
             APP_MSG_QUE_BUFFERDATA_t *pstCurrentMsgCMD   当前运行的消息命令
             pthread_mutex_t *pstMutexLock
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月25日星期五
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_ReadLocalBuffer(MSG_QUE_BUF_DATA_T *ppstBufferData,
                                        APP_MSG_CTLDATA_S *pstCtlData,
                                        MSG_QUE_BUF_DATA_T *pstCurrentMsgCMD)
{
    HI_S32 i;
    HI_S32 iMsgCounter;
    HI_S32 s32OldestIndex;
    HI_S32 iMemoryMove = 0;
    APP_MSG_CTLDATA_S  *pstTempCtlData = NULL; /* 消息控制数据 */
    MSG_QUE_BUF_DATA_T *pstCurrentCMD = NULL; /* 当前正在运行的消
                                                           息命令 */

    if ((NULL == ppstBufferData) ||
        (NULL == pstCtlData) ||
        (NULL == pstCurrentMsgCMD))
    {
		AV_PRINT("Input null pointer, p1=0x%x, p2=0x%x, p3=0x%x",
                   (HI_U32)ppstBufferData, (HI_U32)pstCtlData,
                   (HI_U32)pstCurrentMsgCMD);

        return HI_FAILURE;
    }

    pstCurrentCMD = pstCurrentMsgCMD;

    for (i = 0; i < APP_MSG_QUE_PRIORITY; i++)
    {
        pstTempCtlData = pstCtlData + i;

        iMsgCounter = pstTempCtlData->s32Counter;
        s32OldestIndex = pstTempCtlData->s32OldestIndex;
        /* 检查数据合法性 */
        if ((0 > iMsgCounter) ||
            (APP_MSG_QUE_CNT < iMsgCounter) ||
            (0 > s32OldestIndex) ||
            (APP_MSG_QUE_CNT <= s32OldestIndex))
        {
    		AV_PRINT("Get ctl data failed!p1=%d, p2=%d, p3=%d",
                       pstTempCtlData->s32Counter, pstTempCtlData->s32OldestIndex,
                       pstTempCtlData->s32NewestIndex);

            /* 数据初始化 */
            pstTempCtlData->s32Counter = 0;
            pstTempCtlData->s32OldestIndex = 0;
            pstTempCtlData->s32NewestIndex = 0;

            return HI_FAILURE;
        }

        if (0 == iMsgCounter)
        {
            continue;
        }

        iMemoryMove = i*APP_MSG_QUE_CNT +s32OldestIndex;
        memcpy(pstCurrentCMD, (HI_CHAR *)((HI_U32)ppstBufferData + APP_MSG_QUE_DATA_SIZE*iMemoryMove),
               APP_MSG_QUE_DATA_SIZE);
        pstTempCtlData->s32OldestIndex = (s32OldestIndex + 1) % APP_MSG_QUE_CNT;
        pstTempCtlData->s32Counter--;

        return HI_SUCCESS;
    }

    return HI_FAILURE;

}

/*****************************************************************************
 函 数 名  : APP_RcvMsgQueThread
 功能描述  : 消息队列接收线程
 输入参数  : HI_VOID *pArg
 输出参数  : 无
 返 回 值  : HI_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月24日星期四
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID *APP_RcvAudioMsgQueThread(HI_VOID *pArg)
{
    MSG_QUE_T sQueMsg;    /* 单条消息队列对应结构体 */
    MSG_QUE_BUF_DATA_T *pstCurrentCMD;
    HI_S32 s32Ret;
    HI_S32 s32Counter = 0;
    HI_S32 iProceRet;
    key_t iKey;                   /* 消息队列对应的键值 */

    (HI_VOID)pArg;
	AV_PRINT("PID@%s pid is %d", __FUNCTION__,getpid());

    for (; ; )
    {
        do
        {
            //memset(&sQueMsg, 0, sizeof(MSG_QUE_T));

            /* 只接收APP_MSG_QUE_AUDIO_CMD_TYPE,采用阻塞式发送 */
            s32Ret = msgrcv(g_iAudioMsgQueID, &sQueMsg, sizeof(sQueMsg.cBuffer),
                          APP_AUDIO_RCV_MSG_TYPE, 0);

            if (0 > s32Ret)
            {
        		AV_PRINT("App receive msg failed,iret = %d", s32Ret);
                /* 获取音频消息队列键值 */
                if ((iKey = ftok(IPC_PATH, 0x20)) == -1)
                {
            		AV_PRINT("Creat audio Key Error, ikey = %d", iKey);

                    //return HI_FAILURE;
                }

                /* 获取音频消息队列ID值 */
                if ((g_iAudioMsgQueID = msgget(iKey, IPC_CREAT))== -1)
                {
            		AV_PRINT("Creat audio Message  Error: ikey = %d", g_iAudioMsgQueID);

                    //return HI_FAILURE;
                }
            }
            else
            {
                //AV_PRINT("g_iAudioMsgQueID sQueMsg is %s \n",sQueMsg.cBuffer);
                pstCurrentCMD = (MSG_QUE_BUF_DATA_T *)(sQueMsg.cBuffer);

                s32Counter++;   /* 防止一直读取消息队列 */

                /* 添加到消息队列中 */
                if ((pstCurrentCMD->s32Prio > APP_MSG_QUE_RUN_PRIORITY_TYPE) ||
                    (pstCurrentCMD->s32Prio < APP_MSG_QUE_CTL_PRIORITY_TYPE))
                {
            		AV_PRINT("Add msg Prioity error!Prioity=%d", pstCurrentCMD->s32Prio);
                    continue;
                }

                pthread_mutex_lock(&g_AudioMutexLock); /* 获取互斥信号量 */
                iProceRet = APP_MSG_QUE_AddData(sQueMsg.cBuffer,
                                                    g_stMsgQueAudioMsg[0],
                                                    g_stAudioMsgQueCtlMsg);

                pthread_mutex_unlock(&g_AudioMutexLock); /* 释放互斥信号量 */
                if (iProceRet != HI_SUCCESS)
                {
                    AV_PRINT("Add msg to local buffer error!");
                    continue;
                }

            }
            TM_USleep(0,10000);
        } while ((0 <= s32Counter) && (17 > s32Counter));

        s32Counter = 0;
        //usleep(50000);  /* 释放CPU */
        TM_USleep(0,50000);
    }

	return (HI_VOID *)0;
}


/*****************************************************************************
 函 数 名  : APP_SendMsgQueThread
 功能描述  : 消息队列发送线程
 输入参数  : HI_VOID *pArg
 输出参数  : 无
 返 回 值  : HI_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月24日星期四
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID *APP_SendAudioMsgQueThread(HI_VOID *pArg)
{
    MSG_QUE_T sQueMsg;    /* 单条消息队列对应结构体 */
    MSG_QUE_T stSendMsg;  /* 发送消息队列 */
    //const HI_CHAR cSendMsg[40]= "app send audio msg data!\r\n";
    HI_S32 s32Ret;
    MSG_QUE_BUF_DATA_T *pstCurrentCMD;
    MSG_QUE_BUF_DATA_T *pstSendData = NULL;

    (HI_VOID)pArg;

	AV_PRINT("PID@%s pid is %d", __FUNCTION__,getpid());

    for ( ; ; )
    {
        pthread_mutex_lock(&g_AudioMutexLock); /* 获取互斥信号量 */

        s32Ret = APP_ReadLocalBuffer(g_stMsgQueAudioMsg[0],
                                       g_stAudioMsgQueCtlMsg,
                                       (MSG_QUE_BUF_DATA_T *)(sQueMsg.cBuffer));
        pthread_mutex_unlock(&g_AudioMutexLock); /* 释放互斥信号量 */
        if (HI_SUCCESS != s32Ret)
        {
            //usleep(100000);
            TM_USleep(0,100000);
            continue;
        }

        pstCurrentCMD = (MSG_QUE_BUF_DATA_T *)(sQueMsg.cBuffer);
        s32Ret = APP_AudioMsgProc(pstCurrentCMD, &stSendMsg);
        if (HI_SUCCESS != s32Ret)
        {
        	AV_PRINT("APP_AudioMsgProc failed,iret = %d", s32Ret);

            continue;
        }

        pstSendData = (MSG_QUE_BUF_DATA_T *)(stSendMsg.cBuffer);
        /* 如果消息满，则阻塞 */
        s32Ret = msgsnd(g_iAudioMsgQueID, &stSendMsg, pstSendData->s32Len, 0);
        if (0 > s32Ret)
        {
        	AV_PRINT("App send msg failed,iret = %d", s32Ret);
        }
        TM_USleep(0,10000);
    }
	return NULL;
}

/*****************************************************************************
 函 数 名  : APP_RcvVideoMsgQueThread
 功能描述  : 视频消息队列接收线程
 输入参数  : HI_VOID *pArg
 输出参数  : 无
 返 回 值  : HI_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月24日星期四
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID *APP_RcvVideoMsgQueThread(HI_VOID *pArg)
{
    MSG_QUE_T sQueMsg;    /* 单条消息队列对应结构体 */
    MSG_QUE_BUF_DATA_T *pstCurrentCMD;
    HI_S32 s32Ret;
    HI_S32 s32Counter = 0;
    HI_S32 iProceRet;
    key_t iKey;                   /* 消息队列对应的键值 */

    (HI_VOID)pArg;
	AV_PRINT("PID@%s pid is %d", __FUNCTION__,getpid());

    for (; ; )
    {
        do
        {
            //memset(&sQueMsg, 0, sizeof(MSG_QUE_T));

            /* 只接收APP_VIDEO_RCV_MSG_TYPE，采用阻塞方式 */
            s32Ret = msgrcv(g_iVideoMsgQueID, &sQueMsg, sizeof(sQueMsg.cBuffer),
                          APP_VIDEO_RCV_MSG_TYPE, 0);
            if (0 > s32Ret)
            {
        		AV_PRINT("receive msg failed,iret = %d", s32Ret);
                /* 获取视频消息队列键值 */
                if ((iKey = ftok(IPC_PATH, 0x21)) == -1)
                {
            		AV_PRINT("Creat video Key Error, ikey = %d", iKey);
                    //return HI_FAILURE;
                }

                /* 获取视频消息队列ID值 */
                if ((g_iVideoMsgQueID = msgget(iKey, IPC_CREAT))== -1)
                {
            		AV_PRINT("Creat video Message  Error: ikey = %d", g_iVideoMsgQueID);
                    //return HI_FAILURE;
                }
            }
            else
            {
                AV_PRINT("g_iVideoMsgQueID sQueMsg is %s \n",sQueMsg.cBuffer);
                pstCurrentCMD = (MSG_QUE_BUF_DATA_T *)(sQueMsg.cBuffer);

                s32Counter++;   /* 防止一直读取消息队列 */

                if ((pstCurrentCMD->s32Prio > APP_MSG_QUE_RUN_PRIORITY_TYPE) ||
                    (pstCurrentCMD->s32Prio < APP_MSG_QUE_CTL_PRIORITY_TYPE))
                {
            		AV_PRINT("Add msg Prioity error!Prioity=%d", pstCurrentCMD->s32Prio);
                    continue;
                }

                /* 添加到消息队列中 */
                pthread_mutex_lock(&g_VideoMutexLock); /* 获取互斥信号量 */
                iProceRet = APP_MSG_QUE_AddData(sQueMsg.cBuffer,
                                                    g_stMsgQueVideoMsg[0],
                                                    g_stVideoMsgQueCtlMsg);
                pthread_mutex_unlock(&g_VideoMutexLock); /* 释放互斥信号量 */

                if (iProceRet != HI_SUCCESS)
                {
                    AV_PRINT("Add msg to local buffer error!");
                    continue;
                }

            }

        } while ((0 <= s32Ret) && (17 > s32Counter));

        s32Counter = 0;
        //usleep(50000);  /* 释放CPU */
        TM_USleep(0,50000);

    }
	return NULL;
}

#define TEST_MSGQ
#ifdef TEST_MSGQ
/*发送消息队列测试接口 */
HI_VOID  HI3531_SndMsgQueData(HI_S32 s32MainType , HI_S32 s32MinorType)
{
    MSG_QUE_T stSendMsg;  /* 发送消息队列 */
    HI_S32 s32Ret;
    char sztest[64] = {"abc"};

    stSendMsg.s32MsgType = s32MinorType;
    memcpy(&stSendMsg.cBuffer, sztest, strlen(sztest));

    s32Ret = msgsnd(s32MainType, &stSendMsg, strlen(sztest), 0);
    if (0 > s32Ret)
    {
    	AV_PRINT( "#[app]App send msg failed,iret = %d", s32Ret);
    }

    AV_PRINT("#[app]App send msg Main [%d] Minor [0x%x] ", s32MainType, s32MinorType);
}
#endif

/*****************************************************************************
 函 数 名  : APP_SendVideoMsgQueThread
 功能描述  : 消息队列发送线程
 输入参数  : HI_VOID *pArg
 输出参数  : 无
 返 回 值  : HI_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月24日星期四
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID *APP_SendVideoMsgQueThread(HI_VOID *pArg)
{
    MSG_QUE_T sQueMsg;    /* 单条消息队列对应结构体 */
    MSG_QUE_T stSendMsg;  /* 发送消息 */
    //const HI_CHAR cSendMsg[40]= "app video send msg data!\r\n";
    HI_S32 s32Ret;
//    MSG_QUE_BUF_DATA_T *pstCurrentCMD;
    MSG_QUE_BUF_DATA_T *pstSendData = NULL;

    (HI_VOID)pArg;
	AV_PRINT("PID@%s pid is %d", __FUNCTION__,getpid());

    for ( ; ; )
    {
        pthread_mutex_lock(&g_VideoMutexLock); /* 获取互斥信号量 */
        s32Ret = APP_ReadLocalBuffer(g_stMsgQueVideoMsg[0],
                                       g_stVideoMsgQueCtlMsg,
                                       (MSG_QUE_BUF_DATA_T *)(sQueMsg.cBuffer));
        pthread_mutex_unlock(&g_VideoMutexLock); /* 释放互斥信号量 */
        if (HI_SUCCESS != s32Ret)
        {
            //usleep(100000);
            TM_USleep(0,100000);
            continue;
        }

  //      pstCurrentCMD = (MSG_QUE_BUF_DATA_T *)(sQueMsg.cBuffer);
 //       s32Ret = APP_VideoMsgProc(pstCurrentCMD, &stSendMsg);
 //       if (HI_SUCCESS != s32Ret)
 //       {
 //   		AV_PRINT("APP_VideoMsgProc failed,iret = %d", s32Ret);
 //           continue;
 //       }
        /* 如果消息满，则阻塞 */
        pstSendData = (MSG_QUE_BUF_DATA_T *)stSendMsg.cBuffer;
        s32Ret = msgsnd(g_iVideoMsgQueID, &stSendMsg, pstSendData->s32Len, 0);
        if (0 > s32Ret)
        {
    		AV_PRINT("App send msg failed,iret = %d", s32Ret);
        }
    }
	return NULL;
}

/*****************************************************************************
 函 数 名  : APP_SendAudioAndVideoMsgQueThread
 功能描述  : 发送音视频 消息队列线程
 输入参数  : HI_VOID *pArg
 输出参数  : 无
 返 回 值  : HI_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年12月9日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID *APP_SendAudioAndVideoMsgQueThread(HI_VOID *pArg)
{
    MSG_QUE_T sQueMsg;    /* 单条消息队列对应结构体 */
    MSG_QUE_T stSendMsg;  /* 发送消息队列 */
    HI_S32 s32Ret;
    MSG_QUE_BUF_DATA_T *pstCurrentCMD;
    MSG_QUE_BUF_DATA_T *pstSendData = NULL;
    HI_S32 iMsgStatus = 0x00;

    (HI_VOID)pArg;

	AV_PRINT( "#[app]PID@%s pid is %d", __FUNCTION__,getpid());

    for ( ; ; )
    {
        if (0x03 == iMsgStatus)
        {
            //usleep(200000);
            TM_USleep(0,200000);
        }

        iMsgStatus = 0x00;

        pthread_mutex_lock(&g_AudioMutexLock); /* 获取互斥信号量 */

        s32Ret = APP_ReadLocalBuffer(g_stMsgQueAudioMsg[0],
                                       g_stAudioMsgQueCtlMsg,
                                       (MSG_QUE_BUF_DATA_T *)(sQueMsg.cBuffer));
        pthread_mutex_unlock(&g_AudioMutexLock); /* 释放互斥信号量 */
        if (HI_SUCCESS != s32Ret)
        {
            iMsgStatus = (iMsgStatus|0x01);
        }
        else
        {
            pstCurrentCMD = (MSG_QUE_BUF_DATA_T *)(sQueMsg.cBuffer);
            s32Ret = APP_AudioMsgProc(pstCurrentCMD, &stSendMsg);
            if (HI_SUCCESS != s32Ret)
            {
            	AV_PRINT( "#[app]APP_AudioMsgProc failed,iret = %d", s32Ret);

            }
            else
            {
                pstSendData = (MSG_QUE_BUF_DATA_T *)(stSendMsg.cBuffer);
                /* 如果消息满，则阻塞 */
                s32Ret = msgsnd(g_iAudioMsgQueID, &stSendMsg, pstSendData->s32Len, 0);
                if (0 > s32Ret)
                {
                	AV_PRINT( "#[app]App send msg failed,iret = %d", s32Ret);
                }
            }
        }

        pthread_mutex_lock(&g_VideoMutexLock); /* 获取互斥信号量 */
        s32Ret = APP_ReadLocalBuffer(g_stMsgQueVideoMsg[0],
                                       g_stVideoMsgQueCtlMsg,
                                       (MSG_QUE_BUF_DATA_T *)(sQueMsg.cBuffer));
        pthread_mutex_unlock(&g_VideoMutexLock); /* 释放互斥信号量 */
        if (HI_SUCCESS != s32Ret)
        {
            iMsgStatus = (iMsgStatus|0x02);
        }
        else
        {
            pstCurrentCMD = (MSG_QUE_BUF_DATA_T *)(sQueMsg.cBuffer);
        //    s32Ret = APP_VideoMsgProc(pstCurrentCMD, &stSendMsg);
            if (HI_SUCCESS != s32Ret)
            {
        		AV_PRINT( "#[app]APP_VideoMsgProc failed,iret = %d", s32Ret);

                continue;
            }
            else
            {
                /* 如果消息满，则阻塞 */
                pstSendData = (MSG_QUE_BUF_DATA_T *)stSendMsg.cBuffer;
                s32Ret = msgsnd(g_iVideoMsgQueID, &stSendMsg, pstSendData->s32Len, 0);
                if (0 > s32Ret)
                {
            		AV_PRINT( "#[app]App send msg failed,iret = %d", s32Ret);
                }
            }

        }
    }
	return NULL;
}

/*****************************************************************************
 函 数 名  : APP_InitMsgQue
 功能描述  : 初始化消息队列
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
HI_S32 APP_InitMsgQue(HI_VOID)
{
    key_t iKey;                   /* 消息队列对应的键值 */

    /* 获取音频消息队列键值 */
    if ((iKey = ftok(IPC_PATH, 0x20)) == -1)
    {
		AV_PRINT("Creat audio Key Error, ikey = %d", iKey);
        return HI_FAILURE;
    }

    /* 获取音频消息队列ID值 */
    if ((g_iAudioMsgQueID = msgget(iKey, IPC_CREAT))== -1)
    {
		AV_PRINT("Creat audio Message  Error: ikey = %d", g_iAudioMsgQueID);
        return HI_FAILURE;
    }

    /* 获取视频消息队列键值 */
    if ((iKey = ftok(IPC_PATH, 0x21)) == -1)
    {
		AV_PRINT("Creat video Key Error, ikey = %d", iKey);
        return HI_FAILURE;
    }

    /* 获取视频消息队列ID值 */
    if ((g_iVideoMsgQueID = msgget(iKey, IPC_CREAT))== -1)
    {
		AV_PRINT("Creat video Message  Error: ikey = %d", g_iVideoMsgQueID);
        return HI_FAILURE;
    }

    /* 获取参数设置消息队列键值 */
    if ((iKey = ftok(IPC_PATH, 0x22)) == -1)
    {
		AV_PRINT("Creat parameter set Key Error, ikey = %d", iKey);
        return HI_FAILURE;
    }

    /* 获取参数设置消息队列ID值 */
    if ((g_iParaSetMsgQueID = msgget(iKey, IPC_CREAT))== -1)
    {
		AV_PRINT("Creat parameter set Message  Error: ikey = %d", g_iParaSetMsgQueID);
        return HI_FAILURE;
    }

    /* 获取进程间数据同步消息队列键值 */
    if ((iKey = ftok(IPC_PATH, 0x23)) == -1)
    {
		AV_PRINT("Creat parameter set Key Error, ikey = %d", iKey);
        return HI_FAILURE;
    }

    /* 获取进程间数据同步消息队列ID值 */
    if ((g_iDataSyncMsgQueID = msgget(iKey, IPC_CREAT))== -1)
    {
		AV_PRINT("Creat data sync set Message  Error: ikey = %d", g_iDataSyncMsgQueID);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : APP_InitSem
 功能描述  : 初始化信号量
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
HI_S32 APP_InitSem(HI_VOID)
{
    /* 互斥信号量初始化 */
    pthread_mutex_init (&g_AudioMutexLock, NULL);
    pthread_mutex_init (&g_VideoMutexLock, NULL);
    pthread_mutex_init (&g_ParaSetMutexLock, NULL);

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : APP_InitrwLock
 功能描述  : 读写锁初始化
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  :



 修改历史      :
  1.日    期   : 2012年7月30日星期一
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_InitrwLock(HI_VOID)
{
    HI_S32 s32Ret = 0;

    /* 录像缓冲区读写锁 */
    s32Ret = pthread_rwlock_init(&g_RecMemRWLock, NULL);
    if (s32Ret != 0)
    {
		AV_PRINT("init recode memory rwlock iRes = %d", s32Ret);
        exit(EXIT_FAILURE);
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : APP_MsgQueModeInit
 功能描述  : 消息队列模块初始化
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  : HI_SUCCESS  执行成功
             HI_FAILURE   执行失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年6月1日星期五
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_MsgQueModeInit(HI_VOID)
{
    HI_S32 s32Ret;        /* 返回值 */

    s32Ret = APP_InitMsgQue();
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( "#[app]Init msg que Error!");
        return HI_FAILURE;
    }

    s32Ret = APP_InitSem();
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( "#[app]Init sem Error!");
        return HI_FAILURE;
    }

    s32Ret = APP_InitrwLock();
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( "#[app]Init rwlock Error!");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


HI_S32 HI3521_MsgQueModeInit(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    AV_PRINT("HI3531_MsgQueModeInit enter.");

    s32Ret = APP_MsgQueModeInit();
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("Init msg que mode failed!");
        return HI_FAILURE;
    }
    else
    {
        AV_PRINT("Init msg que mode success!");
    }
    AV_PRINT("HI3531_MsgQueModeInit suc.");

	return s32Ret;
}

HI_S32 HI3521_CreateAiMsgQueRcvThread(HI_VOID)
{
    HI_S32 s32Ret;
	pthread_t AiMsgQueRcvTid;

    /* 创建音频消息队列接收线程 */
    s32Ret = pthread_create(&AiMsgQueRcvTid, NULL, APP_RcvAudioMsgQueThread, NULL);
    if (HI_SUCCESS > s32Ret)
	{
        AV_PRINT( "#[app]Creat audio msg que receive thread failed!");

        return HI_FAILURE;
	}
    return HI_SUCCESS;
}

HI_S32 HI3521_CreateViMsgQueRcvThread(HI_VOID)
{
    HI_S32 s32Ret;
	pthread_t ViMsgQueRcvTid;

    /* 创建视频消息队列接收线程 */
    s32Ret = pthread_create(&ViMsgQueRcvTid, NULL, APP_RcvVideoMsgQueThread, NULL);
	if (HI_SUCCESS > s32Ret)
	{
        AV_PRINT( "#[app]Creat video msg que receive thread failed!");

        return HI_FAILURE;
	}
    return HI_SUCCESS;
}

HI_S32 HI3521_CreateAVMsgQueSndThread(HI_VOID)
{
    HI_S32 s32Ret;
	pthread_t AVMsgQueSendTid;

    s32Ret = pthread_create(&AVMsgQueSendTid, NULL, APP_SendAudioAndVideoMsgQueThread, NULL);
	if (HI_SUCCESS > s32Ret)
	{
        AV_PRINT( "#[app]Creat audiio and video msg que audio process thread failed!");

        return HI_FAILURE;
	}
    return HI_SUCCESS;
}


