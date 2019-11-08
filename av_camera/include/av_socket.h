#ifndef __TM_AV_SOCKET_H_
#define __TM_AV_SOCKET_H_

#include "data_buf.h"

#define Recv_Error "APP"
#define DOMAIN_TIMEOUT		(1)		//一秒
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
    PARAID_DEFAULT_CHN = 0x0B,        //默认通道
    PARAID_RECORD_CHN_CFG = 0x0D,        //录像状态
    PARAID_PHOTO_CHN = 0x0E,        //抓拍通道
    PARAID_IP_MIC_POWER = 0x12,    /* IP通话麦克音量*/
    PARAID_LUMINANCE = 0x19,	/* 亮度*/
    PARAID_CONTRAST = 0x1A,	/* 对比度*/
    PARAID_HUE = 0x1B,    /* 色调*/
    PARAID_SATURATION = 0x1C,    /* 饱和度*/
    PARAID_ACC_ENABLE = 0x24,        //是否启用ACC信号控制硬盘挂载
    PARAID_COOL_DOWN_ENABLE = 0x30,        //是否启用辅助降温
    PARAID_COOL_DOWN_TEMP = 0x31,        //辅助降温启动温度值
} PARA_ID_E;


#define BUS_FRONT_DOOR_OPEN       (0x01)  /*BIT0:  1.前门开门 0.无意义*/
#define BUS_FRONT_DOOR_CLOSE      (0x02)  /*BIT1:  1.前门关门 0.无意义*/
#define BUS_BACK_DOOR_OPEN        (0x04)  /*BIT2:  1.后门开门 0.无意义*/
#define BUS_BACK_DOOR_CLOSE       (0x08)  /*BIT3:  1.后门关门 0.无意义*/

typedef enum Ui_Video_Cmd
{
    AV_TAKE_PHOTO = 1,
    AV_SET_IP,               /* 0x02        设置ip地址与端口 */
    AV_SET_SUB_PARA,         /* 0x03        设置子码流参数 */
    AV_GET_NET_STAT,         /* 0x04        查询与服务器连接状态 */
    AV_REFER_IP,             /* 0x05        查询ip地址与端口 */
    AV_GET_SUB_PARA,         /* 0x06        查询子码流参数 */
    AV_OPEN_VOIP,            /* 0x07        开启对讲 */
    AV_CLOSE_VOIP,           /* 0x08        关闭对讲 */
    AV_SWITCH_SCREEN,        /* 0x09        界面切换 */
    AV_UMOUNT_DISK,          /* 0x0A        卸载硬盘，不再将缓存数据写入硬盘 */
    AV_OPEN_REC,             /* 0x0B        打开录像 */
    AV_CLOSE_REC,            /* 0x0C        关闭录像，将缓存数据写入硬盘后，卸载硬盘 */
    AV_ALARM_EVENT,          /* 0x0D        报警事件通知 */
    AV_SET_REC_CHN,          /* 0x0E        设置录像 */
    UI_UPLOAD_PIC,           /* 0x0F        图片上传信息 */
    AV_HEART_BEAT,           /* 0x10        与monitor的连接维持命令--后续加*/
    UI_AV_LOGIN,  		     /* 0x11        视频程序注册后台视频服务器标志 */
    UI_AV_VOIP,              /* 0x12        IP对讲的协议标志*/
    UI_AV_VER,               /* 0x13        视频程序版本号， 开机的时候发生一次 */
    AV_OSD_SET,              /* 0x14        视频OSD信息设置 */
    AV_BUS_ID,               /* 0x15        车辆ID */
    AV_HDD_COND,			 /* 0x16		硬盘卸载挂载状态条件 */
    UI_REC_DISPLAY,			 /* 0x17  查询当月录像状态 */
    UI_REC_QUREY,			/* 0x18  查询当日录像时间 */
    UI_REC_RESP,				/* 0x19  返回当日录像时间 */
    //AV_HEART_BEAT           0x0A   //
    AV_GET_DISK_STAT = 0x20, /* 0x20 硬盘状态的获取和接收 */
    AV_OPEN_L_PLAY_BACK,     /* 0x21 本地回放开始 */
    AV_CLOSE_L_PLAY_BACK,    /* 0x22 关闭本地回放*/
    AV_GET_VO_PARA,          /* 0x23 获取vo参数*/
    AV_SET_VO_PARA,          /* 0x24 设置vo参数*/
    AV_GET_VI_PARA,          /* 0x25 获取vi参数*/
    AV_SET_VI_PARA,          /* 0x26 设置vi参数*/

    AV_FORMAT_DISK,          /* 0x27 格式化磁盘*/
    AV_FORMAT_DISK_END,      /* 0x28 格式化磁盘*/
    AV_UNLOCK_DISK,          /* 0x29 开硬盘锁锁*/

    AV_DEVERR_STAT,          /* 0x2a 故障状态*/
    AV_DEVERR_RSP,           /* 0x2b 故障接收的回应*/

    AV_START_BK,             /* 0x2c U盘导录像开始*/
    AV_CANCLE_BK,            /* 0x2d U盘导录像取消*/
    AV_BK_STAT,              /* 0x2e U盘导录像状态*/
    AV_HARDWARE_STAT_REQUEST ,// 0x2f 硬件状态请求
    AV_PLAY_CTRL = 0x31,     /*回放控制 */
    AV_PLAY_CTRL_SEEK = 0x32,     /*回放控制 */
    AV_DEC_CHN_SWITCH,
    AV_PLAYBACK_OK,
    AV_SET_CHN_VI_PARA,
    AV_LPB_AO_SWITCH = 0x40,  /* 0x40 LPB提示音输出开关*/
    AV_VOIP_CTRLAOAI = 0x41,   /* 0x41 VOIP提示音输出开关*/
#ifdef PLT_PARA_BK
    AV_PLT_ADDRESS  = 0x42,  /*当前AV连接服务器地址*/
#endif
    AV_VOIP_DIALING = 0x43, //VOIP是否可以振铃
    AV_FUNC_CONFIG   = 0x44, //AV功能使能
#ifdef JHY_SDK
    AV_JHY_PARA_SET = 0x50,          //设置参数
    AV_JHY_PARA_GET = 0x51,          //获取参数
    AV_JHY_PARA_LEARN_BK = 0x52,     //设置背景图片
    AV_JHY_PARA_UPDATE_BIN = 0x53,     //更新摄像机BIN参数
    AV_JHY_PARA_GET_CAM_ID = 0x54,     //获取摄像机ID号
#endif

} UI_VIDEO_CMD_E;

typedef enum RetrieveRecordType_enum
{
    E_RETRIEVE_NORMAL_RECORD = 0x01,		/* 普通录像 */
    E_RETRIEVE_ALARM_RECORD                     /* 报警录像 */
} RetrieveRecordType;



#define ALARM_TRRIGER_BIT 0x0001
typedef enum SCH_CMD
{
    SCH_GET_OSD_DATA = 0x1,         /* 0x1 获取调度的GPS数据 */
    SCH_SET_PLT_INFO = 0x2,         /* 0x2 调度发送视频服务器信息 */
    SCH_RSP_PLT_INFO = 0x3,         /* 0x3 应答获取视频服务器信息 */
    SCH_DEV_SVR_STAT  = 0x4,         /*给调度发自检状态*/
    SCH_ALARM_TRIGGER = 0x9,        /* 0x9 报警 报警AV到MCU经过3ggate转发，来回*/
#ifdef JHY_SDK
    SCH_KL_STA_INFO  = 0xc6,        /*调度->av 进出站信息*/
    SCH_KL_STA_TOTAL_NUM= 0xc7,     /*av->调度 站点人数信息*/
    SCH_KL_STA_RST_NUM= 0xc8,       /*调度->av 人数清零*/
    SCH_KL_RTINFO_RA = 0xcd,       /*av->调度 实时人数信息*/
#endif
} SCH_CMD_E;

typedef enum MCU_CMD
{
    MCU_SND_DISKLED_DATA = 0x51,         /* 0x51 获取调度的GPS数据 */

} MCU_CMD_E;

#define DISKTMP_MAX_THREHOLD			90
#define DISKTMP_MIN_THREHOLD			5
#define DISKTMP_HEAT_MIN_THREHOLD       7
#define DISKTMP_HEAT_MAX_THREHOLD       20

typedef enum BASIC_CMD
{
    BASIC_HARDWARE_STATE_REQ = 0x01,
    BASIC_HARDWARE_STATE_RSP = 0x02,         /* 硬件状态 */
    BASIC_IO_SIG_NOTIFY = 0x03,
    BASIC_TEMP_ACK = 0x04,
    BASIC_GPIO_CMD  = 0x05,
} BASIC_CMD_E;
/* BEGIN: Added by mabuyun, 2015/9/2  DES:添加IP对讲功能  PN:20150902 */

typedef enum BZQ_CMD
{
    BZQ_SND_CTRL_VOIP = 0x04,         /* 0x04 IP对讲控制命令 */
} BZQ_CMD_E;

typedef enum PARA_CMD
{
    AV_UPDATE_PARA = 0x11,         /* 0x11        参数模块同步参数用*/
    PARA_CMD_GET_PARA_COM = 0x0F,   /*获取参数*/
} PARA_CMD_E;

typedef enum GATE_CMD
{
    GATE_MOUDLE_REG_CMD = 0x01, // 模块向Gate发送注册包命令字
} GATE_CMD_E;
//GPIO接口
typedef enum GPIO_CMD
{
    GPIO_CMD_DISK_HEAT = 0x01,      //硬盘加热
    GPIO_CMD_DISK_COOL_DOWN = 0x02,//硬盘风扇降温
} GPIO_CMD_E;
#define GPIO_ACTION_START 1
#define GPIO_ACTION_STOP  0

#define	BZQ_START_VOIP             0x01	    // 打开IP对讲
#define	BZQ_STOP_VOIP              0x00	    // 关闭IP对讲

/* END:   Added by mabuyun, 2015/9/2   PN:20150902 */

/*
0x31：3ggate地址
0x32：AV地址 该地址不用了
0x33： UI
0x34：报站器地址
0x35：phone
0x36：app
0x39：单片机
0x40：服务器
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

//#define AV_SET_REALTIME         0x0E    //设置时间-在tm8706与tm8716中使用
//#define AV_GET_REC_CHN         	0x0F    //获取录像状态
//#define AV_SET_SVR_ADDR         0x11    //设置IP地址与端口-回复时还用0x11
//#define AV_ALARM_EVENT          0x12    //报警事件通知

#define AV_LOCAL_DISK_STAT		0xe0	//获取硬盘状态

#define AV_CLOSE_R_PREVIEW		(10008)	//关闭远程预览
#define AV_OPEN_R_PREVIEW		(10007)	//打开远程预览
#define AV_SEARCH_REC			(10170)   //搜索录像
#define AV_CLOSE_R_PLAYBACK		(10172)   //关闭远程录像回放
#define AV_OPEN_R_PLAYBACK		(10171)   //打开远程录像回放
#define AV_OPEN_R_4PLAYBACK		(10178)   //打开远程4通道录像回放



//答复数据包时填写的命令字
#define	R_TAKE_PHOTO            0xA1	//回复拍照
#define	R_SET_IP                0xA2	//回复设置ip地址与端口
#define	R_SET_SUB_FRAM     		0xA3	//回复设置子码流参数
#define	R_REFER_NETSTAT         0xA4	//回复查询与服务器连接状态
#define	R_REFER_IP              0xA5	//回复查询ip地址与端口
#define	R_REFER_SUB_FRAM        0xA6	//回复查询子码流参数
#define R_OPEN_VOIP             0xA7   //回复开启对讲
#define R_CLOSE_VOIP            0xA8   //回复关闭对讲
#define R_SCREEN_SWITCH         0xA9   //回复界面切换
#define R_HEART_CMD             0xAA   //回复与3g网关的连接维持命令
#define R_OPEN_RECORD			0xAB	//回复打开录像
#define R_CLOSE_RECORD          0xAC   //回复关闭录像
#define R_CTRL_SEND_VIDOE       0xAD   //回复控制视频传输

//回复数据包的实际长度
#define	R_TAKE_PHOTO_LEN			10	//回复拍照数据包的长度
#define	R_SET_IP_LEN				10	//回复设置ip地址与端口数据包长度
#define	R_SET_SUB_FRAM_LEN		    10	//回复设置子码流参数数据包长度
#define	R_REFER_NETSTAT_LEN		    11	//回复查询与服务器连接状态数据包长度
#define	R_REFER_IP_LEN				53	//回复查询ip地址与端口数据包长度
#define	R_REFER_SUB_FRAM_LEN		16	//回复查询子码流参数数据包长度
#define R_OPEN_VOIP_LEN             10  //回复开启对讲长度
#define R_CLOSE_VOIP_LEN            10  //回复开启对讲长度
#define   MIN_LEN                   12  //最小的数据长度


//协议包命令标识
#define UI_PIC_FLAG		    	0x0f	//图片上传，向ui发送上传进度
#define UI_UPDATE 		    	0x10   //U盘升级信息，在此程序中没有用到，在u盘升级程序中用到
#define UI_BACK_DOOL_FLAG   	0x11	//后门开关信号
#define UI_LOGIN_FLAG			0x12   //注册标志，av_camera与视频服务器通讯状态报告命令
#define UI_AV_VOIP  			0x13   //IP对讲的协议标志
//#define UI_AV_VER				0x14   //视频程序版本号， 开机的时候发生一次

#define TAKE_PHOTO_OK       	0 /*拍照成功*/
#define TAKE_PHOTO_ERROR    	1 /*拍照失败*/

typedef struct HW_TEMP_ABNORMAL
{
    int iLowTempCount;  //温度过低计数
    int iHighTempCount; //温度过高计数
    int iEnableAssitDownTmep;   //是否启用风扇降温
    int iCoolDownTemp;  //启动风扇降温警戒线
    int iGpioCoolDownDisk;  //是否已经启动风扇降温
    int iGpioHeatDisk;  //是否启动硬盘加热
} HW_TEMP_ABNORMAL_T;

/*********************************************************************************/
//功能配置
typedef struct AV_FUNC_CFG_ST
{
    HI_CHAR cEnableKl;      //使能客流功能 0:未开启 1:开启
    HI_CHAR cVideoInCount;  //视频输入个数
    HI_CHAR cReserved[64];
} __attribute__((packed))AV_FUNC_CFG_T;

typedef struct tagTGPIOCmd
{
    char cFunc;//1:硬盘加热 2:启动风扇降温
    char cCmd; //1:开始操作 0:停止操作
} __attribute__((packed))TGPIOCmd;
//扩展osd
typedef struct EXT_OSD_ST
{
    HI_S32 iLatitude;              /* 纬度 */
    HI_S32 iLongitude;             /* 经度 */
    HI_U8  cLineNum[8];            /* 线路号 */
    HI_U8  cSpeed[8];              /* 速度值 */
    HI_U8  cReserved[16];          /* 预留 */
    HI_S32 iFlag;                  /*
                                    PRE_Y_CODE_Y = 1, 预览显示osd，编码显示osd
                                    PRE_Y_CODE_N ,   预览显示osd，编码不显示osd
                                    PRE_N_CODE_Y ,   预览不显示osd，编码显示osd
                                    PRE_N_CODE_N ,   预览不显示osd，编码不显示osd
                                    */
} __attribute__((packed))EXT_OSD_T;
//osd
typedef struct OSD_ST
{
    HI_U8  cBusId[32];             /* 车辆编号 */
    HI_U8  cReserved[16];          /* 预留 */
} __attribute__((packed))OSD_T;

typedef struct PLAYBACK_ST
{
    HI_S32 s32ChnSel;		/*0~32 BIT分别表示1~32通道*/
    HI_U32 sTime;			/*开始时间*/
    HI_U32 eTime;			/*结束时间*/
} __attribute__((packed))PLAYBACK_T;

typedef struct PLAY_CTRL_T
{
    HI_S32 s32playstat;
} __attribute__((packed))PLAY_CTRL_ST;

typedef struct REC_STAT_ST
{
    HI_S32 s32Stat;			/*录像状态*/
} __attribute__((packed))REC_STAT_T;

typedef struct ALARM_STAT_ST
{
    HI_U8 alarmStat;			/*MCU过来的报警状态 0000 0001 bit表示，bit0表示报警 */
} __attribute__((packed))ALARM_STAT_T;

typedef struct DISK_STAT_ST
{
    HI_S32 s32Stat;			/*硬盘状态*/
} __attribute__((packed))DISK_STAT_T;


typedef struct ServerAddr_st
{
    char  achIpAddr[16];
    HI_U16 sPort;
} __attribute__ ((__packed__))ServerAddr;

typedef struct DISK_LED_STAT_ST
{
    HI_S32 s32diskled;			/*硬盘led*/
} __attribute__((packed))DISK_LED_STAT_T;

typedef struct SVR_REG_STAT_ST
{
    HI_S32 s32Stat;			/*视频平台注册状态*/
} __attribute__((packed))SVR_REG_STAT_T;

/* BEGIN: Added by mabuyun, 2015/9/7  DES:添加IP对讲功能  PN:20150902 */
typedef struct VOIP_STAT_ST
{
    HI_S32 s32Cmd;			/* 1 开始。0关闭 */
    char  limittime;            /* 限制时长 */
} __attribute__((packed))VOIP_STAT_T;
/* END:   Added by mabuyun, 2015/9/7   PN:20150902 */


typedef struct SNAPSHOT_ST
{
    union
    {
        HI_S32 s32Chn;		/*抓拍通道*/
        HI_S32 s32Ret;		/*抓拍结果*/
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
    HI_U8 u8type;			/*预览类型,单分屏,四分屏,九分屏*/
} __attribute__((packed))PREVIEW_T;

typedef struct LOCAL_BK_ST
{
    time_t sTime;			/*开始时间*/
    time_t eTime;			/*结束时间*/
    HI_S32 s32Chn[12];
    HI_S32 s32ChnCnt;
} __attribute__((packed))LOCAL_BK_T;

typedef struct LBK_STAT_T
{
    HI_S32 s32BkChn;        /*备份通道 */
    HI_S32 s32BkStat;       /*备份状态 */
} __attribute__((packed))LBK_STAT_ST;

#ifdef OSD_SHOW
typedef struct OSD_SHOW_DATA_T
{
    HI_U8 busId[3];
    HI_U8 lineId[4];
    HI_U16   speed;
    HI_U32  latitude;/* 纬度 */
    HI_U32  longitude;/* 经度 */

    HI_U8 ucGpsStatus;		/* 定位状态 */
    HI_U8 ucStationNum;		/* 站点上下行及场区 */
    HI_U8 ucOperate;		/* 运营状态 */
    HI_U8 ucRegStatus;		/* 注册状态 */
    HI_U8 ucLimitSpeed;		/* 限速值 */
    HI_U8 bussignal;		/* 车辆信号 */
    HI_U8 ucPlateNum[9];    /* 车牌号 */
    HI_U8 ucInOrOut;        /* 进出站 */
	HI_U8 ucStnDir;			/* 站点方向,0:上行1:下行*/
	HI_U8 ucIsInPark;		/* 是否在场区,0:不在场区1:在场区*/
	HI_U8 ucParkDir;		/* 场区方向,0:上行1:下行*/
	HI_U8 ucParkCode;		/* 场区编号*/
	HI_U8 ucUpTotalStnNum;	/*上行站点总数*/
	HI_U8 ucDownTotalStnNum;/*下行站点总数*/
} __attribute__((packed))OSD_SHOW_DATA_ST;
#endif

typedef struct OSD_UI_SHOW
{
    HI_U8 reason;        /* 挂载或卸载硬盘原因 */
    HI_U8 PowerStatus;	 /* 主电源状态 */
    HI_U8 PowerVal; 	 /* 主电源电压值 */
    HI_U8 LockStatus;
    HI_U8 CapStatus;
    HI_U8 CapVal;
    HI_U8 TemStatus;
    HI_S8 TemVal;
    HI_U8 telesig;
} __attribute__((packed))OSD_UI_SHOW_ST;


typedef struct Av_VideoPltPara_Info
{
    char ip[15];                     //IP地址B
    char port[2];                 //端口B，配置文件中，端口保存按大端，所以不再转化(服务器采用大端)
    char machineId[3];                      //车载机编号,fan zhuan
#ifdef PLT_PARA_BK
    char ipbk[15];                 //IP地址B
    char portbk[2];                //端口B，配置文件中，端口保存按大端，所以不再转化(服务器采用大端)
#endif
    unsigned char cCompany;         //公司代码
    unsigned char cSubCompany;      //子公司代码
} __attribute__((packed))AV_VIDEOPLTPARA_INFO_ST;


/*故障状态上传 */
/************************************************

 bit31  bit30  bit29  bit28  bit27  bit26  bit25  bit24

 bit23  bit22  bit21  bit20  bit19   bit18   bit17   bit16
 录像8  录像7  录像6  录像5  摄像头8 摄像头7 摄像头6 摄像头5
 bit15  bit14  bit13  bit12  bit11  bit10  bit09  bit08
                                    硬盘   MP3文件 SD卡
 bit07  bit06  bit05  bit04  bit03   bit02   bit01   bit00
 录像4  录像3  录像2  录像1  摄像头4 摄像头3 摄像头2 摄像头1
 ************************************************/
typedef struct DEVERROR_STAT_ST
{
    HI_S32 s32DevStat;			/*故障状态 */
} __attribute__((packed))DEVERROR_STAT_T;

typedef struct LPB_SWITCH_AO_ST
{
    HI_CHAR cSwitchAo;		/*AO开关*/
} __attribute__((packed))LPB_SWITCH_AO_T;

typedef struct DOMAIN_PACKET_HEAD_ST
{
    HI_U8 pkt_label[0x04];		//包起始标识
    HI_U8 pkt_len_h; 			//包数据总长度高字节
    HI_U8 pkt_len_l; 			//包数据总长度低字节
    HI_U8 dst_id;				//包目标地址
    HI_U8 src_id;				//包源地址
    HI_U8 pkt_seq;				//包序号
    HI_U8 pkt_cmd;				//包命令字
} __attribute__((packed))DOMAIN_PACKET_HEAD_T;


#define HW_DISK_LOCK_STAT_BIT   0X01
#define HW_SUPER_CAP_STAT_BIT   0X02
#define HW_MAIN_POWER_STAT_BIT  0X04
#define HW_ACC_STAT_BIT         0X08

#define HW_TEMP_ABNORMAL_TIME   10
#define HW_TEMP_ABNORMAL_VALUE  -127
typedef enum
{
    STAT_ONOFF_UNKNOWN = 0X00,    //未知状态
    STAT_ONOFF_ON,               //开机状态
    STAT_ONOFF_OFF               //关机状态
} TurnOnOffDevStat_e;
typedef enum
{
    PW_STAT_NORMAL=0,/*主、备电源均正常*/
    PW_STAT_MAIN_OK,     /*主电源正常，电源欠压*/
    PW_STAT_BAT_OK,   /*主电源掉电，备用电源正常*/
    PW_STAT_MAIN_AND_BAT_ERR   /*主电源掉电，电源欠压*/
} PwStatType_e;

typedef struct AV_HARD_STA_ST
{
    HI_U8       otherHwStat;        /* 硬件相关状态，参考HW_DISK_STAT_BIT */
    HI_U8       ucPowerStat;
    HI_U8       powerstatus;       /*开关机状态*/
    HI_CHAR     cIoSingle;          //门状态
} __attribute__ ((__packed__))AV_HARD_STA;
typedef struct AV_HARD_TEMP_ST
{
    HI_S8     diskTemp;           /* 硬盘温度 */
    HI_S8     MainBoardTemp;
} __attribute__ ((__packed__))AV_TEMP_STA;
typedef struct AV_CAMERA_STA_ST
{
    HI_U16  cameraStat;         /* 摄像头状态 */
    HI_U16  recordVideoStat;    /* 录像状态 */
} __attribute__ ((__packed__))AV_CAMERA_STA;
typedef struct
{
    unsigned char ucParaId_h;  //参数编号的高字节
    unsigned char ucParaId_l;  //参数编号的低字节
} ParaIdStruct;
#define PARA_MAX_LEN    1024 //单个参数的最大长度
typedef struct
{
    unsigned char ucParaId_h;  //参数编号的高字节
    unsigned char ucParaId_l;  //参数编号的低字节
    unsigned char ucParalen_h;  //参数编号的高字节
    unsigned char ucParalen_l;  //参数编号的低字节
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
    char ip[16];                  //IP地址
    char port[2];                 //端口B，配置文件中，端口保存按大端，所以不再转化(服务器采用大端)
} __attribute__((packed))VIDEO_PLAT_SIP_T;
#endif
typedef struct DOMAIN_CMD_ST
{
    DOMAIN_PACKET_HEAD_T head;	//包头
    union
    {
        HI_S32      s32Ret;						/*执行结果*/
        PREVIEW_T   stPreview;					/*预览*/
        SNAPSHOT_T  stSnapshot;					/*抓拍*/
        REC_MONTH_ST stRecinfo;
        REC_QUREY_REQ_T stRecreq;
        REC_QUREY_RSP_T stRecrsp;
        DEC_CHN_SWITCH_T stDecchn;
        DISK_STAT_T stDiskStat;					/*硬盘状态(挂载/卸载)*/
        PLAY_CTRL_ST stPlayctrl;
        DEVERROR_STAT_T s32DevErrStat;           /*dev故障状态 */
        DISK_LED_STAT_T s32diskled;
        REC_STAT_T	stRecStat;					/*录像状态(开启/关闭)*/
        ALARM_STAT_T stAlarmStat;               /*alarm stat*/
        PLAYBACK_ST stPlayBack;					/*回放*/
        LOCAL_BK_T  stLocalBk;                  /*启动备份*/
        LBK_STAT_ST stLBKStat;                  /*备份状态*/
        AV_VERSION_T stVersion;                 /*视频程序版本*/
        VO_PARA_T   stVideoOutPara;             /*视频输出参数*/
        SVR_REG_STAT_T stSvrRegStat;            /*视频服务器注册状态*/
        VI_PARA_T   stVideoInPara;              /*视频输入参数*/
        VI_PARA_CHN_T stVideoInChnPara;
        EXT_OSD_T   stExtOsd;                   /*扩展osd设置结构*/
        OSD_T       stOsd;                      /*osd 设置结构体*/
        OSD_SHOW_DATA_ST stOsdData;
        LPB_SWITCH_AO_T	stLPBSwitchAo;			/*LPB ao 打开或关闭*/

        /* BEGIN: Added by mabuyun, 2015/9/8  DES:添加IP对讲功能  PN:20150902 */
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
        HI_U8 buff[DOMAIN_BUF_LEN]; //包内容
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
    HI_S32 s32MsgId;			/*消息源ID*/
    MSG_EVENT_PF eventFunc; 	/*消息函数*/
} __attribute__((packed))MSG_EVENT_MAP_T;

typedef struct MSG_ACTION_MAP_ST
{
    HI_S32 s32CmdId;			/*消息命令*/
    MSG_ACTION_PF actionFunc;	/*动作处理函数*/
} __attribute__((packed))MSG_ACTION_MAP_T;

typedef struct MSG_MAP_ST
{
    HI_S32 s32MsgId;			/*消息源ID*/
    HI_S32 s32CmdId;			/*消息命令*/
    MSG_PF pMsgFunc;			/*动作处理函数*/
} __attribute__((packed))MSG_MAP_T;
#ifdef JHY_SDK
#define RET_LEARN_BK_FAILURE    -1
#define RET_LEARN_BK_OPEN_FRONT 1
#define RET_LEARN_BK_SUCCESS    0
#define RET_LEARN_BK_OPEN_BACK  2
#define RET_LEARN_BK_INDEX      3

/*进出站 调度->客流机 的数据结构体*/
#define  EE_BUS_ID_LEN	0x03  //车辆编号长度
typedef struct	SCH_KLSTN_DATA_ST//C6 >这是命令字
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
    //bit 7: 0-非首站, 1-首站.bit 6: 0-非末站, 1-末站
    //bit 5,4 reserved;//bit 3: 0-gps未定位, 1-gps已定位.bit 2: 0-非关门触发, 1-关门触发.
    //bit 1,0: 0x01-进站, 0x02-出站, 0x03-模拟出站
    HI_U16 usCrcReserved;
} __attribute__((packed))SCH_KLSTN_DATA_T;

/*客流机->调度 的客流数据结构体*/
typedef struct	SCH_KLSTN_DATA_REQ_ST//C7 <
{
    HI_U8 cmd;	//0xC6
    HI_U8 u8Reserved[2];
    HI_U8 bagid;
    HI_U8 kl_sta;	//这个也没用到,可以不填
    HI_U8 time[6];
    HI_U8 direct;
    HI_U8 sta_num;	//站点编号
    HI_U8 up_num;
    HI_U8 down_num;
    HI_U8 in_out_flag;
    HI_U8 ucTotalNum;	//这个是上下车总人数,也没用到,可以不填
    HI_U16 usCrcReserved;
} __attribute__((packed))SCH_KLSTN_DATA_REQ_T;

/*客流->调度  实时详细的客流数据*/
typedef struct SCH_KLSTN_REAL_DATA_ST
{
    HI_U8 cmd;	//0xC6
    HI_U8 u8Reserved[3];
    HI_U8 ucFrontDoorStatus;	//调度没用到
    HI_U8 ucFrontDoorCome;
    HI_U8 ucFrontDoorGo;
    HI_U8 ucBackDoorStatus;		//调度没用到
    HI_U8 ucBackDoorCome;
    HI_U8 ucBackDoorGo;
    HI_U16 usCrcReserved;
} __attribute__((packed))SCH_KLSTN_REAL_DATA_T;

typedef struct CAMERA_PARA_ST
{
    int door;             //0:前门  1:后门
    int camerach;         //通道号 用于切换视频使用
    char cameraid[8];     //当前摄像机编号
    int installH;			//高度过滤暂时未用				范围0				默认:0
    int noiseFilter;		//噪声过滤							范围120~200		默认:120
    int leftLimit;			//左边界								范围0~100			默认:0
    int rightLimit;			//右边界								范围260~360		默认:360
    int topLimit;			//上边界								范围0~30			默认:0
    int bottomLimit;		//下边界								范围258~288		默认:288
    int enableC;			//使能通道							范围0~1			默认:1
    int inoutType;			//0为上进下出 1为下进上出		范围0~1			默认:0
    int doorDelay;			//开关门延迟统计					范围0~5			默认:3
    int filterHeight;		//高度过滤							范围0~3			默认:0
    int counterType;		//计数类型							范围8或12或14		默认:8
    char sensitivity[4];//float sensitivity;		//灵敏度								范围10				默认:10
    int beeline[2];			//计数线								范围100~260		默认:0:106 1:174
    int headFilter;			//人头大小过滤						范围80~150			默认:80
    int forgTh;				//前景阈值							范围15~20			默认:15
} __attribute__((packed))CAMERA_PARA_T;


typedef struct UPDATE_CAMERA_BIN_ST
{
    int door;             //0:前门  1:后门
    char cameraid[8];
} __attribute__((packed))UPDATE_CAMERA_BIN_T;


typedef struct CAMERA_ID_ST
{
    char cError;//0:传入参数错误(没有后续字段)  1:成功
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

/* BEGIN: Added by mabuyun, 2015/9/2  DES:添加IP对讲功能  PN:20150902 */
HI_S32 DOMAIN_SndBZQCtrlVOIP(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);
HI_S32 DOMAIN_RcvBZQCtrlVOIP(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);

HI_S32 DOMAIN_RcvUiLoginRsp(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen);




/* END:   Added by mabuyun, 2015/9/2   PN:20150902 */
#endif
#endif
