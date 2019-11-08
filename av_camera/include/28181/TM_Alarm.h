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
#define ALARM_TIME_SEG_DAYS 7      /*1��7��*/
#define ALARM_TIME_SEG_NUMS 4      /*ÿ��ͨ������4��ʱ��� */

#define MD_MAX_CHN_NUMS 8

/*��������ͨ���Ķ�Ӧ��ϵ */
#define ALARM_LINK_CHN_1  0x0001
#define ALARM_LINK_CHN_2  0x0002
#define ALARM_LINK_CHN_3  0x0004
#define ALARM_LINK_CHN_4  0x0008
#define ALARM_LINK_CHN_5  0x0010
#define ALARM_LINK_CHN_6  0x0020
#define ALARM_LINK_CHN_7  0x0040
#define ALARM_LINK_CHN_8  0x0080


#define MAX_TRIGGER_ALARM_NUMS 1  /*IO���������������� */

#define ALARM_EVENV_DISP_LEN 32
#define LOG_EVENV_DISP_LEN 64

#endif

/*ֹͣ¼��*/
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


/*����������*/
#define TM_UI_BEEP_STOP  (0x00)
#define TM_UI_BEEP_START (0x01)
int TM_Start_Alarm(const char* id,int alarmType);
int TM_Reset_Alarm(const char* id,int alarmType);
int TM_Start_Alarm_Beep();



/*���������߳�*/
int DeviceAlarmStart(int Sockfd);
void *TM_Device_alarm(void *arg);
extern int Setalarmenableflag(const char* id, int alarmenableflag);


/*������Ƶ��ʧ����*/
int readXmlViodeLoseParam();
/*�����ƶ������� */
extern int readXmlMDParam(WEBOCX_MD_SET_ST *voidemdparam);
/*�����豸��Ƶ��ʧ����*/
int SetDeviceViodeLoseParam(WEBOCX_LOSTFRAM_ST *voideparam);
/*�����豸�ƶ�������*/
int SetDeviceMDParam();
int SetDeviceMD();


extern void restdevice();


/*��Ƶ��ʧ*/
typedef struct _VOID_LOSE_SEND_AV_STAT_
{
    char channo;    //ͨ����
    char alarmstat; //�Ƿ�澯
    char beep;      //�Ƿ����������
}__attribute__((packed)) VOID_LOSE_SEND_AV_STAT;



/*��������״̬��AV��״̬�ṹ */
/*
���	�ֶ�����	�ֽ���	��������	���˵��
1	ͨ����	1	����	��0��ʼ
2	�Ƿ�¼��	1	�ֽ�	0 ��ʾ��  ������
3	�Ƿ�Խ�	1	�ֽ�	0 ��ʾ��  ������
4	�Ƿ��ƶ����	1	�ֽ�	0 ��ʾ��  ������
5	�Ƿ�澯	1	�ֽ�	0 ��ʾ��  ������
6	�Ƿ����������	1	�ֽ�	0 ��ʾ��  ������	*/

typedef struct _ALARM_SEND_AV_STAT_
{
    char channo;    //ͨ����
    char Recstat;   //�Ƿ�¼��
    char voipstat;  //�Ƿ�Խ�
    char mdstat;    //�Ƿ��ƶ����
    char alarmstat; //�Ƿ�澯
    char beep;      //�Ƿ����������
}__attribute__((packed)) ALARM_SEND_AV_STAT;


/*��������*/
typedef enum AlarmPriorit
{
    onePriorit = 1, /*һ������*/
    twoPriorit,
    threePriorit,
    fourPriorit
}Sip_AlarmPriorit;


/*��������*/
typedef enum AlarmMethod
{
    oneMethod = 1,  /*�绰����*/
    twoMethod,      /*�豸����*/
    threeMethod,    /*���ű���*/
    fourMethod,    /*GPS����*/
    fiveMethod,    /*��Ƶ����*/
    sixMethod,     /*�豸���ϱ���*/
    sevenMethod     /*��������*/
}Sip_AlarmMethod;

/*�豸��������*/
int netConfig(WEBOCX_NETWORK_PARA_ST *pstNetArgs);

#if 0
/*�������� */
typedef enum _ALARM_TYPE_E_
{
    ALARM_TYPE_TRIGGER = 0, /*IO���� */
    ALARM_TYPE_MD = 1,     /*�ƶ���� */
    ALARM_TYPE_RES
}ALARM_TYPE_EM;

typedef struct _ALARM_EVENT_LOG_T_
{
    ALARM_TYPE_EM  emAlarmType;
    char           evenvDisp[ALARM_EVENV_DISP_LEN];
}_ALARM_EVENT_LOG_ST;
/*����������ʱ��� ��С��Ҫ��ƽ̨һ�£���Ҫ�����޸� */
typedef struct _ALARM_TIME_SEG_T_
{
    char startHour;
    char startMin;
    char startSec;
    char endHour;
    char endMin;
    char endSec;
}ALARM_TIME_SEG_ST;    /*sizeof = 6*/

/*д��־����ʱ��ṹ  */
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

/*д��־�ṹ */
typedef struct _ALARM_XML_LOG_T_
{
    ALARM_TYPE_EM   emAlarmType;    /*io  md ��*/
    int             s32IoAlarmNo;    /*����IO�� ,�ƶ����ŵ� */
    bool            bTrigger;        /*�ƶ������߱�����������Ϊ1,����Ϊ0 */
    char            szEventName[LOG_EVENV_DISP_LEN];   /*�����¼�*/
    COMM_TIME_ST    stStartTime;  /*��ʼʱ��*/
    COMM_TIME_ST    stEndTime;   /*����ʱ��*/
}ALARM_XML_LOG_ST;

/*���������¼ */
typedef struct _ALARM_HANDLE_T
{
    bool    bWriteLog;
    bool    bSendAv;
    bool    bSendSip;
    bool    bBeep;
}ALARM_HANDLE_T;

/*IO����*/
typedef struct _TRIGGER_ALARM_T_
{
    bool bEnable;       /*ʹ�ܱ�־ */
    int  s32Interval;   /*ʱ���� �̶� */
    int  s32BeepEnable; /*�Ƿ������� */
    int  s32BeepTime; /*Beep��ʱ�� */
    int  s32BeepInterval; /*Beep�ļ�� */
    int  LinkSnap;      /* λ��ʾ ����ץ�� */
    int  LinkRecord;    /* λ��ʾ ����¼�� */
    int  LinkOutput;    /* λ��ʾ ������� */
    int  LinkRecDuration;   /* ����¼�����ʱ�� �̶�*/
    int  AlarmOutDuration;  /*�������ʱ�� �̶�*/
    ALARM_TIME_SEG_ST  stTriggerTimeSeg[ALARM_TIME_SEG_DAYS][ALARM_TIME_SEG_NUMS];
    ALARM_HANDLE_T     stAlarmHandle;
}TRIGGER_ALARM_ST;

typedef struct _COMM_STAT_T_
{
    int s32IoAlarmStat;  /*λ��ʾ */
    int s32MdStat;   /*λ ��ʾ 1 Ϊ���� 0 �Ǵ���*/
}_COMM_ALARM_STAT_ST;
/*�����ṹ */
typedef struct _COMM_ALARM_T_
{
    _COMM_ALARM_STAT_ST stCommAlarmStat;
    TRIGGER_ALARM_ST stTriggerAlarm[MAX_TRIGGER_ALARM_NUMS];
    MD_ChnAttr_st    stMdAttr[MD_MAX_CHN_NUMS];
    ALARM_HANDLE_T   stMdHandle[MD_MAX_CHN_NUMS];

}COMM_ALARM_ST;

/*��������״̬��AV��״̬�ṹ */
/*
���	�ֶ�����	�ֽ���	��������	���˵��
1	ͨ����	1	����	��0��ʼ
2	�Ƿ�¼��	1	�ֽ�	0 ��ʾ��  ������
3	�Ƿ�Խ�	1	�ֽ�	0 ��ʾ��  ������
4	�Ƿ��ƶ����	1	�ֽ�	0 ��ʾ��  ������
5	�Ƿ�澯	1	�ֽ�	0 ��ʾ��  ������
6	�Ƿ����������	1	�ֽ�	0 ��ʾ��  ������	*/

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

/*���������߳�*/
int DeviceAlarmStart(int Sockfd);
/*������������*/
int readXmlAlarmParam();
/*�����ƶ������� */
int readXmlMDParam();


/*��ȡ�������� */
extern void readAlarmParam(WEBOCX_NODE_SET_ST *pstAlarmNodeSet);

/*��ȡ�ƶ������� */
extern void readMDParam(WEBOCX_MD_SET_ST *pstMdSet);

/*ʹ�ܲ��� */
extern void SetMdEnable(int chnno);

/*ȡ������ */
extern void SetMdDisable(int chnno);

/*������λ */
extern void SetAlarmReset(int alarmno);

/*�����ر� */
extern void SetAlarmDisable(int alarmno);

extern void SetCurrMdStat(int channo);

extern void WriteAlarmStat2Xml(ALARM_XML_LOG_ST *pAlarmXmlLog , bool bTrigger);

#endif





#endif
