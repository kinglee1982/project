#include "TM_APPEvent.h"
#include "TM_MCUSocket.h"
#include "av_public_head.h"
#include "av_crc16.h"

extern int cmd_sockfd;
extern unsigned char GetBagId(void);
/* 外部扩展IO状态 */
/*
BIT7:前门开
BIT6:前门关
BIT5:后门开
BIT4:后门关
BIT3:倒车
BIT2:ACC
BIT1~0:预留
对应位表示变化后的状态,为1时触发，0：未触发
*/
unsigned char g_extIOstatus = 0;

int SendPtzMsgToMcu(char *buf,int len)
{
	short alllen;
	int result;
	int id_len = 4;
	unsigned short crc;
	const char *id = "$$$$";
	TM_CMD_HEAD_ST stCmdData;

	if (cmd_sockfd <= 0)
	{
		printf("APP socket %d error \n",cmd_sockfd);
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
	stCmdData.ucDestDevID = MCUADDER;
	stCmdData.ucBagID = GetBagId();
	stCmdData.ucCmd = MCU485TRANS;

	memcpy(stCmdData.buff, buf, len);

	crc = crc16((unsigned char *)&stCmdData, alllen - 2);
	stCmdData.buff[len] = crc >> 8;
	stCmdData.buff[len + 1] = crc & 0x00FF;
	result = (write(cmd_sockfd, &stCmdData, alllen) == alllen);
	debug_print("SendPtzMsgToMcu",(char*)&stCmdData, alllen);

	if (result != 1)
	{
		printf("AV socket write \n");
		return -1;
	}

	return 0;
}


int SendIOCtrlMsgToMcu( char *buf,int len)
{
	return writeCrc16Data(cmd_sockfd,MCUADDER,MCUIOCTRONL,buf,len);
}

/* IO控制状态， */
int CtrlIOStatue(CtrlIO_e iobit,int flag)
{
	char buf[2] = {0};
	buf[0] |= (1<<iobit); //设置掩码
	if(flag)
	{
		buf[1] |= (1<<iobit);
	}

	return SendIOCtrlMsgToMcu(buf,2);
}


int RebootArm()
{
	char tmp = 0xff;
	return writeCrc16Data(cmd_sockfd,MCUADDER,MCUREBOOTARM,&tmp,1);
}

static int (*s_pFuncIOAlarm)(char*,int);

void setMcuIOAlarmCallback(int (* pFunc)(char*,int))
{
	s_pFuncIOAlarm = pFunc;
}

/*
IO状态掩码	01
BIT7:前门开
BIT6:前门关
BIT5:后门开
BIT4:后门关
BIT3:倒车
BIT2:ACC
BIT1~0:预留
对应位为1时，表示对应的状态有变化

IO状态	01
BIT7:前门开
BIT6:前门关
BIT5:后门开
BIT4:后门关
BIT3:倒车
BIT2:ACC
BIT1~0:预留
对应位表示变化后的状态,为1时触发，0：未触发
*/

int IOMsgProc(int Socketfd,char *buf,int len)
{
	printf(" get IO msg  \n");
	TM_CMD_HEAD_ST *stBuf = (TM_CMD_HEAD_ST *)buf;
	//s_pFuncIOAlarm((char*)stBuf->buff,2);
	g_extIOstatus = stBuf->buff[1];
	return 0;
}

unsigned char GetMcuAlarmIO()
{
	return g_extIOstatus;
}

int TmMCUMsg(int Socketfd,char *buf,int len)
{

	TM_CMD_HEAD_ST *stBuf = (TM_CMD_HEAD_ST *)buf;
	int iRet = 0;

	switch(stBuf->ucCmd)
	{
		case MCUIOREPORT:
		{
			iRet = IOMsgProc(Socketfd,buf,len);
			break;
		}
		default:
			printf("stBuf->ucSrcDevID: default err %d\n",(stBuf->ucSrcDevID));
			break;

	}
	return iRet;
}



