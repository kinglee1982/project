/******************************************************************************

                  版权所有 (C), 2001-2024, 郑州天迈科技股份有限公司

 ******************************************************************************
  文 件 名   : av_store.c
  版 本 号   : 初稿
  作    者   :
  生成日期   : 2014年12月1日
  最近修改   :
  功能描述   : 录像
  函数列表   :
  修改历史   :
  1.日    期   : 2014年12月01日
    作    者   :
    修改内容   :

******************************************************************************/
#include "av_mem.h"

#ifndef __AV_STORE_H__
#define __AV_STORE_H__

/*存储调试 DEBUG,要对存储进行调试直接修改此处即可  */
#define STORE_DEBUG
#ifdef STORE_DEBUG
#define STORE_DBG_PRT(format...) \
{ \
        printf("#[STORE_DBG]%s:[%d]", __FILE__, __LINE__),printf(format),printf("\r\n"); \
}
#endif

#define STOR_FILE_IFRAME_MAX_NUMS          	9000    	/* 文件存储I帧最大数量 */
#define STOR_FILE_IFRAME_FULL         		8500    	/* 单个视频块I帧能够存储最大数 */
#define STOR_FILE_FULL                		0xFF    	/* 视频块满 */

#define STOR_FILE_BLK_NUMS              	(1000)    	/* 最大支持1TB大小的硬盘 */
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
#define STOR_ONE_PART_FILES_MAX             (1000)      //目前2T硬盘最大文件数不超过1000
#define STOR_FRAME_IDX_MAX_NUMS           	(8192)
#define STOR_APP_BLOCK_MAX_TEST        		(0)    		/* 测试时使用 */

#define STOR_RECFILE_MAX_NUMS          		1024    	/* 单个分区最大文件个数 */

#define STOR_SWITCH_PART           			0xFF    	/* 切换分区 */
#define STOR_NO_SWITCH_PART        			0x10     	/* 不需要切换分区 */

#define STOR_LOG_MAX_NUMS   		2048   		/* 存储最大日志条数 */
#define STOR_LOG_ITEM_LEN  		64     		/* 单条日志最大数据长度 */

/**********************************分区和索引信息***************************/
/*索引文件SEEK */
#define STOR_IDX_BLK_HEAD_SEEK       		(0)
#define STOR_IDX_BLK_VER_SEEK				(200*1024)
#define STOR_IDX_BLK_STAT_INFO_SEEK   	    (512*1024)  /* 索引文件中记录视频文件块更新状态的偏移 */
#define STOR_IDX_BLK_CHN_SEEK(channo) 		(((1*(channo))+1)*1024*1024)

#define STOR_IDX_VER       			        1300   		/* 索引文件版本信息版本信息 */
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
#define PART_MOUNT_START_IDX                 5      /*硬盘挂载的起始节点 */
#define PART_MAX_NUMS           		     4      /* 磁盘分区个数 */
#define MAX_PART_ID                          (PART_MAX_NUMS-1)
#define MIN_PART_ID                          0
#define GET_PART_ID(iPartId)                  (iPartId % PART_MAX_NUMS + PART_MOUNT_START_IDX)
#define IS_PART_ID_VAILD(iPartId)              ((iPartId >=0 && iPartId <= MAX_PART_ID))

#ifdef ALARM_REC
/*录像事件4个
 普通 --0
 报警---1*/
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

/*文件块索引头部*/
typedef struct STOR_BLK_IDX_HEAD_ST
{
	HI_S32 en_chns;		 	/*开启录像的通道数量*/
	HI_S32 cur_partition;	/*当前分区*/
	HI_S32 nxt_block;		/*下一个要操作的文件块*/
	HI_S32 all_blocks;      /*所有文件块数量*/
	HI_S32 rest_blocks;
	HI_S32 used_blocks;     /*已用文件块数量*/
	HI_S32 avail_blocks;    /*可用文件块数量*/
	//HI_CHAR blocks_map[STOR_FILE_BLK_NUMS];    /*0:unused 1:useing 2:used 4:bad*/
}STOR_BLK_IDX_HEAD_T;

/*file block*/
typedef struct STOR_FILE_BLK_HEAD_ST
{
    HI_U16 version;                  /*版本号*/
    HI_U32 audio_type;               /*音频类型，当前音频 */
    HI_U32 video_type;               /*视频类型，当前视频，H264,MPEG*/
    HI_U32 last_frame_ts_offset;     /*最后一帧与第一帧的时间偏移*//*使用帧的时间*/
    HI_U32 file_info_start_pos;      /*文件信息开始位置=512B*/
    HI_U32 frame_data_start_pos;     /*音视频数据帧开始位置=1MB*/
    HI_U32 frame_data_end_pos;       /*音视频数据帧结束位置(从文件块的开始计算至音视频数据的结尾)*/
    HI_U32 frame_index_start_pos;    /*记录帧索引的位置=180MB*/
    HI_U32 frame_index_size;         /*记录帧索引的大小=索引帧头+索引项*/
    HI_U32 reserve;                  /*保留字段*/
}__attribute__((packed))STOR_FILE_BLK_HEAD_T;

/*文件块索引项*/
typedef struct STOR_BLK_IDX_ENTRY_ST
{
	HI_U32 chn;				/* 通道号，从file_info*/
	HI_U32 blockid;			/* 文件块ID */
	HI_U32 start_time;		/* 录像开始时间，从gettimeofday获取，然后初始化*/
	HI_U32 end_time;		/* 录像结束时间，初始化为0*/
	HI_U32 width;			/* 录像分辨率——宽，从file_info*/
	HI_U32 height;			/* 录像分辨率——高，从file_info*/
	HI_U32 size;			/* 录像文件大小，初始化为0*/
	HI_S32    status;			/* 录像文件状态 0 -- init, 1 -- writing, 2 -- done, 4 -- using*/
	HI_CHAR   filename[32];	/* 文件名，格式chnN_yyyyMMdd_hhmmss.tmm，NULL结束 */
	HI_CHAR   blockname[32];	/* 文件块名，格式rec0000.tmm，NULL结束 */
	HI_U32 lock_flag;		/* 文件锁定、解锁标志*/
}STOR_BLK_IDX_ENTRY_T;

/*索引文件的 STOR_IDX_BLK_STAT_INFO_SEEK 位置用来存放如下结构 */
typedef struct STOR_BLK_STAT_INFO_ST
{
    HI_U32 s32BlkCounter;               /* 消息计数 */
    HI_U32 s32BlkNewestIndex;           /* 最新接收到的消息下标值 */
    HI_U32 s32BlkOldestIndex;           /* 最旧接收到的消息下标值 */
    HI_CHAR cBlkStat[STOR_RECFILE_MAX_NUMS];
}STOR_BLK_STAT_INFO_T;

typedef struct STOR_CHN_IDX_INFO_ST
{
    HI_S32 s32Counter;               /* 消息计数 */
    HI_S32 s32NewestIndex;           /* 最新接收到的消息下标值 */
    HI_S32 s32OldestIndex;           /* 最旧接收到的消息下标值 */
    STOR_BLK_IDX_ENTRY_T stBlkIdxList[STOR_RECFILE_MAX_NUMS];
}STOR_CHN_IDX_INFO_T;
/**********************************分区和索引信息***************************/


/**********************************参数和一些文件***********************/
#define APP_REC_ERR_LOG      		        "/mnt/a1/recodeerror.log"

/***********************************************************************/

/*file info*/
typedef struct STOR_FILE_INFO_ST
{
	HI_CHAR company[64];       	/*公司名,NULL结束*/
	HI_CHAR priv_info[64];	    /*一些描述产品的信息*/
	HI_CHAR device_name[64];   	/*记录设备名字之类，NULL结束*/
	HI_CHAR bus_line[16];
	HI_CHAR bus_card[16];
	HI_U32 tv_sec;          	/*19700101 经历秒，创建时间 */
	HI_U32 tv_usec;         	/*19700101 经历秒，微妙 */
	HI_U32 frame_rate;      	/*帧率 */
	HI_U32 pixelwidth;      	/*分辨率 width */
	HI_U32 pixelheight;     	/*分辨率 height */
	HI_U32 channels;        	/*音频通道，单还是双 */
	HI_U32 sampling_frequency; 	/*音频采样率 */
	HI_U32 bitdepth;        	/*音频采样位宽 */
	HI_U32 audio_frameLen;  	/*音频帧长 */
	HI_U32 language;        	/*语言类型*/
	HI_U32 chn;             	/*录像通道号 */
	HI_CHAR filename[32];      	/*文件名，格式chnN_yyyyMMdd_hhmmss.tmm，NULL结束 */
	HI_U32 lock_flag;           /*文件锁定*/
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
	HI_U32 ts_offset;       /*当前帧与第一帧的时间偏移*/
	HI_U32 ts_interval;     /*当前帧与前一帧的时间间隔*/
	HI_U32 frame_data_pos;  /*当前帧位置*/
							/*日历时间，19700101 SEC，1B~4B(LOW~HIGH)*/
	HI_U8 nmk_time1;
	HI_U8 nmk_time2;
	HI_U8 nmk_time3;
	HI_U8 nmk_time4;
}__attribute__((packed))STOR_REC_IFRAME_INFO;

typedef struct FILE_IFRAME_LIST_ST
{
	HI_S32 s32Counter;               /* 消息计数 */
	HI_S32 s32NewestIndex;           /* 最新接收到的消息下标值 */
	HI_S32 s32OldestIndex;           /* 最旧接收到的消息下标值 */
	STOR_REC_IFRAME_INFO stRecIFrameInfo[STOR_FILE_IFRAME_MAX_NUMS];
}__attribute__((packed))FILE_IFRAME_LIST_T;

/*存储的GPS数据 */
typedef struct STOR_GPS_FRAME_DATA_T
{
	HI_U8 nmk_time1;
	HI_U8 nmk_time2;
	HI_U8 nmk_time3;
	HI_U8 nmk_time4;
    HI_U32 latitude;        /* 纬度 */
    HI_U32 longitude;       /* 经度 */
    HI_U8  res[64];         /*预留字节 */
}STOR_GPS_FRAME_DATA_ST;

/*gps 数据保存 GPS数据单独保存一个索引  */
typedef struct STOR_GPS_INFO_T
{
	DATA_TYPE_E type;       /*I-FRAME OR P-FRAME*/
	HI_U32 ts_offset;       /*当前帧与第一帧的时间偏移*/
	HI_U32 ts_interval;     /*当前帧与前一帧的时间间隔*/
	HI_U32 frame_data_pos;  /*当前帧位置*/
							/*日历时间，19700101 SEC，1B~4B(LOW~HIGH)*/
	HI_U8 nmk_time1;
	HI_U8 nmk_time2;
	HI_U8 nmk_time3;
	HI_U8 nmk_time4;
}__attribute__((packed))STOR_GPS_INFO_ST;

typedef struct FILE_GPS_LIST_T
{
	HI_S32 s32Counter;               /* 消息计数 */
	HI_S32 s32NewestIndex;           /* 最新接收到的消息下标值 */
	HI_S32 s32OldestIndex;           /* 最旧接收到的消息下标值 */
	STOR_GPS_INFO_ST stGpsInfo[STOR_FILE_IFRAME_MAX_NUMS];
}__attribute__((packed))FILE_GPS_LIST_ST;

typedef struct STOR_BLK_IDX_CHN_HEAD_ST
{
	HI_S32 block_total_num;          /*块数量*/
	HI_S32 block_newest_index;       /*最新索引下标值 */
	HI_S32 block_oldest_index;       /*最老索引下标值*/
}__attribute__((packed))STOR_BLK_IDX_CHN_HEAD_T;

typedef struct STOR_REC_LOG_FILE_ST
{
    HI_S32 s32Counter;               /* 消息计数 */
    HI_S32 s32NewestIndex;           /* 最新接收到的消息下标值 */
    HI_S32 s32OldestIndex;           /* 最旧接收到的消息下标值 */
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
    HI_BOOL bOverWrite; /*是否覆盖*/
    HI_S32 iPartId;    /*正在写partid*/
}STOR_PARAM_ST;

typedef struct _STOR_PART_INOF_T
{
    FILE *pIdxFile;   /*当前索引文件指针，最新分区 */
    STOR_BLK_STAT_INFO_T stBlkStatInfo;        /*块状态信息 */
}STOR_PART_INFO_ST;


typedef struct _HDD_PART_ST
{
    HI_BOOL bOverWrite; /*是否覆盖*/
    HI_S32 iPartId;    /*最新分区，１．重启以文件中读取的为准，２.切分区后以最新分区为准　*/
    HI_S32 partFileNums; /*分区文件个数 */
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
	HI_BOOL bRecording;    /*是否在录像中 */
    HI_BOOL bEnable;        /*参数控制 录像启动 */
    HI_BOOL bSwitchOK;
    HI_S32  iCurrWRId;  /*当前写的分区ID*/
    FILE *pRecFile;   /*录像通道文件指针 */
    FILE *pIdxFile;   /*索引文件指针 */
    HI_CHAR *pRecbuf; /*录像缓冲 */
    STOR_FILE_BLK_HEAD_T stFileBlockHead;      /* 文件块头部信息 */
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

