#ifndef _COMMPROTOCOL__H_
#define _COMMPROTOCOL__H_


#pragma once

//系统命令定义
#define ADDRESSINFOSEARCH_REQ		0x01		//地址信息查询请求
#define ADDRESSINFOSEARCH_RESP		0x02		//地址信息查询响应
#define ADDRESSINFOSET_REQ			0x03		//地址信息设置请求
#define ADDRESSINFOSET_RESP			0x04		//地址信息设置响应

#pragma pack(1)

struct NetCardInfo
{
	char szMAC[32];
	char szIP[32];
	char szMask[32];
	char szGateway[32];
};

struct FrameHead
{
	unsigned char	bVersion;
	unsigned char	bFrameType;
	unsigned int	uFrameTotalLen;
	unsigned int	uFrameSeq;
	unsigned short	sCRCC;
};

struct DSKChannelPara
{
	char szChannelName[20];
	char szChannelID[20];
};

struct HHAddSearchResp
{
	char szMAC[32];
	char szIP[32];
	char szMask[32];
	char szGateway[32];
	char szSoftWareVer[64];
	//tm 设备及通道配置；
	unsigned char bChannelNum;
	DSKChannelPara chnPara[16];

	//tm 服务器设置；
	char szServerIP[32];
	unsigned short	uServerPort;
	char szServerID[20];
};



struct HHAddSetReq
{
	char szMAC[32];
	char szIP[32];
	char szMask[32];
	char szGateway[32];

	//tm 设备及通道配置；
	unsigned char bChannelNum;
	DSKChannelPara chnPara[16];

	//tm 服务器设置；
	char szServerIP[32];
	unsigned short	uServerPort;
	char szServerID[20];


};


struct HHAddSetResp
{
	char szMAC[32];
	char szIP[32];
	char szMask[32];
	char szGateway[32];
	unsigned char uRet;
};

//通信数据包==========================================
struct COMPACKET
{
	FrameHead header;
	union
	{
		HHAddSearchResp AddSearchResp;

		HHAddSetReq AddSetReq;
		HHAddSetResp AddSetResp;
	}body;
};
//=====================================================
#pragma pack()

#endif

