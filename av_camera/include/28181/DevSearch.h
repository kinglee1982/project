/******************************************************************************

                  版权所有 (C), 2001-2011, 郑州天迈科技有限公司

 ******************************************************************************
  文 件 名   : DevSearch.h
  版 本 号   : 初稿
  作    者   : mabuyun
  生成日期   : 2015年9月20日
  最近修改   :
  功能描述   : DevSearch.cpp 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2015年9月20日
    作    者   : mabuyun
    修改内容   : 创建文件

******************************************************************************/


#ifndef __DEVSEARCH_H__
#define __DEVSEARCH_H__

#include "CommProtocol.h"

#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <net/route.h>
#include<signal.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/shm.h>

typedef struct SYS_MSG_BUF{
	long 		Des; ///< Where this message go. See /ref Sys_Msg_Def.h
	int			cmd; ///< command to control system server.
	int			src; ///< Where this message from. See /ref Sys_Msg_Def.h
	int			shmid; ///< Share memory which was used to communicate with system server.
	int			length; ///< Data length.
	int			offset; ///< Offset to begin of share memory.
} SYS_MSG_BUF;


typedef pthread_mutex_t * SemHandl_t;



extern int DestroySem(SemHandl_t hndlSem);
extern void* DevSearchEntry(void*);
extern int GetNetCardInfo(NetCardInfo &netcardinfo);
extern in_addr_t GetNetIP(const char *ifname);
extern int GetNetMacAddress(const char *ifname, unsigned char *mac);
extern in_addr_t GetNetmask(const char *ifname);
extern int GetSoftwareVer(char *cur_version);
extern SemHandl_t MakeSem();
extern int Msg_Init( int msgKey );
extern int NetAddGateway(in_addr_t addr);
extern int NetDelGateway(in_addr_t addr);
extern in_addr_t NetGetGateway(void);
extern int NetSearchGateway(char *buf, in_addr_t *gate_addr);
extern int NetSetGateway(in_addr_t addr);
extern int pShareMemInit(int key);
extern int SemRelease(SemHandl_t hndlSem);
extern int SemWait(SemHandl_t hndlSem);
extern int SetNetCardInfo(NetCardInfo &netcardinfo);
extern int SetNetIP(const char *ifname, in_addr_t addr);
extern int SetNetMask(const char *ifname, in_addr_t addr);
extern void ShareMemWrite(int offset,void *buf, int length);

#endif /* __DEVSEARCH_H__ */
