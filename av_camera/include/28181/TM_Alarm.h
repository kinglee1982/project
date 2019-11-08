#ifndef __TM_ALARM_H_
#define __TM_ALARM_H_

#include <math.h>
#include <time.h>
#include "TM_APPEvent.h"



#if 0
#define ALARM_TRIGGER_START 1
#define ALARM_TRIGGER_END 0

#define IO_ALARM_TRIGGER 1
#define MD_ALARM_TRIGGER 1

#define ALL_CHN_NUMS 4
#define ALARM_CHECK_TIMES 1
#define ALARM_TIME_SEG_DAYS 7      /*1周7天*/
#define ALARM_TIME_SEG_NUMS 4      /*每个通道配置4个时间段 */

#define MD_MAX_CHN_NUMS 8

/*报警联动通道的对应关系 */
#define ALARM_LINK_CHN_1  0x0001
#define ALARM_LINK_CHN_2  0x0002
#define ALARM_LINK_CHN_3  0x0004
#define ALARM_LINK_CHN_4  0x0008
#define ALARM_LINK_CHN_5  0x0010
#define ALARM_LINK_CHN_6  0x0020
#define ALARM_LINK_CHN_7  0x0040
#define ALARM_LINK_CHN_8  0x0080


#define MAX_TRIGGER_ALARM_NUMS 1  /*IO报警触发的最多个数 */

#define ALARM_EVENV_DISP_LEN 32
#define LOG_EVENV_DISP_LEN 64

#endif

/*停止录像*/
#define TM_UI_ADDER (0x33)
#define TM_UI_CMD   (0x02)
#define TM_mcu_addr    (0x39)
#define TM_reboot   (0x16)

extern int cmd_sockfd;
extern device_camer_id sipcamerid[CHNNUM];

typedef struct Stop_Record
{
    int chnnum;
    unsigned char rcord_flag;
}__attribute__((packed)) TM_Stop_Record;
int TM_Record_Control(const char* id,int recordflag);


/*蜂鸣器控制*/
#define TM_UI_BEEP_STOP  (0x00)
#define TM_UI_BEEP_START (0x01)
int TM_Start_Alarm(const char* id,int alarmType);
int TM_Reset_Alarm(const char* id,int alarmType);
int TM_Start_Alarm_Beep();



/*启动报警线程*/
int DeviceAlarmStart(int Sockfd);
void *TM_Device_alarm(void *arg);
extern int Setalarmenableflag(const char* id, int alarmenableflag);


/*读出视频丢失参数*/
int readXmlViodeLoseParam();
/*读出移动侦测参数 */
extern int readXmlMDParam(WEBOCX_MD_SET_ST *voidemdparam);
/*设置设备视频丢失参数*/
int SetDeviceViodeLoseParam(WEBOCX_LOSTFRAM_ST *voideparam);
/*设置设备移动侦测参数*/
int SetDeviceMDParam();
int SetDeviceMD();


extern void restdevice();


/*视频丢失*/
typedef struct _VOID_LOSE_SEND_AV_STAT_
{
    char channo;    //通道号
    char alarmstat; //是否告警
    char beep;      //是否关联蜂鸣器
}__attribute__((packed)) VOID_LOSE_SEND_AV_STAT;



/*报警发送状态给AV的状态结构 */
/*
序号	字段名称	字节数	数据类型	填充说明
1	通道号	1	整型	从0开始
2	是否录像	1	字节	0 表示是  其他否
3	是否对讲	1	字节	0 表示是  其他否
4	是否移动侦测	1	字节	0 表示是  其他否
5	是否告警	1	字节	0 表示是  其他否
6	是否关联蜂鸣器	1	字节	0 表示是  其他否	*/

typedef struct _ALARM_SEND_AV_STAT_
{
    char channo;    //通道号
    char Recstat;   //是否录像
    char voipstat;  //是否对讲
    char mdstat;    //是否移动侦测
    char alarmstat; //是否告警
    char beep;      //是否关联蜂鸣器
}__attribute__((packed)) ALARM_SEND_AV_STAT;


/*报警级别*/
typedef enum AlarmPriorit
{
    onePriorit = 1, /*一级警情*/
    twoPriorit,
    threePriorit,
    fourPriorit
}Sip_AlarmPriorit;


/*报警类型*/
typedef enum AlarmMethod
{
    oneMethod = 1,  /*电话报警*/
    twoMethod,      /*设备报警*/
    threeMethod,    /*短信报警*/
    fourMethod,    /*GPS报警*/
    fiveMethod,    /*视频报警*/
    sixMethod,     /*设备故障报警*/
    sevenMethod     /*其他报警*/
}Sip_AlarmMethod;

/*设备网络设置*/
int netConfig(WEBOCX_NETWORK_PARA_ST *pstNetArgs);

#if 0
/*报警类型 */
typedef enum _ALARM_TYPE_E_
{
    ALARM_TYPE_TRIGGER = 0, /*IO触发 */
    ALARM_TYPE_MD = 1,     /*移动侦测 */
    ALARM_TYPE_RES
}ALARM_TYPE_EM;

typedef struct _ALARM_EVENT_LOG_T_
{
    ALARM_TYPE_EM  emAlarmType;
    char           evenvDisp[ALARM_EVENV_DISP_LEN];
}_ALARM_EVENT_LOG_ST;
/*报警关联的时间段 大小需要和平台一致，不要随意修改 */
typedef struct _ALARM_TIME_SEG_T_
{
    char startHour;
    char startMin;
    char startSec;
    char endHour;
    char endMin;
    char endSec;
}ALARM_TIME_SEG_ST;    /*sizeof = 6*/

/*写日志公共时间结构  */
typedef struct _COMM_TIME_T_
{
    int year;
    int mon;
    int day;
    int hour;
    int min;
    int sec;
    int week;
}COMM_TIME_ST;

/*写日志结构 */
typedef struct _ALARM_XML_LOG_T_
{
    ALARM_TYPE_EM   emAlarmType;    /*io  md 等*/
    int             s32IoAlarmNo;    /*报警IO号 ,移动侦测号等 */
    bool            bTrigger;        /*移动侦测或者报警被触发都为1,否则为0 */
    char            szEventName[LOG_EVENV_DISP_LEN];   /*关联事件*/
    COMM_TIME_ST    stStartTime;  /*开始时间*/
    COMM_TIME_ST    stEndTime;   /*结束时间*/
}ALARM_XML_LOG_ST;

/*报警处理记录 */
typedef struct _ALARM_HANDLE_T
{
    bool    bWriteLog;
    bool    bSendAv;
    bool    bSendSip;
    bool    bBeep;
}ALARM_HANDLE_T;

/*IO报警*/
typedef struct _TRIGGER_ALARM_T_
{
    bool bEnable;       /*使能标志 */
    int  s32Interval;   /*时间间隔 固定 */
    int  s32BeepEnable; /*是否开启声音 */
    int  s32BeepTime; /*Beep的时间 */
    int  s32BeepInterval; /*Beep的间隔 */
    int  LinkSnap;      /* 位表示 联动抓拍 */
    int  LinkRecord;    /* 位表示 联动录像 */
    int  LinkOutput;    /* 位表示 联动输出 */
    int  LinkRecDuration;   /* 联动录像持续时长 固定*/
    int  AlarmOutDuration;  /*报警输出时间 固定*/
    ALARM_TIME_SEG_ST  stTriggerTimeSeg[ALARM_TIME_SEG_DAYS][ALARM_TIME_SEG_NUMS];
    ALARM_HANDLE_T     stAlarmHandle;
}TRIGGER_ALARM_ST;

typedef struct _COMM_STAT_T_
{
    int s32IoAlarmStat;  /*位表示 */
    int s32MdStat;   /*位 表示 1 为触发 0 非触发*/
}_COMM_ALARM_STAT_ST;
/*报警结构 */
typedef struct _COMM_ALARM_T_
{
    _COMM_ALARM_STAT_ST stCommAlarmStat;
    TRIGGER_ALARM_ST stTriggerAlarm[MAX_TRIGGER_ALARM_NUMS];
    MD_ChnAttr_st    stMdAttr[MD_MAX_CHN_NUMS];
    ALARM_HANDLE_T   stMdHandle[MD_MAX_CHN_NUMS];

}COMM_ALARM_ST;

/*报警发送状态给AV的状态结构 */
/*
序号	字段名称	字节数	数据类型	填充说明
1	通道号	1	整型	从0开始
2	是否录像	1	字节	0 表示是  其他否
3	是否对讲	1	字节	0 表示是  其他否
4	是否移动侦测	1	字节	0 表示是  其他否
5	是否告警	1	字节	0 表示是  其他否
6	是否关联蜂鸣器	1	字节	0 表示是  其他否	*/

typedef struct _ALARM_SEND_AV_STAT_
{
    char channo;
    char Recstat;
    char voipstat;
    char mdstat;
    char alarmstat;
    char beep;
}ALARM_SEND_AV_STAT;
extern void *TM_Device_alarm(void *arg);

/*启动报警线程*/
int DeviceAlarmStart(int Sockfd);
/*读出报警参数*/
int readXmlAlarmParam();
/*读出移动侦测参数 */
int readXmlMDParam();


/*读取报警参数 */
extern void readAlarmParam(WEBOCX_NODE_SET_ST *pstAlarmNodeSet);

/*读取移动侦测参数 */
extern void readMDParam(WEBOCX_MD_SET_ST *pstMdSet);

/*使能布防 */
extern void SetMdEnable(int chnno);

/*取消布防 */
extern void SetMdDisable(int chnno);

/*报警复位 */
extern void SetAlarmReset(int alarmno);

/*报警关闭 */
extern void SetAlarmDisable(int alarmno);

extern void SetCurrMdStat(int channo);

extern void WriteAlarmStat2Xml(ALARM_XML_LOG_ST *pAlarmXmlLog , bool bTrigger);

#endif





#endif
