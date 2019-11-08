/******************************************************************************

  Copyright (C), 2001-2011, zhengzhou Tiamaes Technology Co., Ltd.

 ******************************************************************************
  File Name     : av_ocx_process.cpp
  Version       : Initial Draft
  Author        : xiaomengmeng
  Created       : 2014/12/22
  Last Modified :
  Description   : 与WebOcx客户端通信源文件
  Function List :
  History       :
  1.Date        : 2014/12/22
    Author      : xiaomengmeng
    Modification: Created file

******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <map>
#include <arpa/inet.h>
#include <time.h>

#include "BaseClass.h"
#include "av_public_head.h"
#include "av_crc16.h"
#include "3g_packet.h"
#include "av_ocx_def.h"
#include "av_ocx_type.h"
#include "av_ocx_process.h"
#include "DataBufOcx.h"
#include "TM_APPEvent.h"
#include "TM_Alarm.h"

int tmkj_open_video(int) {return -1;}
int tmkj_close_video(int) {return -1;}
int tmkj_start_time_play(int *player_index_p,const struct TM_APP_TimePlayCond_t *prfs_p){return -1;}
int tmkj_stop_time_play(int) {return -1;}
int tmkj_rec_play_drag(TM_APP_REC_PLAY_DRAG_ST * pDragTime){return -1;}
int tmkj_rec_file_serch(struct TM_APP_TimePlayCond_t *prfs, TM_APP_SEND_SVR_ST *pstResult){return -1;}


WebOcxClass::WebOcxClass()
{
	OcxMap = new map<int, WebOcxMsgAction>;
	map<int, WebOcxMsgAction> m;

	pair<map<int, WebOcxMsgAction>::iterator, bool> ret =
		OcxMap->insert(make_pair(AV_WEBOCX_KEEPALIVE, &WebOcxClass::AV_OCX_KeepAlive));
	cout<<PRO_NAME<<"AV_WEBOCX_KEEPALIVE  "<<ret.second<<endl;

	ret = OcxMap->insert(make_pair(AV_WEBOCX_LOGIN, &WebOcxClass::AV_OCX_LogIn));
	cout<<PRO_NAME<<"AV_WEBOCX_LOGIN  "<<ret.second<<endl;

 	ret = OcxMap->insert(make_pair(AV_WEBOCX_LOGOUT, &WebOcxClass::AV_OCX_LogOut));
	cout<<PRO_NAME<<"AV_WEBOCX_LOGOUT  "<<ret.second<<endl;

  	ret = OcxMap->insert(make_pair(AV_WEBOCX_CAMERA_CTRL, &WebOcxClass::AV_OCX_CameraCtrl));
	cout<<PRO_NAME<<"AV_WEBOCX_CAMERA_CTRL  "<<ret.second<<endl;

	ret = OcxMap->insert(make_pair(AV_WEBOCX_REAL_REQ, &WebOcxClass::AV_OCX_RealDataReq));
	cout<<PRO_NAME<<"AV_WEBOCX_REAL_REQ  "<<ret.second<<endl;

	ret = OcxMap->insert(make_pair(AV_WEBOCX_REAL_STOP, &WebOcxClass::AV_OCX_RealDataStop));
	cout<<PRO_NAME<<"AV_WEBOCX_REAL_STOP  "<<ret.second<<endl;

	ret = OcxMap->insert(make_pair(AV_WEBOCX_HISTORY_QUERY, &WebOcxClass::AV_OCX_HistoryQuery));
	cout<<PRO_NAME<<"AV_WEBOCX_HISTORY_QUERY  "<<ret.second<<endl;

	ret = OcxMap->insert(make_pair(AV_WEBOCX_HISTORY_REQ, &WebOcxClass::AV_OCX_HistoryReq));
	cout<<PRO_NAME<<"AV_WEBOCX_HISTORY_REQ  "<<ret.second<<endl;

	ret = OcxMap->insert(make_pair(AV_WEBOCX_PLAYBACK, &WebOcxClass::AV_OCX_PlayBack));
	cout<<PRO_NAME<<"AV_WEBOCX_PLAYBACK  "<<ret.second<<endl;

	ret = OcxMap->insert(make_pair(AV_WEBOCX_STREAM_REQ, &WebOcxClass::AV_OCX_StreamGuide));
	cout<<PRO_NAME<<"AV_WEBOCX_STREAM_REQ  "<<ret.second<<endl;

	ret = OcxMap->insert(make_pair(AV_WEBOCX_MD_GET, &WebOcxClass::AV_OCX_MdGet));
	cout<<PRO_NAME<<"AV_OCX_MdGet  "<<ret.second<<endl;

	ret = OcxMap->insert(make_pair(AV_WEBOCX_MD_SET, &WebOcxClass::AV_OCX_MdSet));
	cout<<PRO_NAME<<"AV_WEBOCX_MD_SET  "<<ret.second<<endl;

	ret = OcxMap->insert(make_pair(AV_WEBOCX_ALARM_GET, &WebOcxClass::AV_OCX_AlarmGet));
	cout<<PRO_NAME<<"AV_OCX_AlarmGet  "<<ret.second<<endl;

	ret = OcxMap->insert(make_pair(AV_WEBOCX_ALARM_SET, &WebOcxClass::AV_OCX_AlarmSet));
	cout<<PRO_NAME<<"AV_WEBOCX_ALARM_SET  "<<ret.second<<endl;

	ret = OcxMap->insert(make_pair(AV_WEBOCX_STATUS_REPORT, &WebOcxClass::AV_OCX_StatusReport));
	cout<<PRO_NAME<<"AV_WEBOCX_STATUS_REPORT  "<<ret.second<<endl;

	ret = OcxMap->insert(make_pair(AV_WEBOCX_DEV_PARA_GET, &WebOcxClass::AV_OCX_DevParaGet));
	cout<<PRO_NAME<<"AV_WEBOCX_DEV_PARA_GET  "<<ret.second<<endl;

	ret = OcxMap->insert(make_pair(AV_WEBOCX_CHN_PARA_GET, &WebOcxClass::AV_OCX_ChnParaGet));
	cout<<PRO_NAME<<"AV_WEBOCX_CHN_PARA_GET  "<<ret.second<<endl;

	ret = OcxMap->insert(make_pair(AV_WEBOCX_DEV_PARA_SET, &WebOcxClass::AV_OCX_DevParaSet));
	cout<<PRO_NAME<<"AV_WEBOCX_DEV_PARA_SET  "<<ret.second<<endl;

	ret = OcxMap->insert(make_pair(AV_WEBOCX_CHN_PARA_SET, &WebOcxClass::AV_OCX_ChnParaSet));
	cout<<PRO_NAME<<"AV_WEBOCX_CHN_PARA_SET  "<<ret.second<<endl;



	ret = OcxMap->insert(make_pair(AV_WEBOCX_NETWORK_PARA_GET, &WebOcxClass::AV_OCX_NetWorkParaGet));
	cout<<PRO_NAME<<"AV_OCX_NetWorkParaGet  "<<ret.second<<endl;

	ret = OcxMap->insert(make_pair(AV_WEBOCX_NETWORK_PARA_SET, &WebOcxClass::AV_OCX_NetWorkParaSet));
	cout<<PRO_NAME<<"AV_OCX_NetWorkParaSet  "<<ret.second<<endl;

	ret = OcxMap->insert(make_pair(AV_WEBOCX_CAMERA_PARA_GET, &WebOcxClass::AV_OCX_CameraParaGet));
	cout<<PRO_NAME<<"AV_OCX_CameraParaGet  "<<ret.second<<endl;

	ret = OcxMap->insert(make_pair(AV_WEBOCX_CAMERA_PARA_SET, &WebOcxClass::AV_OCX_CameraParaSet));
	cout<<PRO_NAME<<"AV_OCX_CameraParaSet  "<<ret.second<<endl;

	ret = OcxMap->insert(make_pair(AV_WEBOCX_FRAME_LOST_GET, &WebOcxClass::AV_OCX_FrameLostGet));
	cout<<PRO_NAME<<"AV_OCX_FrameLostGet  "<<ret.second<<endl;

	ret = OcxMap->insert(make_pair(AV_WEBOCX_FRAME_LOST_SET, &WebOcxClass::AV_OCX_FrameLostSet));
	cout<<PRO_NAME<<"AV_OCX_FrameLostSet  "<<ret.second<<endl;


	ret = OcxMap->insert(make_pair(AV_WEBOCX_TIMESET, &WebOcxClass::AV_OCX_SysTimeSet));
	cout<<PRO_NAME<<"AV_OCX_SysTimeSet  "<<ret.second<<endl;

	ret = OcxMap->insert(make_pair(AV_WEBOCX_28181LOGIN, &WebOcxClass::AV_OCX_PlatLogIn));
	cout<<PRO_NAME<<"AV_OCX_PlatLogIn  "<<ret.second<<endl;

	ret = OcxMap->insert(make_pair(AV_WEBOCX_28181LOGOUT, &WebOcxClass::AV_OCX_PlatLogOut));
	cout<<PRO_NAME<<"AV_OCX_SysTimeSet  "<<ret.second<<endl;

	ret = OcxMap->insert(make_pair(AV_WEBOCX_REMOTE_RESET, &WebOcxClass::AV_OCX_RemoteReset));
	cout<<PRO_NAME<<"AV_WEBOCX_REMOTE_RESET  "<<ret.second<<endl;
	//(*OcxMap)[(AV_WEBOCX_KEEPALIVE+1)] = &WebOcxClass::AV_OCX_KeepAlive;

	pcFrameBuf = new char[( 256 << 10 ) + sizeof(IFLY_MediaFRAMEHDR_t) + WEBOCX_HEAD_COMM_LEN];
	if ( !pcFrameBuf )
	{
		exit(-1);
	}

}

WebOcxClass::~WebOcxClass()
{
    if ( pcFrameBuf )
    {
        delete [] pcFrameBuf;
    }
    if ( OcxMap )
	{
		delete OcxMap;
		OcxMap = NULL;
	}
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_StartWebOcxPthread
 Description  : 启动与插件控件通信线程
 Input        : void
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/22
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_StartWebOcxPthread(void)
{
	int iRet = -1;
	pthread_t p_id;
	cout<<PRO_NAME<<"AV_OCX_StartWebOcxPthread  "<<endl;

	/*创建与服务器交互的线程*/
	iRet = pthread_create(&p_id, NULL, WebOcxClass::AV_OCX_SvrEpollPthread, this);
	if ( iRet < 0 )
	{
		return iRet;
	}
	//usleep(20*1000);
	TM_USleep(0,20*1000);
	return 0;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_SvrEpollPthread
 Description  : 启动Epoll线程
 Input        : void *args
 Output       : None
 Return Value : void*

  History        :
  1.Date         : 2014/12/22
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
void* WebOcxClass::AV_OCX_SvrEpollPthread(void *args)
{
	pthread_detach(pthread_self());
	WebOcxClass *uiP = (WebOcxClass *)args;
	int ListenFd 	= -1;
	int EpollFd 	= -1;
	int nfds 		= 0;
	int i;
	unsigned int uiRet		= ERROR_FAILED;
	struct epoll_event events[AV_EPOLL_SIZE];
	//struct TM_APP_TimePlayCond_t stTmprfs;

	cout<<PRO_NAME<<"AV_OCX_SvrEpollPthread  "<<endl;

	ListenFd = uiP->AV_OCX_SocketInit();
	if (ListenFd <= 0)
	{
		cout<<PRO_NAME<<"AV_OCX_SocketInit fail listen fd %d "<<ListenFd<<endl;
	    exit(-1);
	}

	cout<<PRO_NAME<<"AV_OCX_SvrEpollPthread listen fd %d "<<ListenFd<<endl;
	uiRet = uiP->AV_OCX_EpollInit(ListenFd, &EpollFd);
	if (uiRet != ERROR_SUCCESS)
	{
		cout<<PRO_NAME<<"AV_OCX_EpollInit fail uiRet fd %d "<<uiRet<<endl;
	    exit(-1);
	}

	uiP->AV_OCX_PthreadInit();

	while(1)
	{
		memset(events, 0, sizeof(events));
	    nfds = epoll_wait(EpollFd, events, 20, 2);
		for (i = 0; i < nfds; i++)
		{
		    if (events[i].data.fd == ListenFd)
	    	{
	    	    /* 有新连接 */
				uiP->AV_OCX_Accept(ListenFd, EpollFd);
			}
			else if (events[i].events & EPOLLIN)
			{
				cout<<"new Data fd "<<events[i].data.fd<<endl;

			    /* 有数据接收 */
				uiP->AV_OCX_RecvMsgProc(EpollFd, events[i].data.fd);
			}
		}

		//stTmprfs.chn = 0;
		//stTmprfs.start_time = 1420973823;
		//stTmprfs.end_time = 1420973904;
		//tmkj_start_time_play(&(uiP->iPlayIndex), &stTmprfs);
		//printf("AV_OCX_SvrEpollPthread tmkj_start_time_play\n");

	}

    return NULL;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_SocketInit
 Description  : 初始化服务器监听socket
 Input        : None
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/22
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_SocketInit()
{
	int iRet;
	int sockfd;
	struct sockaddr_in serv_adr;

	bzero(&serv_adr,sizeof(serv_adr));

	/* 创建本地socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd <= 0)
	{
	    return sockfd;
	}

	/* 绑定监听口 */
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_port = htons(AV_OCX_SVR_PORT);
	//iRet = AV_OCX_GetEth0Ip(serv_adr);
	//if (iRet < 0)
	//{
	//	close(sockfd);
	//    return -1;
	//}

	/* 绑定所有地址端口 */
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	setnonblocking(sockfd);

	 //能够让close立即返回，避免产生TIME_WAIT状态
    struct linger m_sLinger;
	m_sLinger.l_onoff = 1;
	m_sLinger.l_linger = 0;
	iRet = setsockopt(sockfd,SOL_SOCKET,SO_LINGER,(char*)&m_sLinger,sizeof(struct linger));
    if(iRet < 0)
    {
        printf("AV Camera: setsockopt error! ret = %d\n", iRet);
    }

	iRet = bind(sockfd, (struct sockaddr *)&serv_adr, sizeof(struct sockaddr));
	if (iRet != ERROR_SUCCESS)
	{
		close(sockfd);
		return -1;
	}

	iRet = listen(sockfd, 255);
	if (iRet != ERROR_SUCCESS)
	{
		close(sockfd);
		return -1;
	}

    return sockfd;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_EpollInit
 Description  : 初始化EPOLL
 Input        : int listen
                int* pEpollFd
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/26
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_EpollInit(int listen,int* pEpollFd)
{
	int Epollfd;
	struct epoll_event ev;

	/* 创建epoll监听fd */
	Epollfd = epoll_create(AV_EPOLL_SIZE);
	if (Epollfd <= 0)
	{
	    return ERROR_FAILED;
	}

	ev.data.fd = listen;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(Epollfd, EPOLL_CTL_ADD, listen, &ev);

	*pEpollFd = Epollfd;
	EpollWaitFd = Epollfd;
	return ERROR_SUCCESS;

}

/*****************************************************************************
 Prototype    : WebOcxClass.setnonblocking
 Description  : 设置描述符为非阻塞
 Input        : int sock
 Output       : None
 Return Value : void

  History        :
  1.Date         : 2014/12/26
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
void WebOcxClass::setnonblocking(int sock)
{
     int opts;

     opts = fcntl(sock, F_GETFL);
     if (opts < 0)
     {
           perror("fcntl(sock,GETFL)");
           exit(1);
     }

     opts = opts | O_NONBLOCK;
     if (fcntl(sock, F_SETFL, opts) < 0)
     {
           perror("fcntl(sock,SETFL,opts)");
           exit(1);
     }
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_GetEth0Ip
 Description  : 获取设备ETH0网卡网络地址
 Input        : struct sockaddr_in *sinaddr
 Output       : None
 Return Value : void

  History        :
  1.Date         : 2014/12/22
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_GetEth0Ip(void *sinaddr)
{

	int socket_fd;
	struct sockaddr_in *sin;
	struct ifreq ifr;
	struct sockaddr_in *sockaddr = (struct sockaddr_in *)sinaddr;

	socket_fd = socket(AF_INET,SOCK_DGRAM,0);
	if(socket_fd ==-1)
	{
		perror("socket error!\n");
		return -1;
	}

	strcpy(ifr.ifr_name,"eth0");

	//这里要特别的注意，根据自己的系统的不同会有所差异有可能会是“eth0“。 如果出现错误提示 No such device 可能是这里的问题

	if(ioctl(socket_fd, SIOCGIFADDR, &ifr) < 0)
	{
		perror("ioctl error\n");
		close(socket_fd);

		return -1;
	}
	else
	{
		sin = (struct sockaddr_in *)&(ifr.ifr_addr);
		printf("current IP = %s\n",inet_ntoa(sin->sin_addr));
	}

	close(socket_fd);

	if (NULL == inet_ntoa(sin->sin_addr))
	{
	    return -1;
	}

	memcpy((void *)&sockaddr->sin_addr, (void *)&sin->sin_addr, sizeof(sin->sin_addr));
	return 0;

}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_Accept
 Description  : 建立连接
 Input        :   int ListenFd
                  int EpollFd
 Output       : None
 Return Value : unsigned int

  History        :
  1.Date         : 2014/12/22
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_Accept(int ListenFd, int EpollFd)
{
	int CurrClient;
	socklen_t socklen = sizeof(struct sockaddr_in);
 	struct sockaddr_in clientaddr;
	struct epoll_event ev;

	CurrClient = accept(ListenFd,(struct sockaddr*)&clientaddr, &socklen);
	if (CurrClient < 0)
	{
	    return ERROR_FAILED;
	}


	/* 新连接加入到epoll监控 */
	setnonblocking(CurrClient);
	ev.data.fd = CurrClient;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(EpollFd, EPOLL_CTL_ADD, CurrClient, &ev);

	cout<<"new accept CurrClient "<<CurrClient<<endl;

    return ERROR_SUCCESS;
}
/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_RecvMsgProc
 Description  : 接收套接字数据
 Input        : int EpollFd
                int ClientFd
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/26
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_RecvMsgProc(int EpollFd, int ClientFd)
{
	int nreads 		= 0;
	char buf[2048];


	nreads = read(ClientFd, buf, 2048);
	if (errno == EINTR)
	{
		cout<<"read errno == EINTR"<<endl;
		//TM_SCHEDULE_Printf(" ERR [%d:%s] !", errno, strerror(errno));
	}

	if (nreads <= 0)
	{
		cout<<"read nreads  "<<nreads<<endl;

		AV_OCX_EpollWaitDel(EpollFd, ClientFd, TRUE);
	}
	else if((nreads > 0) && (nreads < 2048))
	{
		cout<<"read nreads  "<<nreads<<endl;

		debug_print("AV_OCX_RecvMsgProc",buf,nreads);
		WebOcxFd = ClientFd;
		AV_OCX_DataPushBuf(buf, nreads);
		AV_OCX_ReadCycData(ClientFd);
	}

    return ERROR_SUCCESS;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_EpollWaitDel
 Description  : 从EPOLL监听描述符集中，删除监听口
 Input        : int EpollFd
                int ClientFd
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2015/1/3
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_EpollWaitDel(int EpollFd, int ClientFd, bool ChnMapFlag)
{
	struct epoll_event ev;
	int occurs;
	int key;
	map<int, WEBOCX_STREAM_CTRL_ST*>::iterator iter;
	map<int, WEBOCX_STREAM_CTRL_ST*>::iterator iter2;

	ev.data.fd = ClientFd;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(EpollFd, EPOLL_CTL_DEL, ClientFd, &ev);

	iter = StreamFdMap.begin();

	for (;iter != StreamFdMap.end();iter++)
	{
		if (iter->second->fd != ClientFd)
		{
		    continue;
		}


		if (StreamChnMap.count(iter->second->ChnId) && (ChnMapFlag))
		{
			cout<<"Close fd Chid "<<iter->second->ChnId<<endl;
		    StreamChnMap.erase(iter->second->ChnId);
			ChnFd[iter->second->ChnId] = -1;
		}

		AV_OCX_StopStream(iter->second);

		key = iter->second->key;
		delete (iter->second);
		StreamFdMap.erase(key);
		occurs = StreamFdMap.count(key);
		cout<<"AV_OCX_EpollWaitDel stop occurs "<<occurs<<" size "<<StreamFdMap.size()<<endl;

		break;
	}

	if (-1 != ClientFd)
	{
		close(ClientFd);
	}

    return ERROR_SUCCESS;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_ReadCycData
 Description  : 读取循环缓存中数据
 Input        : void
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/25
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_ReadCycData(int fd)
{
	char ReadBuf[2048];
	int iRet = -1;
	//short crc16_data;
	//short recv_crc;

	while (1)
	{
		/*取数据并处理数据*/
		iRet = OcxCycleBuf->PopData(ReadBuf, sizeof(ReadBuf));
		if (iRet <= 0)
		{
		    if (iRet != BUF_EMPUTY)
		    {
				OcxCycleBuf->PrintErrMsg(PRO_NAME, iRet);
				cout<<"PopData err "<<iRet<< endl;
		    }

			break;
		}
		OcxCycleBuf->DebugPrintHex(ReadBuf, iRet);

		#if 0
		/*校验数据*/
		crc16_data = crc16((unsigned char *)ReadBuf, iRet-2);
		recv_crc = (ReadBuf[iRet - 2]<< 8) | (ReadBuf[iRet - 1] & 0xff);

		//zlog_debug(pLogHandler_m, "%s in Recv3gGateDataPthread,c = 0x%02x, recv = 0x%02x",PRO_NAME,recv_crc,crc16_data);

		if (recv_crc != crc16_data )
		{
			zlog_warn(logHandler_m, "%s in %s crc16 is bad! continue", PRO_NAME,_FUN_);
			continue;
		}
		#endif

		cout<<"Pop Data "<<iRet<<endl;
		iRet = AV_OCX_DataCmdProc(fd, ReadBuf, iRet - 2);
		if (iRet == -2)
		{
			printf("%s socket with 3ggate break!\n",PRO_NAME);
		}
	}

	return ERROR_SUCCESS;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_DataCmdProc
 Description  : 处理收到的数据
 Input        : char *head
                int datalen
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/26
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_DataCmdProc(int fd, char *head, int datalen)
{
	AV_WEBOCX_PACKAGE_ST *pPackHead = NULL;
	int WebOcxCmdType;
	int PackageTotLen;
	map<int, WebOcxMsgAction>::iterator itOcxMap;

	if ((head == NULL) || ((unsigned)datalen < WEBOCX_HEAD_COMM_LEN))
	{
		cout<<"DataLen "<<datalen<<"sizeof() "<<sizeof(AV_WEBOCX_PACKAGE_ST)<<endl;
	    return ERROR_FAILED;
	}

	pPackHead = (AV_WEBOCX_PACKAGE_ST *)head;
	PackageTotLen = ntohl(pPackHead->packagelen);

	cout<<"AV_OCX_DataCmdProc recv TolLen "<<PackageTotLen<<"NTOHL "<< pPackHead->packagelen<<endl;

	/* 包总长度为包头标识到CRC校验 */
	if (PackageTotLen < (datalen + 2))
	{
	    return ERROR_FAILED;
	}

	WebOcxCmdType = pPackHead->msgtype;
	cout<<"AV_OCX_DataCmdProc recv MsgType "<<hex<<WebOcxCmdType<<endl;

	itOcxMap = OcxMap->find(WebOcxCmdType);
	if (itOcxMap == OcxMap->end())
	{
		cout<<"AV_OCX_DataCmdProc recv MsgType "<<hex<<WebOcxCmdType<<"not exist!!!"<<endl;
	    return ERROR_FAILED;
	}

	if (NULL != itOcxMap->second)
	{
		cout<<"AV_OCX_DataCmdProc find MsgType "<<hex<<WebOcxCmdType<<endl;
	    (this->*(itOcxMap->second))(fd, head, datalen);
	}

	return ERROR_SUCCESS;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_PthreadInit
 Description  : 进程初始化
 Input        : void
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/26
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_PthreadInit(void)
{

	OcxCycleBuf = new DataBufOcx((char *)"####", 4, 2048);
	if (OcxCycleBuf == NULL)
	{
	    return ERROR_FAILED;
	}

    return ERROR_SUCCESS;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_DataPushBuf
 Description  : 向缓冲区内写入数据
 Input        : char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/23
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_DataPushBuf(char *data, int len)
{
	int iRet = -1;

	iRet = OcxCycleBuf->PushData(data, len);
	if(iRet < 0)
	{
		OcxCycleBuf->PrintErrMsg(PRO_NAME,len);
		cout<<"AV_OCX_DataPushBuf"<<iRet<<endl;
	}

    return ERROR_SUCCESS;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_SendToWeb
 Description  : 发送数据到WEB插件
 Input        : unsigned int CodeId
                char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/26
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_SendToWeb(unsigned int CodeId, char *data, int len)
{
#define BUF_SIZE 1024
	char buf[BUF_SIZE];		    //发送缓冲区
	bzero(buf, sizeof(buf));
	short ret = -1;
	unsigned short crc16_data = 0;
	AV_WEBOCX_PACKAGE_ST stWebSndPackag;
	unsigned char *pSndData;
	unsigned int PackageLen;

	pSndData = (unsigned char *)&stWebSndPackag;
	strncpy(stWebSndPackag.packid, "####", 4);

	PackageLen = WEBOCX_HEAD_COMM_LEN + len + 2;
	//stWebSndPackag.packagelen = htonl(PackageLen);
	stWebSndPackag.packagelen = PackageLen;

	stWebSndPackag.packagesequ = 1;
	stWebSndPackag.msgtype = CodeId;
	memcpy(stWebSndPackag.data, data, len); 	//将数据包复制到发送缓冲区

	crc16_data = crc16(pSndData, PackageLen - 2);
	pSndData[PackageLen - 2] = crc16_data >> 8;
	pSndData[PackageLen - 1] = crc16_data & 0xff;
	//printf("crc16_data = %04x\n", crc16_data);

	debug_print("AV_OCX_SendToWeb",(char*)pSndData, PackageLen);
	if((ret = send(WebOcxFd, pSndData, PackageLen, 0)) <= 0)
    {
		perror("#[av]: in send_to_3g send fault!");
        return -1;
    }

	cout<<" send "<<ret<<" Msg Type "<<CodeId<<endl;

    return ERROR_SUCCESS;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_VideoLinkSendToWeb
 Description  : 发送视频数据到web客户端
 Input        : int fd
                char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/28
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_VideoLinkSendToWeb(unsigned short type, unsigned short usEvent, char *pcData, int iDataLen )
{
    int iRet = -1;

    int iSendDataLen = sizeof(tm_svr_header);

    char *pcSendBuf = new char[iSendDataLen + iDataLen + 1];
    if ( !pcSendBuf )
    {
        return -1;
    }

    bzero(pcSendBuf, iSendDataLen + iDataLen + 1);
    tm_svr_header *pstHead = (tm_svr_header *)pcSendBuf;

    pstHead->type    = htons(type);
    pstHead->event   = htons(usEvent);
    //pstHead->number  = htons(stream_number_m);
    //pstHead->version = htons(sSysVer);
    /*填充设备id号*/
    //memcpy(pstHead->srcID,stRegSvrPara.dev_name, sizeof(stRegSvrPara.dev_name));

    if ( (pcData) && (iDataLen > 0) )
    {
        memcpy(pcSendBuf + iSendDataLen, pcData, iDataLen);
        iSendDataLen += iDataLen;
    }
    pstHead->length  = htonl(iSendDataLen); //在最后填充长度

    //iRet=send(iVideoSvrSockFd,pcSendBuf,iSendDataLen,0); // 发送数据给svr
	if ( iRet <= 0 )
	{
		//zlog_error(plogHandler_m, "%s in %s, send media ack error!", PRO_NAME, _FUN_);
	}

    delete [] pcSendBuf;
    pcSendBuf = NULL;
    return iRet;
}


/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_SendToGate
 Description  : 发送数据给GATE
 Input        : unsigned int CodeId
                char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/27
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_SendToGate(char DestId,int cmd, char *data, int len)
{
	int SockFd;
	short alllen;
	int ret;
	unsigned short crc16_data;
	int id_len = 4;
	char id[] = { "$$$$"};
	TM_CMD_HEAD_ST stCmdData;
	char *pdata;

	if(strlen(id) > 4) id_len = 4;

	/*
	*	+------+------+------------+----------+------+------+------+-----+
	*	|	4  |  2   |      1     |     1    |  1   |  1   |  X   |  2  |
	*	+------+------+------------+----------+------+------+------+-----+
	*	+------+------+------------+----------+------+------+------+-----+
	*	| $$$$ | 长度 | 目标模块ID | 源模块ID | 包ID | 命令 | DATA | CRC |
	*	+------+------+------------+----------+------+------+------+-----+
	*
	*/

	alllen = id_len + 2 + 2 + len + 2;

	pdata = (char *)&stCmdData;

	memcpy(stCmdData.bag_id, id, id_len);

	stCmdData.cmdlen_h =  alllen >> 8; //copy length
	stCmdData.cmdlen_l =  alllen & 0x00FF;

//	stCmdData.ucSrcDevID = 0x02;
	stCmdData.ucDestDevID = 0x02;
//	stCmdData.ucBagID = 0xFF;
	stCmdData.ucCmd = cmd;

	if (NULL != data)
	{
		memcpy(stCmdData.buff, data, len);
	}

	crc16_data = crc16((unsigned char *)&stCmdData, alllen - 2);
	pdata[alllen - 2] = crc16_data >> 8;
	pdata[alllen - 1] = crc16_data & 0x00FF;

	//SockFd = mainProces_m->GetGateSock();
	SockFd = 0;
	cout<<"send gate fd "<<SockFd<<"  Type "<<cmd<<" Len "<<alllen<<endl;

	debug_print("AV_OCX_SendToGate",(char *)&stCmdData, alllen);

	if((ret = send(SockFd, &stCmdData, alllen, 0)) <= 0)
    {
		perror("#[av]: in send_to_3g send fault!");
        return -1;
    }

	return 0;
}
int WebOcxClass::AV_OCX_VideoStreamTimeo(void *pstCtrl, bool bReadDataFlag, int *times)
{
#if 0

	int CurrChnId;
	WEBOCX_STREAM_CTRL_ST *pstStreamInfo = NULL;
	map<int, WEBOCX_STREAM_CTRL_ST *>::iterator iter;

	pstStreamInfo = (WEBOCX_STREAM_CTRL_ST *)pstCtrl;


    ifly_TCP_Stream_Ack_3g st_ackDownload;//发给web
    st_ackDownload.errcode = htonl(CTRL_SUCCESS);
    st_ackDownload.ackid   = htonl(pstStreamInfo->key);
	iter = StreamChnMap.begin();

	for (;iter != StreamChnMap.end();iter++)
	{
		CurrChnId = iter->second->ChnId;
		if (CurrChnId >= 32)
		{
		    continue;
		}

		//printf("ChnId %d Time Out [%d] < 500 bReadDataFlag %d Pause %d \n", CurrChnId, times[CurrChnId], bReadDataFlag, iter->second->pause);
		/* 只有当前通道没有暂停的时候才累计超时 */
		if (iter->second->pause == 0)
		{
			times[CurrChnId]++;
		}

		/* 收到过数据，超过5秒且没有暂停就停止该通道 ||  */
	    if ((bReadDataFlag) &&
			(times[CurrChnId] > AV_VIDEO_NODATA_TIMES))
	    {
	        //close stream
	    	cout<<"Close Stream CurrChnId times[CurrChnId] "<<CurrChnId<<endl;

			printf("Close Stream CurrChnId %d, times[%d] %d, iter->second->pause %d\n", CurrChnId, CurrChnId, times[CurrChnId], iter->second->pause);
			times[CurrChnId] = 0;

			AV_OCX_StopStream(iter->second);

			AV_OCX_EpollWaitDel(EpollWaitFd, iter->second->fd, TRUE);

			AV_OCX_VideoLinkSendToWeb(CTRL_COMMAND, AV_VIDEO_DOWNLOAD_END, (char *)&st_ackDownload, sizeof(ifly_TCP_Stream_Ack_3g));
	    }

		/* 没有收到数据，也没有暂停，50秒后，关闭 */
  	    if ((!bReadDataFlag) &&
			(times[CurrChnId] > (AV_VIDEO_NODATA_TIMES*10)))
	    {
	        //close stream
	    	cout<<"=Close Stream CurrChnId times[CurrChnId]"<<CurrChnId<<endl;

			printf("=Close Stream CurrChnId %d, times[%d] %d, iter->second->pause %d\n", CurrChnId, CurrChnId, times[CurrChnId], iter->second->pause);
			times[CurrChnId] = 0;

			AV_OCX_StopStream(iter->second);
			AV_OCX_EpollWaitDel(EpollWaitFd, iter->second->fd, TRUE);

			AV_OCX_VideoLinkSendToWeb(CTRL_COMMAND, AV_VIDEO_DOWNLOAD_END, (char *)&st_ackDownload, sizeof(ifly_TCP_Stream_Ack_3g));
	    }

	}
#endif
	return ERROR_SUCCESS;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_VideoStreamCtrl
 Description  : 数据流在传送过程中的控制处理
 Input        : void *pstCtrl
                void *pstStreamCtrl
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2015/1/6
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_VideoStreamCtrl(void *pstCtrl)
{
#if 0

	ShareMemHead *pstShmMenHdr = NULL; //数据头
    map<int, WEBOCX_STREAM_CTRL_ST *>::iterator iter;


	pstShmMenHdr = (ShareMemHead *)pstCtrl;
	iter = StreamChnMap.find(pstShmMenHdr->video.byChnIndex);
	if (iter == StreamChnMap.end())
	{
		/* 此帧丢弃 */
		printf("Frame drop chl %d\n", pstShmMenHdr->video.byChnIndex);
	    return ERROR_VIDEO_PAUSE;
	}


    ifly_TCP_Stream_Ack_3g st_ackDownload;//发给web
    st_ackDownload.errcode = htonl(CTRL_SUCCESS);
    st_ackDownload.ackid   = htonl(iter->second->key);

    if (pstShmMenHdr->mediaType == AV_SHARE_MEM_TYPE_END)
    {
    	cout<<"mediaType == AV_SHARE_MEM_TYPE_END "<<endl;

		AV_OCX_StopStream(iter->second);

		AV_OCX_VideoLinkSendToWeb(CTRL_COMMAND, AV_VIDEO_DOWNLOAD_END, (char *)&st_ackDownload, sizeof(ifly_TCP_Stream_Ack_3g));
		AV_OCX_EpollWaitDel(EpollWaitFd, iter->second->fd, TRUE);
        return ERROR_VIDEO_STOP;
    }

	/* 当前状态为暂停时，返回暂停 */
	if (AV_PLAY_PAUSE == iter->second->pause)
	{
		printf("Chn %d, PAUSE\n", pstShmMenHdr->video.byChnIndex);
	    return ERROR_VIDEO_PAUSE;
	}
#endif
	return ERROR_SUCCESS;
}
/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_StartSendVidePthread
 Description  : 发送视频数据到客户端
 Input        : int fd
                char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/28
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_StartSendVidePthread(void *pstCtrl)
{
#if 0

	int iFlag = 0;
	WEBOCX_STREAM_CTRL_ST* pstStreamCtrl = (WEBOCX_STREAM_CTRL_ST*)pstCtrl;
	VideoProcess *pvideo_m = NULL;

	cout<<"AV_OCX_StartSendVidePthread  "<<endl;
	cout<<"MainClass 0x"<<pstStreamCtrl->pMainClass_m<<" 0x"<<pstStreamCtrl->pMainClass_m->videoProcessModule<<endl;

	pvideo_m = pstStreamCtrl->pMainClass_m->videoProcessModule;

	cout<<"GetFunctionSwitchFlag Start "<<endl;

	/*先判断该功能是否使能*/
	pvideo_m->GetFunctionSwitchFlag(iFlag);
	if(iFlag == CLOSE_FUN_SWITCH)
	{
		cout<<"GetFunctionSwitchFlag  CLOSE_FUN_SWITCH"<<endl;

		return -2;
	}

	cout<<"AV_VIDEO_Get_Video_Send_Stat Start "<<endl;
	/*判断是否已经打开*/
	if(pvideo_m->AV_VIDEO_Get_Video_Send_Stat() == VIDEO_START_SEND)
	{
		cout<<"AV_VIDEO_Get_Video_Send_Stat  VIDEO_START_SEND"<<endl;
		/* 如果通道号没有通知APP传数据，则通知APP */
		return -1;
	}

	cout<<"StartFunction Start "<<endl;
	pvideo_m->StartFunction();/*设置功能启动状态*/


	cout<<"DVR_SDK_VideoWriteMemInit Start "<<endl;
    DVR_SDK_VideoWriteMemInit();

	cout<<"AV_OCX_SendVidePthread  Start"<<endl;

	pstStreamCtrl->run = true;
	tm_creat_pthreat(&(pstStreamCtrl->Pthread_id), AV_OCX_SendVidePthread, pstStreamCtrl);
	#endif
	return ERROR_SUCCESS;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_SendFrame
 Description  : 发送视频帧
 Input        : int fd
                unsigned char *pBuf
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/28
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_SendFrame(int fd, unsigned char *pBuf)
{
	int ret = -1;
#if 0
	AV_WEBOCX_PACKAGE_ST *pstHeader;
	if ((!pBuf) || (fd == -1))
	{
	    printf("%s in %s, pBuf = %p , fd = %d\n",PRO_NAME,__FUNCTION__, pBuf, fd);
		return -1;
	}

	pstHeader = (AV_WEBOCX_PACKAGE_ST *)pcFrameBuf;
	memcpy(pstHeader->packid, "####", 4);
	pstHeader->msgtype = 0x1022;

	IFLY_MediaFRAMEHDR_3g_t *tMediaHDR = (IFLY_MediaFRAMEHDR_3g_t *)(pcFrameBuf + WEBOCX_HEAD_COMM_LEN);

	ShareMemHead *shmemHead = (ShareMemHead*)pBuf;		//数据头
	int i_size;

	tMediaHDR->m_wVideoWidth  = htonl(IMG_WIGHT);
	tMediaHDR->m_wVideoHeight = htonl(IMG_HIGHT);
//    if ( stRecTimeDownload.iPlayIndex == AV_VIDEO_DOWN_STARTED)
//    {
//        tMediaHDR->m_byMediaType  = MEDIA_TYPE_DOWNLOAD;//down load rec
//    }
//    else
//    {
//	    tMediaHDR->m_byMediaType  = MEDIA_TYPE_H264; //判断stRecTimeDownload.iPlayIndex
//    }
	tMediaHDR->m_byFrameRate  = VIDEO_FRAM_SPEED;
	tMediaHDR->m_dwFrameID    = htonl(shmemHead->video.FrameID);
	tMediaHDR->m_bKeyFrame    = shmemHead->video.byFrameType;
	tMediaHDR->m_byMediaType = shmemHead->mediaType;
	//printf("AV_OCX_SendFrame m_byMediaType %d  m_bKeyFrame %d\n", tMediaHDR->m_byMediaType, tMediaHDR->m_bKeyFrame);

	tMediaHDR->m_dwTimeStamp  = htonl(shmemHead->video.timeStamp);
	tMediaHDR->m_dwDataSize   = htonl(shmemHead->video.dwlen);

	//cout<<"tMediaHDR->m_dwDataSize "<<tMediaHDR->m_dwDataSize<<endl;
	//cout<<"shmemHead->video.dwlen "<<shmemHead->video.dwlen<<endl;

	i_size = sizeof(IFLY_MediaFRAMEHDR_3g_t);
	memcpy(pcFrameBuf + WEBOCX_HEAD_COMM_LEN + i_size, pBuf+sizeof(ShareMemHead), shmemHead->video.dwlen);
	i_size += shmemHead->video.dwlen;

    if ( !(shmemHead->video.FrameID % 20) )
    {
    }

	pstHeader->packagelen = i_size +  WEBOCX_HEAD_COMM_LEN;

	ret = send(fd, pcFrameBuf, pstHeader->packagelen, 0);
	if (ret < 0)
	{
	    cout<<"Frame Buf Snd "<<ret<<endl;
	}
#endif
	return ret;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_SendVidePthread
 Description  : 发送视频流线程
 Input        : void *args
 Output       : None
 Return Value : void*

  History        :
  1.Date         : 2014/12/28
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
void* WebOcxClass::AV_OCX_SendVidePthread(void *args)
{
#if 0
	WEBOCX_STREAM_CTRL_ST *pStreamCtrl = (WEBOCX_STREAM_CTRL_ST *)args;
	VideoProcess *pVideoClass;
	WebOcxClass *pModuleClass;
	int iRet = -1;
	ShareMemHead *pstshmemHead = NULL; //数据头
	int counter = 0;
	unsigned int  bufLen = 128*1024;
	unsigned char *pBuf = NULL;
	pBuf = new unsigned char[128*1024];
	int DataChnId = 0;
    int iNoDataTimes[32];



    if (NULL == pBuf)
    {
        exit(-1);
    }

    bool bReadDataFlag = false; //用于标记第一次读取数据

	pVideoClass = pStreamCtrl->pMainClass_m->videoProcessModule;
	pModuleClass = pStreamCtrl->pMainClass_m->WebOcxModule;

	cout<<"AV_OCX_SendVidePthread count "<<pModuleClass->StreamFdMap.size()<<endl;

    ifly_TCP_Stream_Ack_3g st_ackDownload;//发给web
    st_ackDownload.errcode = htonl(CTRL_SUCCESS);
    st_ackDownload.ackid   = htonl(pStreamCtrl->key);


	//while (pStreamCtrl->run)
	while (pModuleClass->StreamChnMap.size())
	{
		iRet = DVR_SDK_READ_Video_Data((char *)pBuf, bufLen, &counter);
		if (ERROR_NO_DATA == iRet)
		{
		    usleep(100*1000);
			//pModuleClass->AV_OCX_DebugFdMap();

			//cout<<"AV_OCX_SendVidePthread timeout "<<pModuleClass->StreamChnMap.size()<<endl;
			pModuleClass->AV_OCX_VideoStreamTimeo(pStreamCtrl ,bReadDataFlag, iNoDataTimes);
            continue;
		}
		else if (ERROR_FAILED == iRet)
		{
        	cout<<"DVR_SDK_READ_Video_Data ERROR_FAILED"<<endl;
            continue;
        }


        pstshmemHead = (ShareMemHead *)pBuf;

        bReadDataFlag = TRUE;

		DataChnId = pstshmemHead->video.byChnIndex;
		iNoDataTimes[DataChnId] = 0;

		iRet = pModuleClass->AV_OCX_VideoStreamCtrl(pstshmemHead);
		if (ERROR_VIDEO_STOP == iRet)
		{
		    continue;
		}
		else if (ERROR_VIDEO_PAUSE == iRet)
		{
		    continue;
		}


    	//cout<<"AV_OCX_SendFrame  before ChnId "<<DataChnId<<endl;
		pModuleClass->AV_OCX_SendFrame(pModuleClass->ChnFd[DataChnId], pBuf);
	}

	/*
     *	pModuleClass->AV_OCX_EpollWaitDel(pModuleClass->EpollWaitFd, pStreamCtrl->fd);
     *
     *  LinkKey = pStreamCtrl->key;
     *  occurs1 = pModuleClass->StreamFdMap.count(LinkKey);
     *  delete pStreamCtrl;
     *  pModuleClass->StreamFdMap.erase(LinkKey);
     *  occurs1 = pModuleClass->StreamFdMap.count(LinkKey);
     *  cout<<"AV_OCX_SendVidePthread stop occurs1 "<<occurs1<<" occurs2 "<<occurs2<<endl;
	 */
	pVideoClass->StopFunction();
	cout<<"AV_OCX_SendVidePthread End "<<endl;

    delete [] pBuf;
	pBuf = NULL;
#endif
    pthread_detach(pthread_self());

	return VIDEO_MODULE_SUCCESS;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_StopStream
 Description  : 停止发送视频流，根据流类型不同，向APP发送的消息命令不同
 Input        : WEBOCX_STREAM_CTRL_ST *pStreamCtrl
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2015/1/9
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_StopStream(WEBOCX_STREAM_CTRL_ST *pStreamCtrl)
{
#if 0
	int StreamType;
	StreamType = pStreamCtrl->StreamType;
	switch(StreamType)
	{
	    case AV_REAL_VIDEO_VIEW:
		{
			AV_OCX_StopStreamView(pStreamCtrl, false);
		    break;
		}
	    case AV_HISTORY_VIDEO_VIEW:
		{
			tmkj_stop_time_play(pStreamCtrl->ChnId);
		    break;
		}
	    case AV_HISTORY_VIDEO_DOWNLOAD:
		{
			tmkj_stop_time_download(pStreamCtrl->ChnId);
		    break;
		}
		default:
		{
		    return ERROR_FAILED;
		}
	}
	#endif
	return ERROR_SUCCESS;
}
/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_StopStreamView
 Description  : 停止视频预览码流
 Input        : int fd
                char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/28
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_StopStreamView(WEBOCX_STREAM_CTRL_ST *pStreamCtrl, bool Flag)
{
#if 0
	VideoProcess *pV_m = NULL;

	pV_m = pStreamCtrl->pMainClass_m->videoProcessModule;

	/*如果已经关闭就直接返回*/
	if(pV_m->AV_VIDEO_Get_Video_Send_Stat() == VIDEO_STOP_SEND)
	{
		return VIDEO_MODULE_BUSY;
	}

    //关闭码流传输标志&通知app停止发送码流
    for (int i = 0; i < MAX_CHN_NUM; i++)
	{
	    if ( !Flag )
	    {
	        tmkj_close_video(i);
	    }
        pV_m->AV_VIDEO_SetStreamFlag(i, 0, FALSE);
	}

	pV_m->StopFunction();
	pStreamCtrl->run = FALSE;
#endif
	return 0;
}
/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_KeepAlive
 Description  : 心跳消息处理方法
 Input        : char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/23
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_KeepAlive(int fd, char *data, int len)
{
	cout<<PRO_NAME<<"AV_OCX_KeepAlive"<<endl;
	char ret = 0xFE;

	AV_OCX_SendToWeb(AV_WEBOCX_KEEPALIVE + 1,(char *)&ret, sizeof(ret));

    return ERROR_SUCCESS;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_LogIn
 Description  : 登录消息
 Input        : char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/23
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_LogIn(int fd, char *data, int len)
{
	cout<<PRO_NAME<<"AV_OCX_LogIn"<<endl;
	WEBOCX_LOGIN_REPLY_ST stLogInReply;

	stLogInReply.UserId = 0xA;
	stLogInReply.MaxChnCnt = 4;
	stLogInReply.MaxAudioCnt = 1;
	stLogInReply.MaxPlayBackCnt = 4;
	stLogInReply.MaxSubStreamCnt = 4;
	stLogInReply.MaxAlarmInputCnt = 1;
	stLogInReply.MaxAlarmOutputCnt = 1;
	stLogInReply.MaxHdiskCnt = 1;

	/* 登录包的描述符为控制信令的连接句柄 */
	WebOcxFd = fd;

	AV_OCX_SendToWeb(AV_WEBOCX_LOGIN + 1,(char *)&stLogInReply, sizeof(WEBOCX_LOGIN_REPLY_ST));

    return ERROR_SUCCESS;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_LogOut
 Description  : 用户注销，收到的注销数据有用户ID
 Input        : char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/26
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_LogOut(int fd, char *data, int len)
{
	cout<<PRO_NAME<<"AV_OCX_LogOut"<<endl;
	char ret = 0;
	int UserId;
	char *pUser = NULL;
	map<int, WEBOCX_STREAM_CTRL_ST*>::iterator iter;

	pUser = data;
	UserId = (int)pUser;

	cout<<"USER ID: "<<UserId<<endl;

	iter = StreamFdMap.begin();

	for (;iter != StreamFdMap.end(); iter++)
	{
	    if (iter->second->fd != -1)
    	{
    	    iter->second->run = FALSE;
    	}
	}

	//StreamFdMap.erase(iterBeg, iterEnd);

	AV_OCX_SendToWeb(AV_WEBOCX_LOGOUT + 1,(char *)&ret, sizeof(ret));
	return ERROR_SUCCESS;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_CameraCtrl
 Description  : 云台控制命令
 Input        : char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/26
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_CameraCtrl(int fd, char *data, int len)
{
	cout<<PRO_NAME<<"AV_OCX_CameraCtrl"<<endl;
	AV_WEBOCX_PACKAGE_ST *pstOcxHead = NULL;
	WEBOCX_CAMERA_CTRL_ST *pstCameraMsg = NULL;

	pstOcxHead = (AV_WEBOCX_PACKAGE_ST *)data;

	pstCameraMsg = (WEBOCX_CAMERA_CTRL_ST *)pstOcxHead->data;

	cout<<"Camera Ctrl ChnId "<<pstCameraMsg->ChnId<<endl;
	cout<<"Camera Ctrl Msg "<<pstCameraMsg->CameraCtrl<<endl;
	cout<<"Camera Ctrl Action "<<pstCameraMsg->Action<<endl;
	char ret = 1;
	AV_OCX_SendToWeb(AV_WEBOCX_CAMERA_CTRL + 1,(char *)&ret, sizeof(ret));
	ControlPTZ(0,(UCHAR *)pstCameraMsg, 0);
	#if 0
	AV_OCX_SendToGate(0x02,
					  AV_APP_CAMERA_CTRL,
					  (char *)&pstCameraMsg->ChnId,
					  (sizeof(WEBOCX_CAMERA_CTRL_ST) - sizeof(int)));
	#endif
	return ERROR_SUCCESS;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_CameraCtrlGet
 Description  : 云台控制获取
 Input        : int fd
                char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/31
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_CameraParaGet(int fd, char *data, int len)
{
	AV_WEBOCX_PACKAGE_ST *pstOcxHead = NULL;
	WEBOCX_CAMERA_PARA_ST *pstCameraPara = NULL;

	cout<<"AV_OCX_CameraParaGet"<<endl;

	pstOcxHead = (AV_WEBOCX_PACKAGE_ST *)data;

	pstCameraPara = (WEBOCX_CAMERA_PARA_ST *)pstOcxHead->data;

#if 0
	AV_OCX_SendToGate(0x02,
					  AV_APP_CAMERA_PARA_GET,
					  (char *)&pstCameraPara->ChnID,
					  sizeof(pstCameraPara->ChnID));
#endif
	int i = 0;

	for(i=0;i<CHNNUM;i++)
	{
		printf("pdevicecamPTZparam->chunum:%d\n",pstCameraPara->ChnID);
		if(pstCameraPara->ChnID == i)
		{
			AV_OCX_SendToWeb( AV_WEBOCX_CAMERA_PARA_GET + 1,(char *)&(devicecamPTZparam[i].ChnID),sizeof(WEBOCX_CAMERA_PARA_ST)-sizeof(int));
			break;
		}
	}

    return ERROR_SUCCESS;
}
/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_CameraParaSet
 Description  : 云台控制获取
 Input        : int fd
                char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/31
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_CameraParaSet(int fd, char *data, int len)
{

	AV_WEBOCX_PACKAGE_ST *pstOcxHead = NULL;
	WEBOCX_CAMERA_PARA_ST *pstCameraPara = NULL;

	cout<<"AV_OCX_CameraParaGet"<<endl;

	pstOcxHead = (AV_WEBOCX_PACKAGE_ST *)data;

	pstCameraPara = (WEBOCX_CAMERA_PARA_ST *)pstOcxHead->data;

	//SndLen = len - WEBOCX_HEAD_COMM_LEN - sizeof(int);


	SetXmlCamPTZParam(pstCameraPara->ChnID,pstCameraPara);
	//writeCrc16Data(Socketfd,AVADDER,APP_CAMERA_PARA_SET,&result,1);

	GetXmlCamPTZParam(CHNNUM,devicecamPTZparam);
	GetXmlcamerid(sipcamerid);
	char buff = 1;
	AV_OCX_SendToWeb(AV_WEBOCX_CAMERA_PARA_SET + 1, &buff, sizeof(char));

#if 0
	char buffer[16] = {0};
	TM_CAMER_SEEP_MCU stmcamerseep;
	memset(buffer,0,sizeof(buffer));
	sprintf(buffer,"%d",pstCameraPara->Speed);
	strcpy(stmcamerseep.camerseepmcu,buffer);
	//AV_OCX_SendToGate(MCUADDER,COMSEEP,(char *)&stmcamerseep,sizeof(TM_CAMER_SEEP_MCU));

	AV_OCX_SendToGate(0x02,
					  AV_APP_CAMERA_PARA_SET,
					  (char *)&pstCameraPara->ChnID,
					  SndLen);
#endif
    return ERROR_SUCCESS;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_NetWorkParaGet
 Description  : 云台控制获取
 Input        : int fd
                char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/31
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_NetWorkParaGet(int fd, char *data, int len)
{
	//AV_WEBOCX_PACKAGE_ST *pstOcxHead = NULL;

	cout<<"AV_OCX_NetWorkParaGet"<<endl;

	//pstOcxHead = (AV_WEBOCX_PACKAGE_ST *)data;

	//pstNetworkPara = (WEBOCX_NETWORK_PARA_ST *)pstOcxHead->data;
	AV_OCX_SendToWeb( 2110,
					(char *)&devicenetparam,
					sizeof(WEBOCX_NETWORK_PARA_ST));

#if 0


	AV_OCX_SendToGate(0x02,
					  AV_APP_NETWORK_PARA_GET,
					  NULL,
					  0);
#endif
    return ERROR_SUCCESS;
}
/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_NetWorkParaSet
 Description  : 云台控制获取
 Input        : int fd
                char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/31
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_NetWorkParaSet(int fd, char *data, int len)
{
	AV_WEBOCX_PACKAGE_ST *pstOcxHead = NULL;
	WEBOCX_NETWORK_PARA_ST *pstNetworkPara = NULL;

	cout<<"AV_OCX_NetWorkParaSet"<<endl;

	pstOcxHead = (AV_WEBOCX_PACKAGE_ST *)data;

	pstNetworkPara = (WEBOCX_NETWORK_PARA_ST *)(pstOcxHead->data + sizeof(int));

	printf("pstNetworkPara->IpAddr %s\n", pstNetworkPara->IpAddr);
	printf("pstNetworkPara->NetMask %s\n", pstNetworkPara->NetMask);
	printf("pstNetworkPara->NetGate %s\n", pstNetworkPara->NetGate);
	printf("pstNetworkPara->MultAddr %s\n", pstNetworkPara->MultAddr);
	printf("pstNetworkPara->MacAddr %s\n", pstNetworkPara->MacAddr);
	printf("pstNetworkPara->DnsAddr1 %s\n", pstNetworkPara->DnsAddr1);
	printf("pstNetworkPara->DnsAddr2 %s\n", pstNetworkPara->DnsAddr2);

	char result = 1;
	SetXmlNetParam(pstNetworkPara);
	GetXmlNetParam(&devicenetparam);
	AV_OCX_SendToWeb(AV_WEBOCX_NETWORK_PARA_SET + 1, &result, sizeof(char));

#if 0
	AV_OCX_SendToGate(0x02,
					  AV_APP_NETWORK_PARA_SET,
					  (char *)pstNetworkPara,
					  len - WEBOCX_HEAD_COMM_LEN - sizeof(int));
#endif
    return ERROR_SUCCESS;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_RealDataReq
 Description  : 实时预览请求
 Input        : char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/27
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_RealDataReq(int fd, char *data, int len)
{

	cout<<PRO_NAME<<"AV_OCX_RealDataReq"<<endl;
	int randnum;
 	WEBOCX_STREAM_CTRL_ST *pstStreamInfo = new WEBOCX_STREAM_CTRL_ST;
	AV_WEBOCX_PACKAGE_ST *pstOcxHead = NULL;
	WEBOCX_REAL_VIEW_ST *pstCameraMsg = NULL;

	pstOcxHead = (AV_WEBOCX_PACKAGE_ST *)data;

	pstCameraMsg = (WEBOCX_REAL_VIEW_ST *)pstOcxHead->data;

	srand((unsigned)time(NULL));
	randnum = rand();

	//stStreamInfo.ChnId = pstCameraMsg->ChnId;
	pstStreamInfo->ChnId = pstCameraMsg->ChnId;
	pstStreamInfo->StreamType = pstCameraMsg->StreamType;
	pstStreamInfo->fd = -1;
	pstStreamInfo->pMainClass_m = mainProces_m;
	pstStreamInfo->VideoViewType = AV_REAL_VIDEO_VIEW;
	pstStreamInfo->key = randnum;
	pstStreamInfo->pause = AV_PLAY_NORMAL;

	cout<<"RandHandle "<<randnum<<endl;

	pair<map<int, WEBOCX_STREAM_CTRL_ST*>::iterator, bool> ret = this->StreamFdMap.insert(make_pair(randnum, pstStreamInfo));
	if (ret.second != TRUE)
	{
	    randnum = -1;
	}

	AV_OCX_SendToWeb(0x1010,(char *)&randnum, sizeof(randnum));

	/* 将实时预览的信令转发至APP */
	return ERROR_SUCCESS;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_RealDataStop
 Description  : 实时预览停止命令
 Input        : int fd
                char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/28
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_RealDataStop(int fd, char *data, int len)
{
	AV_WEBOCX_PACKAGE_ST *pstOcxHead = NULL;
	int StreamLinkHandle = -1;
	int occurs = 0;
	char result = 1;
	map<int, WEBOCX_STREAM_CTRL_ST*>::iterator iter;

	cout<<"AV_OCX_RealDataStop"<<endl;

	pstOcxHead = (AV_WEBOCX_PACKAGE_ST *)data;

	StreamLinkHandle = *(int *)pstOcxHead->data;

	occurs = StreamFdMap.count(StreamLinkHandle);
	if (occurs == 0)
	{
		cout<<"StreamLinkHandle "<<StreamLinkHandle<<" occurs "<<occurs<<endl;
		AV_OCX_SendToWeb(AV_WEBOCX_REAL_STOP + 1,(char *)&result, sizeof(result));
		return ERROR_FAILED;
	}

	cout<<"AV_OCX_RealDataStop ok "<<StreamLinkHandle<<" occurs "<<occurs<<endl;
	iter = StreamFdMap.find(StreamLinkHandle);
	iter->second->run = FALSE;

	tmkj_close_video(iter->second->ChnId);


 	AV_OCX_EpollWaitDel(EpollWaitFd, iter->second->fd, TRUE);
	cout<<"AV_OCX_RealDataStop stop occurs "<<endl;

	result = 0;

	AV_OCX_SendToWeb(AV_WEBOCX_REAL_STOP + 1,(char *)&result, sizeof(result));

	return ERROR_SUCCESS;
}
/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_HistoryQuery
 Description  : 历史视频数据查询
 Input        : int fd
                char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/28
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_HistoryQuery(int fd, char *data, int len)
{
	AV_WEBOCX_PACKAGE_ST *pstOcxHead = NULL;
	WEBOCX_HISTORY_QUERY_ST *pHisQuery = NULL;
	struct TM_APP_TimePlayCond_t prfs;
	int ret;
	char buff[256];
	struct tm *FormatTime;
	char StrTime[128];
	TM_APP_SEND_SVR_ST stHisFile;

	cout<<"AV_OCX_HistoryQuery "<<endl;

	bzero(&prfs, sizeof(struct TM_APP_TimePlayCond_t));
	bzero(buff, sizeof(buff));

	pstOcxHead = (AV_WEBOCX_PACKAGE_ST *)data;
	pHisQuery = (WEBOCX_HISTORY_QUERY_ST *)pstOcxHead->data;
	cout<<"pHisQuery->UserId "<<pHisQuery->UserId<<endl;
	cout<<"pHisQuery->ChnId "<<pHisQuery->ChnId<<endl;
	cout<<"pHisQuery->FileType "<<pHisQuery->FileType<<endl;
	cout<<"pHisQuery->StartTime "<<pHisQuery->StartTime<<endl;
	cout<<"pHisQuery->EndTime "<<pHisQuery->EndTime<<endl;


	//ctrl_val = SERVER_CTRL_CMD_MEDIA_STREAM;
	printf("%s in deal svr cmd time play, in time play!\n",PRO_NAME);


	prfs.chn  = (int)pHisQuery->ChnId;

	//这里要加8个小时调整为东8区时区，设备上的时间都是0时区
	prfs.start_time = ntohl(pHisQuery->StartTime) + AV_VIDEO_CONVERT_TIME_ZONE;
	prfs.end_time = ntohl(pHisQuery->EndTime) + AV_VIDEO_CONVERT_TIME_ZONE;

    //prfs.chn = tm_convert_chnMsk_to_chn((unsigned char)prfs.chn);

    //convert_time(prfs.start_time, prfs.end_time); //for debug use

	printf("History Video Chn %d\n", prfs.chn);
	FormatTime = gmtime((time_t *)&prfs.start_time);
	strftime(StrTime, sizeof(StrTime), "%Y-%m-%d %H:%M:%S", FormatTime);
	printf("History Video Start time: %s\n", StrTime);

	FormatTime = gmtime((time_t *)&prfs.end_time);
	strftime(StrTime, sizeof(StrTime), "%Y-%m-%d %H:%M:%S", FormatTime);
	printf("History Video End time: %s\n", StrTime);

	ret = tmkj_rec_file_serch(&prfs, &stHisFile);//refer
	stHisFile.iEntriesNum = htonl(stHisFile.iEntriesNum);
	cout<<"ret "<<ret<<"iEntriesNum "<<stHisFile.iEntriesNum<<endl;
	if ( ret < 0)
	{
		buff[0] = 0;
		AV_OCX_SendToWeb(AV_WEBOCX_HISTORY_QUERY + 1, buff, 4);
	}
	else
	{
		//头+实际的录像列表数长度
		AV_OCX_SendToWeb(AV_WEBOCX_HISTORY_QUERY + 1,(char *)&stHisFile.iEntriesNum, (ret * sizeof(SEARCH_FILE_TIMESEG_ST) + sizeof(int)));
		//printf(RED"%s in %s, refer list num: %d, data len: %d\n"NONE, PRO_NAME, _FUN_, ret,iAnswerSvrDataLen);
	}

	return ERROR_SUCCESS;
}
/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_HistoryReq
 Description  : 历史视频数据请求
 Input        : int fd
                char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/30
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_HistoryReq(int fd, char *data, int len)
{
	int randnum;
 	WEBOCX_STREAM_CTRL_ST *pstStreamInfo = new WEBOCX_STREAM_CTRL_ST;
	AV_WEBOCX_PACKAGE_ST *pstOcxHead = NULL;
	WEBOCX_HISTORY_REQ_ST *pstCameraMsg = NULL;
	struct tm *FormatTime;
	char StrTime[128];

	cout<<"AV_OCX_HistoryReq len "<<len<<endl;
	pstOcxHead = (AV_WEBOCX_PACKAGE_ST *)data;

	pstCameraMsg = (WEBOCX_HISTORY_REQ_ST *)(pstOcxHead->data + sizeof(int));

	srand((unsigned)time(NULL));
	randnum = rand();

	debug_print("AV_OCX_HistoryReq",(char *)pstCameraMsg, sizeof(WEBOCX_HISTORY_REQ_ST));

	pstStreamInfo->ChnId = pstCameraMsg->ChnId;
	//pstStreamInfo->StreamType = pstCameraMsg->StreamType;
	pstStreamInfo->fd = -1;
	pstStreamInfo->pMainClass_m = mainProces_m;
	pstStreamInfo->VideoViewType = HISTORY_VIDEO_ACTION(pstCameraMsg->Action);
	pstStreamInfo->key = randnum;
	pstStreamInfo->pause = AV_PLAY_NORMAL;

	pstStreamInfo->strfs.chn = pstCameraMsg->ChnId;
	pstStreamInfo->strfs.start_time = pstCameraMsg->StartTime  + AV_VIDEO_CONVERT_TIME_ZONE;
	pstStreamInfo->strfs.end_time = pstCameraMsg->EndTime  + AV_VIDEO_CONVERT_TIME_ZONE;

	cout<<"RandHandle "<<randnum<<endl;

	printf("History Video Chn %d\n", pstStreamInfo->strfs.chn);
	FormatTime = gmtime((time_t *)&(pstStreamInfo->strfs.start_time));
	strftime(StrTime, sizeof(StrTime), "%Y-%m-%d %H:%M:%S", FormatTime);
	printf("History Video Start time: %s\n", StrTime);

	FormatTime = gmtime((time_t *)&(pstStreamInfo->strfs.end_time));
	strftime(StrTime, sizeof(StrTime), "%Y-%m-%d %H:%M:%S", FormatTime);
	printf("History Video End time: %s\n", StrTime);

	pair<map<int, WEBOCX_STREAM_CTRL_ST*>::iterator, bool> ret = this->StreamFdMap.insert(make_pair(randnum, pstStreamInfo));
	if (ret.second != TRUE)
	{
	    randnum = -1;
	}

	AV_OCX_SendToWeb(AV_WEBOCX_HISTORY_REQ + 1, (char *)&randnum, sizeof(randnum));

	return ERROR_SUCCESS;
}
/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_PlayBack
 Description  : 历史数据播放，控制命令
 				1.播放 2.暂停 3.停止 4.拖拽 5 单帧 6.回放速度控制
 Input        : int fd
                char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/30
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_PlayBack(int fd, char *data, int len)
{
	AV_WEBOCX_PACKAGE_ST *pstOcxHead = NULL;
	WEBOCX_PLAYBACK_CTRL_ST *pstBackPlayMsg = NULL;
	int MapCnt = 0;
	char result = 0;
	struct tm *FormatTime;
	char StrTime[128];
	map<int, WEBOCX_STREAM_CTRL_ST*>::iterator iter;
	WEBOCX_PLAYBACK_CTRL_ST stSndPlayBack;

	cout<<"AV_OCX_PlayBack"<<endl;
	pstOcxHead = (AV_WEBOCX_PACKAGE_ST *)data;

	pstBackPlayMsg = (WEBOCX_PLAYBACK_CTRL_ST *)pstOcxHead->data;
	MapCnt = StreamFdMap.count(pstBackPlayMsg->AuthenId);
	if (MapCnt == 0)
	{
		result = 1;
		AV_OCX_SendToWeb(AV_WEBOCX_PLAYBACK + 1, &result, sizeof(char));
		return ERROR_FAILED;
	}

	iter = StreamFdMap.find(pstBackPlayMsg->AuthenId);

	if (iter->second->VideoViewType == AV_REAL_VIDEO_VIEW)
	{
		cout<<"Real Video View Can't PlayBack! chnId "<<iter->second->ChnId<<endl;
	    return ERROR_FAILED;
	}

	cout<<"PlayBack Cmd "<<pstBackPlayMsg->CtrlCommd<<endl;

	memcpy(&stSndPlayBack, pstBackPlayMsg, sizeof(WEBOCX_PLAYBACK_CTRL_ST));
	switch(pstBackPlayMsg->CtrlCommd)
	{
		case AV_PLAYBACK_REPLAY:
		{
			stSndPlayBack.CtrlCommd = AV_PLAYBACK_PLAY;
			iter->second->pause = AV_PLAY_NORMAL;
		    break;
		}
	    case AV_PLAYBACK_PLAY:
		{
			stSndPlayBack.CtrlCommd = AV_PLAYBACK_REPLAY;
			iter->second->pause = AV_PLAY_NORMAL;
		    break;
		}
	    case AV_PLAYBACK_PAUSE:
		{
			iter->second->pause = AV_PLAY_PAUSE;
		    break;
		}
	    case AV_PLAYBACK_STOP:
		{
		 	AV_OCX_EpollWaitDel(EpollWaitFd, iter->second->fd, TRUE);
			cout<<"AV_OCX_PlayBack stop occurs "<<endl;
		    break;
		}
	    case AV_PLAYBACK_DRAG:
		{
			stSndPlayBack.StartTime += AV_VIDEO_CONVERT_TIME_ZONE;
			stSndPlayBack.EndStart += AV_VIDEO_CONVERT_TIME_ZONE;

			printf("PlayBack Drag Start:%d\n", stSndPlayBack.StartTime);
			printf("PlayBack Drag End:%d\n", stSndPlayBack.EndStart);

			FormatTime = gmtime((time_t *)&stSndPlayBack.StartTime);
			strftime(StrTime, sizeof(StrTime), "%Y-%m-%d %H:%M:%S", FormatTime);
			printf("PlayBack Drag Start time: %s\n", StrTime);

			FormatTime = gmtime((time_t *)&stSndPlayBack.EndStart);
			strftime(StrTime, sizeof(StrTime), "%Y-%m-%d %H:%M:%S", FormatTime);
			printf("PlayBack Drag End time: %s\n", StrTime);
		    break;
		}
	    case AV_PLAYBACK_FRAME:
		{
			/* 播放状态为单帧 */
			iter->second->pause = AV_PLAY_FRAME;
		    break;
		}
	    case AV_PLAYBACK_RATE:
		{
			stSndPlayBack.StartTime = 0x00;
			if (stSndPlayBack.EndStart < 0)
			{
			    stSndPlayBack.StartTime = 0x01;
			}

			stSndPlayBack.EndStart = (0x01 << abs(stSndPlayBack.EndStart));

			printf("Slowly or quickly %d \n", stSndPlayBack.StartTime);
			printf("Speed %d \n", stSndPlayBack.EndStart);
		    break;
		}
		default:
		{
		    return ERROR_FAILED;
		}
	}

	stSndPlayBack.AuthenId = iter->second->ChnId;		/* 客户端传来的是LinkRand,APP需要通道号 */
	tmkj_rec_play_drag((TM_APP_REC_PLAY_DRAG_ST *)&stSndPlayBack);

	//AV_OCX_SendToGate(AV_TO_APP_MOD, AV_APP_BACKPLAY_CTRL, (char *)&stSndPlayBack, sizeof(WEBOCX_PLAYBACK_CTRL_ST));
	//AV_OCX_SendToWeb(AV_WEBOCX_PLAYBACK + 1, &result, sizeof(char));
	return ERROR_SUCCESS;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_StreamGuide
 Description  : 码流引导信令，该信令是在新的数据连接中发送
 				只有检查到新连接的ID是正确的，才开始打开视频预览
 				并发送视频数据
 Input        : char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/27
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_StreamGuide(int fd, char *data, int len)
{
	int iRet = -1;
	char result = 1;
	AV_WEBOCX_PACKAGE_ST *pstOcxHead = NULL;
	WEBOCX_STREAM_GUIDE_ST *pstGuideMsg = NULL;
	struct TM_APP_TimePlayCond_t stTmprfs;
	struct tm *FormatTime;
	char StrTime[128];
	int occurs;
	map<int, WEBOCX_STREAM_CTRL_ST*>::iterator ItStream;
	map<int, WEBOCX_STREAM_CTRL_ST*>::iterator ItStream2;


	cout<<"AV_OCX_StreamGuide"<<endl;

	pstOcxHead = (AV_WEBOCX_PACKAGE_ST *)data;

	pstGuideMsg = (WEBOCX_STREAM_GUIDE_ST *)pstOcxHead->data;

	ItStream = StreamFdMap.find(pstGuideMsg->AuthenId);

	cout<<"find AuthernId "<<pstGuideMsg->AuthenId<<endl;
	if (ItStream == StreamFdMap.end())
	{
		cout<<"AV_OCX_StreamGuide find AuthenId fail"<<endl;

		//ev.data.fd = fd;
		//ev.events = EPOLLIN | EPOLLET;
		//epoll_ctl(EpollWaitFd, EPOLL_CTL_DEL, fd, &ev);
		//close(fd);
		AV_OCX_SendToWeb(0x1020,(char *)&result, sizeof(result));
	    return ERROR_FAILED;
	}
	cout<<"AV_OCX_StreamGuide find AuthenId success fd "<<fd<<endl;

	occurs = StreamChnMap.count(ItStream->second->ChnId);
	if (occurs != 0)
	{
		/* 随机数已经验证通过，但通道映射已经建立 */
		printf("Chn %d, has been Open His Or Real occurs %d\n", ItStream->second->ChnId, occurs);
		printf("Recv Rand %d, Storage Rand %d\n", pstGuideMsg->AuthenId, ItStream->second->key);
		ItStream2= StreamChnMap.find(ItStream->second->ChnId);

		if (ItStream2->second->fd != fd)
		{
			AV_OCX_EpollWaitDel(EpollWaitFd, fd, FALSE);
		}

	    return ERROR_SUCCESS;
	}

	ItStream->second->fd = fd;

	ChnFd[ItStream->second->ChnId] = fd;
	StreamChnMap.insert(make_pair(ItStream->second->ChnId, ItStream->second));

	cout<<"AV_OCX_StreamGuide Open Video chn  "<<ItStream->second->ChnId<<endl;


	if (ItStream->second->VideoViewType == AV_REAL_VIDEO_VIEW)
	{
		//ItStream->second.ChnId
		/* 收到引导流，通知APP开始指定通道预览，开始传输视频 */
		iRet = tmkj_open_video(ItStream->second->ChnId);
		if (iRet != 0)
		{
			cout<<"tmkj_open_video fail "<< iRet<<endl;

			AV_OCX_SendToWeb(0x1020,(char *)&result, sizeof(result));
			tmkj_close_video(ItStream->second->ChnId);

		    return ERROR_FAILED;
		}
	}
	else if (ItStream->second->VideoViewType == AV_HISTORY_VIDEO_VIEW)
	{
		memcpy(&stTmprfs, &(ItStream->second->strfs), sizeof(struct TM_APP_TimePlayCond_t));

		printf("History Video Chn %d\n", stTmprfs.chn);
		FormatTime = gmtime((time_t *)&stTmprfs.start_time);
		strftime(StrTime, sizeof(StrTime), "%Y-%m-%d %H:%M:%S", FormatTime);
		printf("History Video Start time: %s\n", StrTime);

		FormatTime = gmtime((time_t *)&stTmprfs.end_time);
		strftime(StrTime, sizeof(StrTime), "%Y-%m-%d %H:%M:%S", FormatTime);
		printf("History Video End time: %s\n", StrTime);


		iRet = tmkj_start_time_play(&iPlayIndex, &stTmprfs);
		if ( iRet != 0 )
		{

			printf("#[av]: DVRSDK_startTimePlay error ret = %d\n", iRet);
			AV_OCX_SendToWeb(0x1020,(char *)&result, sizeof(result));
			tmkj_stop_time_play(stTmprfs.chn);
			return ERROR_FAILED;
		}
	}

	AV_OCX_StartSendVidePthread(ItStream->second);

	result = 0;
	AV_OCX_SendToWeb(0x1020,(char *)&result, sizeof(result));
	return ERROR_SUCCESS;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_MdGet
 Description  : 移动侦测配置获取
 Input        : int fd
                char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/31
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_MdGet(int fd, char *data, int len)
{
	cout<<"AV_OCX_MdGet "<<endl;

	AV_OCX_SendToWeb(AV_WEBOCX_MD_GET + 1,(char *)voidemdparam,(sizeof(WEBOCX_MD_SET_ST))*4);

//	AV_OCX_SendToGate(AV_TO_APP_MOD, AV_APP_MD_GET, NULL, 0);

	return ERROR_SUCCESS;
}


/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_MdSet
 Description  :  移动侦测设置
 Input        : char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/27
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_MdSet(int fd, char *data, int len)
{
	char ret = 1;

	AV_WEBOCX_PACKAGE_ST *pstOcxHead = NULL;
	WEBOCX_MD_SET_ST *pstMdData = NULL;

	cout<<PRO_NAME<<"AV_OCX_MdSet len "<<len<<endl;
	cout<<"WEBOCX_MD_SET_ST - UserId "<<(sizeof(WEBOCX_MD_SET_ST) - sizeof(int))<<endl;
	pstOcxHead = (AV_WEBOCX_PACKAGE_ST *)data;
	pstMdData = (WEBOCX_MD_SET_ST *)(pstOcxHead->data+sizeof(int));

	//SndLen = len - WEBOCX_HEAD_COMM_LEN - sizeof(int);

 	SetXmlMDParam(CHNNUM,pstMdData);
	readXmlMDParam(voidemdparam);

	ret = 0;
	AV_OCX_SendToWeb(AV_WEBOCX_MD_SET + 1,(char *)&ret, sizeof(ret));

//	AV_OCX_SendToGate(AV_TO_APP_MOD, AV_APP_MD_SET, (char *)&pstMdData->ChnId, (SndLen));
	return ERROR_SUCCESS;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_AlarmGet
 Description  : 告警获取，转发至APP
 Input        : int fd
                char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/31
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_AlarmGet(int fd, char *data, int len)
{
	cout<<"AV_OCX_AlarmGet"<<endl;

	AV_OCX_SendToWeb(AV_WEBOCX_ALARM_GET + 1, (char *)&devicenodeparam,sizeof(WEBOCX_NODE_SET_ST));
	//AV_OCX_SendToGate(AV_TO_APP_MOD, AV_APP_ALARM_GET, NULL, 0);

	return ERROR_SUCCESS;
}
/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_AlarmSet
 Description  : 告警设置
 Input        : int fd
                char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/31
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_AlarmSet(int fd, char *data, int len)
{
	char ret = 1;

	AV_WEBOCX_PACKAGE_ST *pstOcxHead = NULL;
	//WEBOCX_ARLARM_SET_ST *pstAlarmData = NULL;
	WEBOCX_NODE_SET_ST *pstNodeData = NULL;

	cout<<PRO_NAME<<"AV_OCX_AlarmGet len "<<len<<endl;
	cout<<"WEBOCX_NODE_SET_ST - UserId "<<(sizeof(WEBOCX_NODE_SET_ST) - sizeof(int))<<endl;

	pstOcxHead = (AV_WEBOCX_PACKAGE_ST *)data;
	//pstAlarmData = (WEBOCX_ARLARM_SET_ST *)pstOcxHead->data;
	pstNodeData = (WEBOCX_NODE_SET_ST *)(pstOcxHead->data + sizeof(WEBOCX_ARLARM_SET_ST));

	//SndLen = len - WEBOCX_HEAD_COMM_LEN - sizeof(int);

	SetXmlNodeParam(pstNodeData->NodeId,pstNodeData);
	GetXmlNodeParam(1,devicenodeparam);
	AV_OCX_SendToWeb(AV_WEBOCX_ALARM_SET + 1, &ret, sizeof(char));

//	AV_OCX_SendToGate(AV_TO_APP_MOD, AV_APP_ALARM_SET, (char *)pstNodeData, SndLen);
	return ERROR_SUCCESS;
}


int WebOcxClass::AV_OCX_StatusReport(int fd, char *data, int len)
{
	cout<<"AV_OCX_StatusReport"<<endl;
	//AV_OCX_SendToGate(AV_TO_APP_MOD, AV_APP_STATUS_REPORT, NULL, 0);
	return ERROR_SUCCESS;
}
/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_DevParaGet
 Description  : 客户端读取设备参数
 Input        : char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/27
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_DevParaGet(int fd, char *data, int len)
{
	WEBOCX_DEVPARA_ST stDevPara;

	memset(&stDevPara, 0, sizeof(WEBOCX_DEVPARA_ST));

	cout<<PRO_NAME<<"AV_OCX_DevParaGet len "<<len<<endl;

	strcpy(stDevPara.DevId, "159159159159");
	strcpy(stDevPara.DevName, "天迈郑州71路后门");
	strcpy(stDevPara.PlatformDevId, "259259259259259259");
	strcpy(stDevPara.SipSvrIp, "1.192.121.234");
	stDevPara.SipSvrPort = 10035;
	strcpy(stDevPara.LocalIp, "192.168.33.211");
	stDevPara.LocalPort = 10088;

	AV_OCX_SendToWeb(AV_WEBOCX_DEV_PARA_GET + 1,(char *)&deviceparam,sizeof(WEBOCX_DEVPARA_ST));
	//AV_OCX_SendToGate(AV_TO_APP_MOD,  AV_APP_DEV_PARA_GET, NULL, 0);
	//AV_OCX_SendToWeb(AV_WEBOCX_DEV_PARA_GET + 1,(char *)&stDevPara.DevId, sizeof(WEBOCX_DEVPARA_ST) - sizeof(int));
	return ERROR_SUCCESS;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_ChnParaGet
 Description  : 客户端获取通道参数配置
 Input        : char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/27
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_ChnParaGet(int fd, char *data, int len)
{
	int SndLen = 0;
	char buff[1024];
	WEBOCX_CHNPARA_ST *pstChnCnt;
	//WEBOCX_CHN_INFO_ST *pstChnSetInfo;

	memset(buff, 0, sizeof(buff));
	pstChnCnt = (WEBOCX_CHNPARA_ST *)buff;
	//pstChnSetInfo = (WEBOCX_CHN_INFO_ST *)(buff + sizeof(WEBOCX_CHNPARA_ST));

	pstChnCnt->ChnCnt =CHNNUM;

	cout<<PRO_NAME<<"AV_OCX_ChnParaGet len "<<len<<endl;

	memcpy(buff + sizeof(WEBOCX_CHNPARA_ST),devicecamparam, (sizeof(WEBOCX_CHN_INFO_ST) * CHNNUM));
	SndLen = CHNNUM *sizeof(WEBOCX_CHN_INFO_ST) + sizeof(WEBOCX_CHNPARA_ST);

	AV_OCX_SendToWeb(AV_WEBOCX_CHN_PARA_GET + 1, buff, SndLen);
	//AV_OCX_SendToGate(AV_TO_APP_MOD, AV_APP_CHN_PARA_GET, NULL, 0);
	return ERROR_SUCCESS;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_DevParaSet
 Description  : 客户端设置设备参数
 Input        : char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/27
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_DevParaSet(int fd, char *data, int len)
{
	AV_WEBOCX_PACKAGE_ST *pstOcxHead = NULL;
	WEBOCX_DEVPARA_ST *pstDevPara = NULL;
	char ret = 1;

	cout<<PRO_NAME<<"AV_OCX_DevParaSet len "<<len<<endl;



	pstOcxHead = (AV_WEBOCX_PACKAGE_ST *)data;
	pstDevPara = (WEBOCX_DEVPARA_ST *)(pstOcxHead->data+sizeof(int));

	OcxCycleBuf->DebugPrintHex((char *)pstOcxHead, len);

	OcxCycleBuf->DebugPrintHex((char *)pstDevPara, (len - WEBOCX_HEAD_COMM_LEN));
#if 0
	cout<<"pstDevPara->DevId "<<pstDevPara->DevId<<endl;
	cout<<"pstDevPara->DevName "<<pstDevPara->DevName<<endl;
	cout<<"pstDevPara->PlatformDevId "<<pstDevPara->PlatformDevId<<endl;
	cout<<"pstDevPara->SipSvrIp "<<pstDevPara->SipSvrIp<<endl;
	cout<<"pstDevPara->SipSvrPort "<<pstDevPara->SipSvrPort<<endl;
	cout<<"pstDevPara->LocalIp "<<pstDevPara->LocalIp<<endl;
	cout<<"pstDevPara->LocalPort "<<pstDevPara->LocalPort<<endl;
#endif

	printf("pstDevPara->DevId %s\n", pstDevPara->DevId);
	printf("pstDevPara->DevName %s\n", pstDevPara->DevName);
	printf("pstDevPara->SipSvrIp %s\n", pstDevPara->SipSvrIp);
	printf("pstDevPara->SipSvrPort %d\n", pstDevPara->SipSvrPort);
	printf("pstDevPara->LocalIp %s\n", pstDevPara->LocalIp);
	printf("pstDevPara->LocalPort %d\n", pstDevPara->LocalPort);

	ret = 1;
	SetXmlDeviceParam(pstDevPara);
	GetXmlDeviceParam(&deviceparam);

	AV_OCX_SendToWeb(AV_WEBOCX_DEV_PARA_SET + 1,(char *)&ret, sizeof(ret));
	//AV_OCX_SendToGate(AV_TO_APP_MOD, AV_APP_DEV_PARA_SET, (char *)&pstDevPara->DevId, sizeof(WEBOCX_DEVPARA_ST) - sizeof(int));

	return ERROR_SUCCESS;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_ChnParaSet
 Description  : 通道参数设置
 Input        : char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2014/12/27
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_ChnParaSet(int fd, char *data, int len)
{
	int SndLen;
	AV_WEBOCX_PACKAGE_ST *pstOcxHead = NULL;
	WEBOCX_CHN_INFO_ST *pstChnInfo = NULL;
	char ret = 1;

	cout<<PRO_NAME<<"AV_OCX_ChnParaSet len "<<len<<endl;

	pstOcxHead = (AV_WEBOCX_PACKAGE_ST *)data;
	pstChnInfo = (WEBOCX_CHN_INFO_ST *)(pstOcxHead->data + sizeof(int));

	ret = 0;
	//AV_OCX_SendToWeb(AV_WEBOCX_CHN_PARA_SET + 1,(char *)&ret, sizeof(ret));

	printf("pstChnInfo 0x%x", (int)pstChnInfo);
	printf("pstChnInfo->ChnID %d\n", pstChnInfo->ChnID);
	printf("pstChnInfo->PlatformDevId %s\n", pstChnInfo->PlatformDevId);
	printf("pstChnInfo->ChnName %s\n", pstChnInfo->ChnName);
	printf("pstChnInfo->CameraAddr %d\n", pstChnInfo->CameraAddr);
	printf("pstChnInfo->ProtType %d\n", pstChnInfo->ProtType);


	/* 去除公共头长度，和用户ID的4个字节 */
	SndLen = len - WEBOCX_HEAD_COMM_LEN - sizeof(int);

	OcxCycleBuf->DebugPrintHex((char *)pstChnInfo, SndLen);

	SetXmlCamParam(CHNNUM,pstChnInfo);
	GetXmlCamParam(CHNNUM,devicecamparam);
	GetXmlcamerid(sipcamerid);

	AV_OCX_SendToWeb(AV_WEBOCX_CHN_PARA_SET+1, &ret, sizeof(char));
	return ERROR_SUCCESS;
}


/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_RemoteReset
 Description  : 重启设备
 Input        : int fd
                char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2015/1/8
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_RemoteReset(int fd, char *data, int len)
{
	int result = 0;
	cout<<PRO_NAME<<"AV_OCX_RemoteReset len "<<len<<endl;
	AV_OCX_SendToWeb(AV_WEBOCX_REMOTE_RESET + 1,(char *)&result, sizeof(result));

	//sleep(1);
	TM_USleep(1,0);
	logout();
	//AV_OCX_SendToGate(AV_TO_APP_MOD, AV_GATE_REMOTE_RESET, NULL, 0);
	return ERROR_SUCCESS;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_FrameLostGet
 Description  : 帧丢失参数获取，通道的报警使能
 Input        : int fd
                char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2015/1/9
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_FrameLostGet(int fd, char *data, int len)
{
	cout<<PRO_NAME<<"AV_OCX_FrameLostGet len "<<len<<endl;
	AV_OCX_SendToWeb(AV_WEBOCX_FRAME_LOST_GET_RESP,(char *)&voideloseparam,sizeof(WEBOCX_LOSTFRAM_ST));


#if 0
	AV_OCX_SendToGate(0x02,
					  AV_APP_FRAME_LOST_GET,
					  NULL,
					  0);
#endif
    return ERROR_SUCCESS;
}

/*****************************************************************************
 Prototype    : WebOcxClass.AV_OCX_FrameLostSet
 Description  : 丢帧设置
 Input        : int fd
                char *data
                int len
 Output       : None
 Return Value : int

  History        :
  1.Date         : 2015/1/9
    Author       : xiaomengmeng
    Modification : Created function

*****************************************************************************/
int WebOcxClass::AV_OCX_FrameLostSet(int fd, char *data, int len)
{
	AV_WEBOCX_PACKAGE_ST *pstOcxHead = NULL;
	WEBOCX_LOSTFRAM_ST *pstChnLostFrame = NULL;

	cout<<PRO_NAME<<"AV_OCX_FrameLostSet len "<<len<<endl;

	pstOcxHead = (AV_WEBOCX_PACKAGE_ST *)data;
	pstChnLostFrame = (WEBOCX_LOSTFRAM_ST *)(pstOcxHead->data + sizeof(int));

	voideloseparam.Enable = pstChnLostFrame->Enable;
	voideloseparam.ChnBit = pstChnLostFrame->ChnBit;

	SetXmlVoideLoseParam(pstChnLostFrame);

	char ret =1;
	printf("Enable %d ChnBit %d\n", pstChnLostFrame->Enable, pstChnLostFrame->ChnBit);
	AV_OCX_SendToWeb(AV_WEBOCX_FRAME_LOST_SET_RESP, &ret, sizeof(char));

#if 0
	AV_OCX_SendToGate(0x02,
					  AV_APP_FRAME_LOST_SET,
					  (char *)pstChnLostFrame,
					  sizeof(WEBOCX_LOSTFRAM_ST));
#endif
    return ERROR_SUCCESS;
}

int WebOcxClass::AV_OCX_SysTimeSet(int fd, char *data, int len)
{

	//AV_WEBOCX_PACKAGE_ST *pstOcxHead = NULL;
	//WEBOCX_SYSTIME_ST *pstSysTime = NULL;
	char result = 0;
	//struct rtc_time rtc_tm;
	//memset(&rtc_tm, 0, sizeof (struct rtc_time));

	//pstOcxHead = (AV_WEBOCX_PACKAGE_ST *)data;
	//pstSysTime = (WEBOCX_SYSTIME_ST *)(pstOcxHead->data + sizeof(int));
/*
	rtc_tm.tm_year = pstSysTime->Year;
	rtc_tm.tm_mon = pstSysTime->Month;
	rtc_tm.tm_mday = pstSysTime->Date;
	rtc_tm.tm_hour = pstSysTime->Hour;
	rtc_tm.tm_min = pstSysTime->Minut;
	rtc_tm.tm_sec = pstSysTime->Second;

	printf("System Set %d-%d-%d %d:%d:%d\n",
			pstSysTime->Year+1900,
			pstSysTime->Month,
			pstSysTime->Date,
			pstSysTime->Hour,
			pstSysTime->Minut,
			pstSysTime->Second);
*/


	AV_OCX_SendToWeb(AV_WEBOCX_TIMESET_RESP,(char *)&result, sizeof(char));

    return ERROR_SUCCESS;
}

int WebOcxClass::AV_OCX_PlatLogIn(int fd, char *data, int len)
{
	cout<<PRO_NAME<<"AV_OCX_PlatLogIn len "<<len<<endl;

	AV_OCX_SendToGate(0x02,
					  AV_APP_PLAT_LOGIN,
					  NULL,
					  0);

	return ERROR_SUCCESS;
}

int WebOcxClass::AV_OCX_PlatLogOut(int fd, char *data, int len)
{
	cout<<PRO_NAME<<"AV_OCX_PlatLogOut len "<<len<<endl;

	AV_OCX_SendToGate(0x02,
					  AV_APP_PLAT_LOGOUT,
					  NULL,
					  0);
	return ERROR_SUCCESS;
}

void WebOcxClass::AV_OCX_DebugFdMap(void)
{
	map<int, WEBOCX_STREAM_CTRL_ST*>::iterator iter;

	if (1 != StreamFdMap.size())
	{
		return;
	}

	iter = StreamFdMap.begin();
	if (iter == StreamFdMap.end())
	{
	    return;
	}

	printf("Only One Fd Map: fd %d\n", iter->second->fd);
	printf("Only One Fd Map: ChnId %d\n", iter->second->ChnId);
	printf("Only One Fd Map: key %d\n", iter->second->key);
	printf("Only One Fd Map: VideoViewType %d\n", iter->second->VideoViewType);
	printf("Only One Fd Map: StreamType %d\n", iter->second->StreamType);
	printf("Only One Fd Map: run %d\n", iter->second->run);
	printf("Only One Fd Map: pause %d\n", iter->second->pause);

	return;
}


