#ifndef __AV_VIDEO_PROC_H__
#define __AV_VIDEO_PROC_H__
#include "av_osd.h"
#define TM_APP_RECODE_FILE_FULL_NAME_LENGTH  96


#define APP_VIDEOSTATUS_RUN   0xFF       /* ¼��״̬���� */
#define APP_VIDEOSTATUS_IDLE  0x00      /* ¼��״̬���� */
#define APP_DISK_UNLOCK       0x01       /* Ӳ�������� */
#define APP_VIDEOREPLAY_START 0x02       /* ��Ƶ���ػطš�U�̵�¼����ʷ¼���ϴ���ʼ */
#define APP_VIDEOREPLAY_RUN   0x03       /* ��Ƶ���ػطš�U�̵�¼����ʷ¼���ϴ������� */
#define APP_DISK_UMOUNTING    0x04       /* ��ʼж��Ӳ�� */

#define APP_JPEGSTATUS_RUN    0x01       /* ��Ƶץͼ����ʹ�� */
#define APP_JPEGSTATUS_IDLE   0x00       /* ��Ƶץͼȥʹ�� */

#define APP_VIDEO_MAX_CHN    0x07
#define APP_VIDEO_VIP_FRAME  0x67       /* H264 �ؼ�֡��ʾ */

#define APP_DISK_MOUNT       0x01       /* Ӳ�̹��� */
#define APP_DISK_UMOUNT      0x00       /* Ӳ��ж�� */

#define APP_HDD_LOCK         0x01       /* Ӳ������ */
#define APP_HDD_UNLOCK       0x02       /* Ӳ�̿��� */

#define APP_RECODE_REPLAY         0x01   /* ¼�񱾵ػطű�־ */
#define APP_U_MOVE_RECODE         0x02   /* U�̵�¼���־ */
#define APP_RECODE_DUMP           0x04   /* ��ʷ¼���ϴ���־ */
#define APP_RECODE_RUN            0x80   /* ¼������־ */
#define APP_RECODE_IDLE           0x00   /* �¼����� */
#define APP_RECORD_DOWNLOAD       0x08   /*¼�����ر�־ */
#define APP_RECODEFILE_MAX_CNT   4096
#define APP_RECODEFILE_NAME_LEN  16
#define SF_MAX_NUM 100
#define FILE_NAME_LEN  64
#define MAX_DEC_CHAN_NUM          12  /*���ͨ���� */
#define APP_PLAYBACK_FILE_LEN   64    /* �ط��ļ������� */
#define APP_PLAYBACK_FILE_CNT   10    /* ����ͨ���ط��ļ������� */
#define SWAP(a,b) (a = a^b, b = a^b, a = a^b)
#define PART_INDEX_SEEK_HEAD       (0) /*��������ͷ�����ݵ�ƫ��*/

#define SEARCH_FILE_MAX_TIME (3600*1)    /*������ʱ������ */
#define SEARCH_FILE_MIN_TIME (5)   /*��������Сʱ������ */
#define LOCAL_SEARCH_MAX_TIME (3600*1)*24*31 /*�����������ʱ������ */
#define LOCAL_SEARCH_MIN_TIME (2*60)   /*�����������ʱ������ */

#define MAX_PLAY_BACK_CHN   4   //�����·�ط�
typedef enum PLAY_BACK_EM
{
    PLAY_BACK_STOP = 0,
    PLAY_BACK_1TO4CHN = 1,
    PLAY_BACK_4TO8CHN = 2,
    PLAY_BACK_9CHN = 3,
    PLAY_BACK_BUFF
} PLAY_BACK_E;

typedef struct TM_APP_RECODE_PLAYBACK
{
    int iCMD;
    time_t  iStartTime;
    time_t  iEndTime;
} TM_APP_RECODE_PLAYBACK_t;


/* ¼���ļ��б� */
typedef struct tm_app_recodefilelist
{
    HI_CHAR acRecodeFile[MAX_DEC_CHAN_NUM][32];
} __attribute__ ((__packed__))APP_RECODEFILELIST_T;

/* ¼�������ļ��ṹ�� */
typedef struct tm_app_recodefile
{
    HI_S32 s32Counter;               /* ��Ϣ���� */
    HI_S32 s32NewestIndex;           /* ���½��յ�����Ϣ�±�ֵ */
    HI_S32 s32OldestIndex;           /* ��ɽ��յ�����Ϣ�±�ֵ */
    HI_CHAR filename[APP_RECODEFILE_MAX_CNT*APP_RECODEFILE_NAME_LEN];
} __attribute__ ((__packed__))APP_RECODEFILE_T;

typedef struct tm_app_thread_ctrl_info
{
    pthread_t pid;
    HI_S32 run;
    HI_CHAR argv[128];
} __attribute__ ((__packed__))APP_THREAD_CTRL_INFO_ST;

/* U�̵�¼��ʱ�����ڻظ�UI��¼����� */
typedef struct tm_app_viexport_reply
{
    HI_S32 lChl;                          /* ��ǰ���ڵ�¼���ͨ���� */
    HI_S32 lReply;                        /* ¼�����״̬ */
} __attribute__ ((__packed__))APP_VIEXPORT_REPLAY_ST;

/*ʱ��Ƭ�νṹ */
typedef struct SEARCH_FILE_TIMESEG
{
    time_t tStartTime;
    time_t tEndTime;
    HI_S32    iFileLen;         /*�������ļ������ݣ�����ļ����ȣ�����ƽ̨�ͻ�����������صĽ��� */
} __attribute__ ((__packed__))SF_TIMESEG_ST;


/*ƽ̨�·���¼����������ṹ */
typedef struct APP_SEARCHFILE_REQ_CMD
{
    HI_S32     iChnno;
    time_t  tStartTime;
    time_t  tEndTime;
} __attribute__ ((__packed__))SF_REQ_CMD_ST;

/*¼���������ظ�ƽ̨���ļ�ʱ���б� */
typedef struct APP_SEARCHFILE_RSP
{
    HI_S32     iChnno;
    HI_S32     s32FileSegNum;
    SF_TIMESEG_ST struSearchFile[SF_MAX_NUM];
} __attribute__ ((__packed__))SF_RSP_ST;

/*ƽ̨�·���¼��ط�����ṹ����¼����������������ֹ�Ժ���չ */
typedef struct APP_NET_PLAYFILE_REQ
{
    HI_S32     iChnno;     /*�طŵ�ͨ�� */
    time_t  tStartTime;
    time_t  tEndTime;
} __attribute__ ((__packed__))APP_NET_PLAYFILE_REQ_ST;

/*����¼���ļ�����Žṹ */
typedef struct _SF_NAME_ST_
{
    HI_S32     s32FileType;
    HI_CHAR    szFileName[FILE_NAME_LEN];
} SF_NAME_ST;

/*�����ļ��б���ʱ�ļ���Ϊ10��������ļ�������Ҫ������й��� */
typedef struct _SF_LIST_ST_
{
    HI_S32     iChnno;
    HI_S32     s32FileSegNum;					/*�����������ļ�Ƭ�θ��� */
    SF_NAME_ST stFileName[SF_MAX_NUM];			/*�ļ����ļ����е����� */
    SF_TIMESEG_ST stFileTimeSeg[SF_MAX_NUM];	/*�ļ�ʱ��Ƭ��ʱ�� */
} SF_LIST_ST;

typedef struct APP_LOCAL_PLAYFILE_REQ
{
    HI_S32 iPlayMode;  /*�ļ�����  1-4ͨ����5-8ͨ��9-12 ͨ���ط� */
    time_t  tStartTime;
    time_t  tEndTime;
} APP_LOCAL_PLAYFILE_REQ_ST;

/*���� ��ͣ,���ţ���� ֹͣ ���� */
enum PLAY_CTRL_STAT
{

    LPB_CTRL_SPEEDDOWN = 0,
    LPB_CTRL_PAUSE = 1,
    LPB_CTRL_SPEEDUP = 2,
    LPB_CTRL_STOP = 3,  /*ֹ֮ͣ�� ���ţ��ӿ�ͷ��ʼ���� */
    LPB_CTRL_RETURN = 7

};

typedef struct _SF_PB_
{
    REC_PLAYBACK_T stPB;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    HI_BOOL bWaitSignal;	/*true:�ɹ��ȵ��ź�*/
    HI_BOOL bSendSignal;	/*true:�ɹ������ź�*/
    HI_BOOL bActive;
    HI_S32 Chn;
} SF_PB_ST;



typedef struct _SF_PB_CTRL_
{
    pthread_mutex_t mutex;
    HI_BOOL decexit;	//�����߳��˳���־
    HI_S32 lpbctrlstat;
    HI_BOOL bPause;
    HI_BOOL bSpeedUp;
    HI_BOOL bSpeedDown;
} SF_PB_CTRL;

typedef struct _SF_QRY_
{
    REC_PLAYBACK_T stPB;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    HI_BOOL bWaitSignal;	/*true:�ɹ��ȵ��ź�*/
    HI_BOOL bSendSignal;	/*true:�ɹ������ź�*/
} SF_QRY_ST;
/*���ز����ļ��ṹ�����ڱ��ز��ŵĿ��ƣ�����4ͨ��ͬ�����ŵ� */
typedef struct APP_LOCAL_PLAYFILE_NAME
{
    HI_S32 s32FileSegNum;
    HI_U8 localSearchFileName[SF_MAX_NUM][FILE_NAME_LEN];
    SF_TIMESEG_ST stFileTimeSeg[SF_MAX_NUM];/*�ļ�ʱ��Ƭ��ʱ�� */
} APP_LOCAL_PLAYFILE_NAME_ST;

typedef struct APP_LOCALPLAY_PTHREAD
{
    APP_LOCAL_PLAYFILE_REQ_ST struLocalPlayfile;
    APP_LOCAL_PLAYFILE_NAME_ST struLocalPlayfileName[MAX_DEC_CHAN_NUM];
} APP_LOCALPLAY_PTHREAD_ST;

/*Զ��¼������ */
typedef struct  APP_DOWNLOAD_REC_FILE
{
    HI_S32     iChnno;
    time_t  tStartTime;
    time_t  tEndTime;
} __attribute__ ((__packed__))APP_DOWNLOAD_REC_FILE_ST;

/*�طſ���Э�� */
typedef struct APP_NET_PLAY_CTRL
{
    HI_S32     iChnno;
    HI_S32     iPlayCtrlCmd; /*�طſ���,����Э�������� */
    HI_S32     iSpeed;     /*���ûط��ٶ� ��������Ϊ0x21��Ч��������0 */
    time_t  iSkipStartTime;
    time_t  iSkipEndTime;
} __attribute__ ((__packed__))APP_NET_PLAY_CTRL_ST;

/* ÿһ����Ϣ���ж�Ӧ���ݽṹ�� */
typedef struct APP_PLAYCTRL_MSGQ
{
    HI_S32  s32MsgType;                                	/* ��Ϣ���� */
    HI_CHAR cBuffer[32];     /* ��Ϣ��������1024B */
} __attribute__ ((__packed__))APP_PLAYCTRL_MSGQ_ST;

typedef struct vdec_video_adjust
{
    HI_U32 ch;
    HI_U32 value;
} vdec_video_adjust;
enum BK_STAT_RETURN
{
    BK_FILESEARCH_FAIL=-1,              /* �ļ���ѯʧ�� */
    BK_DIRCAREATE_FAIL=-2,              /* Ŀ¼����ʧ�� */
    BK_FILEOPEN_FAIL=-3,              /* �ļ���ʧ�� */
    BK_FILECOPY_FAIL=-4,              /* �ļ�����ʧ�� */
    BK_MALLOC_FAIL=-5,              /* �ڴ�����ʧ�� */
    BK_POSFRAME_FAIL=-6,              /* ֡ʧ����û�ж�λ��֡ */
    BK_UDISK_EXIT =-7,              /* u���Ѱγ� */
    BK_HDD_ABSENCE=-8,              /* Ӳ��δ���� */
    BK_HDD_UDISK_NOSPASE = -9,           /*u��û�ռ� */
    BK_CHN_CHECK_ERROR  = -10,           /*ͨ�����ʧ�� */
    BK_TIME_CHECK_ERROR  = -11,           /*ͨ�����ʧ�� */
    BK_FINISH=0x5A5A5A5A,              /* ͨ���������� */
    BK_FILEEXPORT_QUIT=0x5B5B5B5B      /* ��¼����� */
};

#define TW2968_SET_HUE          0x01
#define TW2968_SET_CONTRAST     0x02
#define TW2968_SET_BRIGHT       0x04
#define TW2968_SET_SATURATION   0x08
#define TW2968_SET_SHARPNESS   0x0F

#define TW2968_IOC_MAGIC            't'

#define TW2968CMD_READ_REG		      _IOWR(TW2968_IOC_MAGIC, 1, int)
#define TW2968CMD_WRITE_REG	        _IOW(TW2968_IOC_MAGIC, 2, int)

#define TW2968_GET_VIDEO_LOSS		    _IOWR(TW2968_IOC_MAGIC, 4, tw2968_video_loss)
#define TW2968_SET_IMAGE_ADJUST	    _IOW(TW2968_IOC_MAGIC, 5, tw2968_image_adjust)
#define TW2968_GET_IMAGE_ADJUST	    _IOWR(TW2968_IOC_MAGIC, 6, tw2968_image_adjust)

typedef struct _tw2968_image_adjust
{
    unsigned char chip;
    unsigned char chn;
    unsigned char hue;//[7f,80],7f,0,ff,80
    unsigned char contrast;//[0,255]
    signed char brightness;//[-128,127]
    unsigned char saturation;//[0,255]
    unsigned char sharpness;
    unsigned long item_sel;/* use such as "TW2968_SET_HUE|TW2968_SET_CONTRAST" */
} tw2968_image_adjust;

#define IOC_VDEC_SET_VIDEO_MODE			0x07
#define IOC_VDEC_GET_VIDEO_LOSS     	0x09
#define IOC_VDEC_SET_BRIGHTNESS	    	0x0e
#define IOC_VDEC_SET_CONTRAST   		0x0f
#define IOC_VDEC_SET_HUE    			0x10
#define IOC_VDEC_SET_SATURATION  		0x11
#define IOC_VDEC_SET_SHARPNESS  		0x12
#define NVP6114DEV "/dev/nvp6114"
#define TW2968DEV "/dev/tw2968dev"
typedef struct _tw2968_video_loss
{
    unsigned int chip;
    unsigned int ch;
    unsigned int is_lost;
} tw2968_video_loss;
typedef struct _SF_BK_
{
    REC_COPY_T stBK;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    HI_BOOL bWaitSignal;	/*true:�ɹ��ȵ��ź�*/
    HI_BOOL bSendSignal;	/*true:�ɹ������ź�*/
} SF_BK_ST;
#define BK_STAT_SND 10          /*10Sһ��*/
#define LOCAL_BK_MAX_TIME 60*60*12 //12Сʱ
#define LOCAL_BK_MIN_TIME 60
#define BKFILE_IFRAME_START_POS (1048576) /*U�̵�¼���I֡�����1Mλ�� */
#define BKFILE_FRAME_DATA_START_POS (2097152) /*������������2Mλ�� */
#define MK_USB_BK_FILE_NAME(filename,dirname,year,mon,day,hour,min,sec,chn)\
    snprintf(filename, sizeof(filename), "%s/%04d%02d%02d%02d%02d%02d_chn%02d.TMM", dirname,year,mon,day,hour,min,sec,chn)
#define MK_USB_BK_FILE_DIR(dirname,busno) \
    snprintf(dirname, 64, "%s/%s", "/mnt/usb", busno);
#define MK_SSD_BK_FILE_DIR(dirname,busno) \
    snprintf(dirname, 64, "%s", "/mnt/ssd");
#define MK_SSD_BK_FILE_NAME(filename,dirname,year,mon,day,hour,min,sec,chn)\
    snprintf(filename, sizeof(filename), "%s/%04d%02d%02d%02d%02d%02d_chn%02d.TMM", dirname,year,mon,day,hour,min,sec,chn)
extern HI_VOID HI3521_ConvertTime2LocaltimeStr2(HI_CHAR *pIdx, time_t oldTime, time_t newTime);
extern HI_VOID HI3521_ConvertTime2LocaltimeStr(time_t t, HI_CHAR *pTime);
extern HI_VOID AV_SetVideoSnapShotChn(HI_U16 u16SnapChn);
extern HI_VOID AV_SetVideoRecChn(HI_U16 u16RecChn);
int HI3521_RecodePlayback(REC_PLAYBACK_T *pstPlaybackCMD);
extern HI_VOID *HDD_Check(HI_VOID *pArg);
HI_S32 HDD_WrRecStat(HI_S32 iRecode,HI_S32 iEvent, HI_S32 s32LineNum, HI_CHAR *pcFileName);
HI_S32 APP_SetRecMark(HI_S32 bIsOpen, HI_S32 s32SplitMode, HI_S32 s32SplitIndex);
extern HI_S32 APP_GetVideoOutputPara(VO_PARA_T *pstVideoOutPara);
extern HI_S32 APP_SetVideoOutputPara(VO_PARA_T *pstVideoOutPara);
extern HI_S32 APP_GetVideoInputPara(VI_PARA_T *pstVideoInPara);
extern HI_S32 APP_SetVideoInPara(VI_PARA_T *pstVideoInPara);
extern HI_S32 APP_SetVideoInPara_Chn(VI_PARA_CHN_T *pstVideoInPara);
extern HI_BOOL STOR_IsRecOverWrite(HI_VOID);
extern HI_S32 APP_RdVideoParaCfg(const HI_CHAR *cfg_file, HI_VOID *add_t);
HI_S32 HI3521_SdGetVideoInPara(tw2968_image_adjust *videoinpara);
HI_S32 HI3521_Open_SdViDev();
HI_S32 HI3521_Open_HdViDev();
HI_S32 HI3521_HdGetVideoLostStat(HI_VOID);
HI_S32 HI3521_HdSetVideoInBrightness(HI_U32 chl,HI_U32 val);
HI_S32 HI3521_HdSetVideoInContrast(HI_U32 chl,HI_U32 val);
HI_S32 HI3521_HdSetVideoInHue(HI_U32 chl,HI_U32 val);
HI_S32 HI3521_SdSetVideoInPara(tw2968_image_adjust videoinpara);
HI_S32 HI3521_HdSetVideoInSaturation(HI_U32 chl,HI_U32 val);
HI_S32 HI3521_HdSetVideoInSharpness(HI_U32 chl,HI_U32 val);
HI_S32 HI3521_SdGetVideoLostStat(tw2968_video_loss *video_loss);
HI_S32 HI3521_COMM_VDEC_Stop(HI_S32 s32ChnNum);
HI_S32 HI3521_SFByTime2(void *pReq);
HI_S32 HI3521_StartPlayback(REC_PLAYBACK_T *pstLPB);
HI_VOID HI3521_SetAllPlayCtrl(HI_S32 s32playctrl);
HI_VOID HI3521_Setdecchn( HI_U8 chn);
extern HI_S32 APP_GetDefaultChnPara(HI_S32 *pdefaultchn);
extern HI_S32 APP_SetDefaultChnPara(HI_U16 u16Para);
extern HI_S32 HI3521_LocalBK(REC_COPY_T *pstLBK);
extern HI_S32 HI3521_ChkUsbBkTime(time_t *tStartTime, time_t *tEndTime);
extern HI_S32 HI3521_GetChnStoreStat(HI_U16 *chnStore);
extern HI_S32 HI3521_GetDevErrStat(HI_VOID);
extern FILE *HI3521_OpenSF(SF_NAME_ST *pstSFName);
extern HI_S32 HI3521_VideoInParaInit(HI_VOID);
HI_S32 HI3531_SFByTime_V28181(SF_REQ_CMD_ST *pstSFReq, SF_LIST_ST *pstSFList, HI_BOOL bLocal, HI_S32 s32RecType);
#endif /* __AV_VIDEO_PROC_H__ */
