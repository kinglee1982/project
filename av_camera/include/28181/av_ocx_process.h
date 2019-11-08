/******************************************************************************

  Copyright (C), 2001-2011, zhengzhou Tiamaes Technology Co., Ltd.

 ******************************************************************************
  File Name     : av_ocx_process.h
  Version       : Initial Draft
  Author        : xiaomengmeng
  Created       : 2014/12/22
  Last Modified :
  Description   : av_ocx_process.cpp header file
  Function List :
  History       :
  1.Date        : 2014/12/22
    Author      : xiaomengmeng
    Modification: Created file

******************************************************************************/


#ifndef __AV_OCX_PROCESS_H__
#define __AV_OCX_PROCESS_H__

#include <iostream>
#include <list>
#include <map>


//#include "av_struct.h"
//#include "av_main_class.h"
#include "DataBufOcx.h"
#include "av_ocx_def.h"
#include "av_ocx_type.h"

using namespace std;

class WebOcxClass;

typedef int (WebOcxClass::*WebOcxMsgAction)(int, char *, int);
typedef struct WebOcx_Stream_Ctrl
{
    int VideoViewType;  /* 视频类型，1实时预览，2历史回放 */
    int fd;             /* 流媒体传输描述符 */
    int ChnId;
    int StreamType;     /* 码流类型 0主码流视频 1主码流音频 2子码流视频 */
    int run;            /* 流通道启动和退出 */
    int pause;          /* 流通道暂停标志, 1:暂停 0:正常播放 2单帧 */
    pthread_t Pthread_id;
    void *pMainClass_m;
    int key;
    struct TM_APP_TimePlayCond_t strfs;
}__attribute__((packed)) WEBOCX_STREAM_CTRL_ST;


class WebOcxClass
{
public:

    WebOcxClass() ;
    ~WebOcxClass();

    int AV_OCX_StartWebOcxPthread(void);
    static void* AV_OCX_SvrEpollPthread(void *args);
    int AV_OCX_SocketInit();
    int AV_OCX_EpollInit(int listen,int* pEpollFd);
    void setnonblocking(int sock);
    int AV_OCX_GetEth0Ip(void *sinaddr);
    int AV_OCX_Accept(int ListenFd, int EpollFd);
    int AV_OCX_RecvMsgProc(int EpollFd, int ClientFd);
    int AV_OCX_KeepAlive(int fd, char *data, int len);
    int AV_OCX_LogIn(int fd, char *data, int len);
    int AV_OCX_DataPushBuf(char *data, int len);
    int AV_OCX_DataCmdProc(int fd, char *head, int datalen);
    int AV_OCX_ReadCycData(int fd);
    int AV_OCX_PthreadInit(void);
    int AV_OCX_SendToWeb(unsigned int CodeId, char *data, int len);
    int AV_OCX_StreamGuide(int fd, char *data, int len);
    int AV_OCX_VideoStreamCtrl(void *pstCtrl);
    int AV_OCX_StatusReport(int fd, char *data, int len);
    int AV_OCX_RealDataReq(int fd, char *data, int len);
    int AV_OCX_RealDataStop(int fd, char *data, int len);
    int AV_OCX_LogOut(int fd, char *data, int len);
    int AV_OCX_MdSet(int fd, char *data, int len);
    int AV_OCX_PlayBack(int fd, char *data, int len);
    int AV_OCX_HistoryQuery(int fd, char *data, int len);
    int AV_OCX_HistoryReq(int fd, char *data, int len);
    int AV_OCX_DevParaGet(int fd, char *data, int len);
    int AV_OCX_DevParaSet(int fd, char *data, int len);
    int AV_OCX_AlarmSet(int fd, char *data, int len);
    int AV_OCX_CameraCtrl(int fd, char *data, int len);
    int AV_OCX_ChnParaGet(int fd, char *data, int len);
    int AV_OCX_ChnParaSet(int fd, char *data, int len);
    int AV_OCX_RemoteReset(int fd, char *data, int len);
    int AV_OCX_SendToGate(char DestId,int cmd, char *data, int len);
    static void* AV_OCX_SendVidePthread(void *args);
    int AV_OCX_StartSendVidePthread(void *pstStreamCtrl);
    int AV_OCX_SendFrame(int fd, unsigned char *pBuf);
    int AV_OCX_VideoLinkSendToWeb(unsigned short type, unsigned short usEvent, char *pcData, int iDataLen);
    int AV_OCX_StopStreamView(WEBOCX_STREAM_CTRL_ST *pStreamCtrl, bool Flag);
    int AV_OCX_AlarmGet(int fd, char *data, int len);
    int AV_OCX_MdGet(int fd, char *data, int len);
    int AV_OCX_CameraParaGet(int fd, char *data, int len);
    int AV_OCX_CameraParaSet(int fd, char *data, int len);
    int AV_OCX_NetWorkParaGet(int fd, char *data, int len);
    int AV_OCX_NetWorkParaSet(int fd, char *data, int len);
    int AV_OCX_EpollWaitDel(int EpollFd, int ClientFd, bool ChnMapFlag);
    int AV_OCX_VideoStreamTimeo(void *pstCtrl, bool bReadDataFlag, int *times);
    int AV_OCX_StopStream(WEBOCX_STREAM_CTRL_ST *pStreamCtrl);
    int AV_OCX_SysTimeSet(int fd, char *data, int len);
    int AV_OCX_FrameLostSet(int fd, char *data, int len);
    int AV_OCX_FrameLostGet(int fd, char *data, int len);
    int AV_OCX_PlatLogIn(int fd, char *data, int len);
    int AV_OCX_PlatLogOut(int fd, char *data, int len);
    void AV_OCX_DebugFdMap(void);

private:
    void *logHandler_m;
    int WebOcxFd;
    int EpollWaitFd;
    DataBufOcx *OcxCycleBuf;
    void *mainProces_m;
    int StreamFd;
    char *pcFrameBuf;
    int iPlayIndex;
    map<int, WebOcxMsgAction> *OcxMap;
    map<int, WEBOCX_STREAM_CTRL_ST*> StreamFdMap;
    map<int, WEBOCX_STREAM_CTRL_ST*> StreamChnMap;
    int ChnFd[32];
};



#endif /* __AV_OCX_PROCESS_H__ */
