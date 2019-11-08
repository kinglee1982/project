/******************************************************************************

                  版权所有 (C), 2012-2014, 郑州天迈科技有限公司

 ******************************************************************************
  文 件 名   : DataBufOcx.cpp
  版 本 号   : V1.001.05
  作    者   : wuquanwei
  生成日期   : 2012年5月2518日
  最近修改   :
  功能描述   : 循环队列-按照指定的协议格式分包
  函数列表   :
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

#include "DataBufOcx.h"
#include "av_public_head.h"



#define CYCLE_BUF_VER "Ver1.001.09"

/*****************************************
 *函数名    ：初始化变量 私有方法
 *功能描述	：对类中的变量初始化
 *入口参数  ：暂无
 *出口参数  ：暂无
 *返回值 	：无
 *时间  	：2012-5-25 wu
 *****************************************/
BOOL DataBufOcx::InitData()
{
	cout<<"*************************************"<<endl;
	cout<<"  tmkj cycle buf "<<CYCLE_BUF_VER<<"  "<<endl;
	cout<<"  modified: 2013-5-28"<<endl;
	cout<<"*************************************"<<endl<<endl;

	BagFlagLen = 0;

	/*默认是4字段位置 add by wu 2012-11-18-begin*/
	BagLenPos = DEFAULT_BAG_LEN_POS;
	BagLenEndType = BIG_END;
	MinBagLen = 8;
	/*add by wu 2012-11-18-end*/

	memset(BagHeadFlag, '\0', sizeof(BagHeadFlag));
	InitLock();

	return TRUE;
}
/*****************************************
 *函数名    ：构造函数 DataBufOcx
 *功能描述	：对象创建的时候自动调用-增加初始化包标识和缓冲区
 *入口参数  ：  BagFlag----协议包标识，如包标识是"####"
 				BufSize----要初始化的缓冲区长度
 				FlagLen----包标识长度
 *出口参数  ：暂无
 *返回值 	：无
 *时间  	：2012-5-25 wu
 *****************************************/
DataBufOcx::DataBufOcx(char *BagFlag,int FlagLen, int BufSize)
{
	InitData();
	SetBagHeadFlag(BagFlag,FlagLen);
	Create(BufSize);
}
/*****************************************
 *函数名    ：析构函数 ~DataBufOcx
 *功能描述	：对象生命期结束时自动调用
 *入口参数  ：无
 *出口参数  ：无
 *返回值 	：无
 *时间  	：2012-5-25 wu
 *****************************************/
DataBufOcx::~DataBufOcx()
{
	pthread_mutex_destroy(&m_Mutex);
}
BOOL DataBufOcx::InitLock()
{
	pthread_mutex_init (&m_Mutex, NULL);
	return TRUE;
}
/*****************************************
 *函数名    ：PushData
 *功能描述	：向缓冲区写入数据，
 *入口参数  ：pBuf--要写入换成区的数据的缓冲区， nBufLen--要写入的长度，
 			  缓冲写入的最大字节数为: 缓冲区总长度-1;
 *出口参数  ：无
 *返回值 	：成功返回: 成功写入的字节数, 失败返回: 如下值
 			  BUF_FULL -------缓冲区满
 			  NOT_SET_BAG_FLAG---没有设置包标识

 *时间  	：2012-5-25 wu
 *****************************************/
BOOL DataBufOcx::PushData( char * pBuf, int nBufLen )
{
	if((pBuf == NULL) || (nBufLen <= 0))
	{
		cout<<"in DataBufOcx over write WriteBinary, args error"<<endl;
		return FALSE;
	}
	cout<<" PushData Start "<<nBufLen<<endl;

	pthread_mutex_lock(&m_Mutex); /*上锁*/

	if(nBufLen >= GetMaxWriteSize())
	{
		if(GetMaxWriteSize() == 1)
		{
			/* BEGIN: Modified by wuquanwei, 2013/4/24   PN:修改PushData函数，在缓冲区满的时候返回缓冲区满，不再是0.130400001 */
			pthread_mutex_unlock(&m_Mutex);/*解锁*/
			return BUF_FULL;
			/* END:   Modified by wuquanwei, 2013/4/24   PN:修改PushData函数，在缓冲区满的时候返回缓冲区满，不再是0.130400001 */
		}
		else
		{
			nBufLen = GetMaxWriteSize()-1; //缓冲写入的最大字节数为: 缓冲区总长度-1;
		}
	}
	if(BagFlagLen == 0)
	{
		cout<<"in PushData, not set the bag flag!"<<endl;
		pthread_mutex_unlock(&m_Mutex);/*解锁*/
		return NOT_SET_BAG_FLAG;
	}

	cout<<" WriteBinary Start "<<nBufLen<<endl;
	CCircularBuffer::WriteBinary(pBuf, nBufLen );

	pthread_mutex_unlock(&m_Mutex);/*解锁*/

	return nBufLen;
}


//当要取的数据大于缓冲区中可读的数据时，返回缓冲区中的所有数据，
//当要取的数据小于缓冲区中的可读数据时，返回请求的数据
/*****************************************
 *函数名    ：WriteBinary
 *功能描述	：从缓冲区读出数据
 *入口参数  ：pBuf--读取的数据保存缓冲区， nBufLen--要写入的长度
 *出口参数  ：无
 *返回值 	：成功返回: 读取到的长度, 失败返回: FALSE
 *时间  	：2012-5-25 wu
 *****************************************/
BOOL DataBufOcx::ReadBinary( char * pBuf, int nBufLen )
{
	if((pBuf == NULL) || (nBufLen <= 0))
	{
		return FALSE;
	}
	bzero(pBuf, nBufLen);
	int MaxReadBytes = 0;

#if 1
	if(nBufLen >= GetMaxReadSize())
	{
		MaxReadBytes = GetMaxReadSize();
		CCircularBuffer::ReadBinary(pBuf, GetMaxReadSize());
		return MaxReadBytes;
	}
	else
	{
		CCircularBuffer::ReadBinary(pBuf, nBufLen);
		return nBufLen;
	}
#endif
	return TRUE;
}

//分包加取包
/*****************************************
 *函数名    ：PopData
 *功能描述	：从缓冲区读出数据，然后分包，将分好的包返回
 *入口参数  ：buf--读取的数据保存缓冲区， nBufLen--输出缓冲区的长度
 *出口参数  ：无
 *返回值 	：成功返回: 找到的包数据长度, 失败返回: 对应值
 			  NOT_SET_BAG_FLAG---没有设置包标识
 			  BUF_EMPUTY---------缓冲区满
 			  BUF_NO_BAG---------缓冲区没有合法包
 			  BUF_DATA_LESS------缓冲区找到数据包，但是数据包不完整
 			  FALSE--------------pBuf指向NULL或者长度为0

 *时间  	：2012-5-25 wu
 *****************************************/
int DataBufOcx::PopData(char *buf, int BufLen)
{
	int HeadPos = 0;			 	//用于保存包标识开始地址
	int BagLen = 0; 			 	//计算得来的包的长度
	char tmp_buf[24] = {'\0'};  	//用于保存取得的包头和长度值
	unsigned char FindBagFlag = 0;  //验证包标识通过标志，1--包头合法， 2--没有合法包头
	int ret = 0, RetVal = -1;

	pthread_mutex_lock(&m_Mutex); 	/*上锁*/

	if(BagFlagLen == 0)
	{
		cout<<"in PopData, not set the bag flag!"<<endl;
		RetVal = NOT_SET_BAG_FLAG;
		goto EndPop;
	}

	/*判断有无数据*/
	if(GetMaxReadSize() == 0)
	{
		//cout <<"in PopData no data!"<<endl;
		RetVal = BUF_EMPUTY;
		goto EndPop;
	}
	else if(GetMaxReadSize() < MinBagLen + 2)
	{
		RetVal = BUF_DATA_LESS;
		goto EndPop;
	}
/****************************************************************************end if(PopBlockFlag == SET_BLOCK)*/
	while(GetMaxReadSize() >= MinBagLen)/*分包部分代码*/
	{
#ifdef DEBUG
		cout<<"in PopData, GetMaxReadSize() = "<<GetMaxReadSize()<<endl;
#endif
 		HeadPos = m_iReadPtr;	/*备份一下读指针的位置*/
		/*在数据中查找包标识*/
		ret = FindChar( BagHeadFlag[0],  HeadPos);
		if(ret == FALSE)
		{
			RetVal = BUF_NO_BAG;
			//cout<<"in PopData, GetMaxReadSize() = "<<GetMaxReadSize()<<" readP = "<<m_iReadPtr<<endl;

			/* BEGIN: Added by wuquanwei, 2013/4/27   PN:如果缓冲区内的数据已满，并且没有合法包头，就清空缓冲区.130427003 */
			if(GetMaxWriteSize() == 1)
				CCircularBuffer::ClearBuf();
			/* END:   Added by wuquanwei, 2013/4/27   PN:如果缓冲区内的数据已满，并且没有合法包头，就清空缓冲区.130427003 */

			goto EndPop;
		}

		/*先读 包标识+长度，用于判断包头和长度,当队列中的数据小于要
			读取的长度时，不再分包*/
		m_iReadPtr = HeadPos;
		ret = ReadBinary(tmp_buf, MinBagLen);
		if (ret < MinBagLen)
		{
			m_iReadPtr = HeadPos;
			RetVal = BUF_DATA_LESS;
			goto EndPop;
		}

		cout<<"ReadBinary "<<ret<<endl;
		debug_print("PopData",tmp_buf, ret);

		int TmpPos = 0;
		FindBagFlag = CompareBagHead((char *)tmp_buf, TmpPos);
		if(FindBagFlag == 1)
		{
			cout<<"BagLen Data [0]:"<<tmp_buf[BagLenPos]<<"[1]"<<tmp_buf[BagLenPos+1]<<"[2]"<<tmp_buf[BagLenPos+2]<<"[3]"<<tmp_buf[BagLenPos+3]<<endl;
			if(BagLenEndType == BIG_END)/*长度所在字段,高位在前，低位在后*/
			{
				BagLen = (tmp_buf[BagLenPos] << 24) | (tmp_buf[BagLenPos+1] << 16) | (tmp_buf[BagLenPos+2] << 8) | (tmp_buf[BagLenPos+3] & 0xFF);
				cout<<"BIG_END BagLen "<<BagLen<<endl;
			}
			else if(BagLenEndType == LITTLE_END)/*长度所在字段,低位在前，高位在后*/
			{
				BagLen = (tmp_buf[BagLenPos+1] << 8) | (tmp_buf[BagLenPos] & 0xff);
				cout<<"LITTLE_END BagLen "<<BagLen<<endl;
			}
			/*一个异常处理，如果包长度大于缓冲区总长度，说明数据出错。 m_nBufSize--在CircularBuffer中定义*/
			if(BagLen > m_nBufSize)
			{
				cout<<"in PopData, this bag len is Invalued!"<<"BagLen = "<<BagLen<<endl;
				RetVal = BAG_LEN_ERROR;
				goto EndPop;
			}
			else if((BagLen < m_nBufSize) && BagLen > GetMaxReadSize()+MinBagLen) //说明数据不够一包
			{
				cout<<"BagLen "<<BagLen<<" m_nBufSize "<<m_nBufSize<<" MaxReadSize "<<(GetMaxReadSize()+MinBagLen)<<endl;
				/*恢复读指针， 并返回*/
				m_iReadPtr = HeadPos;
				RetVal = BGA_DATA_LESS;
				goto EndPop;
			}

			cout<<"BagLen "<<BagLen<<"m_nBufSize"<<m_nBufSize<<endl;
			if(BagLen <= 0)
			{
				cout<<"BagLen "<<BagLen<<"m_nBufSize"<<m_nBufSize<<endl;

				RetVal = BAG_LEN_ERROR;
				goto EndPop;
			}
			/*重新赋值给读指针，从包头标识处开始读数据*/
			m_iReadPtr = HeadPos;
			if(BagLen < BufLen)
			{
				cout<<"BagLen "<<BagLen<<"BufLen "<<BufLen<<endl;
				ReadBinary(buf, BagLen);
			}
			else
			{
				cout<<"in PopData, user buf is too small, buf len = "<<BufLen<<endl;
				RetVal = BAG_USER_BUF_LEN_LESS;
				goto EndPop;
			}
			/*清除已处理的包标识*/
			SetChar('\0', HeadPos, BagFlagLen);
			//DebugPrintHex(m_pBuf, m_nBufSize);

			RetVal = BagLen;
			cout<<"RetVal "<<RetVal<<"BagLen "<<BagLen<<endl;
			goto EndPop;
		}
		else if(FindBagFlag == 2)      /*包头不合法，继续往下查找*/
		{
			m_iReadPtr = HeadPos + TmpPos; /*改变读指针位置，向下继续寻找包头标识*/
			continue;
		}
	}//end while(GetMaxReadSize() >= BagFlagLen + 2)

	if(GetMaxReadSize() < MinBagLen)
		RetVal = BUF_DATA_LESS;
EndPop:

	pthread_mutex_unlock(&m_Mutex);/*解锁*/

	return RetVal;
}
/*****************************************
 *函数名    ：SetBagHeadFlag
 *功能描述	：设置包头标识
 *入口参数  ：Flag-包头标识 BagFlagLen---包标识长度
 *出口参数  ：无
 *返回值 	：成功返回: TRUE, 失败返回: FALSE
 *时间  	：2012-5-25 wu
 *****************************************/
BOOL  DataBufOcx::SetBagHeadFlag(char *Flag, int FlagLen)
{
	if((unsigned )FlagLen > sizeof(BagHeadFlag))
		return FALSE;
	for(int i = 0; i < FlagLen; i++)
		BagHeadFlag[i] = Flag[i];

	BagFlagLen = FlagLen;        //包标识长度
	return TRUE;
}

/*****************************************************************************
 函 数 名  : DataBufOcx.SetBagLenPos
 功能描述  : 设置包长度字段位置
 输入参数  : int iPos
 输出参数  : 无
 返 回 值  : BOOL
 修改历史      :
  1.日    期   : 2012年11月18日
    作    者   : wuquanwei
    修改内容   : 新生成函数

*****************************************************************************/
BOOL DataBufOcx::SetBagLenPos(int iPos)
{
	if(iPos >= BagFlagLen)
	{
		BagLenPos = iPos;
		MinBagLen =(unsigned short int)(BagLenPos+2);/*最小长度用于分包，先取出包标识与长度值*/
	}
	else
	{
		cout<<"DataBufOcx: in SetBagLenPos, the iPos is too little!"<<endl;
		return BAG_LEN_POS_TOO_LITTLE;
	}
	return TRUE;
}
/*****************************************************************************
 函 数 名  : DataBufOcx.SetBagLenEndType
 功能描述  : 设置长度字段的大小端，默认是大端
 输入参数  : int iType  1---高字节在前，低字节在后，
 						2---高字节在后，低字节在前，
 输出参数  : 无
 返 回 值  : BAG_LEN_END_TYPE_ERROR---输入的itype 错误


 修改历史      :
  1.日    期   : 2012年11月18日
    作    者   : wuquanwei
    修改内容   : 新生成函数

*****************************************************************************/
BOOL DataBufOcx::SetBagLenEndType(int iType)
{
	if((iType != BIG_END) && (iType != LITTLE_END))
	{
		cout<<"DataBufOcx: in SetBagLenEndType, the iType is error! use default!"<<endl;
		return BAG_LEN_END_TYPE_ERROR;
	}
	BagLenEndType = iType;

	return TRUE;
}
/*用于调试打印*/
/*****************************************
 *函数名    ：DebugPrintHex
 *功能描述	：用于调试打印
 *入口参数  ：pBuf-要打印的缓冲区首地址 Len-数据长度
 *出口参数  ：无
 *返回值 	：成功返回: TRUE, 失败返回: FALSE
 *时间  	：2012-5-25 wu
 *****************************************/
BOOL DataBufOcx::DebugPrintHex(char *pBuf, int Len)
{
	if(Len <= 0)
	{
		cout<<"DtaBuf: in DebugPrintHex, no data to print! buf len = "<<Len<<endl;
		return BUF_NO_BAG;
	}
	int i = 0;
    printf("***************************\n");
	for(i = 0; i < Len; i++)
	{
		printf("%02x ", pBuf[i] & 0xff);
		if(!((i+1) % 10)) printf("\n");
	}
	printf("size = %d \n***************************\n", Len);

	return TRUE;
}
/*****************************************
 *函数名    ：CompareBagHead 私有方法
 *功能描述	：用于比较包标识
 *入口参数  ：TmpHeadBuf-从队列中读取的包头
 			  Pos包头标识不相同的字符位置 如: 设置的包头为 "####", TmpHeadBuf的内容为 "##$#"
 			  那么将Pos将返回2，如果是第一位不同，如: "####"与"$###", 返回1
 			  如果包头标识不相同就Pos就返回0 (不相同的时候不会返回0)

 *出口参数  ：无
 *返回值 	：成功返回: FindBagFlag = 1--包头标识相同， 2--包头标识不同
 *时间  	：2012-6-1 wu
 *****************************************/
int DataBufOcx::CompareBagHead(char *TmpHeadBuf, int &TmpPos)
{
	int FindBagFlag = 0;

		for(int i = 0; i < BagFlagLen; i++)
		{
#ifdef DEBUG
			printf("in CompareBagHead: tmp_buf[%d] = 0x%02x, BagHeadFlag[%d] = 0x%02x\n", i, TmpHeadBuf[i],i,BagHeadFlag[i]);
			cout<<"in CompareBagHead: m_iReadPtr = "<<m_iReadPtr<<endl;
			cout<<"in CompareBagHead: Max read size = "<<GetMaxReadSize()<<endl;
#endif

			if(TmpHeadBuf[i] != BagHeadFlag[i])
			{
				FindBagFlag= 2; 	//包头不合法，跳出比较，再次查找包头
				TmpPos =  i;
				if(i == 0) { TmpPos = 1;} //如果i为0，那么读指针加一，防止死循环
				break;
			}
			FindBagFlag = 1;
			TmpPos = 0; 			//包头相同的话返回的Pos就没意义
		}
	return FindBagFlag;
}
/*****************************************
 *函数名    ：PrintErrMsg
 *功能描述	：用于根据返回值打印错误信息
 *入口参数  ：userMsg--用户信息标识，用于区分是那个模块打印的
 			  ErrNo-出错时的返回值
 *出口参数  ：无
 *返回值 	：成功返回: TRUE
 *时间  	：2013-5-29 wu
 *****************************************/
BOOL DataBufOcx::PrintErrMsg(const char *userMsg, int ErrNo)
{
	switch(ErrNo)
	{
		case FALSE:
			cout << userMsg <<"DataBufOcx ERR: fun ret error: FALSE"<<endl;
			break;

        case BAG_USER_BUF_LEN_LESS:
			cout << userMsg <<"DataBufOcx ERR: user buf too short!"<<endl;
			break;

		case BUF_FULL:
			cout << userMsg <<"DataBufOcx ERR: The buf is full!"<<endl;
			break;

		case BUF_EMPUTY:
			cout << userMsg <<"DataBufOcx ERR: The buf is empty!"<<endl;
			break;

		case BUF_NO_BAG:
			cout << userMsg <<"DataBufOcx ERR: The buf has data, but no bag head flag, discarding!"<<endl;
			break;

		case NOT_SET_BAG_FLAG:
			cout << userMsg <<"DataBufOcx ERR: Not set bag head, please set the bag head flag!"<<endl;
			break;

		case BUF_DATA_LESS:
			cout << userMsg <<"DataBufOcx ERR: The left data is less than a packet of data, not pop!"<<endl;
			break;

		case BGA_DATA_LESS:
			cout << userMsg <<"DataBufOcx ERR: find the bag head flag, but the left data is less than a packet of data, not pop!"<<endl;
			break;
		case BAG_LEN_TOO_BIG:
			cout << userMsg <<"DataBufOcx ERR: bag len error, bag len is bigger than buf_len!"<<endl;
			break;
		case BAG_LEN_ERROR:
			cout << userMsg <<"DataBufOcx ERR: bag len error, bag len is negative number"<<endl;
			break;
		case BAG_LEN_POS_TOO_LITTLE:
			cout << userMsg <<"DataBufOcx ERR: bag len pos is too small!"<<endl;
			break;

		case BAG_LEN_END_TYPE_ERROR:
			cout << userMsg <<"DataBufOcx ERR: bag len End type error, default to BigEnd!"<<endl;
			break;


		default: cout << userMsg <<"DataBufOcx ERR: unknow err! errno = "<<ErrNo<<endl;
	}
	return TRUE;
}

/*****************************************************************************
 函 数 名  : DataBufOcx.PrintErrMsg
 功能描述  : 打印错误信息
 输入参数  : int iErrNo
 输出参数  : 无
 返 回 值  : BOOL
 修改历史      :
  1.日    期   : 2013年5月29日 星期三
    作    者   : wuquanwei
    修改内容   : 新生成函数

*****************************************************************************/
BOOL DataBufOcx::PrintErrMsg(int iErrNo)
{
	return PrintErrMsg("", iErrNo);
}



