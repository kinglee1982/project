/******************************************************************************

                  版权所有 (C), 2001-2024, 郑州天迈科技有限公司

 ******************************************************************************
  文 件 名   : drv_app_memoryproc.c
  版 本 号   : 初稿
  作    者   :
  生成日期   : 2012年5月31日星期四
  最近修改   :
  功能描述   : 共享内存处理文件
  函数列表   :
  修改历史   :
  1.日    期   : 2012年5月31日星期四
    作    者   :
    修改内容   : 创建文件

******************************************************************************/
#include "av_public_head.h"
#include "av_common_31.h"
#include "av_mem.h"
#include "av_store.h"
#include "av_audio_init.h"
#include "av_hddfsm.h"
#include "av_video_proc.h"

extern HI_S32 g_iDataSyncMsgQueID;                /* 进程间数据同步消息队列 */
HI_VOID *g_pRecodeMemAddr = NULL;      /* 视频缓冲区共享内存首地址 */
SHM_SEM_MNG_T gShmSemMng[8];		/**/

#ifdef STORE_MULTI_THD
extern CHN_STOR_MGMT_ST g_stChnStoreMgmt[APP_VI_CHNCNT];
#endif

/*****************************************************************************
 函 数 名  : MEM_GetSemId
 功能描述  : 创建信号量
 输入参数  : IN ikey  信号量键值
 输出参数  : 无
 返 回 值  : HI_S32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月18日星期五
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 MEM_GetSemId(IN const key_t ikey)
{
    HI_S32 s32SemId;

    /* 获取信号量ID号 */
    s32SemId = semget(ikey, 1, IPC_CREAT|0666);
    if (-1 == s32SemId)
    {
        AV_PRINT( "#[app]Create semaphore error!");
		return HI_FAILURE;
    }

    return s32SemId;
}

/*****************************************************************************
 函 数 名  : MEM_GetSemKey
 功能描述  : 创建信号量
 输入参数  : const HI_CHAR *pcPath, HI_S32 s32Id
 输出参数  : 无
 返 回 值  : HI_S32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月27
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 MEM_GetSemKey(const HI_CHAR *pcPath, HI_S32 s32Id)
{
    HI_S32 s32Key;

	if ((s32Key = ftok(pcPath, s32Id)) == -1)
    {
		AV_PRINT( "#[app]MEM_GetSemKey Error, s32Key=%d", s32Key);
        return HI_FAILURE;
    }

	return s32Key;
}

/*****************************************************************************
 函 数 名  : MEM_SetSemVal
 功能描述  : 设置共享内存信号量值
 输入参数  : IN const HI_S32 iSemID      对应信号量ID
             IN const HI_S32 iSemValue   信号量设置的值
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
HI_S32 MEM_SetSemVal(IN const HI_S32 s32SemId, IN const HI_S32 s32SemVal)
{
	HI_S32 s32Ret = HI_SUCCESS;
    union semun uSem;

    uSem.pstBuffer = NULL;
    uSem.pusArray= NULL;
    /* 必须设置为1，否则第一次获取信号量失败 */
    uSem.iValue = s32SemVal;

    /* SETVAL: Set the value of semval to arg.val */
	s32Ret = semctl(s32SemId, 0, SETVAL, uSem);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("semctl error!");
		return HI_FAILURE;
	}

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : MEM_DelSem
 功能描述  : 删除信号量
 输入参数  : HI_S32 iSemID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月18日星期五
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 MEM_DelSem(IN const HI_S32 s32SemId)
{
	HI_S32 s32Ret = HI_SUCCESS;
    union semun uSem;

    uSem.pstBuffer = NULL;
    uSem.pusArray= NULL;
    uSem.iValue = 0;

    /* 删除信号量 */
	s32Ret = semctl(s32SemId, 0, IPC_RMID, uSem);
    if (HI_SUCCESS != s32Ret)
    {
		return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : MEM_LockSem
 功能描述  : 信号量使能
 输入参数  : HI_S32 s32SemID    信号量
 输出参数  : 无
 返 回 值  : HI_S32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月18日星期五
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 MEM_LockSem(IN const HI_S32 s32SemID)
{
	HI_S32 s32Ret = HI_SUCCESS;
    struct sembuf stSops;

    stSops.sem_num = 0;
    stSops.sem_op = -1;
    stSops.sem_flg = SEM_UNDO;

    /*
     * 如果sem_op为正数，semop函数将该值加到相应的信号量元素中，
     * 并唤醒所有等待元素增1的进程。如果sem_op为0而信号量的值不为0，
     * semop将阻塞调用进程并增加那个元素的等零进程个数。如果sem_op为负数，
     * semop将该值加到相应的信号量元素中（只要结果不为负数），如结果为负数，
     * semop将阻塞进程等待信号量元素值增加
     */
	s32Ret = semop(s32SemID, &stSops, 1);
    if (HI_FAILURE == s32Ret)
    {
        AV_PRINT( "Sem Pass error!");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : MEM_UnlockSem
 功能描述  : 信号量释放
 输入参数  : iSemID    信号量
 输出参数  : 无
 返 回 值  : HI_S32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月18日星期五
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 MEM_UnlockSem(HI_S32 iSemID)
{
    struct sembuf stSops;

    stSops.sem_num = 0;
    stSops.sem_op = 1;
    stSops.sem_flg = SEM_UNDO;

    /*
     * 如果sem_op为正数，semop函数将该值加到相应的信号量元素中，
     * 并唤醒所有等待元素增1的进程。如果sem_op为0而信号量的值不为0，
     * semop将阻塞调用进程并增加那个元素的等零进程个数。如果sem_op为负数，
     * semop将该值加到相应的信号量元素中（只要结果不为负数），如结果为负数，
     * semop将阻塞进程等待信号量元素值增加
     */

    if (-1 == (semop(iSemID, &stSops, 1)))
    {
        AV_PRINT( "Sem release error!");

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : MEM_RecShmHeadInit
 功能描述  : 共享内存初始化
 输入参数  : SHM_ATTR_T *pstShmAttr  共享内存属性
             HI_S32 s32Offset                              共享内存偏移
             HI_VOID *pData                              要写入共享内存的数据首地址
             HI_S32 s32Len                                 要写入数据长度
             HI_S32 iSemId                               共享内存信号量
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2013年7月3日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 MEM_RecShmHeadInit(SHM_ATTR_T *pstShmAttr, HI_S32 s32Offset,
                             	HI_VOID *pData, HI_S32 s32Len, pthread_rwlock_t *pstLock)
{
	HI_S32 s32Size = 0;
    HI_CHAR *pcShmHead = NULL;  /* 共享内存头部数据指针 */

    if ((NULL == pstShmAttr) || (NULL == pData))
    {
		AV_PRINT("MEM_RecShmHeadInit para Invalid.");
        return HI_FAILURE;
    }

    if (0 == s32Len)
    {
        return HI_SUCCESS;
    }

	s32Size = pstShmAttr->s32ItemSize * pstShmAttr->s32ItemMaxNum;
    if ((s32Len < 0) || (s32Len >= s32Size))
    {
        AV_PRINT("write data len error, len = %d", s32Len);
        return HI_FAILURE;
    }

    pcShmHead = (HI_CHAR *)(pstShmAttr->pShmAddr) + s32Offset;

    pthread_rwlock_wrlock(pstLock);  /* 获取共享内存写锁 */
    memcpy(pcShmHead, pData, s32Len);
    pthread_rwlock_unlock(pstLock);  /* 释放共享内存读锁 */

    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : MEM_ShmDataInit
 功能描述  : 共享内存数据初始化
 输入参数  : SHM_ATTR_T *pstShmAttr
             HI_S32 iSemId
 输出参数  : 无
 返 回 值  :



 修改历史      :
  1.日    期   : 2012年8月21日星期二
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 MEM_ShmDataInit(SHM_ATTR_T *pstShmAttr, HI_S32 s32SemId)
{
    SHM_CTRL_DATA_T *pstShmCtrlData = NULL;  /* 共享内存头部数据指针 */
    HI_S32 i = 0;
    SHM_HEAD_T *pstShmHead = NULL;

    if (NULL == pstShmAttr)
    {
		AV_PRINT("MEM_ShmDataInit para is invalid");
        return HI_FAILURE;
    }

    pstShmCtrlData = (SHM_CTRL_DATA_T *)pstShmAttr->pShmAddr;

    /* 获取共享内存写锁 */
    MEM_LockSem(s32SemId);

    pstShmCtrlData->s32Counter = 0;
    pstShmCtrlData->s32NewestIndex = 0;
    pstShmCtrlData->s32OldestIndex = 0;

    for (i = 0 ; i < pstShmAttr->s32ItemMaxNum; i++)
    {
        pstShmHead = (SHM_HEAD_T *)((HI_CHAR *)pstShmCtrlData +
						sizeof(SHM_CTRL_DATA_T) + i* pstShmAttr->s32ItemSize);
        pstShmHead->dataSize = 0;
    }

    /* 释放共享内存读锁 */
    MEM_UnlockSem(s32SemId);

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : MEM_WrShm
 功能描述  : 向共享内存写数据
 输入参数  : SHM_ATTR_T *pstShmAttr  :共享内存属性指针
             const HI_VOID *pData         :需要写入的数据
             const HI_S32 s32Len            :需要写入数据的长度
             HI_S32 iSemId                :信号量ID
 输出参数  : 无
 返 回 值  :



 修改历史      :
  1.日    期   : 2012年8月8日星期三
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 MEM_WrShm(SHM_ATTR_T *pstShmAttr, const HI_VOID *pData,
                          const HI_S32 s32Len, HI_S32 s32SemId)
{
    HI_VOID *pAddr = NULL;   					/* 指向共享内存写区域地址 */
    SHM_CTRL_DATA_T *pstShmCtrlData = NULL;  	/* 共享内存头部数据指针 */
    HI_S32 s32ItemSize = 0;    					/* 单条数据大小 */
    HI_S32 s32ItemMaxNum = 0; 					/* 共享内存中，存储最大数据条数 */

    if ((NULL == pstShmAttr) || (NULL == pData))
    {
		AV_PRINT("MEM_WrShm para invalid");
        return HI_FAILURE;
    }

    if (s32Len <= 0)
    {
		AV_PRINT("#MEM_WrShm len error! s32Len=%d", s32Len);
        return HI_FAILURE;
    }

    pstShmCtrlData = (SHM_CTRL_DATA_T *)pstShmAttr->pShmAddr;
    s32ItemSize = pstShmAttr->s32ItemSize;
    s32ItemMaxNum = pstShmAttr->s32ItemMaxNum;

    /* 获取共享内存写锁 */
    (HI_VOID)MEM_LockSem(s32SemId);

    if ((pstShmCtrlData->s32Counter < 0) ||
        (pstShmCtrlData->s32Counter > s32ItemMaxNum) ||
        (pstShmCtrlData->s32NewestIndex < 0) ||
        (pstShmCtrlData->s32NewestIndex >= s32ItemMaxNum) ||
        (pstShmCtrlData->s32OldestIndex < 0) ||
        (pstShmCtrlData->s32OldestIndex >= s32ItemMaxNum))
    {
		AV_PRINT("Head info error! counter = %d, newIdx = %d, oldIdx = %d",
						pstShmCtrlData->s32Counter, pstShmCtrlData->s32NewestIndex, pstShmCtrlData->s32OldestIndex);

        pstShmCtrlData->s32Counter = 0;
        pstShmCtrlData->s32NewestIndex = 0;
        pstShmCtrlData->s32OldestIndex = 0;
    }

    pAddr = (HI_CHAR *)pstShmCtrlData + pstShmCtrlData->s32NewestIndex * s32ItemSize + sizeof(SHM_CTRL_DATA_T);
    memcpy(pAddr, pData, s32Len);

	pstShmCtrlData->s32NewestIndex++;
    pstShmCtrlData->s32NewestIndex %= s32ItemMaxNum;
    if (pstShmCtrlData->s32Counter < s32ItemMaxNum)
    {
        pstShmCtrlData->s32Counter++;
    }
    else
    {
	    pstShmCtrlData->s32OldestIndex++;
		pstShmCtrlData->s32OldestIndex %= s32ItemMaxNum;
		AV_PRINT("Head info error! maxnum = %d, counter = %d, newIdx = %d, oldIdx = %d",
		                s32ItemMaxNum, pstShmCtrlData->s32Counter, pstShmCtrlData->s32NewestIndex, pstShmCtrlData->s32OldestIndex);
    }

    /* 释放共享内存读锁 */
    (HI_VOID)MEM_UnlockSem(s32SemId);

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : MEM_RdShm
 功能描述  : 读取共享内存数据
 输入参数  : const HI_VOID *pMemHeadAddr
             HI_VOID *pData
             const HI_S32 s32Len
             HI_S32 iSemId
             HI_S32 *ps32Counter   还剩余条数
 输出参数  : 无
 返 回 值  :



 修改历史      :
  1.日    期   : 2012年8月8日星期三
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 MEM_RdShm(SHM_ATTR_T *pstShmAttr, HI_VOID *pData,
                                     const HI_S32 s32Len, HI_S32 iSemId,
                                     HI_S32 *ps32Counter)
{
    HI_VOID *pAddr = NULL;   					/* 指向共享内存写区域临时地址 */
    SHM_CTRL_DATA_T *pstShmCtrlData = NULL;  	/* 共享内存头部数据指针 */
//    HI_S32 *pTmpData = NULL;
    HI_S32 s32ItemSize = 0;
    HI_S32 s32ItemMaxNum = 0;
	SHM_HEAD_T stShmHead; //数据头

    bzero(&stShmHead, sizeof(stShmHead));

	if ((NULL == pstShmAttr) || (NULL == pData))
    {
		AV_PRINT( "#[app]:Input NULL pointer pMemHeadAddr=%p, pData=%p", pstShmAttr, pData);

        return HI_FAILURE;
    }

    pstShmCtrlData = (SHM_CTRL_DATA_T *)pstShmAttr->pShmAddr;
    s32ItemSize = pstShmAttr->s32ItemSize;
    s32ItemMaxNum = pstShmAttr->s32ItemMaxNum;

    /* 获取共享内存写锁 */
    MEM_LockSem(iSemId);

    if ((pstShmCtrlData->s32Counter < 0) ||
        (pstShmCtrlData->s32Counter > s32ItemMaxNum) ||
        (pstShmCtrlData->s32NewestIndex < 0) ||
        (pstShmCtrlData->s32NewestIndex >= s32ItemMaxNum) ||
        (pstShmCtrlData->s32OldestIndex < 0) ||
        (pstShmCtrlData->s32OldestIndex >= s32ItemMaxNum))
    {
		AV_PRINT( "#[app]Head info error! counter = %d, newIdx = %d, oldIdx = %d",
						pstShmCtrlData->s32Counter, pstShmCtrlData->s32NewestIndex, pstShmCtrlData->s32OldestIndex);
		memset(pstShmCtrlData, 0, sizeof(SHM_CTRL_DATA_T));

        if (NULL != ps32Counter)
        {
            *ps32Counter = pstShmCtrlData->s32Counter;
        }
        /* 释放共享内存读锁 */
        MEM_UnlockSem(iSemId);

        return HI_FAILURE;
    }

    if (pstShmCtrlData->s32Counter > 0)
    {
        pAddr = (HI_CHAR *)pstShmCtrlData + pstShmCtrlData->s32OldestIndex * s32ItemSize + sizeof(SHM_CTRL_DATA_T);
		memcpy(&stShmHead, pAddr, sizeof(stShmHead));

		if (stShmHead.dataSize <= 0)
		{
    		AV_PRINT("#[app]block = %d", pstShmCtrlData->s32Counter);
			pstShmCtrlData->s32OldestIndex++;
			pstShmCtrlData->s32OldestIndex %= s32ItemMaxNum;
			pstShmCtrlData->s32Counter--;
			MEM_UnlockSem(iSemId);
			return HI_FAILURE;
		}
		memcpy(pData, pAddr, stShmHead.dataSize);
  //      pTmpData = (HI_S32 *)pData;
		pstShmCtrlData->s32OldestIndex++;
        pstShmCtrlData->s32OldestIndex %= s32ItemMaxNum;
        pstShmCtrlData->s32Counter--;
    }
	else
	{
	    /* 释放共享内存读锁 */
    	MEM_UnlockSem(iSemId);
		return HI_NO_DATA;
	}

    if (NULL != ps32Counter)
    {
        *ps32Counter = pstShmCtrlData->s32Counter;
    }

    /* 释放共享内存读锁 */
    MEM_UnlockSem(iSemId);

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : APP_IpSpkSndThread
 功能描述  : IP对讲发送数据处理线程
 输入参数  : HI_VOID *pArg
 输出参数  : 无
 返 回 值  : HI_VOID



 修改历史      :
  1.日    期   : 2012年8月8日星期三
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID *APP_IpSpkSndThread(HI_VOID *pArg)
{
    HI_S32 s32Ret = 0;
    const HI_CHAR cSendMsg[1024]= "memory share write msg data!\r\n";
    DATA_SYNC_MSG_QUE_T stDataSync;    /* 发送数据同步消息队列 */
	SHM_SEM_MNG_T *pstShmSemMng = NULL;
	SEM_MNG_T *pstSemMng = NULL;
	SHM_MNG_T *pstShmMng = NULL;

	HI3531_GetShmSemMng(IPC_IPSPK_SND_E, &pstShmSemMng);
	if (NULL == pstShmSemMng)
	{
		AV_PRINT( "#[app]HI3531_GetShmSemMng err! ");
        return NULL;
	}
    AV_PRINT("@%s pid is %d,tid%d", __FUNCTION__,getpid(),gettid());
	pstShmMng = &pstShmSemMng->shmMng;
	pstSemMng = &pstShmSemMng->semMng;
    s32Ret = MEM_ShmDataInit(&pstShmMng->shmAttr, pstSemMng->sysSemId);
    if (HI_SUCCESS != s32Ret)
    {
		AV_PRINT( "#[app]MEM_ShmDataInit err! s32Ret = %d", s32Ret);
        return NULL;
    }

    for ( ; ; )
    {
        //(HI_VOID)sleep(2);
        TM_USleep(2,0);
        s32Ret = MEM_WrShm(&pstShmMng->shmAttr, (HI_VOID *)cSendMsg,
								pstShmMng->shmAttr.s32ItemSize, pstSemMng->sysSemId);
        if (HI_SUCCESS != s32Ret)
        {
            continue;
        }
        else
        {
            stDataSync.s32MsgType = APP_IP_SPEAK_WRITE_TYPE;
            stDataSync.cBuffer[0] = APP_READ_READY;

            /* 如果消息满，则阻塞 */
            s32Ret = msgsnd(g_iDataSyncMsgQueID, &stDataSync, sizeof(stDataSync.cBuffer), IPC_NOWAIT);
            if (0 > s32Ret)
            {
        		AV_PRINT( "#[app]ip spk snd msg, err! iret = %d", s32Ret);
            }
        }
    }

	return NULL;
}

/*****************************************************************************
 函 数 名  : APP_IpSpkRcvThread
 功能描述  : IP对讲接收数据处理线程
 输入参数  : HI_VOID *pArg
 输出参数  : 无
 返 回 值  : HI_VOID



 修改历史      :
  1.日    期   : 2012年8月8日星期三
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID *APP_IpSpkRcvThread(HI_VOID *pArg)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32Counter = 0;
    const HI_S32 s32RcvMsg[1024] = {0};
    DATA_SYNC_MSG_QUE_T stDataSync;
	SHM_SEM_MNG_T *pstShmSemMng = NULL;
	SEM_MNG_T *pstSemMng = NULL;
	SHM_MNG_T *pstShmMng = NULL;

	HI3531_GetShmSemMng(IPC_IPSPK_RCV_E, &pstShmSemMng);
	if (NULL == pstShmSemMng)
	{
		AV_PRINT( "#[app]HI3531_GetShmSemMng err! ");
        return NULL;
	}

	pstShmMng = &pstShmSemMng->shmMng;
	pstSemMng = &pstShmSemMng->semMng;

    for ( ; ; )
    {
        /* 只接收APP_APP_READ_MSG_TYPE,采用阻塞式发送 */
        s32Ret = msgrcv(g_iDataSyncMsgQueID, &stDataSync, sizeof(stDataSync.cBuffer),
                      APP_IP_SPEAK_READ_TYPE, 0);
        if (0 > s32Ret)
        {
    		AV_PRINT( "#[app]ip spk rcv msg err, s32Ret = %d", s32Ret);
        }
        AV_PRINT("g_iDataSyncMsgQueID stDataSync %s \n",stDataSync.cBuffer);

        if (APP_READ_READY != stDataSync.cBuffer[0])
        {
            continue;
        }

        do
        {
            MEM_RdShm(&pstShmMng->shmAttr, (HI_VOID *)s32RcvMsg,
                            pstShmMng->shmAttr.s32ItemSize, pstSemMng->sysSemId, &s32Counter);
    		AV_PRINT("#[app]ip spk rd msg, cRcvMsg = %d, s32Counter = %d", s32RcvMsg[0], s32Counter+1);

            //usleep(1000 *100 );
            TM_USleep(0,1000 *100);
        } while (s32Counter > 0);
    }

	return NULL;
}

/*****************************************************************************
 函 数 名  : APP_VideoPreviewThread
 功能描述  : 远程视频预览处理线程
 输入参数  : HI_VOID *pArg
 输出参数  : 无
 返 回 值  : HI_VOID



 修改历史      :
  1.日    期   : 2012年8月20日星期一
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID *APP_VideoPreviewThread(HI_VOID *pArg)
{
    const HI_CHAR cSendMsg[1024]= "App write video review msg data!\r\n";
    DATA_SYNC_MSG_QUE_T stDataSync;    /* 发送数据同步消息队列 */
    HI_S32 s32Ret = 0;
	SHM_SEM_MNG_T *pstShmSemMng = NULL;
	SEM_MNG_T *pstSemMng = NULL;
	SHM_MNG_T *pstShmMng = NULL;

	HI3531_GetShmSemMng(IPC_VI_PREV_E, &pstShmSemMng);
	if (NULL == pstShmSemMng)
	{
		AV_PRINT( "#[app]HI3531_GetShmSemMng err! ");
        return NULL;
	}

	pstShmMng = &pstShmSemMng->shmMng;
	pstSemMng = &pstShmSemMng->semMng;
    s32Ret = MEM_ShmDataInit(&pstShmMng->shmAttr, pstSemMng->sysSemId);
    if (HI_SUCCESS != s32Ret)
    {
		AV_PRINT( "#[app]MEM_ShmDataInit err! s32Ret = %d", s32Ret);
        return NULL;
    }

    for ( ; ; )
    {
        //(HI_VOID)sleep(2);
        TM_USleep(2,0);
        s32Ret = MEM_WrShm(&pstShmMng->shmAttr, (HI_VOID *)cSendMsg, 1024, pstSemMng->sysSemId);
        if (HI_SUCCESS != s32Ret)
        {
            continue;
        }
        else
        {
            stDataSync.s32MsgType = APP_VIDEO_REVIEW_TYPE;
            stDataSync.cBuffer[0] = APP_READ_READY;

            /* 如果消息满，则阻塞 */
            s32Ret = msgsnd(g_iDataSyncMsgQueID, &stDataSync, sizeof(stDataSync.cBuffer), IPC_NOWAIT);
            if (0 > s32Ret)
            {
        		AV_PRINT( "#[app]video preview snd msg err! s32Ret = %d", s32Ret);
            }
        }
    }

	return NULL;
}

/*****************************************************************************
 函 数 名  : MEM_CreatSem
 功能描述  : 创建信号量
 输入参数  : const HI_CHAR *pcPath
             HI_S32 iID
             HI_S32 iSemID
             HI_S32 iSemValue
 输出参数  : 无
 返 回 值  :



 修改历史      :
  1.日    期   : 2012年8月20日星期一
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 MEM_CreatSem(const HI_CHAR *pcPath, HI_S32 s32Id, HI_S32 *ps32SemId, HI_S32 s32SemVal)
{
    key_t semKey;                             /* 消息队列对应的键值 */
	HI_S32 s32SemId = HI_SUCCESS;
	HI_S32 s32Ret = HI_FAILURE;

    if ((NULL == pcPath) || (NULL == ps32SemId))
    {
		AV_PRINT( "#[app]MEM_CreatSem para invalid !");
        return HI_FAILURE;
    }

	semKey = MEM_GetSemKey(pcPath, s32Id);
    if (HI_FAILURE == semKey )
    {
		AV_PRINT( "#[app]MEM_GetSemKey! semKey = %d", semKey );
        return HI_FAILURE;
    }

	s32SemId = MEM_GetSemId(semKey);
	if (HI_FAILURE == s32SemId)
	{
		AV_PRINT( "#[app]MEM_GetSemId! semId = %d", s32SemId );
        return HI_FAILURE;
	}

    *ps32SemId = s32SemId;

    s32Ret = MEM_SetSemVal(s32SemId, s32SemVal);
	if (HI_FAILURE == s32Ret)
	{
		return s32Ret;
	}

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : MEM_CreatShm
 功能描述  : 创建共享内存
 输入参数  : const HI_CHAR *pcPath  共享内存节点路径
             HI_S32 s32Id    共享内存节点ID值
             HI_VOID **pShmAddr  共享内存首地址
             HI_S32 s32ShmSize 大小
 输出参数  : 无
 返 回 值  :


 修改历史      :
  1.日    期   : 2012年8月20日星期一
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 MEM_CreatShm(const HI_CHAR *pcPath, HI_S32 s32Id, HI_VOID **pShmAddr, HI_S32 s32ShmSize)
{
    key_t shmKey;     	/* 共享内存的键值 */
    HI_S32 s32ShmId;    /* 共享内存Id值 */
    HI_S32 s32Ret = HI_SUCCESS;

    if ((NULL == pcPath) || (0 == s32ShmSize))
    {
		AV_PRINT("MEM_CreatShm para invalid");
        return HI_FAILURE;
    }

    /* 共享内存键值 */
    if ((shmKey = ftok(pcPath, s32Id)) == -1)
    {
		AV_PRINT("Creat shm Key Error, shmKey=%d, s32Id=%d", shmKey, s32Id);
        return HI_FAILURE;
    }

    /* 共享内存ID值 */
    if((s32ShmId = shmget(shmKey, s32ShmSize, 0666|IPC_CREAT )) == -1)
    {
		AV_PRINT("Get ShmId Error, s32ShmId=%d, s32Id=%d, err(%s)", s32ShmId, s32Id, ERR_STR);
        return HI_FAILURE;
    }

    /* 锁定该区域的内存 */
    s32Ret = shmctl(s32ShmId, SHM_LOCK, 0);
    if(HI_SUCCESS != s32Ret )
    {
		AV_PRINT("Lock Shm Error, s32Ret=%d, s32Id=%d, err(%s)", s32Ret, s32Id, ERR_STR);
        return HI_FAILURE;
    }

    /* 获取共享内存首地址 */
    *pShmAddr= shmat(s32ShmId, (const HI_VOID*)0, 0);
    if (-1 == (HI_S32)*pShmAddr)
    {
		AV_PRINT("Get Shm Addr Error, s32Id=%d, err(%s)", s32Id, ERR_STR);
        return HI_FAILURE;
    }

    memset(*pShmAddr, 0, s32ShmSize);
    return HI_SUCCESS;
}
#ifdef STORE_MULTI_THD
HI_S32 MEM_GetShmFullStat(HI_S32 s32Chn, HI_S32 *ps32MemFullStat)
{
    REC_MEM_HEAD_T *pstRecMemHead = NULL;
	SHM_MNG_T *pstShmMng = NULL;
	SHM_ATTR_T *pstRecShmAttr = NULL;
    CHN_STOR_MGMT_ST *pstChnStorMgmt = &g_stChnStoreMgmt[s32Chn];

    pthread_rwlock_rdlock(&pstChnStorMgmt->RecMemRWLock);
	pstShmMng = &pstChnStorMgmt->stShmSemMng.shmMng;
	pstRecShmAttr = &pstShmMng->shmAttr;

    if (NULL == pstRecShmAttr)
    {
		AV_PRINT( "#input para invalid");
        pthread_rwlock_unlock(&pstChnStorMgmt->RecMemRWLock);
        return HI_FAILURE;
    }

    pstRecMemHead = (REC_MEM_HEAD_T *)(pstRecShmAttr->pShmAddr);
    if (NULL == pstRecMemHead)
    {
		AV_PRINT( "#input para invalid");
        pthread_rwlock_unlock(&pstChnStorMgmt->RecMemRWLock);
        return HI_FAILURE;
    }

    *ps32MemFullStat = pstRecMemHead->s32MemFullStat;
    pthread_rwlock_unlock(&pstChnStorMgmt->RecMemRWLock);

    return HI_SUCCESS;
}

HI_S32 MEM_GetShmEmptyStat(HI_S32 s32Chn, HI_S32 *ps32MemFullStat)
{
    REC_MEM_HEAD_T *pstRecMemHead = NULL;
	SHM_MNG_T *pstShmMng = NULL;
	SHM_ATTR_T *pstRecShmAttr = NULL;
    CHN_STOR_MGMT_ST *pstChnStorMgmt = &g_stChnStoreMgmt[s32Chn];

    pthread_rwlock_rdlock(&pstChnStorMgmt->RecMemRWLock);
	pstShmMng = &pstChnStorMgmt->stShmSemMng.shmMng;
	pstRecShmAttr = &pstShmMng->shmAttr;

    if ((NULL == ps32MemFullStat))
    {
		AV_PRINT( "#input para invalid");
        pthread_rwlock_unlock(&pstChnStorMgmt->RecMemRWLock);
        return HI_FAILURE;
    }

    pstRecMemHead = (REC_MEM_HEAD_T *)(pstRecShmAttr->pShmAddr);
    if (NULL == pstRecMemHead)
    {
		AV_PRINT( "#input para invalid");
        pthread_rwlock_unlock(&pstChnStorMgmt->RecMemRWLock);
        return HI_FAILURE;
    }

    *ps32MemFullStat = pstRecMemHead->s32MemEmptyStat;
    pthread_rwlock_unlock(&pstChnStorMgmt->RecMemRWLock);

    return HI_SUCCESS;
}
#endif
/*****************************************************************************
 函 数 名  : MEM_InitChnPreMemHead
 功能描述  :
 输入参数  :
 输出参数  :
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年12月25日
    作    者   :
    修改内容   :

*****************************************************************************/
HI_VOID MEM_InitChnPreMemHead(HI_VOID)
{
    HI_S32 i = 0;
    REC_MEM_HEAD_T *pstRecMemHead = NULL;  /* 共享内存头部数据指针 */
    REC_CHN_MEM_HEAD_T *pstRecChnMemHead = NULL;
	SHM_SEM_MNG_T *pstShmSemMng = NULL;
	SHM_MNG_T *pstShmMng = NULL;
	SHM_ATTR_T *pstShmAttr = NULL;

	HI3531_GetShmSemMng(IPC_VI_PREV_E, &pstShmSemMng);

	pstShmMng = &pstShmSemMng->shmMng;
	pstShmAttr = &pstShmMng->shmAttr;

    pstRecMemHead = (REC_MEM_HEAD_T *)(pstShmAttr->pShmAddr);

    for (i = 0 ; i < pstShmAttr->s32ItemMaxNum; i++)
    {
        pstRecChnMemHead = (REC_CHN_MEM_HEAD_T *)((HI_CHAR *)pstRecMemHead +
                             sizeof(REC_MEM_HEAD_T) + i* pstShmAttr->s32ItemSize);
        pstRecChnMemHead->s32RecDataLen = 0;
        pstRecChnMemHead->s32WrChn = 0;
    }

    return;
 }

#ifdef STORE_MULTI_THD
HI_VOID MEM_InitRecMemHead(HI_VOID)
{
    REC_MEM_HEAD_T stRecMemHead;
    HI_S32 s32ChnCnt = 0;
	SHM_MNG_T *pstShmMng = NULL;
	SHM_ATTR_T *pstShmAttr = NULL;
    CHN_STOR_MGMT_ST *pstChnStorMgmt = NULL;

    for (s32ChnCnt = 0; s32ChnCnt < APP_VENC_CHNCNT; s32ChnCnt++)
    {
        pstChnStorMgmt = &g_stChnStoreMgmt[s32ChnCnt];
    	pstShmMng = &pstChnStorMgmt->stShmSemMng.shmMng;
    	pstShmAttr = &pstShmMng->shmAttr;
        pstChnStorMgmt->bRecording = HI_FALSE;
        memset(&stRecMemHead, 0, sizeof(REC_MEM_HEAD_T));
        stRecMemHead.cWrChnIdx = 0;
    	stRecMemHead.s32MemFullStat = 0x0;
        stRecMemHead.s32MemEmptyStat = 12; /*1100*/
        memcpy(pstShmAttr->pShmAddr, &stRecMemHead, sizeof(REC_MEM_HEAD_T));
    }

    return;
}

#endif

#ifdef STORE_MULTI_THD

HI_VOID MEM_PrintChnStorMgmt(HI_VOID)
{
    HI_S32 s32ChnCnt = 0;
//	SEM_MNG_T *pSemMng = NULL;
	SHM_MNG_T *pShmMng = NULL;;
	SHM_ATTR_T *pShmAttr = NULL;
    CHN_STOR_MGMT_ST *pstChnStorMgmt = NULL;

    for (s32ChnCnt = 0; s32ChnCnt < APP_VENC_CHNCNT; s32ChnCnt++)
    {
        pstChnStorMgmt = &g_stChnStoreMgmt[s32ChnCnt];

//        pSemMng = &pstChnStorMgmt->stShmSemMng.semMng;
    	pShmMng = &pstChnStorMgmt->stShmSemMng.shmMng;
    	pShmAttr = &pShmMng->shmAttr;

        AV_PRINT("s32Chn(%d)Recbuf is s32ItemMaxNum(%d),s32ItemSize(%d),shmSize(%d)",
            s32ChnCnt, pShmAttr->s32ItemMaxNum, pShmAttr->s32ItemSize, pShmMng->shmSize);
    }

}

HI_S32 MEM_WrVencStream2Shm(HI_VOID *pVencData, HI_S32 s32Chn, HI_S32 s32FrameLen)
{
    REC_MEM_HEAD_T *pstRecMemHead = NULL;  /* 共享内存头部数据指针 */
    REC_CHN_MEM_HEAD_T *pstRecChnMemHead = NULL;
    HI_CHAR *pcTmp = NULL;
    HI_S32 s32Index = 0;
    HI_S32 iTmpLen = 0;
    HI_S32 i = 0;
	SHM_MNG_T *pstShmMng = NULL;
	SHM_ATTR_T *pstRecShmAttr = NULL;

    CHN_STOR_MGMT_ST *pstChnStorMgmt = &g_stChnStoreMgmt[s32Chn];

    pthread_rwlock_wrlock(&pstChnStorMgmt->RecMemRWLock);
	pstShmMng = &pstChnStorMgmt->stShmSemMng.shmMng;
	pstRecShmAttr = &pstShmMng->shmAttr;

    if ((NULL == pstRecShmAttr) || (0 == s32FrameLen))
    {
		AV_PRINT("input para invaild");
        pthread_rwlock_unlock(&pstChnStorMgmt->RecMemRWLock);
        return HI_FAILURE;
    }

    if (s32FrameLen > (pstRecShmAttr->s32ItemSize - (HI_S32)sizeof(REC_CHN_MEM_HEAD_T)))
    {
        AV_PRINT("chn(%d) input frame len(%d) > s32ItemSize(%d) - recChnMemHead(%d)",
			s32Chn, s32FrameLen, pstRecShmAttr->s32ItemSize, sizeof(REC_CHN_MEM_HEAD_T));
        pthread_rwlock_unlock(&pstChnStorMgmt->RecMemRWLock);
        return HI_FAILURE;
    }

    pstRecMemHead = (REC_MEM_HEAD_T *)(pstRecShmAttr->pShmAddr);
    if (NULL == pstRecMemHead)
    {
		AV_PRINT("input para invalid");
        pthread_rwlock_unlock(&pstChnStorMgmt->RecMemRWLock);
        return HI_FAILURE;
    }

    s32Index = pstRecMemHead->cWrChnIdx;
    #ifdef MEM_DEBUG
    MEM_DBG_PRT("s32Chn(%d),s32Index(%d),pstRecShmAttr->s32ItemMaxNum(%d)", s32Chn,s32Index,pstRecShmAttr->s32ItemMaxNum);
    #endif
    if (s32Index >= pstRecShmAttr->s32ItemMaxNum)
    {
        AV_PRINT("mem index error s32Index=%d", s32Index);
        pthread_rwlock_unlock(&pstChnStorMgmt->RecMemRWLock);
        return HI_FAILURE;
    }

    /* 获取内存块首地址 */
    pstRecChnMemHead = (REC_CHN_MEM_HEAD_T *)((HI_CHAR *)pstRecMemHead
    					+ sizeof(REC_MEM_HEAD_T)
    					+ s32Index * pstRecShmAttr->s32ItemSize);
    pstRecChnMemHead->s32WrChn = s32Chn;

    iTmpLen = pstRecChnMemHead->s32RecDataLen + s32FrameLen + sizeof(REC_CHN_MEM_HEAD_T);
    #ifdef MEM_DEBUG
    MEM_DBG_PRT("s32Chn(%d) WrFramelen(%d) Tmplen(%d) s32MemFullStat1(0x%x) s32MemEmptyStat(0x%x)",\
                    s32Chn, s32FrameLen, iTmpLen,\
                    pstRecMemHead->s32MemFullStat, pstRecMemHead->s32MemEmptyStat);
    #endif
	if (iTmpLen > pstRecShmAttr->s32ItemSize)
    {
        /* 给该内存块修改为满 */
        pstRecMemHead->s32MemFullStat = (pstRecMemHead->s32MemFullStat | (0x01 << s32Index));

		/* 查找空的内存块 */
        if (pstRecMemHead->s32MemEmptyStat == 0)
        {
		    AV_PRINT("MEM_WrVencStream2Shm, s32Chn(%d) overload!!!",s32Chn);
            pthread_rwlock_unlock(&pstChnStorMgmt->RecMemRWLock);
            return HI_FAILURE;
        }
        #ifdef MEM_DEBUG
        MEM_DBG_PRT("::::s32Chn(%d),s32Index(%d) Full s32MemFullStat2(0x%x) s32MemEmptyStat(0x%x)",\
                    s32Chn, s32Index, pstRecMemHead->s32MemFullStat, pstRecMemHead->s32MemEmptyStat);
        #endif
        /* 查找空的内存块 */
        for (i = 0; i < pstRecShmAttr->s32ItemMaxNum; i++)
        {
			if (pstRecMemHead->s32MemEmptyStat & (0x01 << i))
            {
                /* 将该块内存块设置为正在使用 */
                pstRecMemHead->s32MemEmptyStat = (pstRecMemHead->s32MemEmptyStat ^ (0x01 << i));
                break;
            }
        }

        /* 更新内存头部该通道使用的内存块下标 */
        pstRecMemHead->cWrChnIdx = i;
        pstRecChnMemHead = (REC_CHN_MEM_HEAD_T *)((HI_CHAR *)pstRecMemHead + sizeof(REC_MEM_HEAD_T) +
                             i*pstRecShmAttr->s32ItemSize);
        pstRecChnMemHead->s32RecDataLen = 0;
        #ifdef MEM_DEBUG
        MEM_DBG_PRT("::::next s32Index(%d) s32MemFullStat2(0x%x) s32MemEmptyStat(0x%x)",\
                    i, pstRecMemHead->s32MemFullStat, pstRecMemHead->s32MemEmptyStat);
        #endif

    }

    /* 该内存块有效数据未地址 */
    pcTmp = (HI_CHAR *)pstRecChnMemHead + sizeof(REC_CHN_MEM_HEAD_T) + pstRecChnMemHead->s32RecDataLen;
    pstRecChnMemHead->s32RecDataLen = pstRecChnMemHead->s32RecDataLen + s32FrameLen;
    pstRecChnMemHead->s32WrChn = s32Chn;
    memcpy(pcTmp, pVencData, s32FrameLen);

    pthread_rwlock_unlock(&pstChnStorMgmt->RecMemRWLock);
    return HI_SUCCESS;
}

#endif

/*****************************************************************************
 函 数 名  : APP_CheckChnRecodeDateIsExit
 功能描述  : 检查指定通道录像数据在共享内存中是否存在
 输入参数  : SHM_ATTR_T *pstRecShmAttr
             HI_S32 iSemID
             HI_S32 s32Index
             HI_S32 iChn
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年4月23日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_IsChnRecDataExistInShm(SHM_ATTR_T *pstRecShmAttr, pthread_rwlock_t *pstLock,
                                                 HI_S32 s32Index, HI_S32 iChn)
{
    REC_CHN_MEM_HEAD_T *pstRecChnMemHead = NULL;
    REC_MEM_HEAD_T *pstRecMemHead = NULL;  /* 共享内存头部数据指针 */
    HI_CHAR *pcTmp = NULL;

    if (NULL == pstRecShmAttr)
    {
		AV_PRINT("input para invalid");
        return HI_FAILURE;
    }

    if (s32Index >= pstRecShmAttr->s32ItemMaxNum)
    {
        AV_PRINT( "#[app]Index error,s32Index=%d", s32Index);
        return HI_FAILURE;
    }

    pstRecMemHead = (REC_MEM_HEAD_T *)pstRecShmAttr->pShmAddr;

    if (NULL == pstRecMemHead)
    {
		AV_PRINT( "#input para invalid");
        return HI_FAILURE;
    }

    /* 获取共享内存写锁 */
    pthread_rwlock_rdlock(pstLock);
    pcTmp = (HI_CHAR *)pstRecShmAttr->pShmAddr + sizeof(REC_MEM_HEAD_T)+s32Index*pstRecShmAttr->s32ItemSize;
    pstRecChnMemHead = (REC_CHN_MEM_HEAD_T *)pcTmp;
    if (pstRecChnMemHead->s32WrChn == iChn)
    {
        pthread_rwlock_unlock(pstLock);
        return HI_SUCCESS;
    }
    pthread_rwlock_unlock(pstLock);

    return HI_FAILURE;
}

/*****************************************************************************
 函 数 名  : MEM_GetRecFromOneMem
 功能描述  : 获取一块共享内存数据
 输入参数  : SHM_ATTR_T *pstRecShmAttr
             HI_S32 iSemID
             HI_S32 s32Index 要获取共享内存块下标
             HI_CHAR *pcMemFullStatus
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2013年7月3日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 MEM_GetRecFromOneMem(SHM_ATTR_T *pstRecShmAttr, pthread_rwlock_t *pstLock,
                                           HI_S32 s32Index, HI_CHAR *pDataBuf)
{
    REC_CHN_MEM_HEAD_T *pstRecChnMemHead = NULL;
    REC_MEM_HEAD_T *pstRecMemHead = NULL;  /* 共享内存头部数据指针 */
    HI_CHAR *pcTmp = NULL;

    if ((NULL == pDataBuf) || (NULL == pstRecShmAttr))
    {
		AV_PRINT("MEM_GetRecFromOneMem para invalid");
        return HI_FAILURE;
    }

    if (s32Index >= pstRecShmAttr->s32ItemMaxNum)
    {
        AV_PRINT("Index error,s32Index=%d", s32Index);
        return HI_FAILURE;
    }

    pstRecMemHead = (REC_MEM_HEAD_T *)pstRecShmAttr->pShmAddr;
    if (NULL == pstRecMemHead)
    {
		AV_PRINT("input para invalid");
        return HI_FAILURE;
    }

    /* 获取共享内存写锁 */
    pthread_rwlock_wrlock(pstLock);

    pcTmp = (HI_CHAR *)pstRecShmAttr->pShmAddr + sizeof(REC_MEM_HEAD_T)+s32Index*pstRecShmAttr->s32ItemSize;
    pstRecChnMemHead = (REC_CHN_MEM_HEAD_T *)pcTmp;
    if (pstRecChnMemHead->s32RecDataLen + (HI_S32)sizeof(REC_CHN_MEM_HEAD_T) <= pstRecShmAttr->s32ItemSize)
    {
        memcpy(pDataBuf, pcTmp, pstRecChnMemHead->s32RecDataLen + sizeof(REC_CHN_MEM_HEAD_T));
    }
    else
    {
        AV_PRINT("Data len error! pstRecChnMemHead->s32RecDataLen=%d", pstRecChnMemHead->s32RecDataLen);
    }

    /*读到数据 */
    if (pstRecChnMemHead->s32RecDataLen > 0)
    {
        #ifdef MEM_DEBUG
        MEM_DBG_PRT("start s32MemEmptyStat(%d), s32RecFullStat = 0x%x",
            pstRecMemHead->s32MemEmptyStat, pstRecMemHead->s32MemFullStat);
        #endif
        /* 将该共享内存设置为空闲状态 */
        pstRecMemHead->s32MemEmptyStat = (pstRecMemHead->s32MemEmptyStat |(0x01 << s32Index));
        pstRecMemHead->s32MemFullStat = (pstRecMemHead->s32MemFullStat &(~(0x01 << s32Index)));
        #ifdef MEM_DEBUG
        MEM_DBG_PRT("end   s32MemEmptyStat(%d), s32RecFullStat = 0x%x",
            pstRecMemHead->s32MemEmptyStat, pstRecMemHead->s32MemFullStat);
        #endif
        pthread_rwlock_unlock(pstLock);
        return HI_SUCCESS;
   }

   pthread_rwlock_unlock(pstLock);
   return HI_FAILURE;

}

#ifdef STORE_MULTI_THD
HI_VOID MEM_ShmRecBufInit(HI_VOID)
{
    HI_S32 s32Ret = 0;
    HI_S32 s32ChnCnt = 0;
	SEM_MNG_T *pSemMng = NULL;
	SHM_MNG_T *pShmMng = NULL;;
	SHM_ATTR_T *pShmAttr = NULL;
    CHN_STOR_MGMT_ST *pstChnStorMgmt = NULL;

    for (s32ChnCnt = 0; s32ChnCnt < APP_VENC_CHNCNT; s32ChnCnt++)
    {
        pstChnStorMgmt = &g_stChnStoreMgmt[s32ChnCnt];
        memset(pstChnStorMgmt, 0, sizeof(CHN_STOR_MGMT_ST));

        pSemMng = &pstChnStorMgmt->stShmSemMng.semMng;
    	pShmMng = &pstChnStorMgmt->stShmSemMng.shmMng;
    	pShmAttr = &pShmMng->shmAttr;
    	pSemMng->semPath = (HI_CHAR *)IPC_PATH;
    	pSemMng->userSemId = 0;
    	pSemMng->sysSemId = 0;
    	pShmMng->shmPath = (HI_CHAR *)IPC_PATH;
        if (s32ChnCnt  == 0)
        {
    	    pShmMng->userShmId = SHM_RECBUF_ID_CHAN1;
        }
        if (s32ChnCnt  == 1)
        {
    	    pShmMng->userShmId = SHM_RECBUF_ID_CHAN2;
        }
        if (s32ChnCnt  == 2)
        {
    	    pShmMng->userShmId = SHM_RECBUF_ID_CHAN3;
        }
        if (s32ChnCnt  == 3)
        {
    	    pShmMng->userShmId = SHM_RECBUF_ID_CHAN4;
        }
        if (s32ChnCnt  == 4)
        {
    	    pShmMng->userShmId = SHM_RECBUF_ID_CHAN5;
        }
        if (s32ChnCnt  == 5)
        {
    	    pShmMng->userShmId = SHM_RECBUF_ID_CHAN6;
        }
        if (s32ChnCnt  == 6)
        {
    	    pShmMng->userShmId = SHM_RECBUF_ID_CHAN7;
        }
        if (s32ChnCnt  == 7)
        {
    	    pShmMng->userShmId = SHM_RECBUF_ID_CHAN8;
        }
	  if (s32ChnCnt  == 8)
        {
    	    pShmMng->userShmId = SHM_RECBUF_ID_CHAN9;
        }
        if (s32ChnCnt  == 9)
        {
    	    pShmMng->userShmId = SHM_RECBUF_ID_CHAN10;
        }
        if (s32ChnCnt  == 10)
        {
    	    pShmMng->userShmId = SHM_RECBUF_ID_CHAN11;
        }
        if (s32ChnCnt  == 11)
        {
    	    pShmMng->userShmId = SHM_RECBUF_ID_CHAN12;
        }

    	pShmMng->shmSize = SHM_RECBUF_MAX_SIZE;
    	pShmAttr->pShmAddr = NULL;
    	pShmAttr->s32ItemMaxNum = SHM_RECBUF_MAX_NUM;
    	pShmAttr->s32ItemSize = SHM_RECBUF_ONE_SIZE;


        s32Ret = pthread_rwlock_init(&pstChnStorMgmt->RecMemRWLock, NULL);
        if (s32Ret != 0)
        {
			return;
        }

        AV_PRINT("s32Chn(%d)Recbuf is s32ItemMaxNum(%d),s32ItemSize(%d),shmSize(%d)",
            s32ChnCnt, pShmAttr->s32ItemMaxNum, pShmAttr->s32ItemSize, pShmMng->shmSize);

        MEM_CreatShm(pShmMng->shmPath, pShmMng->userShmId, &pShmAttr->pShmAddr, pShmMng->shmSize);
    }

}
#endif

#ifdef HI3531
/*****************************************************************************
 函 数 名  : HI3531_IpSpkSndThread
 功能描述  :
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  : HI_SUCCESS 执行成功
             HI_FAILURE  执行失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月11日
    作    者   : zlq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3531_IpSpkSndThread(HI_VOID)
{
    HI_S32 s32Ret;
	pthread_t IpSpkSndTid;

    /* 创建音频消息队列接收线程 */
    s32Ret = pthread_create(&IpSpkSndTid, NULL, APP_IpSpkSndThread, NULL);
    if (HI_SUCCESS > s32Ret)
	{
        AV_PRINT( "#[app]Creat HI3531_IpSpkSndThread failed!");

        return HI_FAILURE;
	}
    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : HI3531_IpSpkSndThread
 功能描述  :
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  : HI_SUCCESS 执行成功
             HI_FAILURE  执行失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月11日
    作    者   : zlq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3531_IpSpkRcvThread(HI_VOID)
{
    HI_S32 s32Ret;
	pthread_t IpSpkRcvTid;

    /* 创建音频消息队列接收线程 */
    s32Ret = pthread_create(&IpSpkRcvTid, NULL, APP_IpSpkRcvThread, NULL);
    if (HI_SUCCESS > s32Ret)
	{
        AV_PRINT( "#[app]Creat HI3531_IpSpkRcvThread failed!");

        return HI_FAILURE;
	}
    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : HI3531_ShmSemMngInit
 功能描述  :
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  : HI_SUCCESS 执行成功
             HI_FAILURE  执行失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月11日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3531_ShmSemMngInit(HI_VOID)
{
	SEM_MNG_T *pSemMng = NULL;
	SHM_MNG_T *pShmMng = NULL;;
	SHM_ATTR_T *pShmAttr = NULL;

	pSemMng = &gShmSemMng[0].semMng;
	pShmMng = &gShmSemMng[0].shmMng;
	pShmAttr = &pShmMng->shmAttr;
	pSemMng->semPath = (HI_CHAR *)IPC_PATH;
	pSemMng->userSemId = SEM_IPSPK_SND_ID;
	pSemMng->sysSemId = 0;
	pShmMng->shmPath = (HI_CHAR *)IPC_PATH;
	pShmMng->userShmId = SHM_IPSPK_SND_ID;
	pShmMng->shmSize = SHM_IPSPK_MAX_SIZE;
	pShmAttr->pShmAddr = NULL;
	pShmAttr->s32ItemMaxNum = SHM_IPSPK_MAX_NUM;
	pShmAttr->s32ItemSize = SHM_IPSPK_ONE_SIZE;

	pSemMng = &gShmSemMng[1].semMng;
	pShmMng = &gShmSemMng[1].shmMng;
	pShmAttr = &pShmMng->shmAttr;
	pSemMng->semPath = (HI_CHAR *)IPC_PATH;
	pSemMng->userSemId = SEM_IPSPK_RCV_ID;
	pSemMng->sysSemId = 0;
	pShmMng->shmPath = (HI_CHAR *)IPC_PATH;
	pShmMng->userShmId = SHM_IPSPK_RCV_ID;
	pShmMng->shmSize = SHM_IPSPK_MAX_SIZE;
	pShmAttr->pShmAddr = NULL;
	pShmAttr->s32ItemMaxNum = SHM_IPSPK_MAX_NUM;
	pShmAttr->s32ItemSize = SHM_IPSPK_ONE_SIZE;


	pSemMng = &gShmSemMng[2].semMng;
	pShmMng = &gShmSemMng[2].shmMng;
	pShmAttr = &pShmMng->shmAttr;
	pSemMng->semPath = (HI_CHAR *)IPC_PATH;
	pSemMng->userSemId = SEM_VI_PREV_ID;
	pSemMng->sysSemId = 0;
	pShmMng->shmPath = (HI_CHAR *)IPC_PATH;
	pShmMng->userShmId = SHM_VI_PREV_ID;
	pShmMng->shmSize = SHM_VI_PREV_MAX_SIZE;
	pShmAttr->pShmAddr = NULL;
	pShmAttr->s32ItemMaxNum = SHM_VI_PREV_MAX_NUM;
	pShmAttr->s32ItemSize = SHM_VI_PREV_ONE_SIZE;

	pSemMng = &gShmSemMng[3].semMng;
	pShmMng = &gShmSemMng[3].shmMng;
	pShmAttr = &pShmMng->shmAttr;
	pSemMng->semPath = (HI_CHAR *)IPC_PATH;
	pSemMng->userSemId = SEM_SNAP_ID;
	pSemMng->sysSemId = 0;
	pShmMng->shmPath = (HI_CHAR *)IPC_PATH;
	pShmMng->userShmId = SHM_SNAP_ID;
	pShmMng->shmSize = SHM_SNAP_MAX_SIZE;
	pShmAttr->pShmAddr = NULL;
	pShmAttr->s32ItemMaxNum = SHM_SNAP_MAX_NUM;
	pShmAttr->s32ItemSize = SHM_SNAP_ONE_SIZE;

	return;
}

/*****************************************************************************
 函 数 名  : HI3531_GetShmSemMng
 功能描述  :
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  : HI_SUCCESS 执行成功
             HI_FAILURE  执行失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月11日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3531_GetShmSemMng(IPC_TYPE_E eIpcType, SHM_SEM_MNG_T **pstShmSemMng)
{
	if (eIpcType >= IPC_INVALID)
	{
		*pstShmSemMng = NULL;
		return;
	}
	*pstShmSemMng = &gShmSemMng[eIpcType];
	return;
}

/*****************************************************************************
 函 数 名  : HI3531_ShmInit
 功能描述  :
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  : HI_SUCCESS 执行成功
             HI_FAILURE  执行失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月11日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3531_ShmInit(HI_VOID)
{
	HI_S32 s32IpcType = (HI_S32)IPC_IPSPK_SND_E;
	SHM_SEM_MNG_T *pstShmSemMng = NULL;
	SHM_MNG_T *pShmMng = NULL;
	SHM_ATTR_T *pShmAttr = NULL;

	for (; s32IpcType < (HI_S32)IPC_INVALID; s32IpcType++)
	{
		HI3531_GetShmSemMng((IPC_TYPE_E)s32IpcType, &pstShmSemMng);
		if (pstShmSemMng == NULL)
		{
			AV_PRINT("pstShmSemMng is NULL");
			continue;
		}

		pShmMng = &pstShmSemMng->shmMng;
		pShmAttr = &pShmMng->shmAttr;

		MEM_CreatShm(pShmMng->shmPath, pShmMng->userShmId, &pShmAttr->pShmAddr, pShmMng->shmSize);
	}

	/*录像初始化 */
    MEM_ShmRecBufInit();

    /*录像头数据初始化*/
	MEM_InitRecMemHead();

    MEM_InitChnPreMemHead();

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI3531_SemInit
 功能描述  : 初始化共享内存信号量
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  : HI_SUCCESS  执行成功
             HI_FAILURE   执行失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月27日星期四
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3531_SemInit(HI_VOID)
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_S32 s32IpcType = 0;
	SHM_SEM_MNG_T *pstShmSemMng = NULL;
	SEM_MNG_T *pSemMng = NULL;

	for (s32IpcType = 0; s32IpcType < (HI_S32)IPC_INVALID; s32IpcType++)
	{
		/*REC BUF没有使用SEM*/
	/*	if ((IPC_TYPE_E)s32IpcType == IPC_REC_BUF_E)
		{
			continue;
		}*/

		HI3531_GetShmSemMng((IPC_TYPE_E)s32IpcType, &pstShmSemMng);
		if (pstShmSemMng == NULL)
		{
			AV_PRINT("pstShmSemMng is NULL");
			continue;
		}

		pSemMng = &pstShmSemMng->semMng;
		s32Ret = MEM_CreatSem(pSemMng->semPath, pSemMng->userSemId, &pSemMng->sysSemId, 1);
		if (HI_SUCCESS != s32Ret)
		{
			AV_PRINT("MEM_CreatSem Error");
			return HI_FAILURE;
		}
	}

    return HI_SUCCESS;
}


#endif
