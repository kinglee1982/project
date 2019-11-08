#ifndef __TM_AV_SOCKET_H_
#define __TM_AV_SOCKET_H_

#include "data_buf.h"

#define Recv_Error "APP"
#define DOMAIN_TIMEOUT		(1)		//һ��
#define TM_UNIX_SOCKE_PATH         "/tmp/tm_sock"
#define DOMAIN_PATH			"/root/av_sock"
#define DOMAIN_BUF_LEN		(4096)
#define DOMAIN_PARA_ERR		(-100)
#define DOMAIN_CRC_ERR		(-101)
#define DOMAIN_POP_ERR  	(-102)
#define DOMAIN_PUSH_ERR 	(-103)
#define DOMAIN_RCV_ERR  	(-104)
#define DOMAIN_SND_ERR  	(-105)

typedef enum PARA_ID
{
    PARAID_DEFAULT_CHN = 0x0B,        //Ĭ��ͨ��
    PARAID_RECORD_CHN_CFG = 0x0D,        //¼��״̬
    PARAID_PHOTO_CHN = 0x0E,        //ץ��ͨ��
    PARAID_IP_MIC_POWER = 0x12,    /* IPͨ���������*/
    PARAID_LUMINANCE = 0x19,	/* ����*/
    PARAID_CONTRAST = 0x1A,	/* �Աȶ�*/
    PARAID_HUE = 0x1B,    /* ɫ��*/
    PARAID_SATURATION = 0x1C,    /* ���Ͷ�*/
    PARAID_ACC_ENABLE = 0x24,        //�Ƿ�����ACC�źſ���Ӳ�̹���
    PARAID_COOL_DOWN_ENABLE = 0x30,        //�Ƿ����ø�������
    PARAID_COOL_DOWN_TEMP = 0x31,        //�������������¶�ֵ
} PARA_ID_E;


#define BUS_FRONT_DOOR_OPEN       (0x01)  /*BIT0:  1.ǰ�ſ��� 0.������*/
#define BUS_FRONT_DOOR_CLOSE      (0x02)  /*BIT1:  1.ǰ�Ź��� 0.������*/
#define BUS_BACK_DOOR_OPEN        (0x04)  /*BIT2:  1.���ſ��� 0.������*/
#define BUS_BACK_DOOR_CLOSE       (0x08)  /*BIT3:  1.���Ź��� 0.������*/

typedef enum Ui_Video_Cmd
{
    AV_TAKE_PHOTO = 1,
    AV_SET_IP,               /* 0x02        ����ip��ַ��˿� */
    AV_SET_SUB_PARA,         /* 0x03        �������������� */
    AV_GET_NET_STAT,         /* 0x04        ��ѯ�����������״̬ */
    AV_REFER_IP,             /* 0x05        ��ѯip��ַ��˿� */
    AV_GET_SUB_PARA,         /* 0x06        ��ѯ���������� */
    AV_OPEN_VOIP,            /* 0x07        �����Խ� */
    AV_CLOSE_VOIP,           /* 0x08        �رնԽ� */
    AV_SWITCH_SCREEN,        /* 0x09        �����л� */
    AV_UMOUNT_DISK,          /* 0x0A        ж��Ӳ�̣����ٽ���������д��Ӳ�� */
    AV_OPEN_REC,             /* 0x0B        ��¼�� */
    AV_CLOSE_REC,            /* 0x0C        �ر�¼�񣬽���������д��Ӳ�̺�ж��Ӳ�� */
    AV_ALARM_EVENT,          /* 0x0D        �����¼�֪ͨ */
    AV_SET_REC_CHN,          /* 0x0E        ����¼�� */
    UI_UPLOAD_PIC,           /* 0x0F        ͼƬ�ϴ���Ϣ */
    AV_HEART_BEAT,           /* 0x10        ��monitor������ά������--������*/
    UI_AV_LOGIN,  		     /* 0x11        ��Ƶ����ע���̨��Ƶ��������־ */
    UI_AV_VOIP,              /* 0x12        IP�Խ���Э���־*/
    UI_AV_VER,               /* 0x13        ��Ƶ����汾�ţ� ������ʱ����һ�� */
    AV_OSD_SET,              /* 0x14        ��ƵOSD��Ϣ���� */
    AV_BUS_ID,               /* 0x15        ����ID */
    AV_HDD_COND,			 /* 0x16		Ӳ��ж�ع���״̬���� */
    UI_REC_DISPLAY,			 /* 0x17  ��ѯ����¼��״̬ */
    UI_REC_QUREY,			/* 0x18  ��ѯ����¼��ʱ�� */
    UI_REC_RESP,				/* 0x19  ���ص���¼��ʱ�� */
    //AV_HEART_BEAT           0x0A   //
    AV_GET_DISK_STAT = 0x20, /* 0x20 Ӳ��״̬�Ļ�ȡ�ͽ��� */
    AV_OPEN_L_PLAY_BACK,     /* 0x21 ���ػطſ�ʼ */
    AV_CLOSE_L_PLAY_BACK,    /* 0x22 �رձ��ػط�*/
    AV_GET_VO_PARA,          /* 0x23 ��ȡvo����*/
    AV_SET_VO_PARA,          /* 0x24 ����vo����*/
    AV_GET_VI_PARA,          /* 0x25 ��ȡvi����*/
    AV_SET_VI_PARA,          /* 0x26 ����vi����*/

    AV_FORMAT_DISK,          /* 0x27 ��ʽ������*/
    AV_FORMAT_DISK_END,      /* 0x28 ��ʽ������*/
    AV_UNLOCK_DISK,          /* 0x29 ��Ӳ������*/

    AV_DEVERR_STAT,          /* 0x2a ����״̬*/
    AV_DEVERR_RSP,           /* 0x2b ���Ͻ��յĻ�Ӧ*/

    AV_START_BK,             /* 0x2c U�̵�¼��ʼ*/
    AV_CANCLE_BK,            /* 0x2d U�̵�¼��ȡ��*/
    AV_BK_STAT,              /* 0x2e U�̵�¼��״̬*/
    AV_HARDWARE_STAT_REQUEST ,// 0x2f Ӳ��״̬����
    AV_PLAY_CTRL = 0x31,     /*�طſ��� */
    AV_PLAY_CTRL_SEEK = 0x32,     /*�طſ��� */
    AV_DEC_CHN_SWITCH,
    AV_PLAYBACK_OK,
    AV_SET_CHN_VI_PARA,
    AV_LPB_AO_SWITCH = 0x40,  /* 0x40 LPB��ʾ���������*/
    AV_VOIP_CTRLAOAI = 0x41,   /* 0x41 VOIP��ʾ���������*/
#ifdef PLT_PARA_BK
    AV_PLT_ADDRESS  = 0x42,  /*��ǰAV���ӷ�������ַ*/
#endif
    AV_VOIP_DIALING = 0x43, //VOIP�Ƿ��������
    AV_FUNC_CONFIG   = 0x44, //AV����ʹ��
#ifdef JHY_SDK
    AV_JHY_PARA_SET = 0x50,          //���ò���
    AV_JHY_PARA_GET = 0x51,          //��ȡ����
    AV_JHY_PARA_LEARN_BK = 0x52,     //���ñ���ͼƬ
    AV_JHY_PARA_UPDATE_BIN = 0x53,     //���������BIN����
    AV_JHY_PARA_GET_CAM_ID = 0x54,     //��ȡ�����ID��
#endif

} UI_VIDEO_CMD_E;

typedef enum RetrieveRecordType_enum
{
    E_RETRIEVE_NORMAL_RECORD = 0x01,		/* ��ͨ¼�� */
    E_RETRIEVE_ALARM_RECORD                     /* ����¼�� */
} RetrieveRecordType;



#define ALARM_TRRIGER_BIT 0x0001
typedef enum SCH_CMD
{
    SCH_GET_OSD_DATA = 0x1,         /* 0x1 ��ȡ���ȵ�GPS���� */
    SCH_SET_PLT_INFO = 0x2,         /* 0x2 ���ȷ�����Ƶ��������Ϣ */
    SCH_RSP_PLT_INFO = 0x3,         /* 0x3 Ӧ���ȡ��Ƶ��������Ϣ */
    SCH_DEV_SVR_STAT  = 0x4,         /*�����ȷ��Լ�״̬*/
    SCH_ALARM_TRIGGER = 0x9,        /* 0x9 ���� ����AV��MCU����3ggateת��������*/
#ifdef JHY_SDK
    SCH_KL_STA_INFO  = 0xc6,        /*����->av ����վ��Ϣ*/
    SCH_KL_STA_TOTAL_NUM= 0xc7,     /*av->���� վ��������Ϣ*/
    SCH_KL_STA_RST_NUM= 0xc8,       /*����->av ��������*/
    SCH_KL_RTINFO_RA = 0xcd,       /*av->���� ʵʱ������Ϣ*/
#endif
} SCH_CMD_E;

typedef enum MCU_CMD
{
    MCU_SND_DISKLED_DATA = 0x51,         /* 0x51 ��ȡ���ȵ�GPS���� */

} MCU_CMD_E;

#define DISKTMP_MAX_THREHOLD			90
#define DISKTMP_MIN_THREHOLD			5
#define DISKTMP_HEAT_MIN_THREHOLD       7
#define DISKTMP_HEAT_MAX_THREHOLD       20

typedef enum BASIC_CMD
{
    BASIC_HARDWARE_STATE_REQ = 0x01,
    BASIC_HARDWARE_STATE_RSP = 0x02,         /* Ӳ��״̬ */
    BASIC_IO_SIG_NOTIFY = 0x03,
    BASIC_TEMP_ACK = 0x04,
    BASIC_GPIO_CMD  = 0x05,
} BASIC_CMD_E;
/* BEGIN: Added by mabuyun, 2015/9/2  DES:���IP�Խ�����  PN:20150902 */

typedef enum BZQ_CMD
{
    BZQ_SND_CTRL_VOIP = 0x04,         /* 0x04 IP�Խ��������� */
} BZQ_CMD_E;

typedef enum PARA_CMD
{
    AV_UPDATE_PARA = 0x11,         /* 0x11        ����ģ��ͬ��������*/
    PARA_CMD_GET_PARA_COM = 0x0F,   /*��ȡ����*/
} PARA_CMD_E;

typedef enum GATE_CMD
{
    GATE_MOUDLE_REG_CMD = 0x01, // ģ����Gate����ע���������
} GATE_CMD_E;
//GPIO�ӿ�
typedef enum GPIO_CMD
{
    GPIO_CMD_DISK_HEAT = 0x01,      //Ӳ�̼���
    GPIO_CMD_DISK_COOL_DOWN = 0x02,//Ӳ�̷��Ƚ���
} GPIO_CMD_E;
#define GPIO_ACTION_START 1
#define GPIO_ACTION_STOP  0

#define	BZQ_START_VOIP             0x01	    // ��IP�Խ�
#define	BZQ_STOP_VOIP              0x00	    // �ر�IP�Խ�

/* END:   Added by mabuyun, 2015/9/2   PN:20150902 */

/*
0x31��3ggate��ַ
0x32��AV��ַ �õ�ַ������
0x33�� UI
0x34����վ����ַ
0x35��phone
0x36��app
0x39����Ƭ��
0x40��������
*/
#define	ID_3GGATE			(0x31)
#define	ID_AV				(0x32)
#define	ID_UI				(0x33)
#define	ID_BZQ				(0x34)
#define	ID_PHONE			(0x35)
#define	ID_BASIC			(0x36)
#define ID_MONITOR			(0x37)
#define ID_PARAM			(0x38)
#define	ID_MCU				(0x39)
#define ID_SCH              (0x3a)
#define	ID_SRV				(0x40)
#define ID_LOCAL			(0xa1)
#define ID_INVALID			(0xff)

//#define AV_SET_REALTIME         0x0E    //����ʱ��-��tm8706��tm8716��ʹ��
//#define AV_GET_REC_CHN         	0x0F    //��ȡ¼��״̬
//#define AV_SET_SVR_ADDR         0x11    //����IP��ַ��˿�-�ظ�ʱ����0x11
//#define AV_ALARM_EVENT          0x12    //�����¼�֪ͨ

#define AV_LOCAL_DISK_STAT		0xe0	//��ȡӲ��״̬

#define AV_CLOSE_R_PREVIEW		(10008)	//�ر�Զ��Ԥ��
#define AV_OPEN_R_PREVIEW		(10007)	//��Զ��Ԥ��
#define AV_SEARCH_REC			(10170)   //����¼��
#define AV_CLOSE_R_PLAYBACK		(10172)   //�ر�Զ��¼��ط�
#define AV_OPEN_R_PLAYBACK		(10171)   //��Զ��¼��ط�
#define AV_OPEN_R_4PLAYBACK		(10178)   //��Զ��4ͨ��¼��ط�



//�����ݰ�ʱ��д��������
#define	R_TAKE_PHOTO            0xA1	//�ظ�����
#define	R_SET_IP                0xA2	//�ظ�����ip��ַ��˿�
#define	R_SET_SUB_FRAM     		0xA3	//�ظ���������������
#define	R_REFER_NETSTAT         0xA4	//�ظ���ѯ�����������״̬
#define	R_REFER_IP              0xA5	//�ظ���ѯip��ַ��˿�
#define	R_REFER_SUB_FRAM        0xA6	//�ظ���ѯ����������
#define R_OPEN_VOIP             0xA7   //�ظ������Խ�
#define R_CLOSE_VOIP            0xA8   //�ظ��رնԽ�
#define R_SCREEN_SWITCH         0xA9   //�ظ������л�
#define R_HEART_CMD             0xAA   //�ظ���3g���ص�����ά������
#define R_OPEN_RECORD			0xAB	//�ظ���¼��
#define R_CLOSE_RECORD          0xAC   //�ظ��ر�¼��
#define R_CTRL_SEND_VIDOE       0xAD   //�ظ�������Ƶ����

//�ظ����ݰ���ʵ�ʳ���
#define	R_TAKE_PHOTO_LEN			10	//�ظ��������ݰ��ĳ���
#define	R_SET_IP_LEN				10	//�ظ�����ip��ַ��˿����ݰ�����
#define	R_SET_SUB_FRAM_LEN		    10	//�ظ������������������ݰ�����
#define	R_REFER_NETSTAT_LEN		    11	//�ظ���ѯ�����������״̬���ݰ�����
#define	R_REFER_IP_LEN				53	//�ظ���ѯip��ַ��˿����ݰ�����
#define	R_REFER_SUB_FRAM_LEN		16	//�ظ���ѯ�������������ݰ�����
#define R_OPEN_VOIP_LEN             10  //�ظ������Խ�����
#define R_CLOSE_VOIP_LEN            10  //�ظ������Խ�����
#define   MIN_LEN                   12  //��С�����ݳ���


//Э��������ʶ
#define UI_PIC_FLAG		    	0x0f	//ͼƬ�ϴ�����ui�����ϴ�����
#define UI_UPDATE 		    	0x10   //U��������Ϣ���ڴ˳�����û���õ�����u�������������õ�
#define UI_BACK_DOOL_FLAG   	0x11	//���ſ����ź�
#define UI_LOGIN_FLAG			0x12   //ע���־��av_camera����Ƶ������ͨѶ״̬��������
#define UI_AV_VOIP  			0x13   //IP�Խ���Э���־
//#define UI_AV_VER				0x14   //��Ƶ����汾�ţ� ������ʱ����һ��

#define TAKE_PHOTO_OK       	0 /*���ճɹ�*/
#define TAKE_PHOTO_ERROR    	1 /*����ʧ��*/

typedef struct HW_TEMP_ABNORMAL
{
    int iLowTempCount;  //�¶ȹ��ͼ���
    int iHighTempCount; //�¶ȹ��߼���
    int iEnableAssitDownTmep;   //�Ƿ����÷��Ƚ���
    int iCoolDownTemp;  //�������Ƚ��¾�����
    int iGpioCoolDownDisk;  //�Ƿ��Ѿ��������Ƚ���
    int iGpioHeatDisk;  //�Ƿ�����Ӳ�̼���
} HW_TEMP_ABNORMAL_T;

/*********************************************************************************/
//��������
typedef struct AV_FUNC_CFG_ST
{
    HI_CHAR cEnableKl;      //ʹ�ܿ������� 0:δ���� 1:����
    HI_CHAR cVideoInCount;  //��Ƶ�������
    HI_CHAR cReserved[64];
} __attribute__((packed))AV_FUNC_CFG_T;

typedef struct tagTGPIOCmd
{
    char cFunc;//1:Ӳ�̼��� 2:�������Ƚ���
    char cCmd; //1:��ʼ���� 0:ֹͣ����
} __attribute__((packed))TGPIOCmd;
//��չosd
typedef struct EXT_OSD_ST
{
    HI_S32 iLatitude;              /* γ�� */
    HI_S32 iLongitude;             /* ���� */
    HI_U8  cLineNum[8];            /* ��·�� */
    HI_U8  cSpeed[8];              /* �ٶ�ֵ */
    HI_U8  cReserved[16];          /* Ԥ�� */
    HI_S32 iFlag;                  /*
                                    PRE_Y_CODE_Y = 1, Ԥ����ʾosd��������ʾosd
                                    PRE_Y_CODE_N ,   Ԥ����ʾosd�����벻��ʾosd
                                    PRE_N_CODE_Y ,   Ԥ������ʾosd��������ʾosd
                                    PRE_N_CODE_N ,   Ԥ������ʾosd�����벻��ʾosd
                                    */
} __attribute__((packed))EXT_OSD_T;
//osd
typedef struct OSD_ST
{
    HI_U8  cBusId[32];             /* ������� */
    HI_U8  cReserved[16];          /* Ԥ�� */
} __attribute__((packed))OSD_T;

typedef struct PLAYBACK_ST
{
    HI_S32 s32ChnSel;		/*0~32 BIT�ֱ��ʾ1~32ͨ��*/
    HI_U32 sTime;			/*��ʼʱ��*/
    HI_U32 eTime;			/*����ʱ��*/
} __attribute__((packed))PLAYBACK_T;

typedef struct PLAY_CTRL_T
{
    HI_S32 s32playstat;
} __attribute__((packed))PLAY_CTRL_ST;

typedef struct REC_STAT_ST
{
    HI_S32 s32Stat;			/*¼��״̬*/
} __attribute__((packed))REC_STAT_T;

typedef struct ALARM_STAT_ST
{
    HI_U8 alarmStat;			/*MCU�����ı���״̬ 0000 0001 bit��ʾ��bit0��ʾ���� */
} __attribute__((packed))ALARM_STAT_T;

typedef struct DISK_STAT_ST
{
    HI_S32 s32Stat;			/*Ӳ��״̬*/
} __attribute__((packed))DISK_STAT_T;


typedef struct ServerAddr_st
{
    char  achIpAddr[16];
    HI_U16 sPort;
} __attribute__ ((__packed__))ServerAddr;

typedef struct DISK_LED_STAT_ST
{
    HI_S32 s32diskled;			/*Ӳ��led*/
} __attribute__((packed))DISK_LED_STAT_T;

typedef struct SVR_REG_STAT_ST
{
    HI_S32 s32Stat;			/*��Ƶƽ̨ע��״̬*/
} __attribute__((packed))SVR_REG_STAT_T;

/* BEGIN: Added by mabuyun, 2015/9/7  DES:���IP�Խ�����  PN:20150902 */
typedef struct VOIP_STAT_ST
{
    HI_S32 s32Cmd;			/* 1 ��ʼ��0�ر� */
    char  limittime;            /* ����ʱ�� */
} __attribute__((packed))VOIP_STAT_T;
/* END:   Added by mabuyun, 2015/9/7   PN:20150902 */


typedef struct SNAPSHOT_ST
{
    union
    {
        HI_S32 s32Chn;		/*ץ��ͨ��*/
        HI_S32 s32Ret;		/*ץ�Ľ��*/
    };
} __attribute__((packed))SNAPSHOT_T;

typedef struct REC_MONTH_ST
{
    HI_U32 iStartTime;
    HI_U32 iEndTime;
    HI_U8 recinfo[31];
} __attribute__((packed))REC_MONTH_T;

typedef struct RetrieveResTimeQuantum_st
{
    HI_U32 uiStartTime;
    HI_U32 uiEndTime;
} __attribute__ ((__packed__))RetrieveResTimeQuantum;

typedef struct REC_ITEM_ST
{
    HI_U8   type;
    RetrieveResTimeQuantum rsp_time;
} __attribute__((packed))REC_ITEM_T;


typedef struct REC_QUREY_REQ_ST
{
    RetrieveResTimeQuantum req_time;
} __attribute__((packed))REC_QUREY_REQ_T;

typedef struct DEC_CHN_SWITCH_ST
{
    HI_U8 chn;
} __attribute__((packed))DEC_CHN_SWITCH_T;

typedef struct REC_QUREY_RSP_ST
{
    HI_U8   chn;
    HI_U8 sum;
    REC_ITEM_T rec_items[48];
} __attribute__((packed))REC_QUREY_RSP_T;

typedef struct PREVIEW_ST
{
    HI_U8 u8type;			/*Ԥ������,������,�ķ���,�ŷ���*/
} __attribute__((packed))PREVIEW_T;

typedef struct LOCAL_BK_ST
{
    time_t sTime;			/*��ʼʱ��*/
    time_t eTime;			/*����ʱ��*/
    HI_S32 s32Chn[12];
    HI_S32 s32ChnCnt;
} __attribute__((packed))LOCAL_BK_T;

typedef struct LBK_STAT_T
{
    HI_S32 s32BkChn;        /*����ͨ�� */
    HI_S32 s32BkStat;       /*����״̬ */
} __attribute__((packed))LBK_STAT_ST;

#ifdef OSD_SHOW
typedef struct OSD_SHOW_DATA_T
{
    HI_U8 busId[3];
    HI_U8 lineId[4];
    HI_U16   speed;
    HI_U32  latitude;/* γ�� */
    HI_U32  longitude;/* ���� */

    HI_U8 ucGpsStatus;		/* ��λ״̬ */
    HI_U8 ucStationNum;		/* վ�������м����� */
    HI_U8 ucOperate;		/* ��Ӫ״̬ */
    HI_U8 ucRegStatus;		/* ע��״̬ */
    HI_U8 ucLimitSpeed;		/* ����ֵ */
    HI_U8 bussignal;		/* �����ź� */
    HI_U8 ucPlateNum[9];    /* ���ƺ� */
    HI_U8 ucInOrOut;        /* ����վ */
	HI_U8 ucStnDir;			/* վ�㷽��,0:����1:����*/
	HI_U8 ucIsInPark;		/* �Ƿ��ڳ���,0:���ڳ���1:�ڳ���*/
	HI_U8 ucParkDir;		/* ��������,0:����1:����*/
	HI_U8 ucParkCode;		/* �������*/
	HI_U8 ucUpTotalStnNum;	/*����վ������*/
	HI_U8 ucDownTotalStnNum;/*����վ������*/
} __attribute__((packed))OSD_SHOW_DATA_ST;
#endif

typedef struct OSD_UI_SHOW
{
    HI_U8 reason;        /* ���ػ�ж��Ӳ��ԭ�� */
    HI_U8 PowerStatus;	 /* ����Դ״̬ */
    HI_U8 PowerVal; 	 /* ����Դ��ѹֵ */
    HI_U8 LockStatus;
    HI_U8 CapStatus;
    HI_U8 CapVal;
    HI_U8 TemStatus;
    HI_S8 TemVal;
    HI_U8 telesig;
} __attribute__((packed))OSD_UI_SHOW_ST;


typedef struct Av_VideoPltPara_Info
{
    char ip[15];                     //IP��ַB
    char port[2];                 //�˿�B�������ļ��У��˿ڱ��水��ˣ����Բ���ת��(���������ô��)
    char machineId[3];                      //���ػ����,fan zhuan
#ifdef PLT_PARA_BK
    char ipbk[15];                 //IP��ַB
    char portbk[2];                //�˿�B�������ļ��У��˿ڱ��水��ˣ����Բ���ת��(���������ô��)
#endif
    unsigned char cCompany;         //��˾����
    unsigned char cSubCompany;      //�ӹ�˾����
} __attribute__((packed))AV_VIDEOPLTPARA_INFO_ST;


/*����״̬�ϴ� */
/************************************************

 bit31  bit30  bit29  bit28  bit27  bit26  bit25  bit24

 bit23  bit22  bit21  bit20  bit19   bit18   bit17   bit16
 ¼��8  ¼��7  ¼��6  ¼��5  ����ͷ8 ����ͷ7 ����ͷ6 ����ͷ5
 bit15  bit14  bit13  bit12  bit11  bit10  bit09  bit08
                                    Ӳ��   MP3�ļ� SD��
 bit07  bit06  bit05  bit04  bit03   bit02   bit01   bit00
 ¼��4  ¼��3  ¼��2  ¼��1  ����ͷ4 ����ͷ3 ����ͷ2 ����ͷ1
 ************************************************/
typedef struct DEVERROR_STAT_ST
{
    HI_S32 s32DevStat;			/*����״̬ */
} __attribute__((packed))DEVERROR_STAT_T;

typedef struct LPB_SWITCH_AO_ST
{
    HI_CHAR cSwitchAo;		/*AO����*/
} __attribute__((packed))LPB_SWITCH_AO_T;

typedef struct DOMAIN_PACKET_HEAD_ST
{
    HI_U8 pkt_label[0x04];		//����ʼ��ʶ
    HI_U8 pkt_len_h; 			//�������ܳ��ȸ��ֽ�
    HI_U8 pkt_len_l; 			//�������ܳ��ȵ��ֽ�
    HI_U8 dst_id;				//��Ŀ���ַ
    HI_U8 src_id;				//��Դ��ַ
    HI_U8 pkt_seq;				//�����
    HI_U8 pkt_cmd;				//��������
} __attribute__((packed))DOMAIN_PACKET_HEAD_T;


#define HW_DISK_LOCK_STAT_BIT   0X01
#define HW_SUPER_CAP_STAT_BIT   0X02
#define HW_MAIN_POWER_STAT_BIT  0X04
#define HW_ACC_STAT_BIT         0X08

#define HW_TEMP_ABNORMAL_TIME   10
#define HW_TEMP_ABNORMAL_VALUE  -127
typedef enum
{
    STAT_ONOFF_UNKNOWN = 0X00,    //δ֪״̬
    STAT_ONOFF_ON,               //����״̬
    STAT_ONOFF_OFF               //�ػ�״̬
} TurnOnOffDevStat_e;
typedef enum
{
    PW_STAT_NORMAL=0,/*��������Դ������*/
    PW_STAT_MAIN_OK,     /*����Դ��������ԴǷѹ*/
    PW_STAT_BAT_OK,   /*����Դ���磬���õ�Դ����*/
    PW_STAT_MAIN_AND_BAT_ERR   /*����Դ���磬��ԴǷѹ*/
} PwStatType_e;

typedef struct AV_HARD_STA_ST
{
    HI_U8       otherHwStat;        /* Ӳ�����״̬���ο�HW_DISK_STAT_BIT */
    HI_U8       ucPowerStat;
    HI_U8       powerstatus;       /*���ػ�״̬*/
    HI_CHAR     cIoSingle;          //��״̬
} __attribute__ ((__packed__))AV_HARD_STA;
typedef struct AV_HARD_TEMP_ST
{
    HI_S8     diskTemp;           /* Ӳ���¶� */
    HI_S8     MainBoardTemp;
} __attribute__ ((__packed__))AV_TEMP_STA;
typedef struct AV_CAMERA_STA_ST
{
    HI_U16  cameraStat;         /* ����ͷ״̬ */
    HI_U16  recordVideoStat;    /* ¼��״̬ */
} __attribute__ ((__packed__))AV_CAMERA_STA;
typedef struct
{
    unsigned char ucParaId_h;  //������ŵĸ��ֽ�
    unsigned char ucParaId_l;  //������ŵĵ��ֽ�
} ParaIdStruct;
#define PARA_MAX_LEN    1024 //������������󳤶�
typedef struct
{
    unsigned char ucParaId_h;  //������ŵĸ��ֽ�
    unsigned char ucParaId_l;  //������ŵĵ��ֽ�
    unsigned char ucParalen_h;  //������ŵĸ��ֽ�
    unsigned char ucParalen_l;  //������ŵĵ��ֽ�
} ParaHeadStruct;

#define MAX_DEVINFO_COUNT 100
typedef struct TM_APP_DEVSTATINFO
{
    int iDevStat;
    time_t iTime;
} TM_APP_DEVSTATINFO_S;
typedef struct TM_APP_DEVSTAT
{
    pthread_rwlock_t m_Mutex;
    int iWritePos;
    int iReadPos;
    int iCount;
    TM_APP_DEVSTATINFO_S stDevInfo[MAX_DEVINFO_COUNT];
} TM_APP_DEVSTAT_S;
#ifdef  PLT_PARA_BK
typedef struct VIDEO_PLAT_SIP_ST
{
    char ip[16];                  //IP��ַ
    char port[2];                 //�˿�B�������ļ��У��˿ڱ��水��ˣ����Բ���ת��(���������ô��)
} __attribute__((packed))VIDEO_PLAT_SIP_T;
#endif
typedef struct DOMAIN_CMD_ST
{
    DOMAIN_PACKET_HEAD_T head;	//��ͷ
    union
    {
        HI_S32      s32Ret;						/*ִ�н��*/
        PREVIEW_T   stPreview;					/*Ԥ��*/
        SNAPSHOT_T  stSnapshot;					/*ץ��*/
        REC_MONTH_ST stRecinfo;
        REC_QUREY_REQ_T stRecreq;
        REC_QUREY_RSP_T stRecrsp;
        DEC_CHN_SWITCH_T stDecchn;
        DISK_STAT_T stDiskStat;					/*Ӳ��״̬(����/ж��)*/
        PLAY_CTRL_ST stPlayctrl;
        DEVERROR_STAT_T s32DevErrStat;           /*dev����״̬ */
        DISK_LED_STAT_T s32diskled;
        REC_STAT_T	stRecStat;					/*¼��״̬(����/�ر�)*/
        ALARM_STAT_T stAlarmStat;               /*alarm stat*/
        PLAYBACK_ST stPlayBack;					/*�ط�*/
        LOCAL_BK_T  stLocalBk;                  /*��������*/
        LBK_STAT_ST stLBKStat;                  /*����״̬*/
        AV_VERSION_T stVersion;                 /*��Ƶ����汾*/
        VO_PARA_T   stVideoOutPara;             /*��Ƶ�������*/
        SVR_REG_STAT_T stSvrRegStat;            /*��Ƶ������ע��״̬*/
        VI_PARA_T   stVideoInPara;              /*��Ƶ�������*/
        VI_PARA_CHN_T stVideoInChnPara;
        EXT_OSD_T   stExtOsd;                   /*��չosd���ýṹ*/
        OSD_T       stOsd;                      /*osd ���ýṹ��*/
        OSD_SHOW_DATA_ST stOsdData;
        LPB_SWITCH_AO_T	stLPBSwitchAo;			/*LPB ao �򿪻�ر�*/

        /* BEGIN: Added by mabuyun, 2015/9/8  DES:���IP�Խ�����  PN:20150902 */
        SVR_REG_STAT_T stLoginstat;
        VOIP_STAT_T stVoipCtrl;
        AV_VIDEOPLTPARA_INFO_ST stPlatinfo;
        /* END:   Added by mabuyun, 2015/9/8   PN:20150902 */
        AV_HARD_STA stDeviceState;
        AV_TEMP_STA stHardWareTemp;
        AV_CAMERA_STA stCameraState;
#ifdef  PLT_PARA_BK
        VIDEO_PLAT_SIP_ST stPlatSipInfo;
#endif
        HI_U8 buff[DOMAIN_BUF_LEN]; //������
    };
} __attribute__((packed))DOMAIN_PACKET_T;

typedef enum SOCKET_TYPE_EN
{
    SOCKET_LISTEN_E = 0,
    SOCKET_CONNECT_E = 1,
    SOCKET_INVALID_E = -1
} SOCKET_TYPE_E;

typedef HI_S32 (* MSG_EVENT_PF)(HI_S32, HI_VOID *, HI_S32);
typedef HI_S32 (* MSG_ACTION_PF)(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
typedef HI_S32 (* MSG_PF)(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);

typedef struct MSG_EVENT_MAP_ST
{
    HI_S32 s32MsgId;			/*��ϢԴID*/
    MSG_EVENT_PF eventFunc; 	/*��Ϣ����*/
} __attribute__((packed))MSG_EVENT_MAP_T;

typedef struct MSG_ACTION_MAP_ST
{
    HI_S32 s32CmdId;			/*��Ϣ����*/
    MSG_ACTION_PF actionFunc;	/*����������*/
} __attribute__((packed))MSG_ACTION_MAP_T;

typedef struct MSG_MAP_ST
{
    HI_S32 s32MsgId;			/*��ϢԴID*/
    HI_S32 s32CmdId;			/*��Ϣ����*/
    MSG_PF pMsgFunc;			/*����������*/
} __attribute__((packed))MSG_MAP_T;
#ifdef JHY_SDK
#define RET_LEARN_BK_FAILURE    -1
#define RET_LEARN_BK_OPEN_FRONT 1
#define RET_LEARN_BK_SUCCESS    0
#define RET_LEARN_BK_OPEN_BACK  2
#define RET_LEARN_BK_INDEX      3

/*����վ ����->������ �����ݽṹ��*/
#define  EE_BUS_ID_LEN	0x03  //������ų���
typedef struct	SCH_KLSTN_DATA_ST//C6 >����������
{
    HI_U8 cmd;	//0xC6
    HI_U8 u8Reserved[2];
    HI_U8 bagid;
    HI_U8 time[6];
    HI_U8 direct;
    HI_U8 sta_num;
    HI_U8 line_no[5];
    HI_U8 bus_id[EE_BUS_ID_LEN];
    HI_U8 in_out_flag;
    //bit 7: 0-����վ, 1-��վ.bit 6: 0-��ĩվ, 1-ĩվ
    //bit 5,4 reserved;//bit 3: 0-gpsδ��λ, 1-gps�Ѷ�λ.bit 2: 0-�ǹ��Ŵ���, 1-���Ŵ���.
    //bit 1,0: 0x01-��վ, 0x02-��վ, 0x03-ģ���վ
    HI_U16 usCrcReserved;
} __attribute__((packed))SCH_KLSTN_DATA_T;

/*������->���� �Ŀ������ݽṹ��*/
typedef struct	SCH_KLSTN_DATA_REQ_ST//C7 <
{
    HI_U8 cmd;	//0xC6
    HI_U8 u8Reserved[2];
    HI_U8 bagid;
    HI_U8 kl_sta;	//���Ҳû�õ�,���Բ���
    HI_U8 time[6];
    HI_U8 direct;
    HI_U8 sta_num;	//վ����
    HI_U8 up_num;
    HI_U8 down_num;
    HI_U8 in_out_flag;
    HI_U8 ucTotalNum;	//��������³�������,Ҳû�õ�,���Բ���
    HI_U16 usCrcReserved;
} __attribute__((packed))SCH_KLSTN_DATA_REQ_T;

/*����->����  ʵʱ��ϸ�Ŀ�������*/
typedef struct SCH_KLSTN_REAL_DATA_ST
{
    HI_U8 cmd;	//0xC6
    HI_U8 u8Reserved[3];
    HI_U8 ucFrontDoorStatus;	//����û�õ�
    HI_U8 ucFrontDoorCome;
    HI_U8 ucFrontDoorGo;
    HI_U8 ucBackDoorStatus;		//����û�õ�
    HI_U8 ucBackDoorCome;
    HI_U8 ucBackDoorGo;
    HI_U16 usCrcReserved;
} __attribute__((packed))SCH_KLSTN_REAL_DATA_T;

typedef struct CAMERA_PARA_ST
{
    int door;             //0:ǰ��  1:����
    int camerach;         //ͨ���� �����л���Ƶʹ��
    char cameraid[8];     //��ǰ��������
    int installH;			//�߶ȹ�����ʱδ��				��Χ0				Ĭ��:0
    int noiseFilter;		//��������							��Χ120~200		Ĭ��:120
    int leftLimit;			//��߽�								��Χ0~100			Ĭ��:0
    int rightLimit;			//�ұ߽�								��Χ260~360		Ĭ��:360
    int topLimit;			//�ϱ߽�								��Χ0~30			Ĭ��:0
    int bottomLimit;		//�±߽�								��Χ258~288		Ĭ��:288
    int enableC;			//ʹ��ͨ��							��Χ0~1			Ĭ��:1
    int inoutType;			//0Ϊ�Ͻ��³� 1Ϊ�½��ϳ�		��Χ0~1			Ĭ��:0
    int doorDelay;			//�������ӳ�ͳ��					��Χ0~5			Ĭ��:3
    int filterHeight;		//�߶ȹ���							��Χ0~3			Ĭ��:0
    int counterType;		//��������							��Χ8��12��14		Ĭ��:8
    char sensitivity[4];//float sensitivity;		//������								��Χ10				Ĭ��:10
    int beeline[2];			//������								��Χ100~260		Ĭ��:0:106 1:174
    int headFilter;			//��ͷ��С����						��Χ80~150			Ĭ��:80
    int forgTh;				//ǰ����ֵ							��Χ15~20			Ĭ��:15
} __attribute__((packed))CAMERA_PARA_T;


typedef struct UPDATE_CAMERA_BIN_ST
{
    int door;             //0:ǰ��  1:����
    char cameraid[8];
} __attribute__((packed))UPDATE_CAMERA_BIN_T;


typedef struct CAMERA_ID_ST
{
    char cError;//0:�����������(û�к����ֶ�)  1:�ɹ�
    char cameraid[8];
} __attribute__((packed))CAMERA_ID_T;
#endif


extern HI_S32 DOMAIN_RebootSystem(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_QueryDeviceState(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_VOID DOMAIN_InitCycleBuf(HI_VOID);
extern HI_VOID DOMAIN_PushData2CycleBuf(HI_CHAR *pBuf, HI_S32 s32BufLen);
extern HI_VOID DOMAIN_SetCycleBuf(DataBuf *pCycleBuf);
extern HI_VOID DOMAIN_GetCycleBuf(DataBuf **pCycleBuf);
extern HI_VOID DOMAIN_SetSocket(HI_S32 socketFd);
extern HI_VOID DOMAIN_GetSocket(HI_S32 *pSocketFd);
extern HI_S32 DOMAIN_StartRcvDataThd(HI_VOID);
extern HI_S32 DOMAIN_StartSndDataThd(HI_VOID);
extern HI_S32 DOMAIN_SndRequstToBasic(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_RcvData(HI_S32 *pSocketFd, DataBuf *CycleBuf, HI_CHAR *ReadBuf, HI_S32 ReadLen);
extern HI_S32 DOMAIN_SndData(HI_S32 socketFd, HI_S32 pktDstId, HI_U8 pktCmd, HI_U8 *dataBuf, HI_S16 dataLen);
extern HI_S32 DOMAIN_PackageData(HI_S32 pktDstId, HI_U8 pktCmd, HI_U8 *dataBuf, HI_S16 dataLen);
extern HI_S32 DOMAIN_RecTimeResp(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_SndBZQCtrlVOIP(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_SndUIStartLBK(HI_VOID * pRcvPkt, HI_VOID * pSndPkt, HI_S32 * ps32SndLen);
extern HI_S32 DOMAIN_StartLBK(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_SndUiBkStat(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_StopLBK(HI_VOID * pRcvPkt, HI_VOID * pSndPkt, HI_S32 * ps32SndLen);
extern HI_S32 DOMAIN_Snapshot(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_Preview(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_RecDisplay(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_RecQuery(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern void HI3521_LoaclRecQuery(RetrieveResTimeQuantum req_time);
extern HI_S32 DOMAIN_SetVoPara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_GetVoPara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_GetViPara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_SetViPara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_SetChnViPara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_StartLPB(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_StopLPB(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_localPlayCtrlStat(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_localPlayCtrl_Seek(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_DecChnSwitch(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_SwitchLPBAo(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_LPBOK(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_GetCommonPara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_UpdatePara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_SndRequstToPara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_RecvRegAckFromGate(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
#ifdef JHY_SDK
extern HI_S32 DOMAIN_SndKlRealTimeInfoToSch(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_HandleIoSingle(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_SndKeliuInfoToSch(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_RcvSchStaInfo(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_RetsetKlNum(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_LearnBKGround(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_UpdateCameraPara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_SetCameraPara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_GetCameraPara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_GetCameraId(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
#endif
extern HI_S32 DOMAIN_GetFuncConfig(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_HandleHardWareSta(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_DiskStat(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_LocalDiskStat(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_FormatDisk(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_FormatDiskEnd(HI_VOID * pRcvPkt, HI_VOID * pSndPkt, HI_S32 * ps32SndLen);
extern HI_S32 DOMAIN_GetOsdData(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_RcvSvrDevStatRsp(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_RcvUiDevStatRsp(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_SendDevErrStat(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_SendDevSvrStat(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
#ifdef PLT_PARA_BK
extern HI_S32 DOMAIN_RcvUIQueryPlatInfo(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_GetPlatformInfo(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_SndUIPlatInfo(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
#endif
extern HI_S32 DOMAIN_RcvSchPlatInfoRsp(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_SndSchPlatInfoReq(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
HI_S32 DOMAIN_RcvUiLoginRsp(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
HI_S32 DOMAIN_SndSVRLoginStat(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
HI_S32 DOMAIN_SndUIOpenVOIP(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
HI_S32 DOMAIN_SndUIStopVOIP(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
HI_S32 DOMAIN_RcvUIStartDialing(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
HI_S32 DOMAIN_RcvUIStopVOIP(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
HI_S32 DOMAIN_RcvUIStartVOIP(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
HI_S32 DOMAIN_SndGpioReqToBasic(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
HI_S32 DOMAIN_RecvHardWareTemp(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_SetUiOsd(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
#if 0
extern HI_S32 DOMAIN_OpenRec(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_CloseRec(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_DiskKeyOpen(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);

extern HI_S32 DOMAIN_DiskStat(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_LocalDiskStat(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_SetRecChn(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);


extern HI_S32 DOMAIN_UnlockKey(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_CloseVoip(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_GetSubPara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_SetSubPara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_GetNetStat(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_ExtOsdSet(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_GetAvVersion(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_OsdSet(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);

extern HI_S32 DOMAIN_SwitchLPBAo(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_SendDevErrStat(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_RcvUiDevStatRsp(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_SndUIStartLBK(HI_VOID * pRcvPkt, HI_VOID * pSndPkt, HI_S32 * ps32SndLen);
extern HI_S32 DOMAIN_StartLBK(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_SndUiBkStat(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_StopLBK(HI_VOID * pRcvPkt, HI_VOID * pSndPkt, HI_S32 * ps32SndLen);
extern HI_S32 DOMAIN_McuAlarmTrigger(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_SndMcuAlarmRsp(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_SndUiLPBCtrlStat(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
extern HI_S32 DOMAIN_SndMcuDiskLed(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);

extern HI_S32 DOMAIN_SndSchPlatInfoReq(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);

/* BEGIN: Added by mabuyun, 2015/9/2  DES:���IP�Խ�����  PN:20150902 */
HI_S32 DOMAIN_SndBZQCtrlVOIP(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
HI_S32 DOMAIN_RcvBZQCtrlVOIP(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);

HI_S32 DOMAIN_RcvUiLoginRsp(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);




/* END:   Added by mabuyun, 2015/9/2   PN:20150902 */
#endif
#endif
