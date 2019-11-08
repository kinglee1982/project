#ifndef __TM_APPEVENT_H_
#define __TM_APPEVENT_H_
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <pthread.h>
#include <strings.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>
#include <arpa/inet.h>

#include "Nvs.h"
#include "av_ocx_def.h"
#include "av_ocx_type.h"

#include "tinyxml2API.h"

#define CHNNUM 0x0c//8
#define CHN_MAX_NUM     0x0c//8
#define UCHAR   unsigned char

#define APPADDER (0x36)
#define AVADDER (0x32)
#define MCUADDER (0x39)
#define COMSEEP  (0x17)



extern device_camer_id sipcamerid[CHNNUM];
extern int bootflag;
/*声明全局变量*/
extern WEBOCX_NETWORK_PARA_ST devicenetparam;
extern WEBOCX_CAMERA_PARA_ST devicecamPTZparam[CHNNUM];
extern WEBOCX_DEVPARA_ST deviceparam;
extern WEBOCX_CHN_INFO_ST devicecamparam[CHNNUM];
extern WEBOCX_NODE_SET_ST devicenodeparam[1];
extern WEBOCX_LOSTFRAM_ST voideloseparam;
extern WEBOCX_MD_SET_ST voidemdparam[CHNNUM];



#define TM_CMD_HEAD_ST_CMDPOS 10

typedef struct camerseep
{
    char camerseepmcu[CHNNUM];//[8];
    short int lave;
}__attribute__((packed)) TM_CAMER_SEEP_MCU;




int TmAvMsg(int Socketfd,char *buf,int len);
int AvMsgProc(int Socketfd,char *buf,int len);
int ControlPTZ(int Socketfd,UCHAR *buf,int len);
int SetDeviceNetParam(int Socketfd,UCHAR *buf,int len);
int GetCamPTZParam(int Socketfd,UCHAR *buf,int len);
int SetCamPTZParam(int Socketfd,UCHAR *buf,int len);
int SetDeviceParam(int Socketfd,UCHAR *buf,int len);
int GetDeviceCameraAddr(int Socketfd);
int SetDeviceCameraAddr(int Socketfd,UCHAR *buf,int len);
int SetDeviceAralm(int Socketfd,UCHAR *buf,int len);
int SetDeviceMD(int Socketfd,UCHAR *buf,int len);
int SetVoideLoseParam(int Socketfd,UCHAR *buf,int len);
int writeCrc16Data(const int fd, int DestId,char cmd ,const char *buf, short len);




#endif


