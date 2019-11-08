
#ifndef __AV_MEM_PROC_H__
#define __AV_MEM_PROC_H__

/*调试 DEBUG,要对存储进行调试直接修改此处即可  */
//#define MEM_DEBUG
#ifdef MEM_DEBUG
#define MEM_DBG_PRT(format...) \
{ \
        printf("#[MEM_DBG]%s:[%d]", __FILE__, __LINE__),printf(format),printf("\r\n"); \
}
#endif

#define SHM_IPSPK_SND_ID           0x10            /* IP对讲发送共享内存ID值 */
#define SHM_IPSPK_RCV_ID            0x11            /* IP对讲接收共享内存ID值 */
#define SHM_VI_PREV_ID            0x12            /* 视频预览共享内存ID值 */
#define SHM_SNAP_ID             0x13            /* 拍照共享内存ID值 */

#ifdef STORE_MULTI_THD
#define SHM_RECBUF_ID_CHAN1              0xe1            /* 录像缓冲器内存ID值 */
#define SHM_RECBUF_ID_CHAN2              0xe2            /* 录像缓冲器内存ID值 */
#define SHM_RECBUF_ID_CHAN3              0xe3            /* 录像缓冲器内存ID值 */
#define SHM_RECBUF_ID_CHAN4              0xe4            /* 录像缓冲器内存ID值 */
#define SHM_RECBUF_ID_CHAN5              0xe5            /* 录像缓冲器内存ID值 */
#define SHM_RECBUF_ID_CHAN6              0xe6            /* 录像缓冲器内存ID值 */
#define SHM_RECBUF_ID_CHAN7              0xe7            /* 录像缓冲器内存ID值 */
#define SHM_RECBUF_ID_CHAN8              0xe8            /* 录像缓冲器内存ID值 */

#define SHM_RECBUF_ID_CHAN9              0xe9            /* 录像缓冲器内存ID值 */
#define SHM_RECBUF_ID_CHAN10              0xea            /* 录像缓冲器内存ID值 */
#define SHM_RECBUF_ID_CHAN11              0xeb            /* 录像缓冲器内存ID值 */
#define SHM_RECBUF_ID_CHAN12              0xec            /* 录像缓冲器内存ID值 */

#endif

#define SEM_IPSPK_SND_ID       0x40            /* IP对讲发送信号量ID值 */
#define SEM_IPSPK_RCV_ID        0x41            /* IP对讲接收信号量ID值 */
#define SEM_VI_PREV_ID        0x42            /* 视频预览信号量ID值 */
#define SEM_SNAP_ID         0x43            /* 拍照信号量ID值 */

#define   SHM_IPSPK_MAX_SIZE 		102400      /* 共享内存的大小100K */
#define   SHM_IPSPK_ONE_SIZE    		1088        /* IP对讲单条数据大小1088B */
#define   SHM_IPSPK_MAX_NUM    		94          /* IP对讲共享内存中存储最大条数 */

#define   SHM_VI_PREV_MAX_SIZE    		6554624    /* 视频预览共享内存区域大小352*288*1.5*50*4 */
#define   SHM_VI_PREV_ONE_SIZE 	65536     /* 视频预览共享内存单条数据大小 352*288*1.5 */
#define   SHM_VI_PREV_MAX_NUM  	100        /* 视频预览共享内存中存储的总条数 */

#define   SHM_SNAP_MAX_SIZE    		3687424      /* 拍照共享内存区域大小 409600 * 9 + 1024 */
#define   SHM_SNAP_ONE_SIZE  	409600     /* 拍照共享内存单条数据大小702*576*1.5 */
#define   SHM_SNAP_MAX_NUM   	9          /* 拍照共享内存中存储最大条数 */

#ifdef STORE_MULTI_THD
/* 录像缓冲区单条数据大小 8+1024*1024 */
#define   SHM_RECBUF_ONE_SIZE     		(sizeof(REC_CHN_MEM_HEAD_T)+1024*1024*2)
/*测试数据 6K,现在测试的数据没有图像 */
//#define   SHM_RECBUF_ONE_SIZE     		(sizeof(REC_CHN_MEM_HEAD_T)+6*1024)
/*每个通道有4块 */
#define   SHM_RECBUF_MAX_NUM      		4
/* 录像缓冲区 12+(8+1024*1024)*4 */
#define   SHM_RECBUF_MAX_SIZE     		sizeof(REC_MEM_HEAD_T)+(SHM_RECBUF_ONE_SIZE*SHM_RECBUF_MAX_NUM)
#else

#define   SHM_RECBUF_MAX_SIZE     		8388752    /* 录像缓冲区 16+(8+512*1024)*16 */
#define   SHM_RECBUF_ONE_SIZE     		524296     /* 录像缓冲区单条数据大小 8+512*1024 */
//#define   SHM_RECBUF_ONE_SIZE     	4194368     /* 录像缓冲区单条数据大小 8+512*1024 */
#define   SHM_RECBUF_MAX_NUM      		16
//#define   SHM_RECBUF_MAX_NUM      	2
#endif

//录像内存
/*********************************************************************
内存头     内存块1         内存块2                               内存块16
|--------|------|--------|-------|-------|---*****------------|-------|------|
|MEMHEAD | HEAD |  DATA  | HEAD  | DATA  |                    | HEAD  | DATA |
|--------|------|--------|-------|-------|---*****------------|-------|------|

 *********************************************************************/
#ifdef STORE_MULTI_THD
typedef struct REC_MEM_HEAD_ST
{
    HI_S32 cWrChnIdx;    /* 正在 使用的内存块，使用的内存块下标 */
    HI_S32 s32MemFullStat;     /* 写满的内存块，每一个bit代表一块内存 */
    HI_S32 s32MemEmptyStat;     /* 空闲的内存块，每一个bit代表一块内存 */
}REC_MEM_HEAD_T;
#else
typedef struct REC_MEM_HEAD_ST
{
    HI_CHAR cWrChnIdx[8];    /* 正在 使用的内存块，使用的内存块下标 */
    HI_S32 s32MemFullStat;     /* 写满的内存块，每一个bit代表一块内存 */
    HI_S32 s32MemEmptyStat;     /* 空闲的内存块，每一个bit代表一块内存 */
}REC_MEM_HEAD_T;
#endif
typedef struct REC_CHN_MEM_HEAD_ST
{
    HI_S32 s32WrChn;              //当前块内存写的通道
    HI_S32 s32RecDataLen;         //当前块内存写的数据
}REC_CHN_MEM_HEAD_T;

typedef struct SEM_MNG_ST
{
	HI_CHAR *semPath;		/*系统路径*/
	HI_S32 userSemId;		/*用户SEM ID*/
	HI_S32 sysSemId;		/*系统SEM ID*/
}SEM_MNG_T;

typedef struct SHM_MNG_ST
{
	HI_CHAR *shmPath;
	HI_S32 userShmId;
	HI_S32 shmSize;
	SHM_ATTR_T shmAttr;
}SHM_MNG_T;

typedef struct SHM_SEM_MNG_ST
{
	SEM_MNG_T semMng;
	SHM_MNG_T shmMng;
}SHM_SEM_MNG_T;

typedef enum IPC_TYPE_EN
{
	IPC_IPSPK_SND_E = 0,
	IPC_IPSPK_RCV_E,
	IPC_VI_PREV_E,
	IPC_SNAP_E,
	IPC_INVALID
}IPC_TYPE_E;

typedef union semun
{
	HI_S32 iValue;
	struct semid_ds *pstBuffer;
	HI_U16 *pusArray;
}SEM_ARG_U;

extern HI_S32 MEM_WrShm(SHM_ATTR_T *pstShmAttr, const HI_VOID *pData,
							        const HI_S32 s32Len, HI_S32 iSemId);
extern HI_S32 MEM_RdShm(SHM_ATTR_T *pstShmAttr, HI_VOID *pData, const HI_S32 s32Len,
							        HI_S32 iSemId, HI_S32 *ps32Counter);
extern HI_S32 MEM_ShmDataInit(SHM_ATTR_T *pstShmAttr, HI_S32 iSemId);
#ifdef STORE_MULTI_THD
extern HI_S32 MEM_GetShmFullStat(HI_S32 s32Chn, HI_S32 *ps32MemFullStat);
extern HI_S32 MEM_GetShmEmptyStat(HI_S32 s32Chn, HI_S32 *ps32MemFullStat);
extern HI_S32 MEM_WrVencStream2Shm(HI_VOID *pVencData, HI_S32 s32Chn, HI_S32 s32FrameLen);
#else
extern HI_S32 MEM_GetShmFullStat(SHM_ATTR_T * pstRecShmAttr, pthread_rwlock_t * pstLock, HI_S32 * ps32MemFullStat);
extern HI_S32 MEM_GetShmEmptyStat(SHM_ATTR_T *pstRecShmAttr, pthread_rwlock_t *pstLock, HI_S32 *ps32MemFullStat);
extern HI_S32 MEM_WrVencStream2Shm(SHM_ATTR_T *pstRecShmAttr, pthread_rwlock_t *pstLock,                   	                HI_VOID *pRecData, HI_S32 s32Chn, HI_S32 s32FrameLen);
#endif
extern HI_VOID MEM_InitChnRecMemHead(HI_VOID);
extern HI_S32 MEM_GetRecFromOneMem(SHM_ATTR_T *pstRecShmAttr, pthread_rwlock_t *pstLock,
                                                  HI_S32 s32Index, HI_CHAR *pcDataHead);
extern HI_S32 APP_IsChnRecDataExistInShm(SHM_ATTR_T * pstRecShmAttr, pthread_rwlock_t * pstLock,
                                             HI_S32 s32Index, HI_S32 iChn);
extern HI_S32 HI3531_IpSpkSndThread(HI_VOID);
extern HI_S32 HI3531_IpSpkRcvThread(HI_VOID);
extern HI_S32 HI3531_SemInit(HI_VOID);
extern HI_S32 HI3531_ShmInit(HI_VOID);
extern HI_VOID MEM_InitRecMemHead(HI_VOID);
extern HI_VOID HI3531_ShmSemMngInit(HI_VOID);
extern HI_VOID HI3531_GetShmSemMng(IPC_TYPE_E eIpcType, SHM_SEM_MNG_T **pstShmSemMng);
extern HI_S32 HI3531_WrAencStream2Shm(AUDIO_STREAM_S *pstStream, HI_S32 s32Chn);

#endif /* __AV_MEM_PROC_H__ */

