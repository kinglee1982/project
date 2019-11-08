/******************************************************************************

                  版权所有 (C), 2001-2011, 郑州天迈科技有限公司

 ******************************************************************************
  文 件 名   : av_common.h
  版 本 号   : 初稿
  作    者   :
  生成日期   : 2012年5月16日星期三
  最近修改   :
  功能描述   : App模块内部公用头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2012年5月16日星期三
    作    者   :
    修改内容   : 创建文件

******************************************************************************/
#ifndef __AV_COMMON_H__
#define __AV_COMMON_H__
extern "C"
{
#define IPC_PATH         	                "/opt"          /* 消息队列指定文件名 */
#define APP_RECODE_LOG      				"/mnt/nand/log/recodelog.log"
#define APP_VENC_ERROR_LOG      			"/mnt/a1/vencerror.log"
#define APP_DEV_CFG          			    "/mnt/flash/app_cfg.cfg"
#define APP_HIFB_DEVICE    				    "/dev/fb0"
#define APP_UI_HIFB         			    "/dev/vd"
#define HDD_MOUNT_FILE 					    "/proc/mounts"
#define LOG_FILE_PATH_SEND					"/mnt/flash/log/app_send.txt"  //日志路径
#define LOG_FILE_PATH_RCV					"/mnt/flash/log/app_rcv.txt"  //日志路径
#define TW2865_FILE     					"/dev/tw2865dev"

#define IN
#define OUT
#define INOUT
#define HI_NO_DATA     -2
#define HI_BLOCK_FULL  -3
#define HI_FILE_END    -4

#define TRUE              1
#define FALSE             0

#define APP_AUDIO_RCV_MSG_TYPE        	0x10            /* APP接收的音频消息类型 */
#define APP_VIDEO_RCV_MSG_TYPE        	0x20            /* APP接收的视频消息类型 */
#define APP_PARASET_RCV_MSG_TYPE      	0x30            /* APP接收的参数设置消息类型 */
#define APP_UI_RCV_MSG_TYPE           	0x62            /* UI模块接收的消息类型 */

#define APP_MP3PLAYNAMENUM  			0x10            /* MP3文件列表最大值 */
#define APP_MP3PLAYRUN      			0x01            /* 运行状态 */
#define APP_MP3PLAYIDLE     			0x00            /* 空闲状态 */
#define APP_IPSPEAKRUN      			0x01            /* IP对讲运行状态 */
#define APP_IPSPEAKIDLE     			0x00            /* IP对讲空闲状态 */
#define APP_MP3PLAYFILELEN  			64              /* 文件名长度 */

#define	APP_MSG_QUE_BUF_SIZE             1043      		/* 消息队列数据缓冲区大小 */
#define APP_MSG_QUE_REV_BUFFER_SIZE      1024      		/* 消息数据长度 */
#define APP_MSG_QUE_CTL_PRIORITY_TYPE    0x00      		/* 控制消息消息优先级类型 */
#define APP_MSG_QUE_ASK_PRIORITY_TYPE    0x10      		/* 查询消息优先级类型 */
#define APP_MSG_QUE_RUN_PRIORITY_TYPE    0x20      		/* 执行消息优先级类型 */
#define APP_MSG_QUE_NOMSG_PRIORITY_TYPE  0x30      		/* 没有消息的消息优先级类型 */

#define APP_MP3PLAYCTLRUN   		  0x01   		  /* MP3播放控制命令字运行状态 */

#define APP_IP_SPEAK_READ_TYPE         0x40            /* IP对讲App模块读取消息 */
#define APP_IP_SPEAK_WRITE_TYPE        0x41            /* IP对讲App模块发送消息 */
#define APP_VIDEO_REVIEW_TYPE          0x42            /* 远程视频预览发送消息 */
#define APP_PHOTO_GRAPH_TYPE           0x43            /* 拍照发送消息 */

/* 视频输入参数默认值 */
#define APP_VIDEO_INPUT_BRIGHT      50    /* 亮度 */
#define APP_VIDEO_INPUT_CONTRAST    50    /* 对比度 */
#define APP_VIDEO_INPUT_HUE         50    /* 色调 */
#define APP_VIDEO_INPUT_SATURATION  50    /* 饱和度 */
#define APP_VIDEO_INPUT_SHARPNESS   50    /* 锐度 */

/* 视频输出参数默认值 */
#define APP_VIDEO_OUTPUT_BRIGHT      55    /* 亮度 */
#define APP_VIDEO_OUTPUT_CONTRAST    55    /* 对比度 */
#define APP_VIDEO_OUTPUT_HUE         55    /* 色调 */
#define APP_VIDEO_OUTPUT_SATURATION  55    /* 饱和度 */

#define APP_VIDEO_CFG_VERSION     1002

#define REC_FILENAME_LEN  96

#define   APP_READ_READY      0x00     /* 共享内存可读 */
#define   APP_READ_UNREADY    0X01     /* 共享内存不可读 */

#define APP_PLAYBACK_BOTH_FIRST  0x60
#define APP_PLAYBACK_BOTH_FIRST_SECOND  0x3C
#define APP_PLAYBACK_ONLY_FIRST  0x30
#define APP_PLAYBACK_ONLY_SECOND  0x0C

#define APP_VI_MAXCHNCNT   0x0C   /* 视频输入的最大通道个数 */
#define APP_VO_CHNCNT   0x0C   /* 视频输出的通道个数 */
#define APP_VI_CHNCNT   0x0C   /* 视频输入的通道个数 */

#define APP_VO_DECCNT  0x04

#define APP_VENC_CHNCNT   0x0c   /* 视频编码的通道个数 */
#define APP_VDEC_CHNCNT   0x0c   /* 视频解码的通道个数 */

#define BT656_WORKMODE VI_WORK_MODE_4D1
#define AenSize0 PIC_CIF
#define AenSize1 PIC_QCIF

#define APP_VIDEOOUT_ISMORE_YES  0x01
#define APP_VIDEOOUT_ISMORE_NO  0x00

#define APP_RECODE_POINT_1_X  560
#define APP_RECODE_POINT_1_Y  317

#define APP_RECODE_POINT_2_X  680
#define APP_RECODE_POINT_2_Y  317

#define APP_RECODE_POINT_3_X  560
#define APP_RECODE_POINT_3_Y  377

#define APP_RECODE_POINT_4_X  680
#define APP_RECODE_POINT_4_Y  377

#define APP_RECODE_POINT_5_X  1140
#define APP_RECODE_POINT_5_Y  600

#define APP_RECODE_WIDTH    32
#define APP_RECODE_HEIGHT    32

/* BEGIN: Added by  , 2014/9/17  DES:在视频界面添加通道号和分割线  PN:14091701006 */
#define APP_RECODE_CHN_1_X  (APP_RECODE_POINT_1_X - 98)
#define APP_RECODE_CHN_1_Y  APP_RECODE_POINT_1_Y

#define APP_RECODE_CHN_2_X  (APP_RECODE_POINT_2_X + 56)
#define APP_RECODE_CHN_2_Y  APP_RECODE_POINT_2_Y

#define APP_RECODE_CHN_3_X  (APP_RECODE_POINT_3_X - 98)
#define APP_RECODE_CHN_3_Y  APP_RECODE_POINT_3_Y

#define APP_RECODE_CHN_4_X  (APP_RECODE_POINT_4_X + 56)
#define APP_RECODE_CHN_4_Y  APP_RECODE_POINT_4_Y

#define APP_RECODE_CHN_5_X  (APP_RECODE_POINT_5_X - 98)
#define APP_RECODE_CHN_5_Y  APP_RECODE_POINT_5_Y

#define APP_RECODE_CHN_WIDTH    182
#define APP_RECODE_CHN_HEIGHT    APP_RECODE_HEIGHT

#define APP_RECODE_LINE_1_X  1280/2
#define APP_RECODE_LINE_1_Y  0

#define APP_RECODE_LINE_2_X  0
#define APP_RECODE_LINE_2_Y  720/2

#define APP_RECODE_LINE_WIDTH     1280
#define APP_RECODE_LINE_HEIGHT    2

#define APP_RECODE_LINE1_WIDTH     1
#define APP_RECODE_LINE1_HEIGHT    718



#define SCREEN_WIDTH					1280
#define SCREEN_HEIGHT					718
#define SCREEN_BPP				        16
#define SCREEN_SIZE						(SCREEN_WIDTH * SCREEN_HEIGHT * SCREEN_BPP / 8)
#define VIR_SCREEN_WIDTH				SCREEN_WIDTH
#define VIR_SCREEN_HEIGHT				(SCREEN_HEIGHT+2)*2

#define IMAGE_WIDTH     640
#define IMAGE_HEIGHT    352
#define IMAGE_SIZE      (640*352*2)
#define IMAGE_NUM       14

#define APP_VIDEO_FRAMERATE 25  /* 视频目标帧率 */

#define APP_VIDEO_SUB_FRAMERATE   16 /* 在视频界面添加通道号和分割线  */

#define APP_AUDIO_SEND_MSG_TYPE        0x11            /* APP模块向报站器模块返回音频消息类型 */
#define APP_SET_WARING_CHN             0x03            /* 打开提示音通道 */
#define APP_OPEN_CHN                   0x01            /* 打开通道 */
#define APP_CLOSE_CHN                  0x02            /* 关闭通道 */

#define TM_VO_DEV   2   /* 标清 */

#define APP_DISK_PART_NUM           0x04  /* 磁盘分区个数 */


#define STAT_LOCK_MAX_NUM		(16)	/*状态锁最大数目*/

#define ERR_STR		strerror(errno)			/*错误信息*/

#define SET_BIT_MASK(mask,bit)     (mask = mask|(1<<bit))
#define CLEAR_BIT_MASK(mask,bit)   (mask = mask&(~(1<<bit)))

/*
typedef enum
{
    false=0,
	true=1
}bool;
*/
typedef struct IFLY_MP3_PLAY_ST
{
	HI_U32	mp3FileNum;
	HI_U32   	left;
	/*3531还没有编译这个接口，这个在后期调试时再修改，这个接口还要在调试时注意*/
	//HI_CHAR  	(*mp3_name)[64];
	HI_CHAR *mp3_name;
	HI_CHAR 	reserved[16];
}IFLY_MP3_PLAY_T;

/* 每一条消息队列对应数据结构体 */
typedef struct MSG_QUE_ST
{
    HI_S32  s32MsgType;                                /* 消息类型 */
    HI_CHAR cBuffer[APP_MSG_QUE_BUF_SIZE+1];     /* 消息数据内容1044B */
}MSG_QUE_T;

/* 一条消息中，buffer对应的数据结构 */
typedef struct MSG_QUE_BUF_DATA_ST
{
    HI_S32 s32Len;                   /* 消息长度 */
    HI_S32 s32RspType;            /* 需要返回的数据的消息类型 */
    HI_S32 s32Prio;               /* 该条消息的优先级 */
    HI_S32 s32Cmd;                   /* 该条消息的对应的命令字 */
    HI_CHAR cData[APP_MSG_QUE_REV_BUFFER_SIZE];        /* 消息内容1024B */
    HI_U16 u16Crc;               /* 该条消息的CRC值 */
}__attribute__ ((__packed__))MSG_QUE_BUF_DATA_T;

typedef struct SHM_CTRL_DATA_ST
{
    HI_S32 s32Counter;               /* 消息计数 */
    HI_S32 s32NewestIndex;           /* 最新接收到的消息下标值 */
    HI_S32 s32OldestIndex;           /* 最旧接收到的消息下标值 */
}SHM_CTRL_DATA_T;

typedef struct DATA_SYNC_MSG_QUE_ST
{
    HI_S32  s32MsgType;                                	/* 消息类型 */
    HI_CHAR cBuffer[4];     /* 消息数据内容4B */
}DATA_SYNC_MSG_QUE_T;

//音频属性
typedef struct IFLY_AUDIO_ATTR_ST
{
	HI_U8	audioEnctype;					//预览音频编码类型
	HI_U8	audioBitPerSample;				//预览音频位数
	HI_U16  audioSamplePerSec;				//预览音频采样率
	HI_U16  audioFrameSize;					//预览音频每帧大小
	HI_U16	audioFrameDurTime;				//预览音频每帧间隔
	HI_U8	reserved[4];					//预留
}IFLY_AUDIO_ATTR_T;

/* 返回数据的结构 */
typedef struct MSG_RESULT_ST
{
    HI_S32 s32Result;
    HI_VOID *pData;
}__attribute__ ((__packed__))MSG_RESULT_T;

//系统信息
typedef struct SYS_INFO_ST
{
	HI_CHAR	devicename[32];				//设备名
	HI_CHAR	devicemodel[32];			//设备型号
	HI_CHAR	deviceser[32];				//设备序列号
	HI_CHAR	version[64];				//软件版本
}SYS_INFO_T;

/* 视频属性结构 */
typedef struct VIDEO_ATTR_ST
{
	HI_U8	videoEncType;					//视频编码类型 PT_H264
	HI_U16	max_videowidth;					// u32PicWidth
	HI_U16	max_videoheight;					// u32PicHeight
	HI_U8	reserved[3];					//预留
}VIDEO_ATTR_T;

typedef struct  OSD_DEV_NUM_ST
{
	HI_CHAR busno[32]; //设备号
	HI_U32  x; /*设备号osd的x坐标,[0,352]*/
	HI_U32  y; /*设备号osd的y坐标,[0,288]*/
	HI_U8   reserved[32];  //预留
}OSD_DEV_NUM_T;//设备号osd

typedef struct OSD_EXT_DATA_ST
{
	HI_CHAR extno[32]; //扩展osd的显示内容
	HI_U32  x; /*扩展osd的x坐标,[0,352]*/
	HI_U32  y; /*扩展osd的y坐标,[0,288]*/
	HI_U8   reserved[32];  //预留
}OSD_EXT_DATA_T;//扩展osd

typedef enum OSD_SHOW_TYPE_EN
{
	PRE_Y_CODE_Y = 1,/*预览显示osd，编码显示osd*/
	PRE_Y_CODE_N ,   /*预览显示osd，编码不显示osd*/
	PRE_N_CODE_Y ,   /*预览不显示osd，编码显示osd*/
	PRE_N_CODE_N ,   /*预览不显示osd，编码不显示osd*/
}OSD_SHOW_TYPE_E;

/*osd显示类型结构体*/
typedef struct OSD_SHOW_MODE_ST
{
	OSD_SHOW_TYPE_E osd_show;
	HI_CHAR 	reserved[60];
}OSD_SHOW_MODE_T;

/* 设备号OSD数据结构 */
typedef struct OSD_DEV_ST
{
    OSD_DEV_NUM_T stDevNum;
    OSD_SHOW_TYPE_E enShowType;
}OSD_DEV_T;

/* 扩展OSD数据 */
typedef struct OSD_EXT_ST
{
    OSD_EXT_DATA_T stExtData;
    OSD_SHOW_TYPE_E enShowType;
}OSD_EXT_T;

typedef struct VO_PARA_ST
{
    HI_S32 s32CscLuma;               /* 亮度 */
    HI_S32 s32CscContr;              /* 对比度 */
    HI_S32 s32CscHue;                /* 色调 */
    HI_S32 s32CscSatu;               /* 饱和度 */
}VO_PARA_T;

/* 调整视频输入参数 */
typedef struct VI_PARA_ST
{
    HI_S32 s32CscLuma[APP_VI_CHNCNT];               /* 亮度 */
    HI_S32 s32CscContr[APP_VI_CHNCNT];              /* 对比度 */
    HI_S32 s32CscHue[APP_VI_CHNCNT];                /* 色调 */
    HI_S32 s32CscSatu[APP_VI_CHNCNT];               /* 饱和度 */
    HI_S32 s32Sharpness[APP_VI_CHNCNT];             /* 锐度 */
}VI_PARA_T;
typedef struct VI_PARA_CHN_ST
{
    HI_S32 s32Chn;
    HI_S32 s32CscLuma;               /* 亮度 */
    HI_S32 s32CscContr;              /* 对比度 */
    HI_S32 s32CscHue;                /* 色调 */
    HI_S32 s32CscSatu;               /* 饱和度 */
    HI_S32 s32Sharpness;             /* 锐度 */
}VI_PARA_CHN_T;
/* 视频输入参数 */
typedef struct VIVO_CFG_ST
{
    HI_S32 s32CscLuma;               /* 亮度 */
    HI_S32 s32CscContr;              /* 对比度 */
    HI_S32 s32CscHue;                /* 色调 */
    HI_S32 s32CscSatu;               /* 饱和度 */
    HI_S32 s32Sharpness;             /* 锐度 */
}VIVO_CFG_T;


typedef struct AV_VERSION
{
    HI_U8 version[24];
}AV_VERSION_T;

/* 视频参数配置 */
typedef struct VIDEO_CFG_ST
{
    HI_S32 s32Ver;
    VIVO_CFG_T stInputCfg[APP_VI_CHNCNT];
    VIVO_CFG_T stOutputCfg;
    HI_S32 s32PreviewType;
}VIDEO_CFG_T;

typedef struct REC_FILE_INFO_ST
{
    HI_S32 s32FileStat;
    HI_CHAR cFileName[2][REC_FILENAME_LEN];
}REC_FILE_INFO_T;

typedef struct REC_COPY_ST
{
    time_t tStartTime;
    time_t tEndTime;
    HI_S32 s32Chn[12];
    HI_S32 s32ChnCnt;
}REC_COPY_T;

typedef struct REC_PLAYBACK_ST
{
    HI_S32  s32Cmd;
    time_t  tStartTime;
    time_t  tEndTime;
}__attribute__ ((__packed__))REC_PLAYBACK_T;

typedef struct REC_PLAYBACK_PART_ST
{
    HI_S32 s32FirstPart;
    HI_S32 s32SecondPart;
}REC_PLAYBACK_PART_T;

/* 抓图 */
typedef struct JPEG_SNAP_ST
{
	HI_S32 s32Chn;
	HI_S32 s32Reso;
	HI_S32 s32Quality;
	HI_CHAR reserved[16];
}JPEG_SNAP_T;

//视频数据头
typedef struct
{
	HI_U8 byChnIndex;
	HI_U8 byMediaType;
	HI_U8 byFrameType;
	HI_U8 byLastSnd;
	HI_U8 isqcif;
	HI_U32 dwlen;
	HI_U32 timeStamp;
	HI_U32 FrameID;
}NET_SND_HEAD_T;

//音频数据头
typedef struct VOIP_HEAD_ST
{
	HI_U8 m_byMediaType;
	HI_U8 m_byFrameRate;
	HI_U8 m_bKeyFrame;
	HI_S32 m_wVideoWidth;
	HI_S32 m_wVideoHeight;
	HI_U32 m_dwDataSize;
	HI_U32 m_dwFrameID;
	HI_U32 m_dwTimeStamp;
}__attribute__ ((__packed__))VOIP_HEAD_T;

/*共享内存数据头*/
typedef struct SHM_HEAD_ST
{
	HI_U8 mediaType;		//数据类型-0-图片；1-视频；2-音频
	HI_U32 dataSize;           //数据头+数据长度
	union
	{
	    NET_SND_HEAD_T  video;
        VOIP_HEAD_T voip;
   	};
}SHM_HEAD_T;

/* 共享内存属性数据结构，该数据为只读属性，不需要信号量保护 */
typedef struct SHM_ATTR_ST
{
	HI_VOID *pShmAddr;       	/* 共享内存首地址 */
	HI_S32 s32ItemSize;         /* 共享内存中，一块数据大小 */
	HI_S32 s32ItemMaxNum;      	/* 共享内存中，存储最大块数 */
}SHM_ATTR_T;

typedef struct IP_BIG_PKT_HEAD_ST
{
    HI_U8 m_byMediaType; //00 00 00 00
    HI_U8 *m_pData;       //
    HI_U8 play_index;    //32
	HI_U32 m_dwPreBufSize;//无
    HI_U32 m_dwDataSize;  //955
    HI_U8 m_byFrameRate; // 无
	HI_U32 m_dwFrameID;   //无
	HI_U32 m_dwTimeStamp; //无
    union
    {
        struct
		{
			HI_U32    m_bKeyFrame;    //频帧类型(I or P)
			HI_U16  m_wVideoWidth;  //视频帧宽
			HI_U16       m_wVideoHeight; //视频帧宽
			HI_U32       m_wBitRate;
		}m_tVideoParam;

        HI_U8    m_byAudioMode;//音频模式 16
    };
}IP_BIG_PKT_HEAD_T;

typedef enum hiVOU_GLAYER_E
{
	G0 = 0,
	G1 = 1,
	G2 = 2,
	G3 = 3,
	HC = 4,
	VOU_GLAYER_BUTT
}VOU_GLAYER_E;

/*存储的数据类型*/
typedef enum data_type_en
{
	DATA_VIDEO_IFRAME=0,
    DATA_VIDEO_PFRAME,
    DATA_AUDIO,
    DATA_GPS,
    DATA_FRAME_INDEX,
    DATA_FILE_INFO,
    DATA_INVALID=-1
}DATA_TYPE_E;

/*frame head*/
typedef struct FRAME_HEAD_ST
{
    //HI_U32 u32Chn;          /* 通道号 */
    HI_U64 ts;             /*当前帧的时间戳，hisi时间戳(USEC)*/
	HI_U32 ts_offset;      /*当前帧与第一帧间时间偏移(MSEC)，(pack[0].u64PTS - first_ts)/1000*/
    HI_U32 ts_interval;    /*当前帧与前一帧的时间间隔(MSEC)*/
	HI_U32 len;            /*FRAME_HEAD_T后面跟随的数据的长度(BYTES)*/
	DATA_TYPE_E type;      /*0:video-Iframe,1:video-Pframe,2:audio,3:gps,4:frame_index*/

    /*
    如果数据帧为视频，表示视频状态
    如果数据帧为音视，表示音频数据数量
    如果数据帧为GPS，表示无意义
    */
    union
    {
    	HI_U32 gps_id;      /*表示GPS ID*/
        HI_U32 audio_num;   /*音频数据数量*/
        HI_U32 video_stat;  /*视频状态*/
        HI_U32 index_id;    /*索引ID*/
    }u;

    /*日历时间，19700101 SEC，1B~4B(LOW~HIGH)*/
    HI_U8 nmk_time1;
	HI_U8 nmk_time2;
	HI_U8 nmk_time3;
	HI_U8 nmk_time4;
    HI_U8 flag[8];         /*"IFRAME" OR "PFRAME"*/
    HI_U8 Chn[8];         /*"IFRAME" OR "PFRAME"*/
}__attribute__((packed))FRAME_HEAD_T;



/*状态读写锁结构体*/
typedef struct RWDataLockSt
{
	union
	{
		HI_S32	stat;				/*状态值*/
		HI_VOID *data;				/*数据指针*/
	};

	pthread_rwlock_t rwLock; 		/*读写锁*/
}RW_LOCK_T;


/*状态读写锁类型*/
typedef enum statLockTypeEn
{
	LOCK_REC_STAT_E = 0,		/*录像状态读写锁*/
	LOCK_VO_CHN_E,				/*视频输出通道读写锁*/
	LOCK_SNAP_STAT_E,			/*视频抓图状态读写锁*/
	LOCK_VIDEO_PREVIEW_E,		/*预览状态*/
	LOCK_OSD_DEV_E,				/*视频OSD设备号叠加读写锁*/
	LOCK_OSD_EXT_E,				/*扩展OSD视频数据叠加*/
	LOCK_DISK_STAT_E,			/*硬盘状态读写锁*/
	LOCK_DISK_EVENT_E,			/*硬盘事件读写锁*/
	LOCK_MP3_PLAY_LIST_E,		/*MP3列表读写锁*/
	LOCK_MP3_PLAY_STAT_E,		/*MP3播放状态*/
	LOCK_MP3_PLAY_CTRL_E,		/*MP3播放控制*/
	LOCK_MP3_PLAY_STRUCT_E, 	/*MP3播放结构体*/
	LOCK_MP3_VOL_E,				/*音量设置读写锁*/
	LOCK_IP_SPEAK_E,			/*IP对讲运行状态读写锁*/
	LOCK_REC_BUF_E,				/*录像缓冲区读写锁*/
	LOCK_INVALID_E
}RW_LOCK_TYPE_E;

/*状态锁对应表*/
typedef struct lockMapSt
{
	RW_LOCK_TYPE_E eLockType;	/*状态锁类型*/
	union
	{
		HI_S32	stat;				/*状态值*/
		HI_VOID *pData;				/*数据指针*/
	};
}LOCK_MAP_T;

extern HI_U16 HI3531_CRC16(HI_U8 *u8Dt, HI_U16 u16Len);
extern HI_S32 HI3521_InitRWLock(HI_VOID);
extern HI_S32 HI3531_GetLock(RW_LOCK_TYPE_E eLockType, RW_LOCK_T **pLock);
extern HI_S32 HI3531_SetLockStat(RW_LOCK_T *pLock, HI_S32 s32Stat);
extern HI_S32 HI3531_GetLockStat(RW_LOCK_T *pLock, HI_S32 *pS32Stat);
extern HI_S32 HI3531_SetLockData(RW_LOCK_T *pLock, HI_VOID *pData, HI_S32 s32DataLen);
extern HI_S32 HI3531_GetLockData(RW_LOCK_T *pLock, HI_VOID *pData, HI_S32 s32DataLen);
extern HI_CHAR *HI3531_GetTime(HI_CHAR *pTimeBuf, HI_S32 s32Len);
extern HI_S32 HI3531_WriteLog(HI_CHAR *pLogFile,  HI_CHAR *pLogContent, HI_S32 s32AddTime, HI_S32 s32NewFile);
extern HI_VOID HI3521_CovTime2Str(time_t t, HI_CHAR *pTime);
}
#endif /* __APP_COMMON_H__ */

