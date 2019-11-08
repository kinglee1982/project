/******************************************************************************

  Copyright (C), 2001-2011, zhengzhou Tiamaes Technology Co., Ltd.

 ******************************************************************************
  File Name     : av_ocx_def.h
  Version       : Initial Draft
  Author        : xiaomengmeng
  Created       : 2014/12/23
  Last Modified :
  Description   : web�ؼ��궨��
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

/* OCX-->AV��Ϣ���AV-->OCXΪԭ�����һ */
#define AV_WEBOCX_KEEPALIVE             0x0001      /* ���� */
#define AV_WEBOCX_LOGIN                 0x1001      /* ��¼ */
#define AV_WEBOCX_LOGOUT                0x1003      /* ע�� */
#define AV_WEBOCX_CAMERA_CTRL           0x1007      /* ��̨���� */
#define AV_WEBOCX_REAL_REQ              0x1009      /* ����ʵʱ���� */
#define AV_WEBOCX_REAL_STOP             0x1011      /* ֹͣʵʱ���� */
#define AV_WEBOCX_HISTORY_QUERY         0x1013      /* ��ѯ��ʷ���� */
#define AV_WEBOCX_HISTORY_REQ           0x1015      /* ������ʷ���� */
#define AV_WEBOCX_PLAYBACK              0x1017      /* �طſ��� */
#define AV_WEBOCX_STREAM_REQ            0x1019      /* ������Ƶ�� */
#define AV_WEBOCX_MD_GET                0x2001      /* �ƶ�����ȡ */
#define AV_WEBOCX_MD_SET                0x2003      /* �ƶ�������� */
#define AV_WEBOCX_ALARM_GET             0x2005      /* �澯��ȡ */
#define AV_WEBOCX_ALARM_SET             0x2007      /* �澯���� */
#define AV_WEBOCX_STATUS_REPORT         0x2009      /* ״̬�ϱ� */
#define AV_WEBOCX_DEV_PARA_GET          0x2101      /* �豸������ȡ */
#define AV_WEBOCX_CHN_PARA_GET          0x2103      /* ͨ��������ȡ */
#define AV_WEBOCX_DEV_PARA_SET          0x2105      /* �豸�������� */
#define AV_WEBOCX_CHN_PARA_SET          0x2107      /* ͨ���������� */
#define AV_WEBOCX_NETWORK_PARA_GET      0x2109      /* ���������ȡ */
#define AV_WEBOCX_NETWORK_PARA_SET      0x2111      /* ����������� */
#define AV_WEBOCX_CAMERA_PARA_GET       0x2113      /* ��̨������ȡ */
#define AV_WEBOCX_CAMERA_PARA_SET       0x2115      /* ��̨�������� */
#define AV_WEBOCX_FRAME_LOST_GET		0x2117		/* ��ȡ֡��ʧ���� */
#define AV_WEBOCX_FRAME_LOST_GET_RESP	0x2118		/* ��ȡ֡��ʧ����Ӧ�� */
#define AV_WEBOCX_FRAME_LOST_SET		0x2119
#define AV_WEBOCX_FRAME_LOST_SET_RESP	0x2120
#define AV_WEBOCX_TIMESET				0x2121
#define AV_WEBOCX_TIMESET_RESP			0x2122
#define AV_WEBOCX_28181LOGIN			0x2123		//28181��¼ע��
#define AV_WEBOCX_28181LOGIN_RESP		0x2124
#define AV_WEBOCX_28181LOGOUT			0x2125
#define AV_WEBOCX_28181LOGOUT_RESP		0x2126
//#define AV_WEBOCX_REMOTE_RESET          0x3101      /* Զ������ */
#define AV_WEBOCX_REMOTE_RESET          0x3001      /* Զ������ */


/* AV-->APP��Ϣ����,GATEת��ʱ����֧��0x13-0x2F�������֣�����ʱ��ע�� */
typedef enum
{
    AV_APP_CAMERA_CTRL = 0x13,      /* ��̨���� */
    AV_APP_MD_GET,
    AV_APP_MD_SET,                  /* 0x15 �ƶ�������� */
    AV_APP_ALARM_GET,               /* 0x16 �澯��ȡ */
    AV_APP_ALARM_SET,               /* 0x17 �澯���� */
    AV_APP_STATUS_REPORT,           /* 0x18 ״̬�ϱ� */
    AV_APP_DEV_PARA_GET,            /* 0x19 �豸������ȡ */
    AV_APP_CHN_PARA_GET,            /* 0x1A ͨ��������ȡ */
    AV_APP_DEV_PARA_SET,            /* 0x1B �豸�������� */
    AV_APP_CHN_PARA_SET,            /* 0x1C ͨ���������� */
    AV_APP_BACKPLAY_CTRL,           /* 0x1D �طſ��� */
    AV_APP_CAMERA_PARA_GET,         /* 0x1E ��̨������ȡ */
    AV_APP_CAMERA_PARA_SET,         /* 0x1F ��̨������ȡ */
    AV_APP_NETWORK_PARA_GET,        /* 0x20 ���������ȡ */
    AV_APP_NETWORK_PARA_SET,        /* 0x21 ���������ȡ */
    AV_GATE_REMOTE_RESET,           /* 0x22 �ͻ���Զ���豸���� */
    AV_APP_FRAME_LOST_GET,          /* 0x23 ��֡������ȡ */
    AV_APP_FRAME_LOST_SET,          /* 0x24 ��֡�������� */
    AV_APP_PLAT_LOGIN,
    AV_APP_PLAT_LOGOUT,             /* 0x26 28181ע�� */
    AV_APP_ALARM_REPORT             /* 0x27 �����ϱ� */
}AV_APP_CMD_E;

#define AV_TO_APP_MOD               0x36

#define AV_REAL_VIDEO_VIEW              0x01
#define AV_HISTORY_VIDEO_VIEW           0x02
#define AV_HISTORY_VIDEO_DOWNLOAD       0x03

#define HISTORY_VIDEO_ACTION(x) ((x) == 0 ? AV_HISTORY_VIDEO_VIEW : AV_HISTORY_VIDEO_DOWNLOAD)

/* ��ʷ���ݻطſ������� */
#define AV_PLAYBACK_PLAY    0x01    /* ���� */
#define AV_PLAYBACK_PAUSE   0x02    /* ��ͣ */
#define AV_PLAYBACK_REPLAY  0x03    /* ���²��� */
#define AV_PLAYBACK_STOP    0x04    /* ֹͣ */
#define AV_PLAYBACK_FRAME   0x05    /* ��֡ */
#define AV_PLAYBACK_DRAG    0x06    /* ��ק */
#define AV_PLAYBACK_RATE    0x07    /* ���� */

#define ERROR_VIDEO_PAUSE   0x02    /* ��Ƶ����ͣ���� */
#define ERROR_VIDEO_STOP    0x03    /* ��Ƶ��ֹͣ���� */

#define AV_PLAY_NORMAL      0x00
#define AV_PLAY_PAUSE       0x01    /* ��ͣ״̬ */
#define AV_PLAY_FRAME       0x02    /* ��֡ */

#define MAX_CHN_NUM 4
/*****************************************************/
#define AV_VIDEO_CONVERT_TIME_ZONE  8*60*60 //���ڵ���ʱ��-�豸������UTCʱ�䣬�������Ǳ���ʱ�����õ�ʱҪ����
#define VIDEO_MODULE_SUCCESS  0 /*ִ�гɹ�*/
#define VIDEO_MODULE_BUSY    -1 /*ģ��æ-�Ѿ�����ƵԤ���ٴδ򿪻����Ѿ��ر��ٴιرվ����ش�ֵ*/
#define VIDEO_MODULE_CLOSE   -2 /*�ù���ģ��״̬Ϊ�ر�-δ����״̬*/
#define VIDEO_MODULE_P_FAULT -3 /*������Ӧ�����߳�ʧ��*/
#define VIDEO_START_SEND    true /*����Ƶ��������*/
#define VIDEO_STOP_SEND     false /*�ر���Ƶ��������*/


#endif


