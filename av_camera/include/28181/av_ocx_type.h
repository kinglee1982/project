/******************************************************************************

  Copyright (C), 2001-2011, zhengzhou Tiamaes Technology Co., Ltd.

 ******************************************************************************
  File Name     : av_ocx_type.h
  Version       : Initial Draft
  Author        : xiaomengmeng
  Created       : 2014/12/23
  Last Modified :
  Description   : web控件通信数据结构
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
    char packid[4];     /* 4个字节包标识#### */
    unsigned int packagelen;    /* 数据包总长度包括CRC */
    unsigned int msgtype;
    unsigned int packagesequ;
    char data[OCX_RECV_MAX_SIZE];
}__attribute__ ((__packed__)) AV_WEBOCX_PACKAGE_ST;

#define WEBOCX_HEAD_COMM_LEN    (sizeof(AV_WEBOCX_PACKAGE_ST) - OCX_RECV_MAX_SIZE)

/* 内部通信数据包结构 */
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

/* 终端登录应答数据结构 */
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

/* 云台控制数据结构 */
typedef struct WebOcx_Camera_Ctrl
{
    int UserId;         /* 用户ID */
    int ChnId;          /* 通道ID */
    int CameraCtrl;     /* 云台控制命令 CMD  */
    int Speed;           /*速度*/
    int Action;         /* 动作  data */
    char Parar[32];     /*可为空，预置位时为名字*/
}__attribute__((packed)) WEBOCX_CAMERA_CTRL_ST;


/* 移动侦测布防时间数据结构 */
typedef struct WebOcx_Alert_Time
{
    char AlertStartHour;
    char AlertStartMinu;
    char AlertStartSecond;
    char AlertEndHour;
    char AlertEndMinu;
    char AlertEndSecond;
}__attribute__((packed)) WEBOCX_ALERT_TIME_ST;


/* 侦测区域坐标数据结构 */
typedef struct WebOcx_Area_Coords
{
    unsigned int x;
    unsigned int y;
}__attribute__((packed)) WEBOCX_AREA_COORDS_ST;
/* 侦测区域数据结构 */
typedef struct WebOcx_Detect_Area
{
    WEBOCX_AREA_COORDS_ST TopLeft;
    WEBOCX_AREA_COORDS_ST BottomRight;
}__attribute__((packed)) WEBOCX_DETECT_AREA_ST;
/* 移动侦测设置数据结构 */
typedef struct WebOcx_Md_Set
{
    //int UserId;             /* 用户ID */
    int ChnId;
    WEBOCX_ALERT_TIME_ST AlertTime[7][4]; /* 布防警戒时间 7天4个时间段 */
    char AlertEnable;       /* 警戒使能 */
    char AlarmEnable;
    char Senibility;        /* 灵敏度 */
    //char AlarmLinkIO;     /* 报警联动IO */
    char DetectInterval;    /* 侦测间隔时间 */
    int LinkSnap;           /* 联动抓拍 */
    int LinkRecord;         /* 联动录像 */
    int LinkOutput;         /* 联动输出 */
    char LinkRecDuration;   /* 联动录像持续时长 */
    char AlarmOutDuration;  /* 告警信息持续时长 */
    char BuzzEnable;        /* 蜂鸣器使能开关 */
    char LinkBuzzDuration;  /* 联动蜂鸣器持续时长 */
    char DetectAreaNum;     /* 侦测区域数 */
    WEBOCX_DETECT_AREA_ST stMdArea[4];
}__attribute__((packed)) WEBOCX_MD_SET_ST;


/* 告警节点设置数据结构 */
typedef struct WebOcx_Node_Set
{
    char NodeId;
    char AlarmStatus;           /* 告警状态 0:常开 1:常闭 */
    char AlarmEvent[32];
    WEBOCX_ALERT_TIME_ST AlertTime[7][4]; /* 布防警戒时间 */
    char AlertEnable;
    char AlarmEnable;
    char BuzzerEnable;
    char DetectInterval;/* 侦测间隔时间 */
    int LinkSnap;      /* 联动抓拍 */
    int LinkRecord;    /* 联动录像 */
    int LinkOutput;    /* 联动输出 */
    char LinkRecDuration;   /* 联动录像持续时长 */
    char AlarmOutDuration;
    char BuzzerDuration;
}__attribute__((packed)) WEBOCX_NODE_SET_ST;
/* 告警设置数据结构 */
typedef struct WebOcx_Alarm_Set
{
    int UserId;        /* 用户ID */
}__attribute__((packed)) WEBOCX_ARLARM_SET_ST;

/* 设备参数数据结构 */
typedef struct WebOcx_DevPara
{
    //int UserId;
    char DevId[32];         /* 字符串 */
    char DevName[64];       /* 字符串 */
    char PlatformDevId[40]; /* 字符串 */
    char Domain[32];        /* 域名 */
    char PassWd[32];        /* 注册密码 */
    int TimeOut;            /* 有效期限 */
    char SipSvrIp[32];      /* 字符串 */
    int SipSvrPort;
    char LocalIp[32];       /* 设备IP */
    int LocalPort;

}__attribute__((packed)) WEBOCX_DEVPARA_ST;


/* 通道参数数据结构 */
typedef struct WebOcx_Chn_Info
{
    int ChnID;
    char PlatformDevId[64];
    char ChnName[40];
    short CameraAddr;
    short ProtType;
}__attribute__((packed)) WEBOCX_CHN_INFO_ST;

/* 获取通道参数数据结构 */
typedef struct WebOcx_ChnPara
{
    int ChnCnt;
}__attribute__((packed)) WEBOCX_CHNPARA_ST;

typedef struct WebOcx_Real_View
{
    int UserId;        /* 用户ID */
    int ChnId;
    int StreamType;     /* 码流类型 0主码流视频 1主码流音频 2子码流视频 */
}__attribute__((packed)) WEBOCX_REAL_VIEW_ST;

typedef struct WebOcx_Stream_Guide
{
    int UserId;        /* 用户ID */
    int AuthenId;       /* 认证连接有效性ID */
}__attribute__((packed)) WEBOCX_STREAM_GUIDE_ST;

/* 历史视频录像查询数据结构 */
typedef struct WebOcx_History_Query
{
    int UserId;         /* 用户ID */
    int ChnId;         /* 通道 */
    char FileType;      /* 文件类型 */
    int StartTime;      /* 开始时间 */
    int EndTime;        /* 结束时间 */
}__attribute__((packed)) WEBOCX_HISTORY_QUERY_ST;

/* 历史视频录像查询数据结构 */
typedef struct WebOcx_History_Reply
{
    char FileName[16];      /* 文件名 */
    int StartTime;      /* 开始时间 */
    int EndTime;        /* 结束时间 */
}__attribute__((packed)) WEBOCX_HISTORY_REPLY_ST;

typedef struct WebOcx_History_Req
{
    int ChnId;
    int Action;     /* 0回放, 1下载 */
    unsigned int StartTime;
    unsigned int EndTime;
}__attribute__((packed)) WEBOCX_HISTORY_REQ_ST;

/* 回放控制 */
typedef struct WebOcx_PlayBack_Ctrl
{
    int AuthenId;
    int CtrlCommd;     /* 播放命令,1:播放 2:暂停 3:停止 4:拖拽 5:单帧 */
    int StartTime;      /* 回放参数,播放命令为4拖拽时有效，为拖拽时间点 */
    int EndStart;
}__attribute__((packed)) WEBOCX_PLAYBACK_CTRL_ST;

/* 云台参数数据结构 */
typedef struct WebOcx_Camera_Para
{
    int UserId;         /* 用户ID */
    int ChnID;          /* 通道号 */
    int CameraAddr;     /* 云台地址 */
    int CameraProto;    /* 云台协议 */
    int SerialId;       /* 串口号 */
    int DataBit;        /* 串口数据位，默认8 */
    int StopBit;        /* 串口停止位，默认1 */
    int AuthenRule;     /* 验证规则-1:无，0:偶校验 1:奇校验，默认-1 */
    int Speed;          /* 波特率，默认9600 */
}__attribute__((packed)) WEBOCX_CAMERA_PARA_ST;

/* 网络参数数据结构 */
typedef struct WebOcx_Network_Para
{
    char IpAddr[32];
    char NetMask[32];
    char NetGate[32];       /* 网关地址 */
    char MultAddr[32];      /* 多播地址 */
    char MacAddr[32];
    char DnsAddr1[32];
    char DnsAddr2[32];
}__attribute__((packed)) WEBOCX_NETWORK_PARA_ST;

/* 丢帧设置数据结构 */
typedef struct WebOcx_LostFrame
{
    char Enable;
    int ChnBit;
}__attribute__((packed)) WEBOCX_LOSTFRAM_ST;

/* 校时数据结构 */
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
	int chn;						        //录像通道:0-3
	unsigned int start_time;				//从"1970-01-01 00:00:00"开始的秒数
	unsigned int end_time;					//从"1970-01-01 00:00:00"开始的秒数

}__attribute__ ((__packed__))TM_APP_TimePlayCond_st;
typedef struct TM_APP_REC_PLAY_DRAG
{
    int           chn;                //回放的通道号
    int           ctrl_cmd;           //录像回放控制子命令 0x20-暂停; 0x21-设置速率; 0x22-快放; 0x23-慢放; 0x24-拖动控制
    unsigned int  start_time;
    unsigned int  end_time;
}__attribute__ ((__packed__)) TM_APP_REC_PLAY_DRAG_ST;
#define SEARCH_FILE_MAX_TIME_SEGMENT        10          //录像查询每次返回的最大条目数

//录像查询内容条目
typedef struct
{
	unsigned int start_time;				//从"1970-01-01 00:00:00"开始的秒数
	unsigned int end_time;					//从"1970-01-01 00:00:00"开始的秒数
    int len;                                //该录像数据段

}SEARCH_FILE_TIMESEG_ST;

typedef struct TM_APP_SEND_SVR
{
    char    iChnno;
    int     iEntriesNum;
    SEARCH_FILE_TIMESEG_ST struSearchFile[SEARCH_FILE_MAX_TIME_SEGMENT];
}__attribute__ ((__packed__)) TM_APP_SEND_SVR_ST;


//typedef *WebOcxMsgAction p;

#endif

