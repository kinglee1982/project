
#ifndef __AV_MEM_PROC_H__
#define __AV_MEM_PROC_H__

/*���� DEBUG,Ҫ�Դ洢���е���ֱ���޸Ĵ˴�����  */
//#define MEM_DEBUG
#ifdef MEM_DEBUG
#define MEM_DBG_PRT(format...) \
{ \
        printf("#[MEM_DBG]%s:[%d]", __FILE__, __LINE__),printf(format),printf("\r\n"); \
}
#endif

#define SHM_IPSPK_SND_ID           0x10            /* IP�Խ����͹����ڴ�IDֵ */
#define SHM_IPSPK_RCV_ID            0x11            /* IP�Խ����չ����ڴ�IDֵ */
#define SHM_VI_PREV_ID            0x12            /* ��ƵԤ�������ڴ�IDֵ */
#define SHM_SNAP_ID             0x13            /* ���չ����ڴ�IDֵ */

#ifdef STORE_MULTI_THD
#define SHM_RECBUF_ID_CHAN1              0xe1            /* ¼�񻺳����ڴ�IDֵ */
#define SHM_RECBUF_ID_CHAN2              0xe2            /* ¼�񻺳����ڴ�IDֵ */
#define SHM_RECBUF_ID_CHAN3              0xe3            /* ¼�񻺳����ڴ�IDֵ */
#define SHM_RECBUF_ID_CHAN4              0xe4            /* ¼�񻺳����ڴ�IDֵ */
#define SHM_RECBUF_ID_CHAN5              0xe5            /* ¼�񻺳����ڴ�IDֵ */
#define SHM_RECBUF_ID_CHAN6              0xe6            /* ¼�񻺳����ڴ�IDֵ */
#define SHM_RECBUF_ID_CHAN7              0xe7            /* ¼�񻺳����ڴ�IDֵ */
#define SHM_RECBUF_ID_CHAN8              0xe8            /* ¼�񻺳����ڴ�IDֵ */

#define SHM_RECBUF_ID_CHAN9              0xe9            /* ¼�񻺳����ڴ�IDֵ */
#define SHM_RECBUF_ID_CHAN10              0xea            /* ¼�񻺳����ڴ�IDֵ */
#define SHM_RECBUF_ID_CHAN11              0xeb            /* ¼�񻺳����ڴ�IDֵ */
#define SHM_RECBUF_ID_CHAN12              0xec            /* ¼�񻺳����ڴ�IDֵ */

#endif

#define SEM_IPSPK_SND_ID       0x40            /* IP�Խ������ź���IDֵ */
#define SEM_IPSPK_RCV_ID        0x41            /* IP�Խ������ź���IDֵ */
#define SEM_VI_PREV_ID        0x42            /* ��ƵԤ���ź���IDֵ */
#define SEM_SNAP_ID         0x43            /* �����ź���IDֵ */

#define   SHM_IPSPK_MAX_SIZE 		102400      /* �����ڴ�Ĵ�С100K */
#define   SHM_IPSPK_ONE_SIZE    		1088        /* IP�Խ��������ݴ�С1088B */
#define   SHM_IPSPK_MAX_NUM    		94          /* IP�Խ������ڴ��д洢������� */

#define   SHM_VI_PREV_MAX_SIZE    		6554624    /* ��ƵԤ�������ڴ������С352*288*1.5*50*4 */
#define   SHM_VI_PREV_ONE_SIZE 	65536     /* ��ƵԤ�������ڴ浥�����ݴ�С 352*288*1.5 */
#define   SHM_VI_PREV_MAX_NUM  	100        /* ��ƵԤ�������ڴ��д洢�������� */

#define   SHM_SNAP_MAX_SIZE    		3687424      /* ���չ����ڴ������С 409600 * 9 + 1024 */
#define   SHM_SNAP_ONE_SIZE  	409600     /* ���չ����ڴ浥�����ݴ�С702*576*1.5 */
#define   SHM_SNAP_MAX_NUM   	9          /* ���չ����ڴ��д洢������� */

#ifdef STORE_MULTI_THD
/* ¼�񻺳����������ݴ�С 8+1024*1024 */
#define   SHM_RECBUF_ONE_SIZE     		(sizeof(REC_CHN_MEM_HEAD_T)+1024*1024*2)
/*�������� 6K,���ڲ��Ե�����û��ͼ�� */
//#define   SHM_RECBUF_ONE_SIZE     		(sizeof(REC_CHN_MEM_HEAD_T)+6*1024)
/*ÿ��ͨ����4�� */
#define   SHM_RECBUF_MAX_NUM      		4
/* ¼�񻺳��� 12+(8+1024*1024)*4 */
#define   SHM_RECBUF_MAX_SIZE     		sizeof(REC_MEM_HEAD_T)+(SHM_RECBUF_ONE_SIZE*SHM_RECBUF_MAX_NUM)
#else

#define   SHM_RECBUF_MAX_SIZE     		8388752    /* ¼�񻺳��� 16+(8+512*1024)*16 */
#define   SHM_RECBUF_ONE_SIZE     		524296     /* ¼�񻺳����������ݴ�С 8+512*1024 */
//#define   SHM_RECBUF_ONE_SIZE     	4194368     /* ¼�񻺳����������ݴ�С 8+512*1024 */
#define   SHM_RECBUF_MAX_NUM      		16
//#define   SHM_RECBUF_MAX_NUM      	2
#endif

//¼���ڴ�
/*********************************************************************
�ڴ�ͷ     �ڴ��1         �ڴ��2                               �ڴ��16
|--------|------|--------|-------|-------|---*****------------|-------|------|
|MEMHEAD | HEAD |  DATA  | HEAD  | DATA  |                    | HEAD  | DATA |
|--------|------|--------|-------|-------|---*****------------|-------|------|

 *********************************************************************/
#ifdef STORE_MULTI_THD
typedef struct REC_MEM_HEAD_ST
{
    HI_S32 cWrChnIdx;    /* ���� ʹ�õ��ڴ�飬ʹ�õ��ڴ���±� */
    HI_S32 s32MemFullStat;     /* д�����ڴ�飬ÿһ��bit����һ���ڴ� */
    HI_S32 s32MemEmptyStat;     /* ���е��ڴ�飬ÿһ��bit����һ���ڴ� */
}REC_MEM_HEAD_T;
#else
typedef struct REC_MEM_HEAD_ST
{
    HI_CHAR cWrChnIdx[8];    /* ���� ʹ�õ��ڴ�飬ʹ�õ��ڴ���±� */
    HI_S32 s32MemFullStat;     /* д�����ڴ�飬ÿһ��bit����һ���ڴ� */
    HI_S32 s32MemEmptyStat;     /* ���е��ڴ�飬ÿһ��bit����һ���ڴ� */
}REC_MEM_HEAD_T;
#endif
typedef struct REC_CHN_MEM_HEAD_ST
{
    HI_S32 s32WrChn;              //��ǰ���ڴ�д��ͨ��
    HI_S32 s32RecDataLen;         //��ǰ���ڴ�д������
}REC_CHN_MEM_HEAD_T;

typedef struct SEM_MNG_ST
{
	HI_CHAR *semPath;		/*ϵͳ·��*/
	HI_S32 userSemId;		/*�û�SEM ID*/
	HI_S32 sysSemId;		/*ϵͳSEM ID*/
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

