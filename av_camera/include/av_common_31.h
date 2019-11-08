/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ֣�������Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : av_common.h
  �� �� ��   : ����
  ��    ��   :
  ��������   : 2012��5��16��������
  ����޸�   :
  ��������   : Appģ���ڲ�����ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2012��5��16��������
    ��    ��   :
    �޸�����   : �����ļ�

******************************************************************************/
#ifndef __AV_COMMON_H__
#define __AV_COMMON_H__
extern "C"
{
#define IPC_PATH         	                "/opt"          /* ��Ϣ����ָ���ļ��� */
#define APP_RECODE_LOG      				"/mnt/nand/log/recodelog.log"
#define APP_VENC_ERROR_LOG      			"/mnt/a1/vencerror.log"
#define APP_DEV_CFG          			    "/mnt/flash/app_cfg.cfg"
#define APP_HIFB_DEVICE    				    "/dev/fb0"
#define APP_UI_HIFB         			    "/dev/vd"
#define HDD_MOUNT_FILE 					    "/proc/mounts"
#define LOG_FILE_PATH_SEND					"/mnt/flash/log/app_send.txt"  //��־·��
#define LOG_FILE_PATH_RCV					"/mnt/flash/log/app_rcv.txt"  //��־·��
#define TW2865_FILE     					"/dev/tw2865dev"

#define IN
#define OUT
#define INOUT
#define HI_NO_DATA     -2
#define HI_BLOCK_FULL  -3
#define HI_FILE_END    -4

#define TRUE              1
#define FALSE             0

#define APP_AUDIO_RCV_MSG_TYPE        	0x10            /* APP���յ���Ƶ��Ϣ���� */
#define APP_VIDEO_RCV_MSG_TYPE        	0x20            /* APP���յ���Ƶ��Ϣ���� */
#define APP_PARASET_RCV_MSG_TYPE      	0x30            /* APP���յĲ���������Ϣ���� */
#define APP_UI_RCV_MSG_TYPE           	0x62            /* UIģ����յ���Ϣ���� */

#define APP_MP3PLAYNAMENUM  			0x10            /* MP3�ļ��б����ֵ */
#define APP_MP3PLAYRUN      			0x01            /* ����״̬ */
#define APP_MP3PLAYIDLE     			0x00            /* ����״̬ */
#define APP_IPSPEAKRUN      			0x01            /* IP�Խ�����״̬ */
#define APP_IPSPEAKIDLE     			0x00            /* IP�Խ�����״̬ */
#define APP_MP3PLAYFILELEN  			64              /* �ļ������� */

#define	APP_MSG_QUE_BUF_SIZE             1043      		/* ��Ϣ�������ݻ�������С */
#define APP_MSG_QUE_REV_BUFFER_SIZE      1024      		/* ��Ϣ���ݳ��� */
#define APP_MSG_QUE_CTL_PRIORITY_TYPE    0x00      		/* ������Ϣ��Ϣ���ȼ����� */
#define APP_MSG_QUE_ASK_PRIORITY_TYPE    0x10      		/* ��ѯ��Ϣ���ȼ����� */
#define APP_MSG_QUE_RUN_PRIORITY_TYPE    0x20      		/* ִ����Ϣ���ȼ����� */
#define APP_MSG_QUE_NOMSG_PRIORITY_TYPE  0x30      		/* û����Ϣ����Ϣ���ȼ����� */

#define APP_MP3PLAYCTLRUN   		  0x01   		  /* MP3���ſ�������������״̬ */

#define APP_IP_SPEAK_READ_TYPE         0x40            /* IP�Խ�Appģ���ȡ��Ϣ */
#define APP_IP_SPEAK_WRITE_TYPE        0x41            /* IP�Խ�Appģ�鷢����Ϣ */
#define APP_VIDEO_REVIEW_TYPE          0x42            /* Զ����ƵԤ��������Ϣ */
#define APP_PHOTO_GRAPH_TYPE           0x43            /* ���շ�����Ϣ */

/* ��Ƶ�������Ĭ��ֵ */
#define APP_VIDEO_INPUT_BRIGHT      50    /* ���� */
#define APP_VIDEO_INPUT_CONTRAST    50    /* �Աȶ� */
#define APP_VIDEO_INPUT_HUE         50    /* ɫ�� */
#define APP_VIDEO_INPUT_SATURATION  50    /* ���Ͷ� */
#define APP_VIDEO_INPUT_SHARPNESS   50    /* ��� */

/* ��Ƶ�������Ĭ��ֵ */
#define APP_VIDEO_OUTPUT_BRIGHT      55    /* ���� */
#define APP_VIDEO_OUTPUT_CONTRAST    55    /* �Աȶ� */
#define APP_VIDEO_OUTPUT_HUE         55    /* ɫ�� */
#define APP_VIDEO_OUTPUT_SATURATION  55    /* ���Ͷ� */

#define APP_VIDEO_CFG_VERSION     1002

#define REC_FILENAME_LEN  96

#define   APP_READ_READY      0x00     /* �����ڴ�ɶ� */
#define   APP_READ_UNREADY    0X01     /* �����ڴ治�ɶ� */

#define APP_PLAYBACK_BOTH_FIRST  0x60
#define APP_PLAYBACK_BOTH_FIRST_SECOND  0x3C
#define APP_PLAYBACK_ONLY_FIRST  0x30
#define APP_PLAYBACK_ONLY_SECOND  0x0C

#define APP_VI_MAXCHNCNT   0x0C   /* ��Ƶ��������ͨ������ */
#define APP_VO_CHNCNT   0x0C   /* ��Ƶ�����ͨ������ */
#define APP_VI_CHNCNT   0x0C   /* ��Ƶ�����ͨ������ */

#define APP_VO_DECCNT  0x04

#define APP_VENC_CHNCNT   0x0c   /* ��Ƶ�����ͨ������ */
#define APP_VDEC_CHNCNT   0x0c   /* ��Ƶ�����ͨ������ */

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

/* BEGIN: Added by  , 2014/9/17  DES:����Ƶ�������ͨ���źͷָ���  PN:14091701006 */
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

#define APP_VIDEO_FRAMERATE 25  /* ��ƵĿ��֡�� */

#define APP_VIDEO_SUB_FRAMERATE   16 /* ����Ƶ�������ͨ���źͷָ���  */

#define APP_AUDIO_SEND_MSG_TYPE        0x11            /* APPģ����վ��ģ�鷵����Ƶ��Ϣ���� */
#define APP_SET_WARING_CHN             0x03            /* ����ʾ��ͨ�� */
#define APP_OPEN_CHN                   0x01            /* ��ͨ�� */
#define APP_CLOSE_CHN                  0x02            /* �ر�ͨ�� */

#define TM_VO_DEV   2   /* ���� */

#define APP_DISK_PART_NUM           0x04  /* ���̷������� */


#define STAT_LOCK_MAX_NUM		(16)	/*״̬�������Ŀ*/

#define ERR_STR		strerror(errno)			/*������Ϣ*/

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
	/*3531��û�б�������ӿڣ�����ں��ڵ���ʱ���޸ģ�����ӿڻ�Ҫ�ڵ���ʱע��*/
	//HI_CHAR  	(*mp3_name)[64];
	HI_CHAR *mp3_name;
	HI_CHAR 	reserved[16];
}IFLY_MP3_PLAY_T;

/* ÿһ����Ϣ���ж�Ӧ���ݽṹ�� */
typedef struct MSG_QUE_ST
{
    HI_S32  s32MsgType;                                /* ��Ϣ���� */
    HI_CHAR cBuffer[APP_MSG_QUE_BUF_SIZE+1];     /* ��Ϣ��������1044B */
}MSG_QUE_T;

/* һ����Ϣ�У�buffer��Ӧ�����ݽṹ */
typedef struct MSG_QUE_BUF_DATA_ST
{
    HI_S32 s32Len;                   /* ��Ϣ���� */
    HI_S32 s32RspType;            /* ��Ҫ���ص����ݵ���Ϣ���� */
    HI_S32 s32Prio;               /* ������Ϣ�����ȼ� */
    HI_S32 s32Cmd;                   /* ������Ϣ�Ķ�Ӧ�������� */
    HI_CHAR cData[APP_MSG_QUE_REV_BUFFER_SIZE];        /* ��Ϣ����1024B */
    HI_U16 u16Crc;               /* ������Ϣ��CRCֵ */
}__attribute__ ((__packed__))MSG_QUE_BUF_DATA_T;

typedef struct SHM_CTRL_DATA_ST
{
    HI_S32 s32Counter;               /* ��Ϣ���� */
    HI_S32 s32NewestIndex;           /* ���½��յ�����Ϣ�±�ֵ */
    HI_S32 s32OldestIndex;           /* ��ɽ��յ�����Ϣ�±�ֵ */
}SHM_CTRL_DATA_T;

typedef struct DATA_SYNC_MSG_QUE_ST
{
    HI_S32  s32MsgType;                                	/* ��Ϣ���� */
    HI_CHAR cBuffer[4];     /* ��Ϣ��������4B */
}DATA_SYNC_MSG_QUE_T;

//��Ƶ����
typedef struct IFLY_AUDIO_ATTR_ST
{
	HI_U8	audioEnctype;					//Ԥ����Ƶ��������
	HI_U8	audioBitPerSample;				//Ԥ����Ƶλ��
	HI_U16  audioSamplePerSec;				//Ԥ����Ƶ������
	HI_U16  audioFrameSize;					//Ԥ����Ƶÿ֡��С
	HI_U16	audioFrameDurTime;				//Ԥ����Ƶÿ֡���
	HI_U8	reserved[4];					//Ԥ��
}IFLY_AUDIO_ATTR_T;

/* �������ݵĽṹ */
typedef struct MSG_RESULT_ST
{
    HI_S32 s32Result;
    HI_VOID *pData;
}__attribute__ ((__packed__))MSG_RESULT_T;

//ϵͳ��Ϣ
typedef struct SYS_INFO_ST
{
	HI_CHAR	devicename[32];				//�豸��
	HI_CHAR	devicemodel[32];			//�豸�ͺ�
	HI_CHAR	deviceser[32];				//�豸���к�
	HI_CHAR	version[64];				//����汾
}SYS_INFO_T;

/* ��Ƶ���Խṹ */
typedef struct VIDEO_ATTR_ST
{
	HI_U8	videoEncType;					//��Ƶ�������� PT_H264
	HI_U16	max_videowidth;					// u32PicWidth
	HI_U16	max_videoheight;					// u32PicHeight
	HI_U8	reserved[3];					//Ԥ��
}VIDEO_ATTR_T;

typedef struct  OSD_DEV_NUM_ST
{
	HI_CHAR busno[32]; //�豸��
	HI_U32  x; /*�豸��osd��x����,[0,352]*/
	HI_U32  y; /*�豸��osd��y����,[0,288]*/
	HI_U8   reserved[32];  //Ԥ��
}OSD_DEV_NUM_T;//�豸��osd

typedef struct OSD_EXT_DATA_ST
{
	HI_CHAR extno[32]; //��չosd����ʾ����
	HI_U32  x; /*��չosd��x����,[0,352]*/
	HI_U32  y; /*��չosd��y����,[0,288]*/
	HI_U8   reserved[32];  //Ԥ��
}OSD_EXT_DATA_T;//��չosd

typedef enum OSD_SHOW_TYPE_EN
{
	PRE_Y_CODE_Y = 1,/*Ԥ����ʾosd��������ʾosd*/
	PRE_Y_CODE_N ,   /*Ԥ����ʾosd�����벻��ʾosd*/
	PRE_N_CODE_Y ,   /*Ԥ������ʾosd��������ʾosd*/
	PRE_N_CODE_N ,   /*Ԥ������ʾosd�����벻��ʾosd*/
}OSD_SHOW_TYPE_E;

/*osd��ʾ���ͽṹ��*/
typedef struct OSD_SHOW_MODE_ST
{
	OSD_SHOW_TYPE_E osd_show;
	HI_CHAR 	reserved[60];
}OSD_SHOW_MODE_T;

/* �豸��OSD���ݽṹ */
typedef struct OSD_DEV_ST
{
    OSD_DEV_NUM_T stDevNum;
    OSD_SHOW_TYPE_E enShowType;
}OSD_DEV_T;

/* ��չOSD���� */
typedef struct OSD_EXT_ST
{
    OSD_EXT_DATA_T stExtData;
    OSD_SHOW_TYPE_E enShowType;
}OSD_EXT_T;

typedef struct VO_PARA_ST
{
    HI_S32 s32CscLuma;               /* ���� */
    HI_S32 s32CscContr;              /* �Աȶ� */
    HI_S32 s32CscHue;                /* ɫ�� */
    HI_S32 s32CscSatu;               /* ���Ͷ� */
}VO_PARA_T;

/* ������Ƶ������� */
typedef struct VI_PARA_ST
{
    HI_S32 s32CscLuma[APP_VI_CHNCNT];               /* ���� */
    HI_S32 s32CscContr[APP_VI_CHNCNT];              /* �Աȶ� */
    HI_S32 s32CscHue[APP_VI_CHNCNT];                /* ɫ�� */
    HI_S32 s32CscSatu[APP_VI_CHNCNT];               /* ���Ͷ� */
    HI_S32 s32Sharpness[APP_VI_CHNCNT];             /* ��� */
}VI_PARA_T;
typedef struct VI_PARA_CHN_ST
{
    HI_S32 s32Chn;
    HI_S32 s32CscLuma;               /* ���� */
    HI_S32 s32CscContr;              /* �Աȶ� */
    HI_S32 s32CscHue;                /* ɫ�� */
    HI_S32 s32CscSatu;               /* ���Ͷ� */
    HI_S32 s32Sharpness;             /* ��� */
}VI_PARA_CHN_T;
/* ��Ƶ������� */
typedef struct VIVO_CFG_ST
{
    HI_S32 s32CscLuma;               /* ���� */
    HI_S32 s32CscContr;              /* �Աȶ� */
    HI_S32 s32CscHue;                /* ɫ�� */
    HI_S32 s32CscSatu;               /* ���Ͷ� */
    HI_S32 s32Sharpness;             /* ��� */
}VIVO_CFG_T;


typedef struct AV_VERSION
{
    HI_U8 version[24];
}AV_VERSION_T;

/* ��Ƶ�������� */
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

/* ץͼ */
typedef struct JPEG_SNAP_ST
{
	HI_S32 s32Chn;
	HI_S32 s32Reso;
	HI_S32 s32Quality;
	HI_CHAR reserved[16];
}JPEG_SNAP_T;

//��Ƶ����ͷ
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

//��Ƶ����ͷ
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

/*�����ڴ�����ͷ*/
typedef struct SHM_HEAD_ST
{
	HI_U8 mediaType;		//��������-0-ͼƬ��1-��Ƶ��2-��Ƶ
	HI_U32 dataSize;           //����ͷ+���ݳ���
	union
	{
	    NET_SND_HEAD_T  video;
        VOIP_HEAD_T voip;
   	};
}SHM_HEAD_T;

/* �����ڴ��������ݽṹ��������Ϊֻ�����ԣ�����Ҫ�ź������� */
typedef struct SHM_ATTR_ST
{
	HI_VOID *pShmAddr;       	/* �����ڴ��׵�ַ */
	HI_S32 s32ItemSize;         /* �����ڴ��У�һ�����ݴ�С */
	HI_S32 s32ItemMaxNum;      	/* �����ڴ��У��洢������ */
}SHM_ATTR_T;

typedef struct IP_BIG_PKT_HEAD_ST
{
    HI_U8 m_byMediaType; //00 00 00 00
    HI_U8 *m_pData;       //
    HI_U8 play_index;    //32
	HI_U32 m_dwPreBufSize;//��
    HI_U32 m_dwDataSize;  //955
    HI_U8 m_byFrameRate; // ��
	HI_U32 m_dwFrameID;   //��
	HI_U32 m_dwTimeStamp; //��
    union
    {
        struct
		{
			HI_U32    m_bKeyFrame;    //Ƶ֡����(I or P)
			HI_U16  m_wVideoWidth;  //��Ƶ֡��
			HI_U16       m_wVideoHeight; //��Ƶ֡��
			HI_U32       m_wBitRate;
		}m_tVideoParam;

        HI_U8    m_byAudioMode;//��Ƶģʽ 16
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

/*�洢����������*/
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
    //HI_U32 u32Chn;          /* ͨ���� */
    HI_U64 ts;             /*��ǰ֡��ʱ�����hisiʱ���(USEC)*/
	HI_U32 ts_offset;      /*��ǰ֡���һ֡��ʱ��ƫ��(MSEC)��(pack[0].u64PTS - first_ts)/1000*/
    HI_U32 ts_interval;    /*��ǰ֡��ǰһ֡��ʱ����(MSEC)*/
	HI_U32 len;            /*FRAME_HEAD_T�����������ݵĳ���(BYTES)*/
	DATA_TYPE_E type;      /*0:video-Iframe,1:video-Pframe,2:audio,3:gps,4:frame_index*/

    /*
    �������֡Ϊ��Ƶ����ʾ��Ƶ״̬
    �������֡Ϊ���ӣ���ʾ��Ƶ��������
    �������֡ΪGPS����ʾ������
    */
    union
    {
    	HI_U32 gps_id;      /*��ʾGPS ID*/
        HI_U32 audio_num;   /*��Ƶ��������*/
        HI_U32 video_stat;  /*��Ƶ״̬*/
        HI_U32 index_id;    /*����ID*/
    }u;

    /*����ʱ�䣬19700101 SEC��1B~4B(LOW~HIGH)*/
    HI_U8 nmk_time1;
	HI_U8 nmk_time2;
	HI_U8 nmk_time3;
	HI_U8 nmk_time4;
    HI_U8 flag[8];         /*"IFRAME" OR "PFRAME"*/
    HI_U8 Chn[8];         /*"IFRAME" OR "PFRAME"*/
}__attribute__((packed))FRAME_HEAD_T;



/*״̬��д���ṹ��*/
typedef struct RWDataLockSt
{
	union
	{
		HI_S32	stat;				/*״ֵ̬*/
		HI_VOID *data;				/*����ָ��*/
	};

	pthread_rwlock_t rwLock; 		/*��д��*/
}RW_LOCK_T;


/*״̬��д������*/
typedef enum statLockTypeEn
{
	LOCK_REC_STAT_E = 0,		/*¼��״̬��д��*/
	LOCK_VO_CHN_E,				/*��Ƶ���ͨ����д��*/
	LOCK_SNAP_STAT_E,			/*��Ƶץͼ״̬��д��*/
	LOCK_VIDEO_PREVIEW_E,		/*Ԥ��״̬*/
	LOCK_OSD_DEV_E,				/*��ƵOSD�豸�ŵ��Ӷ�д��*/
	LOCK_OSD_EXT_E,				/*��չOSD��Ƶ���ݵ���*/
	LOCK_DISK_STAT_E,			/*Ӳ��״̬��д��*/
	LOCK_DISK_EVENT_E,			/*Ӳ���¼���д��*/
	LOCK_MP3_PLAY_LIST_E,		/*MP3�б��д��*/
	LOCK_MP3_PLAY_STAT_E,		/*MP3����״̬*/
	LOCK_MP3_PLAY_CTRL_E,		/*MP3���ſ���*/
	LOCK_MP3_PLAY_STRUCT_E, 	/*MP3���Žṹ��*/
	LOCK_MP3_VOL_E,				/*�������ö�д��*/
	LOCK_IP_SPEAK_E,			/*IP�Խ�����״̬��д��*/
	LOCK_REC_BUF_E,				/*¼�񻺳�����д��*/
	LOCK_INVALID_E
}RW_LOCK_TYPE_E;

/*״̬����Ӧ��*/
typedef struct lockMapSt
{
	RW_LOCK_TYPE_E eLockType;	/*״̬������*/
	union
	{
		HI_S32	stat;				/*״ֵ̬*/
		HI_VOID *pData;				/*����ָ��*/
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

