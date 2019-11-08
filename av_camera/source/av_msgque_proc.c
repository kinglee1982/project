/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ֣�������Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : drv_app_msgqueproc.c
  �� �� ��   : ����
  ��    ��   :
  ��������   : 2012��5��31��������
  ����޸�   :
  ��������   : ��Ϣ���д����ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2012��5��31��������
    ��    ��   :
    �޸�����   : �����ļ�

******************************************************************************/
#include "av_public_head.h"
#include "av_common_31.h"
#include "mp3dec.h"
#include "av_public_ctrl_cmd.h"
#include "av_mp3_play.h"
#include "av_msgque_proc.h"
#include "av_video_proc.h"

HI_S32 g_iAudioMsgQueID;                   /* ��Ƶ��Ϣ����IDֵ */
HI_S32 g_iVideoMsgQueID;                   /* ��Ƶ��Ϣ����IDֵ */
HI_S32 g_iParaSetMsgQueID;                 /* ����������Ϣ����IDֵ */
HI_S32 g_iDataSyncMsgQueID;                /* ���̼�����ͬ����Ϣ���� */

static MSG_QUE_BUF_DATA_T g_stMsgQueAudioMsg[APP_MSG_QUE_PRIORITY][APP_MSG_QUE_CNT];

APP_MSG_CTLDATA_S g_stAudioMsgQueCtlMsg[APP_MSG_QUE_PRIORITY];  /* ��Ƶ��Ϣ���п��ƶ��б��ػ��������� */

static MSG_QUE_BUF_DATA_T g_stMsgQueVideoMsg[APP_MSG_QUE_PRIORITY][APP_MSG_QUE_CNT];

APP_MSG_CTLDATA_S g_stVideoMsgQueCtlMsg[APP_MSG_QUE_PRIORITY];  /* ��Ƶ��Ϣ���п��ƶ��б��ػ��������� */

APP_MSG_CTLDATA_S g_stParaSetMsgQueCtlMsg[APP_MSG_QUE_PRIORITY];  /* ����������Ϣ���п��ƶ��б��ػ��������� */

pthread_mutex_t g_AudioMutexLock; /* ��Ƶ���ݿ��ƻ����� */
pthread_mutex_t g_VideoMutexLock; /* ��Ƶ���ݿ��ƻ����� */
pthread_mutex_t g_ParaSetMutexLock; /* �����������ݿ��ƻ����� */
pthread_rwlock_t g_RecMemRWLock;  /* ¼�񻺳�����д�� */

/*****************************************************************************
 �� �� ��  : APP_MSG_QUE_AddData
 ��������  : ����ȡ����Ϣ���м��뵽������Ϣ��������
 �������  : const HI_CHAR *pcData
             APP_MSG_QUE_BUFFERDATA_t *pstBufferData
             APP_MSG_QUE_CTLDATA_t *cpBuffer

 �������  : ��
 �� �� ֵ  : HI_SUCCESS
             HI_FAILURE
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��24��������
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
HI_S32 APP_MSG_QUE_AddData(const HI_CHAR *pcData,
                                          MSG_QUE_BUF_DATA_T *ppstBufferData,
                                          APP_MSG_CTLDATA_S *pstCtlData
                                         )
{
    HI_S32 iMsgPrioity = 0;
    /* ����Ϣ�����ж�ȡ����Ϣ */
    MSG_QUE_BUF_DATA_T *pstMsgQueBuffer = NULL;
    /* ��Ϣ���п�����Ϣ */
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
    /* Ӧ�÷��õĿ�������Ϣ���������ݺ󣬽��и����±�ֵ */
    pstMsgQueCtlData->s32NewestIndex = (pstMsgQueCtlData->s32NewestIndex + 1) % APP_MSG_QUE_CNT;

    return HI_SUCCESS;

}

/*****************************************************************************
 �� �� ��  : APP_ReadLocalBuffer
 ��������  : ���µ�ǰ�������е���Ϣ
 �������  : APP_MSG_QUE_BUFFERDATA_t *pstBufferData      ������Ϣ�������
             APP_MSG_QUE_CTLDATA_t *pstCtlData            ������Ϣ��������������
             APP_MSG_QUE_BUFFERDATA_t *pstCurrentMsgCMD   ��ǰ���е���Ϣ����
             pthread_mutex_t *pstMutexLock
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��25��������
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
HI_S32 APP_ReadLocalBuffer(MSG_QUE_BUF_DATA_T *ppstBufferData,
                                        APP_MSG_CTLDATA_S *pstCtlData,
                                        MSG_QUE_BUF_DATA_T *pstCurrentMsgCMD)
{
    HI_S32 i;
    HI_S32 iMsgCounter;
    HI_S32 s32OldestIndex;
    HI_S32 iMemoryMove = 0;
    APP_MSG_CTLDATA_S  *pstTempCtlData = NULL; /* ��Ϣ�������� */
    MSG_QUE_BUF_DATA_T *pstCurrentCMD = NULL; /* ��ǰ�������е���
                                                           Ϣ���� */

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
        /* ������ݺϷ��� */
        if ((0 > iMsgCounter) ||
            (APP_MSG_QUE_CNT < iMsgCounter) ||
            (0 > s32OldestIndex) ||
            (APP_MSG_QUE_CNT <= s32OldestIndex))
        {
    		AV_PRINT("Get ctl data failed!p1=%d, p2=%d, p3=%d",
                       pstTempCtlData->s32Counter, pstTempCtlData->s32OldestIndex,
                       pstTempCtlData->s32NewestIndex);

            /* ���ݳ�ʼ�� */
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
 �� �� ��  : APP_RcvMsgQueThread
 ��������  : ��Ϣ���н����߳�
 �������  : HI_VOID *pArg
 �������  : ��
 �� �� ֵ  : HI_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��24��������
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
HI_VOID *APP_RcvAudioMsgQueThread(HI_VOID *pArg)
{
    MSG_QUE_T sQueMsg;    /* ������Ϣ���ж�Ӧ�ṹ�� */
    MSG_QUE_BUF_DATA_T *pstCurrentCMD;
    HI_S32 s32Ret;
    HI_S32 s32Counter = 0;
    HI_S32 iProceRet;
    key_t iKey;                   /* ��Ϣ���ж�Ӧ�ļ�ֵ */

    (HI_VOID)pArg;
	AV_PRINT("PID@%s pid is %d", __FUNCTION__,getpid());

    for (; ; )
    {
        do
        {
            //memset(&sQueMsg, 0, sizeof(MSG_QUE_T));

            /* ֻ����APP_MSG_QUE_AUDIO_CMD_TYPE,��������ʽ���� */
            s32Ret = msgrcv(g_iAudioMsgQueID, &sQueMsg, sizeof(sQueMsg.cBuffer),
                          APP_AUDIO_RCV_MSG_TYPE, 0);

            if (0 > s32Ret)
            {
        		AV_PRINT("App receive msg failed,iret = %d", s32Ret);
                /* ��ȡ��Ƶ��Ϣ���м�ֵ */
                if ((iKey = ftok(IPC_PATH, 0x20)) == -1)
                {
            		AV_PRINT("Creat audio Key Error, ikey = %d", iKey);

                    //return HI_FAILURE;
                }

                /* ��ȡ��Ƶ��Ϣ����IDֵ */
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

                s32Counter++;   /* ��ֹһֱ��ȡ��Ϣ���� */

                /* ��ӵ���Ϣ������ */
                if ((pstCurrentCMD->s32Prio > APP_MSG_QUE_RUN_PRIORITY_TYPE) ||
                    (pstCurrentCMD->s32Prio < APP_MSG_QUE_CTL_PRIORITY_TYPE))
                {
            		AV_PRINT("Add msg Prioity error!Prioity=%d", pstCurrentCMD->s32Prio);
                    continue;
                }

                pthread_mutex_lock(&g_AudioMutexLock); /* ��ȡ�����ź��� */
                iProceRet = APP_MSG_QUE_AddData(sQueMsg.cBuffer,
                                                    g_stMsgQueAudioMsg[0],
                                                    g_stAudioMsgQueCtlMsg);

                pthread_mutex_unlock(&g_AudioMutexLock); /* �ͷŻ����ź��� */
                if (iProceRet != HI_SUCCESS)
                {
                    AV_PRINT("Add msg to local buffer error!");
                    continue;
                }

            }
            TM_USleep(0,10000);
        } while ((0 <= s32Counter) && (17 > s32Counter));

        s32Counter = 0;
        //usleep(50000);  /* �ͷ�CPU */
        TM_USleep(0,50000);
    }

	return (HI_VOID *)0;
}


/*****************************************************************************
 �� �� ��  : APP_SendMsgQueThread
 ��������  : ��Ϣ���з����߳�
 �������  : HI_VOID *pArg
 �������  : ��
 �� �� ֵ  : HI_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��24��������
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
HI_VOID *APP_SendAudioMsgQueThread(HI_VOID *pArg)
{
    MSG_QUE_T sQueMsg;    /* ������Ϣ���ж�Ӧ�ṹ�� */
    MSG_QUE_T stSendMsg;  /* ������Ϣ���� */
    //const HI_CHAR cSendMsg[40]= "app send audio msg data!\r\n";
    HI_S32 s32Ret;
    MSG_QUE_BUF_DATA_T *pstCurrentCMD;
    MSG_QUE_BUF_DATA_T *pstSendData = NULL;

    (HI_VOID)pArg;

	AV_PRINT("PID@%s pid is %d", __FUNCTION__,getpid());

    for ( ; ; )
    {
        pthread_mutex_lock(&g_AudioMutexLock); /* ��ȡ�����ź��� */

        s32Ret = APP_ReadLocalBuffer(g_stMsgQueAudioMsg[0],
                                       g_stAudioMsgQueCtlMsg,
                                       (MSG_QUE_BUF_DATA_T *)(sQueMsg.cBuffer));
        pthread_mutex_unlock(&g_AudioMutexLock); /* �ͷŻ����ź��� */
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
        /* �����Ϣ���������� */
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
 �� �� ��  : APP_RcvVideoMsgQueThread
 ��������  : ��Ƶ��Ϣ���н����߳�
 �������  : HI_VOID *pArg
 �������  : ��
 �� �� ֵ  : HI_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��24��������
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
HI_VOID *APP_RcvVideoMsgQueThread(HI_VOID *pArg)
{
    MSG_QUE_T sQueMsg;    /* ������Ϣ���ж�Ӧ�ṹ�� */
    MSG_QUE_BUF_DATA_T *pstCurrentCMD;
    HI_S32 s32Ret;
    HI_S32 s32Counter = 0;
    HI_S32 iProceRet;
    key_t iKey;                   /* ��Ϣ���ж�Ӧ�ļ�ֵ */

    (HI_VOID)pArg;
	AV_PRINT("PID@%s pid is %d", __FUNCTION__,getpid());

    for (; ; )
    {
        do
        {
            //memset(&sQueMsg, 0, sizeof(MSG_QUE_T));

            /* ֻ����APP_VIDEO_RCV_MSG_TYPE������������ʽ */
            s32Ret = msgrcv(g_iVideoMsgQueID, &sQueMsg, sizeof(sQueMsg.cBuffer),
                          APP_VIDEO_RCV_MSG_TYPE, 0);
            if (0 > s32Ret)
            {
        		AV_PRINT("receive msg failed,iret = %d", s32Ret);
                /* ��ȡ��Ƶ��Ϣ���м�ֵ */
                if ((iKey = ftok(IPC_PATH, 0x21)) == -1)
                {
            		AV_PRINT("Creat video Key Error, ikey = %d", iKey);
                    //return HI_FAILURE;
                }

                /* ��ȡ��Ƶ��Ϣ����IDֵ */
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

                s32Counter++;   /* ��ֹһֱ��ȡ��Ϣ���� */

                if ((pstCurrentCMD->s32Prio > APP_MSG_QUE_RUN_PRIORITY_TYPE) ||
                    (pstCurrentCMD->s32Prio < APP_MSG_QUE_CTL_PRIORITY_TYPE))
                {
            		AV_PRINT("Add msg Prioity error!Prioity=%d", pstCurrentCMD->s32Prio);
                    continue;
                }

                /* ��ӵ���Ϣ������ */
                pthread_mutex_lock(&g_VideoMutexLock); /* ��ȡ�����ź��� */
                iProceRet = APP_MSG_QUE_AddData(sQueMsg.cBuffer,
                                                    g_stMsgQueVideoMsg[0],
                                                    g_stVideoMsgQueCtlMsg);
                pthread_mutex_unlock(&g_VideoMutexLock); /* �ͷŻ����ź��� */

                if (iProceRet != HI_SUCCESS)
                {
                    AV_PRINT("Add msg to local buffer error!");
                    continue;
                }

            }

        } while ((0 <= s32Ret) && (17 > s32Counter));

        s32Counter = 0;
        //usleep(50000);  /* �ͷ�CPU */
        TM_USleep(0,50000);

    }
	return NULL;
}

#define TEST_MSGQ
#ifdef TEST_MSGQ
/*������Ϣ���в��Խӿ� */
HI_VOID  HI3531_SndMsgQueData(HI_S32 s32MainType , HI_S32 s32MinorType)
{
    MSG_QUE_T stSendMsg;  /* ������Ϣ���� */
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
 �� �� ��  : APP_SendVideoMsgQueThread
 ��������  : ��Ϣ���з����߳�
 �������  : HI_VOID *pArg
 �������  : ��
 �� �� ֵ  : HI_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��24��������
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
HI_VOID *APP_SendVideoMsgQueThread(HI_VOID *pArg)
{
    MSG_QUE_T sQueMsg;    /* ������Ϣ���ж�Ӧ�ṹ�� */
    MSG_QUE_T stSendMsg;  /* ������Ϣ */
    //const HI_CHAR cSendMsg[40]= "app video send msg data!\r\n";
    HI_S32 s32Ret;
//    MSG_QUE_BUF_DATA_T *pstCurrentCMD;
    MSG_QUE_BUF_DATA_T *pstSendData = NULL;

    (HI_VOID)pArg;
	AV_PRINT("PID@%s pid is %d", __FUNCTION__,getpid());

    for ( ; ; )
    {
        pthread_mutex_lock(&g_VideoMutexLock); /* ��ȡ�����ź��� */
        s32Ret = APP_ReadLocalBuffer(g_stMsgQueVideoMsg[0],
                                       g_stVideoMsgQueCtlMsg,
                                       (MSG_QUE_BUF_DATA_T *)(sQueMsg.cBuffer));
        pthread_mutex_unlock(&g_VideoMutexLock); /* �ͷŻ����ź��� */
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
        /* �����Ϣ���������� */
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
 �� �� ��  : APP_SendAudioAndVideoMsgQueThread
 ��������  : ��������Ƶ ��Ϣ�����߳�
 �������  : HI_VOID *pArg
 �������  : ��
 �� �� ֵ  : HI_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��12��9��
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
HI_VOID *APP_SendAudioAndVideoMsgQueThread(HI_VOID *pArg)
{
    MSG_QUE_T sQueMsg;    /* ������Ϣ���ж�Ӧ�ṹ�� */
    MSG_QUE_T stSendMsg;  /* ������Ϣ���� */
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

        pthread_mutex_lock(&g_AudioMutexLock); /* ��ȡ�����ź��� */

        s32Ret = APP_ReadLocalBuffer(g_stMsgQueAudioMsg[0],
                                       g_stAudioMsgQueCtlMsg,
                                       (MSG_QUE_BUF_DATA_T *)(sQueMsg.cBuffer));
        pthread_mutex_unlock(&g_AudioMutexLock); /* �ͷŻ����ź��� */
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
                /* �����Ϣ���������� */
                s32Ret = msgsnd(g_iAudioMsgQueID, &stSendMsg, pstSendData->s32Len, 0);
                if (0 > s32Ret)
                {
                	AV_PRINT( "#[app]App send msg failed,iret = %d", s32Ret);
                }
            }
        }

        pthread_mutex_lock(&g_VideoMutexLock); /* ��ȡ�����ź��� */
        s32Ret = APP_ReadLocalBuffer(g_stMsgQueVideoMsg[0],
                                       g_stVideoMsgQueCtlMsg,
                                       (MSG_QUE_BUF_DATA_T *)(sQueMsg.cBuffer));
        pthread_mutex_unlock(&g_VideoMutexLock); /* �ͷŻ����ź��� */
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
                /* �����Ϣ���������� */
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
 �� �� ��  : APP_InitMsgQue
 ��������  : ��ʼ����Ϣ����
 �������  : HI_VOID
 �������  : ��
 �� �� ֵ  : HI_SUCCESS  ִ�гɹ�
             HI_FAILURE   ִ��ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��31��������
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
HI_S32 APP_InitMsgQue(HI_VOID)
{
    key_t iKey;                   /* ��Ϣ���ж�Ӧ�ļ�ֵ */

    /* ��ȡ��Ƶ��Ϣ���м�ֵ */
    if ((iKey = ftok(IPC_PATH, 0x20)) == -1)
    {
		AV_PRINT("Creat audio Key Error, ikey = %d", iKey);
        return HI_FAILURE;
    }

    /* ��ȡ��Ƶ��Ϣ����IDֵ */
    if ((g_iAudioMsgQueID = msgget(iKey, IPC_CREAT))== -1)
    {
		AV_PRINT("Creat audio Message  Error: ikey = %d", g_iAudioMsgQueID);
        return HI_FAILURE;
    }

    /* ��ȡ��Ƶ��Ϣ���м�ֵ */
    if ((iKey = ftok(IPC_PATH, 0x21)) == -1)
    {
		AV_PRINT("Creat video Key Error, ikey = %d", iKey);
        return HI_FAILURE;
    }

    /* ��ȡ��Ƶ��Ϣ����IDֵ */
    if ((g_iVideoMsgQueID = msgget(iKey, IPC_CREAT))== -1)
    {
		AV_PRINT("Creat video Message  Error: ikey = %d", g_iVideoMsgQueID);
        return HI_FAILURE;
    }

    /* ��ȡ����������Ϣ���м�ֵ */
    if ((iKey = ftok(IPC_PATH, 0x22)) == -1)
    {
		AV_PRINT("Creat parameter set Key Error, ikey = %d", iKey);
        return HI_FAILURE;
    }

    /* ��ȡ����������Ϣ����IDֵ */
    if ((g_iParaSetMsgQueID = msgget(iKey, IPC_CREAT))== -1)
    {
		AV_PRINT("Creat parameter set Message  Error: ikey = %d", g_iParaSetMsgQueID);
        return HI_FAILURE;
    }

    /* ��ȡ���̼�����ͬ����Ϣ���м�ֵ */
    if ((iKey = ftok(IPC_PATH, 0x23)) == -1)
    {
		AV_PRINT("Creat parameter set Key Error, ikey = %d", iKey);
        return HI_FAILURE;
    }

    /* ��ȡ���̼�����ͬ����Ϣ����IDֵ */
    if ((g_iDataSyncMsgQueID = msgget(iKey, IPC_CREAT))== -1)
    {
		AV_PRINT("Creat data sync set Message  Error: ikey = %d", g_iDataSyncMsgQueID);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 �� �� ��  : APP_InitSem
 ��������  : ��ʼ���ź���
 �������  : HI_VOID
 �������  : ��
 �� �� ֵ  : HI_SUCCESS  ִ�гɹ�
             HI_FAILURE   ִ��ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��5��31��������
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
HI_S32 APP_InitSem(HI_VOID)
{
    /* �����ź�����ʼ�� */
    pthread_mutex_init (&g_AudioMutexLock, NULL);
    pthread_mutex_init (&g_VideoMutexLock, NULL);
    pthread_mutex_init (&g_ParaSetMutexLock, NULL);

    return HI_SUCCESS;
}

/*****************************************************************************
 �� �� ��  : APP_InitrwLock
 ��������  : ��д����ʼ��
 �������  : HI_VOID
 �������  : ��
 �� �� ֵ  :



 �޸���ʷ      :
  1.��    ��   : 2012��7��30������һ
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
HI_S32 APP_InitrwLock(HI_VOID)
{
    HI_S32 s32Ret = 0;

    /* ¼�񻺳�����д�� */
    s32Ret = pthread_rwlock_init(&g_RecMemRWLock, NULL);
    if (s32Ret != 0)
    {
		AV_PRINT("init recode memory rwlock iRes = %d", s32Ret);
        exit(EXIT_FAILURE);
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 �� �� ��  : APP_MsgQueModeInit
 ��������  : ��Ϣ����ģ���ʼ��
 �������  : HI_VOID
 �������  : ��
 �� �� ֵ  : HI_SUCCESS  ִ�гɹ�
             HI_FAILURE   ִ��ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��6��1��������
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
HI_S32 APP_MsgQueModeInit(HI_VOID)
{
    HI_S32 s32Ret;        /* ����ֵ */

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

    /* ������Ƶ��Ϣ���н����߳� */
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

    /* ������Ƶ��Ϣ���н����߳� */
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


