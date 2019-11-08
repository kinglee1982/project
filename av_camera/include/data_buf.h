/******************************************************************************

                  版权所有 (C), 2012-2014, 郑州天迈科技有限公司

 ******************************************************************************
  文 件 名   : DataBuf.h
  版 本 号   : 初稿
  作    者   : wuquanwei
  生成日期   : 2012年5月25日

  修改历史   :
  1.日    期   : 2012年5月25日
    作    者   : wuquanwei
    修改内容   : 创建文件

2012-05-25: 初步测试用版本 V1.001.01 wuquanwei
2012-05-29: 增加缓冲区中有数据包头，但是数据包不完整的情况处理 V1.001.02 wuquanwei
2012-06-01: 修改包标识相关的代码，设置包标识函数增加标识长度 V1.001.03 wuquanwei
2012-06-08: 1、修改CircularBuffer.cpp中的GetMaxReadSize()和
			PeekChar( int iPos, char & ch )
			增加判断读指针和总长度的比较:如果出现m_iReadPtr > m_nBufSize的情况
			将m_iReadPtr = m_nBufSize % m_iReadPtr,这样防止越界出现的段错误。
			2、增加打印出错信息函数PrintErrMsg(int ErrNo)
			V1.001.04 wuquanwei

2012-11-18: 功能扩展
			1、增加版本号打印
			2、增加包长度起始字段设置
			3、增加包长度大小端设置
			V1.001.05 wuquanwei

2012-12-12: 修改
			1、去掉信号量的使用，去掉阻塞设置接口，没有合法数据包立即返回。
			2、增加互斥锁，在pushdata和popdata中互斥
			V1.001.06 wuquanwei

2013-4-24: 修改
			1、缓冲区还剩一个字节可写空间时就返回缓冲区满。
			V1.001.07 wuquanwei

2013-4-27: 修改
			1、如果缓冲区满，并且没有合法数据包头，那就清空缓冲区
			V1.001.08 wuquanwei

2013-5-28: 修改
			1、错误码打印函数修改，添加接口:
			PrintErrMsg(const char *userMsg,int ErrNo);
			打印信息可以添加个标识头，用于区分是那个模块打印的。

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

//缓冲区缺省值
#define DEFAULT_BUF_LEN  1024
//返回值类型
#define BAG_USER_BUF_LEN_LESS -2/*取包时用户缓冲区长度小于包长度*/
#define BUF_FULL		 -3 //缓冲区满
#define BUF_EMPUTY		 -4 //缓冲区空，没有数据
#define BUF_NO_BAG		 -5 //缓冲区有数据，但是没有找到合法的数据包，没有找到包头，将这些数据丢弃
#define NOT_SET_BAG_FLAG -6 //未设置包头标识
#define BUF_DATA_LESS    -7 //缓冲区有数据,但是不够一包数据，继续接收
#define BGA_DATA_LESS    -8 //缓冲区有数据,并且找到包头和长度，但是不是一个完整的包,继续接收
#define BAG_LEN_TOO_BIG  -9 //要取的当前数据包长度大于整个缓冲区数据包长度，长度出错
#define BAG_LEN_ERROR    -10//长度是负值
#define BAG_LEN_POS_TOO_LITTLE  -11/*包长度字段的位置太小*/
#define BAG_LEN_END_TYPE_ERROR  -12/*包长度字段大小端设置的不对*/


//#define DEBUG //调试信息
/*add by wu 2012-11-18 begin*/
/************************************************************************/
#define DEFAULT_BAG_LEN_POS   4 /*缺省的包长度字段起始位置*/
#define BIG_END               1 /*长度一般为两个字节，大端模式-高字节在前，低字节在后*/
#define LITTLE_END            2 /*长度小端模式-高字节在后，低字节在前*/

/*add by wu 2012-11-18 end*/
/************************************************************************/

using namespace std;
class DataBuf : public CCircularBuffer
{
protected:
	char BagHeadFlag[10]; //包标识
	sem_t mutex;//信号量
	int BagFlagLen; //包标识长度
	//int PopBlockFlag; //阻塞标识
	pthread_mutex_t m_Mutex;
    /*add by wu 2012-11-18 begin*/
    /*包长度所在的字段起始位置，默认是在包头后面*/
    //int iBagLenPos;
    int BagLenPos;
    int BagLenEndType;
    /*最小长度-包括包头至长度字段结束的长度如: 包头长为4，长度值从包头结束开始，
    那么最小长度为4+2 = 6 字节，如果包头与长度中间有一个其他字节，那么最小长度为7*/
    unsigned short int MinBagLen;
    /*add by wu 2012-11-18 end*/

public:
	// 构造函数
	DataBuf(char *BagFlag,int FlagLen, int BufSize);
	// 析构函数
	virtual ~DataBuf();
	//设置包头标识
	BOOL SetBagHeadFlag(char *Flag,int FlagLen);
	//BOOL SetPopBlock(); //设置阻塞读数据
	//BOOL SetPopNoBlock();//设置非阻塞
	/*设置包长度字段位置*/
	BOOL SetBagLenPos(int iPos);
    /*设置包长度字段大小端模式，大端模式-高字节在前，低字节在后
                                小端模式-高字节在后，低字节在前 */
    BOOL SetBagLenEndType(int iType);
    //将数据写入缓冲区
	virtual BOOL PushData( char * pBuf, int nBufLen );
	//取包，分好包的数据包，每次调用返回一包
	virtual int PopData(char *buf,int BufLen);

	//调试用
	BOOL DebugPrintHex(char *pBuf, int Len);
    BOOL PrintErrMsg(int ErrNo); //打印错误信息 add by wu
    BOOL PrintErrMsg(const char *userMsg,int ErrNo);

private:

	BOOL InitLock();
	BOOL InitData();
    //读数据
	virtual BOOL ReadBinary( char * pBuf, int nBufLen );
	int CompareBagHead(char *TmpHeadBuf, int &TmpPos);//比较包头
};

#endif

