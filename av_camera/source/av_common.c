/******************************************************************************

                  版权所有 (C), 2001-2011, 郑州天迈科技有限公司

 ******************************************************************************
  文 件 名   : drv_app_common.c
  版 本 号   : 初稿
  作    者   : xzhang
  生成日期   : 2014年11月21日
  最近修改   :
  功能描述   : 
  函数列表   :
  修改历史   :
  1.日    期   : 2014年11月21日
    作    者   : xzhang
    修改内容   : 创建文件

******************************************************************************/
#include "av_public_head.h"
#include "av_common_31.h"
#include "av_video_proc.h"
#include "av_public_ctrl_cmd.h"


/*状态控制,用于控制对不同的状态互斥访问*/
RW_LOCK_T gRWLock[LOCK_INVALID_E];

/*状态与锁类型的映射表*/
LOCK_MAP_T gLockMap[LOCK_INVALID_E];

/* OSD设备号显示相关结构体 */
OSD_DEV_T gOsdDev;                

OSD_EXT_T gOsdExt;

/* MP3播放结构体 */
IFLY_MP3_PLAY_T gMp3PlayStruct;  

/* MP3播放列表 */
HI_CHAR gMp3PlayList[APP_MP3PLAYNAMENUM][APP_MP3PLAYFILELEN];

int g_iAvPrintfFlag = 1;


/*crc16表格(低地址存放高字节)*/
const HI_U16 gCrc16Table[256]=
{
    0x0000,0xC0C1,0xC181,0x0140,0xC301,0x03C0,0x0280,0xC241,
    0xC601,0x06C0,0x0780,0xC741,0x0500,0xC5C1,0xC481,0x0440,
    0xCC01,0x0CC0,0x0D80,0xCD41,0x0F00,0xCFC1,0xCE81,0x0E40,
    0x0A00,0xCAC1,0xCB81,0x0B40,0xC901,0x09C0,0x0880,0xC841,
    0xD801,0x18C0,0x1980,0xD941,0x1B00,0xDBC1,0xDA81,0x1A40,
    0x1E00,0xDEC1,0xDF81,0x1F40,0xDD01,0x1DC0,0x1C80,0xDC41,
    0x1400,0xD4C1,0xD581,0x1540,0xD701,0x17C0,0x1680,0xD641,
    0xD201,0x12C0,0x1380,0xD341,0x1100,0xD1C1,0xD081,0x1040,
    0xF001,0x30C0,0x3180,0xF141,0x3300,0xF3C1,0xF281,0x3240,
    0x3600,0xF6C1,0xF781,0x3740,0xF501,0x35C0,0x3480,0xF441,
    0x3C00,0xFCC1,0xFD81,0x3D40,0xFF01,0x3FC0,0x3E80,0xFE41,
    0xFA01,0x3AC0,0x3B80,0xFB41,0x3900,0xF9C1,0xF881,0x3840,
    0x2800,0xE8C1,0xE981,0x2940,0xEB01,0x2BC0,0x2A80,0xEA41,
    0xEE01,0x2EC0,0x2F80,0xEF41,0x2D00,0xEDC1,0xEC81,0x2C40,
    0xE401,0x24C0,0x2580,0xE541,0x2700,0xE7C1,0xE681,0x2640,
    0x2200,0xE2C1,0xE381,0x2340,0xE101,0x21C0,0x2080,0xE041,
    0xA001,0x60C0,0x6180,0xA141,0x6300,0xA3C1,0xA281,0x6240,
    0x6600,0xA6C1,0xA781,0x6740,0xA501,0x65C0,0x6480,0xA441,
    0x6C00,0xACC1,0xAD81,0x6D40,0xAF01,0x6FC0,0x6E80,0xAE41,
    0xAA01,0x6AC0,0x6B80,0xAB41,0x6900,0xA9C1,0xA881,0x6840,
    0x7800,0xB8C1,0xB981,0x7940,0xBB01,0x7BC0,0x7A80,0xBA41,
    0xBE01,0x7EC0,0x7F80,0xBF41,0x7D00,0xBDC1,0xBC81,0x7C40,
    0xB401,0x74C0,0x7580,0xB541,0x7700,0xB7C1,0xB681,0x7640,
    0x7200,0xB2C1,0xB381,0x7340,0xB101,0x71C0,0x7080,0xB041,
    0x5000,0x90C1,0x9181,0x5140,0x9301,0x53C0,0x5280,0x9241,
    0x9601,0x56C0,0x5780,0x9741,0x5500,0x95C1,0x9481,0x5440,
    0x9C01,0x5CC0,0x5D80,0x9D41,0x5F00,0x9FC1,0x9E81,0x5E40,
    0x5A00,0x9AC1,0x9B81,0x5B40,0x9901,0x59C0,0x5880,0x9841,
    0x8801,0x48C0,0x4980,0x8941,0x4B00,0x8BC1,0x8A81,0x4A40,
    0x4E00,0x8EC1,0x8F81,0x4F40,0x8D01,0x4DC0,0x4C80,0x8C41,
    0x4400,0x84C1,0x8581,0x4540,0x8701,0x47C0,0x4680,0x8641,
    0x8201,0x42C0,0x4380,0x8341,0x4100,0x81C1,0x8081,0x4040
};

 
/*****************************************************************************
 函 数 名  : HI3531_CRC16
 功能描述  : crc16		X^16 + X^15 + X^2 + 1
 输入参数  : HI_U8 *u8Dt, HI_U16 u16Len
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月26日
    作    者   : zlq
    修改内容   : 新生成函数

*****************************************************************************/
HI_U16 HI3531_CRC16(HI_U8 *u8Dt, HI_U16 u16Len)
{
	HI_U16 u16Cnt;
	HI_U16 u16Res;

	for (u16Cnt = 0; u16Cnt < u16Len; u16Cnt++)
	{
		if (u8Dt[u16Cnt]!=0)
		{
			u16Res = 0;
			for (u16Cnt = 0; u16Cnt < u16Len; u16Cnt++)
			{
				u16Res = (gCrc16Table[u8Dt[u16Cnt] ^ (u16Res & 0xFF)] ^ (u16Res / 0x100));
			}
			u16Res = (u16Res >> 8) |(u16Res << 8);	 //适应小端模式
			return (u16Res);
		}
	}
	
	return (0xFFFF);
}


/*****************************************************************************
 函 数 名  : HI3531_InitStatMap
 功能描述  : 初始化状态与锁类型的映射表
 输入参数  : HI_VOID
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月21日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3531_InitStatMap(HI_VOID)
{	
	memset(gLockMap, 0, sizeof(gLockMap));
	
	/*lock record stat */
	gLockMap[LOCK_REC_STAT_E].stat = APP_VIDEOSTATUS_IDLE;
	//gLockMap[LOCK_REC_STAT_E].stat = APP_VIDEOSTATUS_RUN;
	gLockMap[LOCK_REC_STAT_E].eLockType = LOCK_REC_STAT_E;
	
	/*vout lock stat */
	gLockMap[LOCK_VO_CHN_E].stat = APP_VIDEOOUT_ISMORE_YES;
	gLockMap[LOCK_VO_CHN_E].eLockType = LOCK_VO_CHN_E;
	
	/*snap lock init */
	gLockMap[LOCK_SNAP_STAT_E].stat = APP_JPEGSTATUS_IDLE;
	gLockMap[LOCK_SNAP_STAT_E].eLockType = LOCK_SNAP_STAT_E;
	
	/*preview lock init */
	gLockMap[LOCK_VIDEO_PREVIEW_E].stat = APP_VIDEOSTATUS_IDLE;
	gLockMap[LOCK_VIDEO_PREVIEW_E].eLockType = LOCK_VIDEO_PREVIEW_E;
	
	/*osd dev lock init */
	gLockMap[LOCK_OSD_DEV_E].pData = &gOsdDev;
	gLockMap[LOCK_OSD_DEV_E].eLockType = LOCK_OSD_DEV_E;
	
	/*ext osd lock init */
	gLockMap[LOCK_OSD_EXT_E].pData = &gOsdExt;
	gLockMap[LOCK_OSD_EXT_E].eLockType = LOCK_OSD_EXT_E;

	/*ceshi zoulq*/
	/*disk stat init*/
	gLockMap[LOCK_DISK_STAT_E].stat = APP_DISK_UMOUNT;
	//gLockMap[LOCK_DISK_STAT_E].stat = APP_DISK_MOUNT;
	gLockMap[LOCK_DISK_STAT_E].eLockType = LOCK_DISK_STAT_E;
	
	/*disk event init*/
	//gLockMap[LOCK_DISK_EVENT_E].stat = APP_VIDEOSTATUS_RUN;
	gLockMap[LOCK_DISK_EVENT_E].stat = APP_VIDEOSTATUS_IDLE;
	gLockMap[LOCK_DISK_EVENT_E].eLockType = LOCK_DISK_EVENT_E;
	
	/*mp3 name list init*/
	gLockMap[LOCK_MP3_PLAY_LIST_E].pData = gMp3PlayList;
	gLockMap[LOCK_MP3_PLAY_LIST_E].eLockType = LOCK_MP3_PLAY_LIST_E;
	
	/*mp3 play stat init*/
	gLockMap[LOCK_MP3_PLAY_STAT_E].stat = APP_MP3PLAYIDLE;
	gLockMap[LOCK_MP3_PLAY_STAT_E].eLockType = LOCK_MP3_PLAY_STAT_E;
	
	/*mp3 play ctrl init*/
	gLockMap[LOCK_MP3_PLAY_CTRL_E].stat = CTRL_CMD_PLAYMP3FILE_STOP;
	gLockMap[LOCK_MP3_PLAY_CTRL_E].eLockType = LOCK_MP3_PLAY_CTRL_E;
	
	/*mp3 play struct init*/
	gLockMap[LOCK_MP3_PLAY_STRUCT_E].pData = &gMp3PlayStruct;
	gLockMap[LOCK_MP3_PLAY_STRUCT_E].eLockType = LOCK_MP3_PLAY_STRUCT_E;
	
	/*mp3 vol init*/
	gLockMap[LOCK_MP3_VOL_E].stat = 16;
	gLockMap[LOCK_MP3_VOL_E].eLockType = LOCK_MP3_VOL_E;
	
	/*ip speak init*/
	gLockMap[LOCK_IP_SPEAK_E].stat = APP_IPSPEAKIDLE;
	gLockMap[LOCK_IP_SPEAK_E].eLockType = LOCK_IP_SPEAK_E;
	
	/*rec buf init*/
	gLockMap[LOCK_REC_BUF_E].stat = 0;
	gLockMap[LOCK_REC_BUF_E].eLockType = LOCK_REC_BUF_E;
	
	return;
}

/*****************************************************************************
 函 数 名  : HI3531_GetLockMapElem
 功能描述  : 获状态与锁类型的映射表元素
 输入参数  : HI_VOID
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月21日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3531_GetLockMapElem(RW_LOCK_TYPE_E eLockType, HI_S32 *pS32Stat)
{	
	if (eLockType != gLockMap[eLockType].eLockType)
	{
		return HI_FAILURE;
	}
	
	*pS32Stat = gLockMap[eLockType].stat;
	
	return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : HI3531_InitData
 功能描述  : 初始化状态
 输入参数  : HI_VOID
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月21日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3531_InitData(RW_LOCK_TYPE_E eLockType)
{
	HI_S32 s32Stat = 0;
	HI_S32 s32Ret = HI_SUCCESS;

	s32Ret = HI3531_GetLockMapElem(eLockType, &s32Stat);
	if (s32Ret != HI_SUCCESS)
	{
		AV_PRINT( "HI3531_GetLockMapElem err s32Ret = %d", s32Ret);
		return s32Ret;	
	}
	
	gRWLock[eLockType].stat = s32Stat;
	
	return s32Ret;	
}

/*****************************************************************************
 函 数 名  : HI3531_InitLock
 功能描述  : 初始化锁
 输入参数  : HI_VOID
			 HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2014年11月21日
	作	  者   : xzhang
	修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3531_InitLock(RW_LOCK_TYPE_E eLockType)
{
	HI_S32 s32Ret = HI_SUCCESS;

	s32Ret = pthread_rwlock_init(&gRWLock[eLockType].rwLock, NULL);
	if (s32Ret != HI_SUCCESS)
	{
		AV_PRINT( "init rwlock err s32Ret = %d", s32Ret);
		return s32Ret;	
	}

	return s32Ret;
}


/*****************************************************************************
 函 数 名  : HI3531_InitRWLock
 功能描述  : 初始化状态控制
 输入参数  : HI_VOID
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月21日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_InitRWLock(HI_VOID)
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_S32 s32LockType = LOCK_REC_STAT_E;
	//RW_LOCK_TYPE_E s32LockType = LOCK_REC_STAT_E;
	
    AV_PRINT("HI3521_InitRWLock enter.");
	HI3531_InitStatMap();
	
	memset(gRWLock, 0, sizeof(gRWLock));
	
	for (s32LockType = LOCK_REC_STAT_E; s32LockType < LOCK_INVALID_E; ++s32LockType)
	{
		s32Ret = HI3531_InitLock((RW_LOCK_TYPE_E)s32LockType);
		if (s32Ret != HI_SUCCESS)
		{
			AV_PRINT( "HI3531_InitLock s32Ret(%x)", s32Ret);
			return s32Ret;	
		}

		s32Ret = HI3531_InitData((RW_LOCK_TYPE_E)s32LockType);
		if (s32Ret != HI_SUCCESS)
		{
			AV_PRINT( "HI3531_InitData s32Ret(%x)", s32Ret);
			return s32Ret;	
		}
	}
    AV_PRINT("HI3531_InitRWLock suc.");
	return s32Ret;
}


/*****************************************************************************
 函 数 名  : HI3531_GetLock
 功能描述  : 获取状态锁
 输入参数  : RW_LOCK_TYPE_E eLockType, RW_LOCK_T **pLock
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月21日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3531_GetLock(RW_LOCK_TYPE_E eLockType, RW_LOCK_T **pLock)
{
	if (eLockType >= LOCK_INVALID_E)
	{
		return HI_FAILURE;
	}
	
	*pLock = &gRWLock[eLockType];
	
	return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : HI3531_SetLockStat
 功能描述  : 设置锁状态
 输入参数  : RW_LOCK_T *pLock, HI_S32 stat
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月21日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3531_SetLockStat(RW_LOCK_T *pLock, HI_S32 s32Stat)
{
	HI_S32 s32Ret = HI_SUCCESS;

	if (NULL == pLock)
	{
		return HI_FAILURE;
	}
    s32Ret = pthread_rwlock_wrlock(&pLock->rwLock);
    if (s32Ret != HI_SUCCESS)
	{
        AV_PRINT( "pthread_rwlock_wrlock");
		return HI_FAILURE;
	}
	pLock->stat = s32Stat;
    s32Ret = pthread_rwlock_unlock(&pLock->rwLock);
    if (s32Ret != HI_SUCCESS)
	{
        AV_PRINT( "pthread_rwlock_unlock");
		return HI_FAILURE;
	}
	return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI3531_GetLockStat
 功能描述  : 获取锁状态
 输入参数  : RW_LOCK_T * pLock, HI_S32 stat
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月21日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3531_GetLockStat(RW_LOCK_T * pLock, HI_S32 *pS32Stat)
{
	HI_S32 s32Ret = HI_SUCCESS;

	if (NULL == pLock)
	{
		return HI_FAILURE;
	}
	
    s32Ret = pthread_rwlock_rdlock(&pLock->rwLock);
    if (s32Ret != HI_SUCCESS)
	{
        AV_PRINT( "pthread_rwlock_wrlock");
		return HI_FAILURE;
	}
	*pS32Stat = pLock->stat;

    s32Ret = pthread_rwlock_unlock(&pLock->rwLock);
    if (s32Ret != HI_SUCCESS)
	{
        AV_PRINT( "pthread_rwlock_unlock");
		return HI_FAILURE;
	}

	return HI_SUCCESS;
}



/*****************************************************************************
 函 数 名  : HI3531_SetLockData
 功能描述  : 设置锁管理的数据
 输入参数  : RW_LOCK_T *pLock, HI_VOID *pData, HI_S32 s32DataLen
             HI_VOID
 输出参数  : RW_LOCK_T *pLock,
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月25日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3531_SetLockData(RW_LOCK_T *pLock, HI_VOID *pData, HI_S32 s32DataLen)
{
	HI_S32 s32Ret = HI_SUCCESS;

	if (NULL == pLock)
	{
		return HI_FAILURE;
	}
	
	s32Ret = pthread_rwlock_wrlock(&pLock->rwLock);
	if (s32Ret != HI_SUCCESS)
	{
		AV_PRINT( "pthread_rwlock_wrlock");
		return HI_FAILURE;
	}
	
	memcpy(pLock->data, pData, s32DataLen);

	s32Ret = pthread_rwlock_unlock(&pLock->rwLock);
	if (s32Ret != HI_SUCCESS)
	{
		AV_PRINT( "pthread_rwlock_unlock");
		return HI_FAILURE;
	}

	return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : HI3531_GetLockData
 功能描述  : 获取锁管理的数据
 输入参数  : RW_LOCK_T *pLock, HI_S32 s32DataLen
             HI_VOID
 输出参数  : HI_VOID *pData, 
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月25日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3531_GetLockData(RW_LOCK_T *pLock, HI_VOID *pData, HI_S32 s32DataLen)
{
	HI_S32 s32Ret = HI_SUCCESS;

	if (NULL == pLock)
	{
		return HI_FAILURE;
	}
	
	s32Ret = pthread_rwlock_rdlock(&pLock->rwLock);
	if (s32Ret != HI_SUCCESS)
	{
		AV_PRINT( "pthread_rwlock_wrlock");
		return HI_FAILURE;
	}
	
	memcpy(pData, pLock->data, s32DataLen);

	s32Ret = pthread_rwlock_unlock(&pLock->rwLock);
	if (s32Ret != HI_SUCCESS)
	{
		AV_PRINT( "pthread_rwlock_unlock");
		return HI_FAILURE;
	}

	return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI3531_GetTime
 功能描述  : 获取锁管理的数据
 输入参数  : HI_CHAR *pTimeBuf, HI_S32 len
             HI_VOID
 输出参数  : 
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月26日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_CHAR *HI3531_GetTime(HI_CHAR *pTimeBuf, HI_S32 s32Len)
{
	HI_CHAR timeBuf[32] = {0};
	time_t nowTime = 0;
	struct tm *pTm = NULL;

	if (s32Len < (HI_S32)sizeof(timeBuf))
	{
		return NULL;
	}
	
	nowTime = time((time_t *)NULL);
	pTm = localtime(&nowTime);
	if (strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", pTm) == 0)
    {
		perror("strftime returned 0");
		return NULL;
	}

	if(pTimeBuf != NULL)
	{
		memcpy(pTimeBuf, timeBuf, sizeof(timeBuf));
	}
	else
	{
		return NULL;
	}
	
    return pTimeBuf;
}

 
/*****************************************************************************
 函 数 名  : HI3531_GetTime
 功能描述  : 写日志文件
 输入参数  : HI_CHAR *pLogFile--日志文件,  HI_CHAR *pLogContent--日志内容,
 			 HI_S32 s32AddTime--打印的时候是否加上时间: 0----不加时间，1----加时间?
 			 HI_S32 s32NewFile--是否新建日志文件:   0----不新建，在原来的日志文件的基础上添加内容，
             										1----新建日志文件
 输出参数  : 
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月26日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3531_WriteLog(HI_CHAR *pLogFile,  HI_CHAR *pLogContent, HI_S32 s32AddTime, HI_S32 s32NewFile)
{
    FILE *fp = NULL;		//日志文件
	HI_CHAR timeBuf[32] = {'\0'};

    if (s32NewFile == 0)//
    {
        if ((fp= fopen(pLogFile, "a+")) < 0)//日志名存在则打开，不存在创建
        {
            perror("in write log fopen log file error");
        }
    }
    else
    {
        if ((fp= fopen(pLogFile, "w+")) < 0) //新建日志文件
        {
            perror("in write log fopen log file error");
        }
    }

    if (s32AddTime)//加时间
    {
        HI3531_GetTime(timeBuf, sizeof(timeBuf));
        fprintf(fp, "%s", timeBuf);//ctime(&timeval));
    }

    fprintf(fp, "\t\t%s", pLogContent); //将日志内容写入到文件
    fclose(fp);

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI3531_CovTime2Str
 功能描述  : 
 输入参数  : HI_CHAR *pTimeBuf, HI_S32 len
             HI_VOID
 输出参数  : 
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月26日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3521_CovTime2Str(time_t t, HI_CHAR *pTime)
{
    struct tm *tmp;

    if (NULL == pTime)
    {
        AV_PRINT("Input NULL pointer");
        return;
    }
    tmp = localtime(&t);
    sprintf(pTime,"%04d%02d%02d_%02d%02d%02d",
            tmp->tm_year+1900, tmp->tm_mon+1, tmp->tm_mday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
}




