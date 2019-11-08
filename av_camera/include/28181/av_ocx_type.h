/******************************************************************************

  Copyright (C), 2001-2011, zhengzhou Tiamaes Technology Co., Ltd.

 ******************************************************************************
  File Name     : av_ocx_type.h
  Version       : Initial Draft
  Author        : xiaomengmeng
  Created       : 2014/12/23
  Last Modified :
  Description   : web�ؼ�ͨ�����ݽṹ
  Function List :
  History       :
  1.Date        : 2014/12/23
    Author      : xiaomengmeng
    Modification: Created file

******************************************************************************/
#ifndef __AV_OCX_TYPE_H__
#define __AV_OCX_TYPE_H__

#include "av_ocx_def.h"

typedef struct Av_WebOcx_Package
{
    char packid[4];     /* 4���ֽڰ���ʶ#### */
    unsigned int packagelen;    /* ���ݰ��ܳ��Ȱ���CRC */
    unsigned int msgtype;
    unsigned int packagesequ;
    char data[OCX_RECV_MAX_SIZE];
}__attribute__ ((__packed__)) AV_WEBOCX_PACKAGE_ST;

#define WEBOCX_HEAD_COMM_LEN    (sizeof(AV_WEBOCX_PACKAGE_ST) - OCX_RECV_MAX_SIZE)

/* �ڲ�ͨ�����ݰ��ṹ */
typedef struct Tm_Cmd_Head
{
    char bag_id[0x04];
    char cmdlen_h;
    char cmdlen_l;
    char ucDestDevID;
    char ucSrcDevID;
    char ucBagID;
    char ucCmd;
    char buff[2048];
}__attribute__((packed)) TM_CMD_HEAD_ST;

/* �ն˵�¼Ӧ�����ݽṹ */
typedef struct WebOcx_LogIn_Reply
{
    int  UserId;
    char MaxChnCnt;
    char MaxAudioCnt;
    char MaxSubStreamCnt;
    char MaxPlayBackCnt;
    char MaxAlarmInputCnt;
    char MaxAlarmOutputCnt;
    char MaxHdiskCnt;
}__attribute__((packed)) WEBOCX_LOGIN_REPLY_ST;

/* ��̨�������ݽṹ */
typedef struct WebOcx_Camera_Ctrl
{
    int UserId;         /* �û�ID */
    int ChnId;          /* ͨ��ID */
    int CameraCtrl;     /* ��̨�������� CMD  */
    int Speed;           /*�ٶ�*/
    int Action;         /* ����  data */
    char Parar[32];     /*��Ϊ�գ�Ԥ��λʱΪ����*/
}__attribute__((packed)) WEBOCX_CAMERA_CTRL_ST;


/* �ƶ���Ⲽ��ʱ�����ݽṹ */
typedef struct WebOcx_Alert_Time
{
    char AlertStartHour;
    char AlertStartMinu;
    char AlertStartSecond;
    char AlertEndHour;
    char AlertEndMinu;
    char AlertEndSecond;
}__attribute__((packed)) WEBOCX_ALERT_TIME_ST;


/* ��������������ݽṹ */
typedef struct WebOcx_Area_Coords
{
    unsigned int x;
    unsigned int y;
}__attribute__((packed)) WEBOCX_AREA_COORDS_ST;
/* ����������ݽṹ */
typedef struct WebOcx_Detect_Area
{
    WEBOCX_AREA_COORDS_ST TopLeft;
    WEBOCX_AREA_COORDS_ST BottomRight;
}__attribute__((packed)) WEBOCX_DETECT_AREA_ST;
/* �ƶ�����������ݽṹ */
typedef struct WebOcx_Md_Set
{
    //int UserId;             /* �û�ID */
    int ChnId;
    WEBOCX_ALERT_TIME_ST AlertTime[7][4]; /* ��������ʱ�� 7��4��ʱ��� */
    char AlertEnable;       /* ����ʹ�� */
    char AlarmEnable;
    char Senibility;        /* ������ */
    //char AlarmLinkIO;     /* ��������IO */
    char DetectInterval;    /* �����ʱ�� */
    int LinkSnap;           /* ����ץ�� */
    int LinkRecord;         /* ����¼�� */
    int LinkOutput;         /* ������� */
    char LinkRecDuration;   /* ����¼�����ʱ�� */
    char AlarmOutDuration;  /* �澯��Ϣ����ʱ�� */
    char BuzzEnable;        /* ������ʹ�ܿ��� */
    char LinkBuzzDuration;  /* ��������������ʱ�� */
    char DetectAreaNum;     /* ��������� */
    WEBOCX_DETECT_AREA_ST stMdArea[4];
}__attribute__((packed)) WEBOCX_MD_SET_ST;


/* �澯�ڵ��������ݽṹ */
typedef struct WebOcx_Node_Set
{
    char NodeId;
    char AlarmStatus;           /* �澯״̬ 0:���� 1:���� */
    char AlarmEvent[32];
    WEBOCX_ALERT_TIME_ST AlertTime[7][4]; /* ��������ʱ�� */
    char AlertEnable;
    char AlarmEnable;
    char BuzzerEnable;
    char DetectInterval;/* �����ʱ�� */
    int LinkSnap;      /* ����ץ�� */
    int LinkRecord;    /* ����¼�� */
    int LinkOutput;    /* ������� */
    char LinkRecDuration;   /* ����¼�����ʱ�� */
    char AlarmOutDuration;
    char BuzzerDuration;
}__attribute__((packed)) WEBOCX_NODE_SET_ST;
/* �澯�������ݽṹ */
typedef struct WebOcx_Alarm_Set
{
    int UserId;        /* �û�ID */
}__attribute__((packed)) WEBOCX_ARLARM_SET_ST;

/* �豸�������ݽṹ */
typedef struct WebOcx_DevPara
{
    //int UserId;
    char DevId[32];         /* �ַ��� */
    char DevName[64];       /* �ַ��� */
    char PlatformDevId[40]; /* �ַ��� */
    char Domain[32];        /* ���� */
    char PassWd[32];        /* ע������ */
    int TimeOut;            /* ��Ч���� */
    char SipSvrIp[32];      /* �ַ��� */
    int SipSvrPort;
    char LocalIp[32];       /* �豸IP */
    int LocalPort;

}__attribute__((packed)) WEBOCX_DEVPARA_ST;


/* ͨ���������ݽṹ */
typedef struct WebOcx_Chn_Info
{
    int ChnID;
    char PlatformDevId[64];
    char ChnName[40];
    short CameraAddr;
    short ProtType;
}__attribute__((packed)) WEBOCX_CHN_INFO_ST;

/* ��ȡͨ���������ݽṹ */
typedef struct WebOcx_ChnPara
{
    int ChnCnt;
}__attribute__((packed)) WEBOCX_CHNPARA_ST;

typedef struct WebOcx_Real_View
{
    int UserId;        /* �û�ID */
    int ChnId;
    int StreamType;     /* �������� 0��������Ƶ 1��������Ƶ 2��������Ƶ */
}__attribute__((packed)) WEBOCX_REAL_VIEW_ST;

typedef struct WebOcx_Stream_Guide
{
    int UserId;        /* �û�ID */
    int AuthenId;       /* ��֤������Ч��ID */
}__attribute__((packed)) WEBOCX_STREAM_GUIDE_ST;

/* ��ʷ��Ƶ¼���ѯ���ݽṹ */
typedef struct WebOcx_History_Query
{
    int UserId;         /* �û�ID */
    int ChnId;         /* ͨ�� */
    char FileType;      /* �ļ����� */
    int StartTime;      /* ��ʼʱ�� */
    int EndTime;        /* ����ʱ�� */
}__attribute__((packed)) WEBOCX_HISTORY_QUERY_ST;

/* ��ʷ��Ƶ¼���ѯ���ݽṹ */
typedef struct WebOcx_History_Reply
{
    char FileName[16];      /* �ļ��� */
    int StartTime;      /* ��ʼʱ�� */
    int EndTime;        /* ����ʱ�� */
}__attribute__((packed)) WEBOCX_HISTORY_REPLY_ST;

typedef struct WebOcx_History_Req
{
    int ChnId;
    int Action;     /* 0�ط�, 1���� */
    unsigned int StartTime;
    unsigned int EndTime;
}__attribute__((packed)) WEBOCX_HISTORY_REQ_ST;

/* �طſ��� */
typedef struct WebOcx_PlayBack_Ctrl
{
    int AuthenId;
    int CtrlCommd;     /* ��������,1:���� 2:��ͣ 3:ֹͣ 4:��ק 5:��֡ */
    int StartTime;      /* �طŲ���,��������Ϊ4��קʱ��Ч��Ϊ��קʱ��� */
    int EndStart;
}__attribute__((packed)) WEBOCX_PLAYBACK_CTRL_ST;

/* ��̨�������ݽṹ */
typedef struct WebOcx_Camera_Para
{
    int UserId;         /* �û�ID */
    int ChnID;          /* ͨ���� */
    int CameraAddr;     /* ��̨��ַ */
    int CameraProto;    /* ��̨Э�� */
    int SerialId;       /* ���ں� */
    int DataBit;        /* ��������λ��Ĭ��8 */
    int StopBit;        /* ����ֹͣλ��Ĭ��1 */
    int AuthenRule;     /* ��֤����-1:�ޣ�0:żУ�� 1:��У�飬Ĭ��-1 */
    int Speed;          /* �����ʣ�Ĭ��9600 */
}__attribute__((packed)) WEBOCX_CAMERA_PARA_ST;

/* ����������ݽṹ */
typedef struct WebOcx_Network_Para
{
    char IpAddr[32];
    char NetMask[32];
    char NetGate[32];       /* ���ص�ַ */
    char MultAddr[32];      /* �ಥ��ַ */
    char MacAddr[32];
    char DnsAddr1[32];
    char DnsAddr2[32];
}__attribute__((packed)) WEBOCX_NETWORK_PARA_ST;

/* ��֡�������ݽṹ */
typedef struct WebOcx_LostFrame
{
    char Enable;
    int ChnBit;
}__attribute__((packed)) WEBOCX_LOSTFRAM_ST;

/* Уʱ���ݽṹ */
typedef struct WebOcx_SysTime
{
    char Year;
    char Month;
    char Date;
    char Hour;
    char Minut;
    char Second;
}__attribute__((packed)) WEBOCX_SYSTIME_ST;


typedef struct TM_APP_TimePlayCond_t
{
	int chn;						        //¼��ͨ��:0-3
	unsigned int start_time;				//��"1970-01-01 00:00:00"��ʼ������
	unsigned int end_time;					//��"1970-01-01 00:00:00"��ʼ������

}__attribute__ ((__packed__))TM_APP_TimePlayCond_st;
typedef struct TM_APP_REC_PLAY_DRAG
{
    int           chn;                //�طŵ�ͨ����
    int           ctrl_cmd;           //¼��طſ��������� 0x20-��ͣ; 0x21-��������; 0x22-���; 0x23-����; 0x24-�϶�����
    unsigned int  start_time;
    unsigned int  end_time;
}__attribute__ ((__packed__)) TM_APP_REC_PLAY_DRAG_ST;
#define SEARCH_FILE_MAX_TIME_SEGMENT        10          //¼���ѯÿ�η��ص������Ŀ��

//¼���ѯ������Ŀ
typedef struct
{
	unsigned int start_time;				//��"1970-01-01 00:00:00"��ʼ������
	unsigned int end_time;					//��"1970-01-01 00:00:00"��ʼ������
    int len;                                //��¼�����ݶ�

}SEARCH_FILE_TIMESEG_ST;

typedef struct TM_APP_SEND_SVR
{
    char    iChnno;
    int     iEntriesNum;
    SEARCH_FILE_TIMESEG_ST struSearchFile[SEARCH_FILE_MAX_TIME_SEGMENT];
}__attribute__ ((__packed__)) TM_APP_SEND_SVR_ST;


//typedef *WebOcxMsgAction p;

#endif

