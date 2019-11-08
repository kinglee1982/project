#ifndef _COMMPROTOCOL__H_
#define _COMMPROTOCOL__H_


#pragma once

//ϵͳ�����
#define ADDRESSINFOSEARCH_REQ		0x01		//��ַ��Ϣ��ѯ����
#define ADDRESSINFOSEARCH_RESP		0x02		//��ַ��Ϣ��ѯ��Ӧ
#define ADDRESSINFOSET_REQ			0x03		//��ַ��Ϣ��������
#define ADDRESSINFOSET_RESP			0x04		//��ַ��Ϣ������Ӧ

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
	//tm �豸��ͨ�����ã�
	unsigned char bChannelNum;
	DSKChannelPara chnPara[16];

	//tm ���������ã�
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

	//tm �豸��ͨ�����ã�
	unsigned char bChannelNum;
	DSKChannelPara chnPara[16];

	//tm ���������ã�
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

//ͨ�����ݰ�==========================================
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

