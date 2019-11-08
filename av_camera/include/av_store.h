/******************************************************************************

                  ��Ȩ���� (C), 2001-2024, ֣�������Ƽ��ɷ����޹�˾

 ******************************************************************************
  �� �� ��   : av_store.c
  �� �� ��   : ����
  ��    ��   :
  ��������   : 2014��12��1��
  ����޸�   :
  ��������   : ¼��
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2014��12��01��
    ��    ��   :
    �޸�����   :

******************************************************************************/
#include "av_mem.h"

#ifndef __AV_STORE_H__
#define __AV_STORE_H__

/*�洢���� DEBUG,Ҫ�Դ洢���е���ֱ���޸Ĵ˴�����  */
#define STORE_DEBUG
#ifdef STORE_DEBUG
#define STORE_DBG_PRT(format...) \
{ \
        printf("#[STORE_DBG]%s:[%d]", __FILE__, __LINE__),printf(format),printf("\r\n"); \
}
#endif

#define STOR_FILE_IFRAME_MAX_NUMS          	9000    	/* �ļ��洢I֡������� */
#define STOR_FILE_IFRAME_FULL         		8500    	/* ������Ƶ��I֡�ܹ��洢����� */
#define STOR_FILE_FULL                		0xFF    	/* ��Ƶ���� */

#define STOR_FILE_BLK_NUMS              	(1000)    	/* ���֧��1TB��С��Ӳ�� */
#define STOR_FILE_BLK_NUMS_PER_CHN      	(STOR_FILE_BLK_NUMS)

#define STOR_FILE_INFO_SIZE              	(2*1024)
#define STOR_FILE_BLK_HEAD_SIZE        		(512)

#ifdef STORE_10M
#define STOR_FRAME_DATA_END_ADDR         	(10*1024*1024)
#else
    #ifdef MMZ_CHG
#define STOR_FRAME_DATA_END_ADDR         	(506*1024*1024)
    #else
/*max frame data seek*/
#define STOR_FRAME_DATA_END_ADDR         	(250*1024*1024)    /*250*/
    #endif
#endif
#define STOR_FILE_INFO_START_ADDR        	STOR_FILE_BLK_HEAD_SIZE
#define STOR_FRAME_DATA_START_ADDR       	(STOR_FILE_INFO_SIZE + STOR_FILE_BLK_HEAD_SIZE)
/*add gps info list */
#ifdef REC_SAVE_GPS
	#ifdef MMZ_CHG
#define STOR_GPS_IDX_START_POS   			(508*1024*1024 + 0) /*gps list*/
#define STOR_FRAME_IDX_START_POS   			(510*1024*1024 + 0) /*frame list*/
    #else
#define STOR_GPS_IDX_START_POS   			(254*1024*1024 + 0) /*gps list*/
#define STOR_FRAME_IDX_START_POS   			(255*1024*1024 + 0) /*frame list*/
    #endif
#else
	#ifdef MMZ_CHG
#define STOR_FRAME_IDX_START_POS			(510*1024*1024 + 0) /*frame list*/
	#else
#define STOR_FRAME_IDX_START_POS   			(255*1024*1024 + 0)
	#endif
#endif

#ifdef MMZ_CHG
#define STOR_FILE_MAX_SIZE   				(512*1024*1024)
#else
#define STOR_FILE_MAX_SIZE   				(256*1024*1024)
#endif
#define STOR_ONE_PART_FILES_MAX             (1000)      //Ŀǰ2TӲ������ļ���������1000
#define STOR_FRAME_IDX_MAX_NUMS           	(8192)
#define STOR_APP_BLOCK_MAX_TEST        		(0)    		/* ����ʱʹ�� */

#define STOR_RECFILE_MAX_NUMS          		1024    	/* ������������ļ����� */

#define STOR_SWITCH_PART           			0xFF    	/* �л����� */
#define STOR_NO_SWITCH_PART        			0x10     	/* ����Ҫ�л����� */

#define STOR_LOG_MAX_NUMS   		2048   		/* �洢�����־���� */
#define STOR_LOG_ITEM_LEN  		64     		/* ������־������ݳ��� */

/**********************************������������Ϣ***************************/
/*�����ļ�SEEK */
#define STOR_IDX_BLK_HEAD_SEEK       		(0)
#define STOR_IDX_BLK_VER_SEEK				(200*1024)
#define STOR_IDX_BLK_STAT_INFO_SEEK   	    (512*1024)  /* �����ļ��м�¼��Ƶ�ļ������״̬��ƫ�� */
#define STOR_IDX_BLK_CHN_SEEK(channo) 		(((1*(channo))+1)*1024*1024)

#define STOR_IDX_VER       			        1300   		/* �����ļ��汾��Ϣ�汾��Ϣ */
#ifdef HDD_SIZE_CHECK
#define FILE_BLOCK_SIZE_TEST  "/mnt/a5/rec0000.tmm"
#endif
#define PART_MOUNT_PATH  "/mnt/a"
#define PART_FILEIDX00 "index00.tmm"
#define PART_FILEIDX01 "index01.tmm"
#define GET_PART_INDEX_NAME(szFileName,iPartid)  sprintf(szFileName,"%s%d/%s", \
                                                            PART_MOUNT_PATH,iPartid,\
                                                            PART_FILEIDX00)

#define GET_PART_RECFILE_NAME(szFileName,iPartid,iFileno) sprintf(szFileName,  \
                                                                    "%s%d/rec%04d.tmm",\
                                                                    PART_MOUNT_PATH,\
                                                                    iPartid,\
                                                                    iFileno)
#define PART_MOUNT_START_IDX                 5      /*Ӳ�̹��ص���ʼ�ڵ� */
#define PART_MAX_NUMS           		     4      /* ���̷������� */
#define MAX_PART_ID                          (PART_MAX_NUMS-1)
#define MIN_PART_ID                          0
#define GET_PART_ID(iPartId)                  (iPartId % PART_MAX_NUMS + PART_MOUNT_START_IDX)
#define IS_PART_ID_VAILD(iPartId)              ((iPartId >=0 && iPartId <= MAX_PART_ID))

#ifdef ALARM_REC
/*¼���¼�4��
 ��ͨ --0
 ����---1*/
#define REC_EVENT_TYPE_NUM   4

typedef enum
{
    EVENT_NORMAL_REC_E = 0,
    EVENT_ALARM_REC_E,
    EVENT_BUFF = -1,
}EVENT_REC_TYPE_EM;

typedef struct REC_EVENT_INFO_T
{
    HI_S32 s32Trigger;
    HI_U32 s32StartTime;
    HI_U32 s32EndTime;
    HI_S32 s32BkOK;
    HI_U8  res[8];
}REC_EVENT_INFO;
#endif

typedef struct STOR_REC_CACHE_T
{
    HI_S32 m_iBufferPos;
    HI_U8* m_pcBuffer;
}__attribute__((packed))STOR_REC_CACHE_ST;

/*�ļ�������ͷ��*/
typedef struct STOR_BLK_IDX_HEAD_ST
{
	HI_S32 en_chns;		 	/*����¼���ͨ������*/
	HI_S32 cur_partition;	/*��ǰ����*/
	HI_S32 nxt_block;		/*��һ��Ҫ�������ļ���*/
	HI_S32 all_blocks;      /*�����ļ�������*/
	HI_S32 rest_blocks;
	HI_S32 used_blocks;     /*�����ļ�������*/
	HI_S32 avail_blocks;    /*�����ļ�������*/
	//HI_CHAR blocks_map[STOR_FILE_BLK_NUMS];    /*0:unused 1:useing 2:used 4:bad*/
}STOR_BLK_IDX_HEAD_T;

/*file block*/
typedef struct STOR_FILE_BLK_HEAD_ST
{
    HI_U16 version;                  /*�汾��*/
    HI_U32 audio_type;               /*��Ƶ���ͣ���ǰ��Ƶ */
    HI_U32 video_type;               /*��Ƶ���ͣ���ǰ��Ƶ��H264,MPEG*/
    HI_U32 last_frame_ts_offset;     /*���һ֡���һ֡��ʱ��ƫ��*//*ʹ��֡��ʱ��*/
    HI_U32 file_info_start_pos;      /*�ļ���Ϣ��ʼλ��=512B*/
    HI_U32 frame_data_start_pos;     /*����Ƶ����֡��ʼλ��=1MB*/
    HI_U32 frame_data_end_pos;       /*����Ƶ����֡����λ��(���ļ���Ŀ�ʼ����������Ƶ���ݵĽ�β)*/
    HI_U32 frame_index_start_pos;    /*��¼֡������λ��=180MB*/
    HI_U32 frame_index_size;         /*��¼֡�����Ĵ�С=����֡ͷ+������*/
    HI_U32 reserve;                  /*�����ֶ�*/
}__attribute__((packed))STOR_FILE_BLK_HEAD_T;

/*�ļ���������*/
typedef struct STOR_BLK_IDX_ENTRY_ST
{
	HI_U32 chn;				/* ͨ���ţ���file_info*/
	HI_U32 blockid;			/* �ļ���ID */
	HI_U32 start_time;		/* ¼��ʼʱ�䣬��gettimeofday��ȡ��Ȼ���ʼ��*/
	HI_U32 end_time;		/* ¼�����ʱ�䣬��ʼ��Ϊ0*/
	HI_U32 width;			/* ¼��ֱ��ʡ�������file_info*/
	HI_U32 height;			/* ¼��ֱ��ʡ����ߣ���file_info*/
	HI_U32 size;			/* ¼���ļ���С����ʼ��Ϊ0*/
	HI_S32    status;			/* ¼���ļ�״̬ 0 -- init, 1 -- writing, 2 -- done, 4 -- using*/
	HI_CHAR   filename[32];	/* �ļ�������ʽchnN_yyyyMMdd_hhmmss.tmm��NULL���� */
	HI_CHAR   blockname[32];	/* �ļ���������ʽrec0000.tmm��NULL���� */
	HI_U32 lock_flag;		/* �ļ�������������־*/
}STOR_BLK_IDX_ENTRY_T;

/*�����ļ��� STOR_IDX_BLK_STAT_INFO_SEEK λ������������½ṹ */
typedef struct STOR_BLK_STAT_INFO_ST
{
    HI_U32 s32BlkCounter;               /* ��Ϣ���� */
    HI_U32 s32BlkNewestIndex;           /* ���½��յ�����Ϣ�±�ֵ */
    HI_U32 s32BlkOldestIndex;           /* ��ɽ��յ�����Ϣ�±�ֵ */
    HI_CHAR cBlkStat[STOR_RECFILE_MAX_NUMS];
}STOR_BLK_STAT_INFO_T;

typedef struct STOR_CHN_IDX_INFO_ST
{
    HI_S32 s32Counter;               /* ��Ϣ���� */
    HI_S32 s32NewestIndex;           /* ���½��յ�����Ϣ�±�ֵ */
    HI_S32 s32OldestIndex;           /* ��ɽ��յ�����Ϣ�±�ֵ */
    STOR_BLK_IDX_ENTRY_T stBlkIdxList[STOR_RECFILE_MAX_NUMS];
}STOR_CHN_IDX_INFO_T;
/**********************************������������Ϣ***************************/


/**********************************������һЩ�ļ�***********************/
#define APP_REC_ERR_LOG      		        "/mnt/a1/recodeerror.log"

/***********************************************************************/

/*file info*/
typedef struct STOR_FILE_INFO_ST
{
	HI_CHAR company[64];       	/*��˾��,NULL����*/
	HI_CHAR priv_info[64];	    /*һЩ������Ʒ����Ϣ*/
	HI_CHAR device_name[64];   	/*��¼�豸����֮�࣬NULL����*/
	HI_CHAR bus_line[16];
	HI_CHAR bus_card[16];
	HI_U32 tv_sec;          	/*19700101 �����룬����ʱ�� */
	HI_U32 tv_usec;         	/*19700101 �����룬΢�� */
	HI_U32 frame_rate;      	/*֡�� */
	HI_U32 pixelwidth;      	/*�ֱ��� width */
	HI_U32 pixelheight;     	/*�ֱ��� height */
	HI_U32 channels;        	/*��Ƶͨ����������˫ */
	HI_U32 sampling_frequency; 	/*��Ƶ������ */
	HI_U32 bitdepth;        	/*��Ƶ����λ�� */
	HI_U32 audio_frameLen;  	/*��Ƶ֡�� */
	HI_U32 language;        	/*��������*/
	HI_U32 chn;             	/*¼��ͨ���� */
	HI_CHAR filename[32];      	/*�ļ�������ʽchnN_yyyyMMdd_hhmmss.tmm��NULL���� */
	HI_U32 lock_flag;           /*�ļ�����*/
    #ifdef ALARM_REC
    REC_EVENT_INFO  stRecEventInfo[REC_EVENT_TYPE_NUM];
    HI_U8 reserve[32];
    #else
	HI_U32 reserve;
    #endif
	//event_list_t event_list;
}__attribute__((packed)) STOR_FILE_INFO_T;

typedef struct STOR_REC_IFRAME_INFO_ST
{
	//uHI_S3232 u32Chn;
	DATA_TYPE_E type;       /*I-FRAME OR P-FRAME*/
	HI_U32 ts_offset;       /*��ǰ֡���һ֡��ʱ��ƫ��*/
	HI_U32 ts_interval;     /*��ǰ֡��ǰһ֡��ʱ����*/
	HI_U32 frame_data_pos;  /*��ǰ֡λ��*/
							/*����ʱ�䣬19700101 SEC��1B~4B(LOW~HIGH)*/
	HI_U8 nmk_time1;
	HI_U8 nmk_time2;
	HI_U8 nmk_time3;
	HI_U8 nmk_time4;
}__attribute__((packed))STOR_REC_IFRAME_INFO;

typedef struct FILE_IFRAME_LIST_ST
{
	HI_S32 s32Counter;               /* ��Ϣ���� */
	HI_S32 s32NewestIndex;           /* ���½��յ�����Ϣ�±�ֵ */
	HI_S32 s32OldestIndex;           /* ��ɽ��յ�����Ϣ�±�ֵ */
	STOR_REC_IFRAME_INFO stRecIFrameInfo[STOR_FILE_IFRAME_MAX_NUMS];
}__attribute__((packed))FILE_IFRAME_LIST_T;

/*�洢��GPS���� */
typedef struct STOR_GPS_FRAME_DATA_T
{
	HI_U8 nmk_time1;
	HI_U8 nmk_time2;
	HI_U8 nmk_time3;
	HI_U8 nmk_time4;
    HI_U32 latitude;        /* γ�� */
    HI_U32 longitude;       /* ���� */
    HI_U8  res[64];         /*Ԥ���ֽ� */
}STOR_GPS_FRAME_DATA_ST;

/*gps ���ݱ��� GPS���ݵ�������һ������  */
typedef struct STOR_GPS_INFO_T
{
	DATA_TYPE_E type;       /*I-FRAME OR P-FRAME*/
	HI_U32 ts_offset;       /*��ǰ֡���һ֡��ʱ��ƫ��*/
	HI_U32 ts_interval;     /*��ǰ֡��ǰһ֡��ʱ����*/
	HI_U32 frame_data_pos;  /*��ǰ֡λ��*/
							/*����ʱ�䣬19700101 SEC��1B~4B(LOW~HIGH)*/
	HI_U8 nmk_time1;
	HI_U8 nmk_time2;
	HI_U8 nmk_time3;
	HI_U8 nmk_time4;
}__attribute__((packed))STOR_GPS_INFO_ST;

typedef struct FILE_GPS_LIST_T
{
	HI_S32 s32Counter;               /* ��Ϣ���� */
	HI_S32 s32NewestIndex;           /* ���½��յ�����Ϣ�±�ֵ */
	HI_S32 s32OldestIndex;           /* ��ɽ��յ�����Ϣ�±�ֵ */
	STOR_GPS_INFO_ST stGpsInfo[STOR_FILE_IFRAME_MAX_NUMS];
}__attribute__((packed))FILE_GPS_LIST_ST;

typedef struct STOR_BLK_IDX_CHN_HEAD_ST
{
	HI_S32 block_total_num;          /*������*/
	HI_S32 block_newest_index;       /*���������±�ֵ */
	HI_S32 block_oldest_index;       /*���������±�ֵ*/
}__attribute__((packed))STOR_BLK_IDX_CHN_HEAD_T;

typedef struct STOR_REC_LOG_FILE_ST
{
    HI_S32 s32Counter;               /* ��Ϣ���� */
    HI_S32 s32NewestIndex;           /* ���½��յ�����Ϣ�±�ֵ */
    HI_S32 s32OldestIndex;           /* ��ɽ��յ�����Ϣ�±�ֵ */
    HI_CHAR cLogList[STOR_LOG_MAX_NUMS * STOR_LOG_ITEM_LEN];
}STOR_REC_LOG_T;

typedef enum STOR_FILE_BLK_STAT_EN
{
	STOR_FILE_BLK_STAT_UNUSED     = 0,
	STOR_FILE_BLK_STAT_USING  	= 1,
	STOR_FILE_BLK_STAT_USED     = 2,
	STOR_FILE_BLK_STAT_BAD    = 4,
}STOR_FILE_BLK_STAT_E;

#ifdef STORE_MULTI_THD
typedef struct _STOR_PARAM_T
{
    HI_BOOL bOverWrite; /*�Ƿ񸲸�*/
    HI_S32 iPartId;    /*����дpartid*/
}STOR_PARAM_ST;

typedef struct _STOR_PART_INOF_T
{
    FILE *pIdxFile;   /*��ǰ�����ļ�ָ�룬���·��� */
    STOR_BLK_STAT_INFO_T stBlkStatInfo;        /*��״̬��Ϣ */
}STOR_PART_INFO_ST;


typedef struct _HDD_PART_ST
{
    HI_BOOL bOverWrite; /*�Ƿ񸲸�*/
    HI_S32 iPartId;    /*���·����������������ļ��ж�ȡ��Ϊ׼����.�з����������·���Ϊ׼��*/
    HI_S32 partFileNums; /*�����ļ����� */
#ifdef HDD_SIZE_CHECK
    HI_S32 hddinfo;/*0 ----->256M ,1----> 512M*/
#endif
    pthread_mutex_t partMutexLock;
    pthread_mutex_t partSwitchLock;
    pthread_mutex_t blkStatLock;
    STOR_PART_INFO_ST  stStorePartInfo[PART_MAX_NUMS+1];
}HDD_PART_ST;


typedef struct _CHN_STOR_MGMT_T_
{
    HI_S32 iChan;
	HI_BOOL bRecording;    /*�Ƿ���¼���� */
    HI_BOOL bEnable;        /*�������� ¼������ */
    HI_BOOL bSwitchOK;
    HI_S32  iCurrWRId;  /*��ǰд�ķ���ID*/
    FILE *pRecFile;   /*¼��ͨ���ļ�ָ�� */
    FILE *pIdxFile;   /*�����ļ�ָ�� */
    HI_CHAR *pRecbuf; /*¼�񻺳� */
    STOR_FILE_BLK_HEAD_T stFileBlockHead;      /* �ļ���ͷ����Ϣ */
    SHM_SEM_MNG_T stShmSemMng;
    pthread_rwlock_t RecMemRWLock;
	pthread_t tid;
}CHN_STOR_MGMT_ST;

#endif

extern CHN_STOR_MGMT_ST *STOR_GetChnStoreMgmt(HI_S32 s32Chn);
extern HI_S32 STOR_UpdateIFrameIdx(FILE_IFRAME_LIST_T *pstIFrameList, FRAME_HEAD_T *pstFrameHead,
									HI_S32 s32CurrWrFilePos, HI_U64 s32FrameSeek,
									HI_S32 u32ChnId, HI_S32 *s32ChnStat);
extern HI_VOID STOR_UpdateIdxEntry(STOR_REC_IFRAME_INFO *pstIFRAMEInfo, HI_S32 s32CurrWrFilePos, HI_U64 s32FrameSeek, FRAME_HEAD_T *pstFrameHead);
extern HI_S32 STOR_WrFrameData(FILE *pRecFilefp, HI_CHAR *pDataBuf, HI_S32 s32Buflen, HI_S32 s32CurrWrFilePos);
extern HI_VOID *STOR_StoreRecThd(HI_VOID *pArg);
extern HI_S32 STOR_GetFileIFrameList(FILE* pFile, FILE_IFRAME_LIST_T *pstIFrameList);
extern HI_S32 STOR_GetRecPartId(HI_VOID);
extern HI_S32 STOR_GetIFrameSeek(FILE* pFile, HI_U32 *pU32StartAddr, time_t tFrameStartTime);
extern HI_S32 STOR_SetVideoFileHeadInfo(FILE* pFile, STOR_FILE_BLK_HEAD_T *pstBlkIdxHead, HI_U32 u32Addr);
extern HI_S32 STOR_GetVideoFileHeadInfo(FILE* pFile, STOR_FILE_BLK_HEAD_T *pstBlkIdxHead, HI_U32 u32Addr);
extern HI_S32 HI3531_StartHddMng(HI_VOID);
extern HI_S32 HI3521_StartAvencRec(HI_VOID);
extern HI_S32 STOR_GetDiskNewestOldestTime(HI_CHAR *pcIndexName, HI_S32 s32Chn, time_t *pNewestTime, time_t *pOldestTime);
extern HI_S32 STOR_InitIdxBlk(FILE *pRecLogFp);
extern HI_S32 STOR_SetFileIFrameList(FILE* pFile, FILE_IFRAME_LIST_T *pstIFrameList);
extern HI_S32 STOR_CreateRecLogFile(HI_VOID);
#ifdef STORE_MULTI_THD
extern HI_S32 STOR_StartStore(HI_VOID);
extern CHN_STOR_MGMT_ST *STOR_GetStoreMgmt(HI_S32 s32Chn);
extern HI_BOOL STOR_GetChnRecStat(HI_S32 s32Chn);
#endif
extern HI_U32 STOR_CalTime1(STOR_REC_IFRAME_INFO *pstIFRAMEInfo);
extern HI_U32 STOR_CalTime2(FRAME_HEAD_T *pstFrameHead);
extern HI_VOID STOR_DebugIdxEntry(FILE_IFRAME_LIST_T * pstIFrameList);
extern HI_VOID STOR_DebugFileBlkHead(STOR_FILE_BLK_HEAD_T * pstFileHead);
extern HI_S32 STOR_SetVideoFileInfo(FILE * pFile, STOR_FILE_INFO_T * pstFileInfo);
extern HI_S32 STOR_GetBlkIdxByChn(FILE *pIdxBlkFp, STOR_CHN_IDX_INFO_T *pstChnIdxInfo, HI_S32 s32Chn);
extern HI_S32 STOR_UpdateEventRecBkFlag(FILE *pFile, EVENT_REC_TYPE_EM eEventType);
extern HI_S32 STOR_GetVideoFileInfo(FILE* pFile, STOR_FILE_INFO_T *pstFileInfo);
extern HI_S32 STOR_SetVideoFileInfo(FILE* pFile, STOR_FILE_INFO_T *pstFileInfo);

#ifdef REC_SAVE_GPS
extern HI_S32 STOR_SetGpsInfoList(FILE* pFile, FILE_GPS_LIST_ST *pstGpsList);
extern HI_S32 STOR_InsertGpsInfoByIFrame(FRAME_HEAD_T * pIFrameHead, HI_U8 * pVencData);
#endif
#endif /* __AV_STORE_H__ */

