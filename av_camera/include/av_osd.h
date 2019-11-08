#ifndef __AV_OSD_H__
#define __AV_OSD_H__

#define BUS_ID_LEN 16
#define OSD_DEBUG
/*现在支持到6行 */
#define MAX_OSD_TITLE		6

#define OSD_TYPE_TIME	0
#define OSD_TYPE_TEXT	1

#define TIME_STRING_LEN		(32)		/*时间格式串长度*/
#define RASTER_LENGTH		1024
#define NUMBER_DIGIT(num, bit) (((num)/(bit))%10)

#define MAX_OSD_STRLEN TIME_STRING_LEN
#define LINE_BIT_LEN  5

#define PARSE_RGB1555(rgbBuf, src) 				\
do{							\
	HI_U32 rgb = 0;\
	rgb = (src) & 0xFF * 32 / 256;\
	rgb = rgb |(((((src) & 0x0000FF00)>>8) * 32 / 256) << 5);\
	rgb = rgb |(((((src) & 0x00FF0000)>>16) * 32 / 256) << 10);\
	rgbBuf[0] = (rgb) & 0xFF;				\
	rgbBuf[1] = ((rgb) & 0xFF00) >> 8;		\
} while(0)

/*
time format
*/
enum date_fmt {
	DF_YYMMDD = 0,		//年 月 日
	DF_MMDDYY,			//月 日 年
	DF_DDMMYY			//日 月 年
};

enum date_spec {
	DS_DOT	= 0,		//点分格式
	DS_DASH,			//杠分格式
	DS_SLASH			//斜线格式
};

enum time_fmt {
	TF_24	= 0,		//24小时
	TF_12				//12小时
};


typedef struct hiPoint_Font_S
{
    HI_S32   s32Width;   /* The width of one HI_CHARacter  */
    HI_S32   s32Height;  /* The height of one HI_CHARacter */
    HI_CHAR *pFont;      /* The font library            */
} POINT_FONT_S;

typedef struct tagSYSTEM_TIME{
    HI_S32 year;
	HI_S32 month;
	HI_S32 day;
	HI_S32 wday;
	HI_S32 hour;
	HI_S32 minute;
	HI_S32 second;
	HI_S32 isdst;
}SYSTEM_TIME;

typedef struct tagOSD_TIME
{
	SYSTEM_TIME sysTime;
	HI_S32 dateFmt;
	HI_S32 dateSpec;
	HI_S32 timeFmt;
} OSD_TIME;

typedef struct hiBUFFER_S
{
	HI_CHAR *pAddr;
	HI_S32 nSize;
} BUFFER_S;

typedef struct tagCAPTURE_TITLE_PARAM
{
	HI_S32 index;
	HI_S32 enable;
	HI_U16 x;
	HI_U16 y;
	HI_U16 width;
	HI_U16 height;
	HI_U32 fg_color;					/*!< text color, rgba 8:8:8:8 */
	HI_U32 bg_color;					/*!< background color, rgba 8:8:8:8 */
    HI_S32 rastersize;
	HI_U8 *raster;
}CAPTURE_TITLE_PARAM;

typedef struct tagOSD_TIMEPARAM
{
	HI_S32 changed;
	HI_S32 imagesize[2];
	HI_S32 jpegsize;
	HI_U32 osdHandle[2];
	HI_U32 snaposdHandle;
	BUFFER_S StBufFont;
	BUFFER_S StBufRGB;
	OSD_TIME osdTime;
	CAPTURE_TITLE_PARAM Param;
	pthread_mutex_t lock;
}OSD_TIMEPARAM;

typedef struct tagOSD_TITLEPARAM
{
	//HI_S32 TitleLen;
	//HI_S32 imagesize[2];
	HI_U32 osdHandle[2];
	HI_U32 snaposdHandle;
	BUFFER_S StBufRGB;
	CAPTURE_TITLE_PARAM Param;
	//HI_U8 raster[RASTER_LENGTH];

}OSD_TITLEPARAM;


#define PARA_BUS_ID_LEN 3
#define PARA_LINE_LEN 4

typedef struct OSD_SCH_DATA
{
    HI_U8 busId[PARA_BUS_ID_LEN];
    HI_U8 lineId[PARA_LINE_LEN];
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
}__attribute__((packed))OSD_SCH_DATA_ST;

typedef struct OSD_UI_DATA
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
}__attribute__((packed))OSD_UI_DATA_ST;

typedef struct OSD_PHONE_DATA
{
	HI_U8 telsig;		/* 通信信号 */
}__attribute__((packed))OSD_PHONE_DATA_ST;

typedef struct OSD_TITLE_DATA_T
{
	OSD_SCH_DATA_ST stOsdSchInfo;
	OSD_UI_DATA_ST stOsdUiInfo;
	OSD_PHONE_DATA_ST stOsdPhoneInfo;
}__attribute__((packed))OSD_TITLE_DATA_ST;

HI_VOID HI3521_InitOsdTitle(HI_S32 s32Title, HI_S32 s32Chn, HI_S32 s32StreamType);
HI_VOID HI3521_OsdShow();
HI_S32 HI3521_OsdInit(HI_VOID);
HI_VOID HI3521_SetTimeOsd(HI_VOID);
HI_S32 HI3521_CaptureSetTime_Snap(HI_S32 s32Chn, const SYSTEM_TIME * pTime);
HI_VOID	HI3531_SnapOsdShow(HI_S32 s32Chn);
void HI3521_GetBusId(char *pBusId,int iLen);

OSD_TITLE_DATA_ST *HI3521_GetTitleOsdData();
#endif //__AV_OSD_H__