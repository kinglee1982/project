/******************************************************************************

                  ��Ȩ���� (C), 2012-2014, ֣�������Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : DataBuf.h
  �� �� ��   : ����
  ��    ��   : wuquanwei
  ��������   : 2012��5��25��

  �޸���ʷ   :
  1.��    ��   : 2012��5��25��
    ��    ��   : wuquanwei
    �޸�����   : �����ļ�

2012-05-25: ���������ð汾 V1.001.01 wuquanwei
2012-05-29: ���ӻ������������ݰ�ͷ���������ݰ���������������� V1.001.02 wuquanwei
2012-06-01: �޸İ���ʶ��صĴ��룬���ð���ʶ�������ӱ�ʶ���� V1.001.03 wuquanwei
2012-06-08: 1���޸�CircularBuffer.cpp�е�GetMaxReadSize()��
			PeekChar( int iPos, char & ch )
			�����ж϶�ָ����ܳ��ȵıȽ�:�������m_iReadPtr > m_nBufSize�����
			��m_iReadPtr = m_nBufSize % m_iReadPtr,������ֹԽ����ֵĶδ���
			2�����Ӵ�ӡ������Ϣ����PrintErrMsg(int ErrNo)
			V1.001.04 wuquanwei

2012-11-18: ������չ
			1�����Ӱ汾�Ŵ�ӡ
			2�����Ӱ�������ʼ�ֶ�����
			3�����Ӱ����ȴ�С������
			V1.001.05 wuquanwei

2012-12-12: �޸�
			1��ȥ���ź�����ʹ�ã�ȥ���������ýӿڣ�û�кϷ����ݰ��������ء�
			2�����ӻ���������pushdata��popdata�л���
			V1.001.06 wuquanwei

2013-4-24: �޸�
			1����������ʣһ���ֽڿ�д�ռ�ʱ�ͷ��ػ���������
			V1.001.07 wuquanwei

2013-4-27: �޸�
			1�������������������û�кϷ����ݰ�ͷ���Ǿ���ջ�����
			V1.001.08 wuquanwei

2013-5-28: �޸�
			1���������ӡ�����޸ģ���ӽӿ�:
			PrintErrMsg(const char *userMsg,int ErrNo);
			��ӡ��Ϣ������Ӹ���ʶͷ�������������Ǹ�ģ���ӡ�ġ�

			V1.001.09 wuquanwei

******************************************************************************/

#ifndef __DATABUF_H_
#define __DATABUF_H_
#define ON_LINUX


#include <string>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#ifdef ON_LINUX
#include <semaphore.h>
#endif
#include "circular_buffer.h"

//#include "DataBuf.h"

/************************************************************************/
#ifndef WIN32
typedef int BOOL;
#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))
#define CopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))
#endif

#ifndef NULL
#define NULL			0
#endif

#ifndef TRUE
#define TRUE			1
#endif

#ifndef FALSE
#define FALSE			0
#endif

//������ȱʡֵ
#define DEFAULT_BUF_LEN  1024
//����ֵ����
#define BAG_USER_BUF_LEN_LESS -2/*ȡ��ʱ�û�����������С�ڰ�����*/
#define BUF_FULL		 -3 //��������
#define BUF_EMPUTY		 -4 //�������գ�û������
#define BUF_NO_BAG		 -5 //�����������ݣ�����û���ҵ��Ϸ������ݰ���û���ҵ���ͷ������Щ���ݶ���
#define NOT_SET_BAG_FLAG -6 //δ���ð�ͷ��ʶ
#define BUF_DATA_LESS    -7 //������������,���ǲ���һ�����ݣ���������
#define BGA_DATA_LESS    -8 //������������,�����ҵ���ͷ�ͳ��ȣ����ǲ���һ�������İ�,��������
#define BAG_LEN_TOO_BIG  -9 //Ҫȡ�ĵ�ǰ���ݰ����ȴ����������������ݰ����ȣ����ȳ���
#define BAG_LEN_ERROR    -10//�����Ǹ�ֵ
#define BAG_LEN_POS_TOO_LITTLE  -11/*�������ֶε�λ��̫С*/
#define BAG_LEN_END_TYPE_ERROR  -12/*�������ֶδ�С�����õĲ���*/


//#define DEBUG //������Ϣ
/*add by wu 2012-11-18 begin*/
/************************************************************************/
#define DEFAULT_BAG_LEN_POS   4 /*ȱʡ�İ������ֶ���ʼλ��*/
#define BIG_END               1 /*����һ��Ϊ�����ֽڣ����ģʽ-���ֽ���ǰ�����ֽ��ں�*/
#define LITTLE_END            2 /*����С��ģʽ-���ֽ��ں󣬵��ֽ���ǰ*/

/*add by wu 2012-11-18 end*/
/************************************************************************/

using namespace std;
class DataBuf : public CCircularBuffer
{
protected:
	char BagHeadFlag[10]; //����ʶ
	sem_t mutex;//�ź���
	int BagFlagLen; //����ʶ����
	//int PopBlockFlag; //������ʶ
	pthread_mutex_t m_Mutex;
    /*add by wu 2012-11-18 begin*/
    /*���������ڵ��ֶ���ʼλ�ã�Ĭ�����ڰ�ͷ����*/
    //int iBagLenPos;
    int BagLenPos;
    int BagLenEndType;
    /*��С����-������ͷ�������ֶν����ĳ�����: ��ͷ��Ϊ4������ֵ�Ӱ�ͷ������ʼ��
    ��ô��С����Ϊ4+2 = 6 �ֽڣ������ͷ�볤���м���һ�������ֽڣ���ô��С����Ϊ7*/
    unsigned short int MinBagLen;
    /*add by wu 2012-11-18 end*/

public:
	// ���캯��
	DataBuf(char *BagFlag,int FlagLen, int BufSize);
	// ��������
	virtual ~DataBuf();
	//���ð�ͷ��ʶ
	BOOL SetBagHeadFlag(char *Flag,int FlagLen);
	//BOOL SetPopBlock(); //��������������
	//BOOL SetPopNoBlock();//���÷�����
	/*���ð������ֶ�λ��*/
	BOOL SetBagLenPos(int iPos);
    /*���ð������ֶδ�С��ģʽ�����ģʽ-���ֽ���ǰ�����ֽ��ں�
                                С��ģʽ-���ֽ��ں󣬵��ֽ���ǰ */
    BOOL SetBagLenEndType(int iType);
    //������д�뻺����
	virtual BOOL PushData( char * pBuf, int nBufLen );
	//ȡ�����ֺð������ݰ���ÿ�ε��÷���һ��
	virtual int PopData(char *buf,int BufLen);

	//������
	BOOL DebugPrintHex(char *pBuf, int Len);
    BOOL PrintErrMsg(int ErrNo); //��ӡ������Ϣ add by wu
    BOOL PrintErrMsg(const char *userMsg,int ErrNo);

private:

	BOOL InitLock();
	BOOL InitData();
    //������
	virtual BOOL ReadBinary( char * pBuf, int nBufLen );
	int CompareBagHead(char *TmpHeadBuf, int &TmpPos);//�Ƚϰ�ͷ
};

#endif

