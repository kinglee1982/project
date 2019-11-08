/******************************************************************************

  Copyright (C), 2001-2011, zhengzhou Tiamaes Technology Co., Ltd.

 ******************************************************************************
  File Name     : av_ocx_def.h
  Version       : Initial Draft
  Author        : xiaomengmeng
  Created       : 2014/12/23
  Last Modified :
  Description   : web控件宏定义
  Function List :
  History       :
  1.Date        : 2014/12/23
    Author      : xiaomengmeng
    Modification: Created file

******************************************************************************/
#ifndef __AV_OCX_DEF_H__
#define __AV_OCX_DEF_H__

#define PRO_NAME    "av"

#define AV_EPOLL_SIZE 1024
#define OCX_RECV_MAX_SIZE		4096
#define AV_OCX_SVR_PORT		10088
#define   ERROR_SUCCESS     0
#define   ERROR_FAILED      1

/* OCX-->AV消息命令，AV-->OCX为原命令加一 */
#define AV_WEBOCX_KEEPALIVE             0x0001      /* 心跳 */
#define AV_WEBOCX_LOGIN                 0x1001      /* 登录 */
#define AV_WEBOCX_LOGOUT                0x1003      /* 注销 */
#define AV_WEBOCX_CAMERA_CTRL           0x1007      /* 云台控制 */
#define AV_WEBOCX_REAL_REQ              0x1009      /* 请求实时数据 */
#define AV_WEBOCX_REAL_STOP             0x1011      /* 停止实时数据 */
#define AV_WEBOCX_HISTORY_QUERY         0x1013      /* 查询历史数据 */
#define AV_WEBOCX_HISTORY_REQ           0x1015      /* 请求历史数据 */
#define AV_WEBOCX_PLAYBACK              0x1017      /* 回放控制 */
#define AV_WEBOCX_STREAM_REQ            0x1019      /* 请求视频流 */
#define AV_WEBOCX_MD_GET                0x2001      /* 移动侦测获取 */
#define AV_WEBOCX_MD_SET                0x2003      /* 移动侦测设置 */
#define AV_WEBOCX_ALARM_GET             0x2005      /* 告警获取 */
#define AV_WEBOCX_ALARM_SET             0x2007      /* 告警设置 */
#define AV_WEBOCX_STATUS_REPORT         0x2009      /* 状态上报 */
#define AV_WEBOCX_DEV_PARA_GET          0x2101      /* 设备参数获取 */
#define AV_WEBOCX_CHN_PARA_GET          0x2103      /* 通道参数获取 */
#define AV_WEBOCX_DEV_PARA_SET          0x2105      /* 设备参数设置 */
#define AV_WEBOCX_CHN_PARA_SET          0x2107      /* 通道参数设置 */
#define AV_WEBOCX_NETWORK_PARA_GET      0x2109      /* 网络参数获取 */
#define AV_WEBOCX_NETWORK_PARA_SET      0x2111      /* 网络参数设置 */
#define AV_WEBOCX_CAMERA_PARA_GET       0x2113      /* 云台参数获取 */
#define AV_WEBOCX_CAMERA_PARA_SET       0x2115      /* 云台参数设置 */
#define AV_WEBOCX_FRAME_LOST_GET		0x2117		/* 获取帧丢失配置 */
#define AV_WEBOCX_FRAME_LOST_GET_RESP	0x2118		/* 获取帧丢失配置应答 */
#define AV_WEBOCX_FRAME_LOST_SET		0x2119
#define AV_WEBOCX_FRAME_LOST_SET_RESP	0x2120
#define AV_WEBOCX_TIMESET				0x2121
#define AV_WEBOCX_TIMESET_RESP			0x2122
#define AV_WEBOCX_28181LOGIN			0x2123		//28181登录注销
#define AV_WEBOCX_28181LOGIN_RESP		0x2124
#define AV_WEBOCX_28181LOGOUT			0x2125
#define AV_WEBOCX_28181LOGOUT_RESP		0x2126
//#define AV_WEBOCX_REMOTE_RESET          0x3101      /* 远程重启 */
#define AV_WEBOCX_REMOTE_RESET          0x3001      /* 远程重启 */


/* AV-->APP消息命令,GATE转发时，仅支持0x13-0x2F的命令字，扩充时请注意 */
typedef enum
{
    AV_APP_CAMERA_CTRL = 0x13,      /* 云台控制 */
    AV_APP_MD_GET,
    AV_APP_MD_SET,                  /* 0x15 移动侦测设置 */
    AV_APP_ALARM_GET,               /* 0x16 告警获取 */
    AV_APP_ALARM_SET,               /* 0x17 告警设置 */
    AV_APP_STATUS_REPORT,           /* 0x18 状态上报 */
    AV_APP_DEV_PARA_GET,            /* 0x19 设备参数获取 */
    AV_APP_CHN_PARA_GET,            /* 0x1A 通道参数获取 */
    AV_APP_DEV_PARA_SET,            /* 0x1B 设备参数设置 */
    AV_APP_CHN_PARA_SET,            /* 0x1C 通道参数设置 */
    AV_APP_BACKPLAY_CTRL,           /* 0x1D 回放控制 */
    AV_APP_CAMERA_PARA_GET,         /* 0x1E 云台参数获取 */
    AV_APP_CAMERA_PARA_SET,         /* 0x1F 云台参数获取 */
    AV_APP_NETWORK_PARA_GET,        /* 0x20 网络参数获取 */
    AV_APP_NETWORK_PARA_SET,        /* 0x21 网络参数获取 */
    AV_GATE_REMOTE_RESET,           /* 0x22 客户端远端设备重启 */
    AV_APP_FRAME_LOST_GET,          /* 0x23 丢帧报警获取 */
    AV_APP_FRAME_LOST_SET,          /* 0x24 丢帧报警设置 */
    AV_APP_PLAT_LOGIN,
    AV_APP_PLAT_LOGOUT,             /* 0x26 28181注销 */
    AV_APP_ALARM_REPORT             /* 0x27 报警上报 */
}AV_APP_CMD_E;

#define AV_TO_APP_MOD               0x36

#define AV_REAL_VIDEO_VIEW              0x01
#define AV_HISTORY_VIDEO_VIEW           0x02
#define AV_HISTORY_VIDEO_DOWNLOAD       0x03

#define HISTORY_VIDEO_ACTION(x) ((x) == 0 ? AV_HISTORY_VIDEO_VIEW : AV_HISTORY_VIDEO_DOWNLOAD)

/* 历史数据回放控制命令 */
#define AV_PLAYBACK_PLAY    0x01    /* 播放 */
#define AV_PLAYBACK_PAUSE   0x02    /* 暂停 */
#define AV_PLAYBACK_REPLAY  0x03    /* 重新播放 */
#define AV_PLAYBACK_STOP    0x04    /* 停止 */
#define AV_PLAYBACK_FRAME   0x05    /* 单帧 */
#define AV_PLAYBACK_DRAG    0x06    /* 拖拽 */
#define AV_PLAYBACK_RATE    0x07    /* 变速 */

#define ERROR_VIDEO_PAUSE   0x02    /* 视频流暂停传输 */
#define ERROR_VIDEO_STOP    0x03    /* 视频流停止传输 */

#define AV_PLAY_NORMAL      0x00
#define AV_PLAY_PAUSE       0x01    /* 暂停状态 */
#define AV_PLAY_FRAME       0x02    /* 单帧 */

#define MAX_CHN_NUM 4
/*****************************************************/
#define AV_VIDEO_CONVERT_TIME_ZONE  8*60*60 //用于调整时区-设备现在是UTC时间，服务器是北京时间再用到时要调整
#define VIDEO_MODULE_SUCCESS  0 /*执行成功*/
#define VIDEO_MODULE_BUSY    -1 /*模块忙-已经打开视频预览再次打开或者已经关闭再次关闭均返回此值*/
#define VIDEO_MODULE_CLOSE   -2 /*该功能模块状态为关闭-未启用状态*/
#define VIDEO_MODULE_P_FAULT -3 /*创建相应处理线程失败*/
#define VIDEO_START_SEND    true /*打开视频码流发送*/
#define VIDEO_STOP_SEND     false /*关闭视频码流发送*/


#endif


