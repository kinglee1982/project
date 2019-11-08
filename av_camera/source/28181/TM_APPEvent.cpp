#include "TM_APPEvent.h"
#include "TM_APP_PtzManager.h"
#include "TM_MCUSocket.h"
#include "tinyxml2API.h"
#include "TM_Alarm.h"
#include "av_public_head.h"
#include "av_crc16.h"

using namespace std;
//extern DataBuf *Socket_ReceiveBuf;

/*全局变量*/
WEBOCX_NETWORK_PARA_ST devicenetparam;
WEBOCX_CAMERA_PARA_ST devicecamPTZparam[CHNNUM];
WEBOCX_DEVPARA_ST deviceparam;
WEBOCX_CHN_INFO_ST devicecamparam[CHNNUM];
WEBOCX_NODE_SET_ST devicenodeparam[1];
WEBOCX_LOSTFRAM_ST voideloseparam;
WEBOCX_MD_SET_ST voidemdparam[CHNNUM];



unsigned char GetBagId(void)
{
	static unsigned char ulBagId = 0;
    return ulBagId++;
}

int writeCrc16Data(const int fd, int DestId,char cmd ,const char *buf, short len)
{
	short alllen;
	int result;
	int id_len = 4;
	unsigned short crc;
	const char *id = "$$$$";
	TM_CMD_HEAD_ST stCmdData;

	if (fd <= 0)
	{
	    return -1;
	}

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

	alllen = id_len + 2 + 4 + len + 2;

	memcpy(stCmdData.bag_id, id, id_len);

	stCmdData.cmdlen_h =  alllen >> 8; //copy length
	stCmdData.cmdlen_l =  alllen & 0x00FF;

	stCmdData.ucSrcDevID = APPADDER;
	stCmdData.ucDestDevID = DestId;
	//stCmdData.ucBagID = GetBagId();
	stCmdData.ucCmd = cmd;


	memcpy(stCmdData.buff, buf, len);

	crc = crc16((unsigned char *)&stCmdData, alllen - 2);
	stCmdData.buff[len] = crc >> 8;
	stCmdData.buff[len + 1] = crc & 0x00FF;

    printf("app socket write data +++++++++++++++++++++++\n");
	//Socket_ReceiveBuf->DebugPrintHex((char *)&stCmdData,alllen);

	result = (write(fd, &stCmdData, alllen) == alllen);
	if (result != 1)
	{
	  perror("app socket write error+++++++++++++++++++++++\n");
      /* BEGIN: Added by wuquanwei, 2015/1/15   PN:写失败没有关闭描述符 */
      shutdown(fd, SHUT_RDWR);
      /* END:   Added by wuquanwei, 2015/1/15   PN:写失败没有关闭描述符 */
	}
    else
    	printf("app send  out data is OK \n");

	return 0;
}

int TmAvMsg(int Socketfd,char *buf,int len)
{

	TM_CMD_HEAD_ST *stBuf = (TM_CMD_HEAD_ST *)buf;
	int iRet = 0;

	switch(stBuf->ucSrcDevID)
	{
		case AVADDER:
				iRet = AvMsgProc(Socketfd,buf,len);
				if(iRet < 0)
					{
						printf("AvMsgProc%d:\n",iRet);
					}
			break;
		default:
			//printf("stBuf->ucSrcDevID: default err %d\n",(stBuf->ucSrcDevID));
			break;

	}

	return iRet;
}


int AvMsgProc(int Socketfd,char *buf,int len)
{
#if 0
	int datalen = 0;
	TM_CMD_HEAD_ST *pstLocalCmdHdr = NULL;
	UCHAR *pData = NULL;

	pstLocalCmdHdr = (TM_CMD_HEAD_ST *)buf;
	pData = pstLocalCmdHdr->buff;

	/* 长度除去包括命令字段在内的报文头部10字节，不包含CRC校验2字节 */
	datalen = len - TM_CMD_HEAD_ST_CMDPOS;
	printf("current AvMsgProc rcv all msg len is %d.\n",len);
	switch(pstLocalCmdHdr->ucCmd)
	{
		case APP_CAMERA_CTRL:
			ControlPTZ(Socketfd,pData,datalen);
			//OK
			break;
		case APP_MD_GET:
			writeCrc16Data(Socketfd,AVADDER,APP_MD_GET,(char *)voidemdparam,(sizeof(WEBOCX_MD_SET_ST))*4);
			//OK
			break;
		case APP_MD_SET:
			SetDeviceMD(Socketfd,pData,datalen);
			printf("readXmlMDParam is start\n");
			readXmlMDParam(voidemdparam);
			//
			break;
		case APP_ALARM_GET:
			writeCrc16Data(Socketfd,AVADDER,APP_ALARM_GET,(char *)&devicenodeparam,sizeof(WEBOCX_NODE_SET_ST));
			//OK
			break;
		case APP_ALARM_SET:
			SetDeviceAralm(Socketfd,pData,datalen);
			GetXmlNodeParam(1,devicenodeparam);
			//OK
			break;
		case APP_DEV_PARA_GET:
			writeCrc16Data(Socketfd,AVADDER,APP_DEV_PARA_GET,(char *)&deviceparam,sizeof(WEBOCX_DEVPARA_ST));
			//OK
			break;
		case APP_DEV_PARA_SET:
			SetDeviceParam(Socketfd,pData,datalen);
			GetXmlDeviceParam(&deviceparam);
			//OK
			break;
		case APP_CHN_PARA_GET:
			GetDeviceCameraAddr(Socketfd);
			//OK
			break;
		case APP_CHN_PARA_SET:
			SetDeviceCameraAddr(Socketfd,pData,datalen);
			GetXmlCamParam(CHNNUM,devicecamparam);
			GetXmlcamerid(sipcamerid);
			//OK
			break;
		case APP_NETWORK_PARA_GET:
			writeCrc16Data(Socketfd,AVADDER,APP_NETWORK_PARA_GET,(char *)&devicenetparam,sizeof(WEBOCX_NETWORK_PARA_ST));
			//OK
			break;
		case APP_NETWORK_PARA_SET:
			SetDeviceNetParam(Socketfd,pData,datalen);
			GetXmlNetParam(&devicenetparam);
			//OK
			break;
		case APP_CAMERA_PARA_GET:
			GetCamPTZParam(Socketfd,pData,datalen);
			//OK
			break;
    	case APP_CAMERA_PARA_SET:
			SetCamPTZParam(Socketfd,pData,datalen);
			GetXmlCamPTZParam(CHNNUM,devicecamPTZparam);
			GetXmlcamerid(sipcamerid);
			//OK
			break;
		case APP_VOIDLOSE_PARA_GET:
			writeCrc16Data(Socketfd,AVADDER,APP_VOIDLOSE_PARA_GET,(char *)&voideloseparam,sizeof(WEBOCX_LOSTFRAM_ST));
			//OK
			break;
    	case APP_VOIDLOSE_PARA_SET:
			SetVoideLoseParam(Socketfd,pData,datalen);
			//OK
			break;
		case APP_DEVICE_REBOOT:
			printf("device reboot\n");
			bootflag = 1;
			break;
		default:
			printf("pstLocalCmdHdr->ucCmd:%d\n",pstLocalCmdHdr->ucCmd);
			break;
	}
#endif
	return 0;

}

/*PTZ控制*/
int ControlPTZ(int Socketfd,UCHAR *buf,int len)
{
	CPTZManager m_cptz;
	m_cptz.SetSendDataHandle(SendPtzMsgToMcu);
	WEBOCX_CAMERA_CTRL_ST* devicePTZ = NULL;
	devicePTZ = (WEBOCX_CAMERA_CTRL_ST *)buf;
	//char result = 1;
	int i = 0;

	for(i=0;i<CHNNUM;i++)
	{
		printf("devicePTZ->ChnId:%d\n",devicePTZ->ChnId);
		if(devicePTZ->ChnId == i)
		{
			if(NULL == devicePTZ->Parar)
			{
				m_cptz.onPtzHandle(sipcamerid[i].CameraPDSIP,devicePTZ->Action,sipcamerid[i].CamerAddSIP,devicePTZ->Speed,devicePTZ->CameraCtrl,devicePTZ->Parar);
			}
			else
			{
				m_cptz.onPtzHandle(sipcamerid[i].CameraPDSIP,devicePTZ->CameraCtrl,sipcamerid[i].CamerAddSIP,devicePTZ->Speed,devicePTZ->Action,devicePTZ->Parar);
			}

			break;
		}
	}

	//writeCrc16Data(Socketfd,AVADDER,APP_CAMERA_CTRL,&result,1);
	return 0;
}

/*设置系统IP信息*/
int SetDeviceNetParam(int Socketfd,UCHAR *buf,int len)
{
	//char result = 1;
	WEBOCX_NETWORK_PARA_ST *pdevicenetparam = NULL;
	pdevicenetparam = (WEBOCX_NETWORK_PARA_ST *)buf;

	SetXmlNetParam(pdevicenetparam);
	//writeCrc16Data(Socketfd,AVADDER,APP_NETWORK_PARA_SET,&result,1);
	//设置系统IP信息
	netConfig(pdevicenetparam);

	return 0;
}

/*获取PTZ信息*/
int GetCamPTZParam(int Socketfd,UCHAR *buf,int len)
{
	WEBOCX_CAMERA_PARA_ST *pdevicecamPTZparam = NULL;
	pdevicecamPTZparam = (WEBOCX_CAMERA_PARA_ST *)buf;
	int i = 0;

	for(i=0;i<CHNNUM;i++)
	{
		printf("pdevicecamPTZparam->chunum:%d\n",pdevicecamPTZparam->ChnID);
		if(pdevicecamPTZparam->ChnID == i)
		{
			//writeCrc16Data(Socketfd,AVADDER,APP_CAMERA_PARA_GET,(char *)&devicecamPTZparam[i],sizeof(WEBOCX_CAMERA_PARA_ST)*4);
			break;
		}
	}

	return 0;
}

/*设置PTZ信息*/
int SetCamPTZParam(int Socketfd,UCHAR *buf,int len)
{
	//char result = 1;
	char buffer[16] = {0};
	TM_CAMER_SEEP_MCU stmcamerseep;
	WEBOCX_CAMERA_PARA_ST *pdevicecamPTZparam = NULL;
	pdevicecamPTZparam = (WEBOCX_CAMERA_PARA_ST *)buf;

	SetXmlCamPTZParam(pdevicecamPTZparam->ChnID,pdevicecamPTZparam);
	//writeCrc16Data(Socketfd,AVADDER,APP_CAMERA_PARA_SET,&result,1);

#if 1
	memset(buffer,0,sizeof(buffer));
	sprintf(buffer,"%d",pdevicecamPTZparam->Speed);
	strcpy(stmcamerseep.camerseepmcu,buffer);

	//writeCrc16Data(Socketfd,MCUADDER,COMSEEP,(char *)&stmcamerseep,sizeof(TM_CAMER_SEEP_MCU));
#endif
	return 0;
}

/*设置设备信息*/
int SetDeviceParam(int Socketfd,UCHAR *buf,int len)
{
	//char result = 1;
	WEBOCX_DEVPARA_ST *pDeviceParame =NULL;
	pDeviceParame = (WEBOCX_DEVPARA_ST *)buf;
	SetXmlDeviceParam(pDeviceParame);

	//writeCrc16Data(Socketfd,AVADDER,APP_DEV_PARA_SET,&result,1);
	return 0;
}

/*获取摄像头信息*/
int GetDeviceCameraAddr(int Socketfd)
{
	char buffer[1024] = {0};
//	int len = 0;

	WEBOCX_CHNPARA_ST cameranum;
	cameranum.ChnCnt = CHNNUM;

	memcpy(buffer, &cameranum, sizeof(WEBOCX_CHNPARA_ST));
	memcpy(buffer + sizeof(WEBOCX_CHNPARA_ST),devicecamparam, (sizeof(WEBOCX_CHN_INFO_ST) * cameranum.ChnCnt));

	//len = sizeof(WEBOCX_CHNPARA_ST) + (sizeof(WEBOCX_CHN_INFO_ST) * cameranum.ChnCnt);

	//writeCrc16Data(Socketfd,AVADDER,APP_CHN_PARA_GET,buffer,len);
	return 0;
}

/*设置摄像头信息*/
int SetDeviceCameraAddr(int Socketfd,UCHAR *buf,int len)
{
	//char result = 1;
	WEBOCX_CHN_INFO_ST* pCameraAdd= NULL;

	pCameraAdd = (WEBOCX_CHN_INFO_ST*)buf;
	#if 0
	SetXmlCamParam(pCameraAdd->Chnnum,pCameraAdd);
	printf("pCameraAdd->Chnnum:%d",pCameraAdd->Chnnum);
	#endif
	SetXmlCamParam(CHNNUM,pCameraAdd);

	//writeCrc16Data(Socketfd,AVADDER,APP_CHN_PARA_SET,&result,1);

	return 0;
}

/*设置节点信息*/
int SetDeviceAralm(int Socketfd,UCHAR *buf,int len)
{
	//char result = 1;
	WEBOCX_NODE_SET_ST *pNodeNum = NULL;
	pNodeNum = (WEBOCX_NODE_SET_ST *)buf;

	SetXmlNodeParam(pNodeNum->NodeId,pNodeNum);

	//writeCrc16Data(Socketfd,AVADDER,APP_ALARM_SET,&result,1);

	return 0;
}

/*设置视频丢失信息*/
int SetVoideLoseParam(int Socketfd,UCHAR *buf,int len)
{
	//char result = 1;
	WEBOCX_LOSTFRAM_ST *voideparam = NULL;
	voideparam = (WEBOCX_LOSTFRAM_ST *)buf;

	voideloseparam.Enable = voideparam->Enable;
	voideloseparam.ChnBit = voideparam->ChnBit;

	SetXmlVoideLoseParam(voideparam);
	//writeCrc16Data(Socketfd,AVADDER,APP_VOIDLOSE_PARA_SET,&result,1);

	return 0;
}

/*设置移动侦测信息*/
int SetDeviceMD(int Socketfd,UCHAR *buf,int len)
{
	//char result = 0;
	WEBOCX_MD_SET_ST *pDeviceMd = NULL;
	pDeviceMd = (WEBOCX_MD_SET_ST *)buf;

	SetXmlMDParam(CHNNUM,pDeviceMd);
	//SetXmlMDParam(pDeviceMd->ChnNum,pDeviceMd);
	//writeCrc16Data(Socketfd,AVADDER,APP_MD_SET,&result,1);

	return 0;
}



