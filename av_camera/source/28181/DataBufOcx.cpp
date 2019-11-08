/******************************************************************************

                  ��Ȩ���� (C), 2012-2014, ֣�������Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : DataBufOcx.cpp
  �� �� ��   : V1.001.05
  ��    ��   : wuquanwei
  ��������   : 2012��5��2518��
  ����޸�   :
  ��������   : ѭ������-����ָ����Э���ʽ�ְ�
  �����б�   :
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

#include "DataBufOcx.h"
#include "av_public_head.h"



#define CYCLE_BUF_VER "Ver1.001.09"

/*****************************************
 *������    ����ʼ������ ˽�з���
 *��������	�������еı�����ʼ��
 *��ڲ���  ������
 *���ڲ���  ������
 *����ֵ 	����
 *ʱ��  	��2012-5-25 wu
 *****************************************/
BOOL DataBufOcx::InitData()
{
	cout<<"*************************************"<<endl;
	cout<<"  tmkj cycle buf "<<CYCLE_BUF_VER<<"  "<<endl;
	cout<<"  modified: 2013-5-28"<<endl;
	cout<<"*************************************"<<endl<<endl;

	BagFlagLen = 0;

	/*Ĭ����4�ֶ�λ�� add by wu 2012-11-18-begin*/
	BagLenPos = DEFAULT_BAG_LEN_POS;
	BagLenEndType = BIG_END;
	MinBagLen = 8;
	/*add by wu 2012-11-18-end*/

	memset(BagHeadFlag, '\0', sizeof(BagHeadFlag));
	InitLock();

	return TRUE;
}
/*****************************************
 *������    �����캯�� DataBufOcx
 *��������	�����󴴽���ʱ���Զ�����-���ӳ�ʼ������ʶ�ͻ�����
 *��ڲ���  ��  BagFlag----Э�����ʶ�������ʶ��"####"
 				BufSize----Ҫ��ʼ���Ļ���������
 				FlagLen----����ʶ����
 *���ڲ���  ������
 *����ֵ 	����
 *ʱ��  	��2012-5-25 wu
 *****************************************/
DataBufOcx::DataBufOcx(char *BagFlag,int FlagLen, int BufSize)
{
	InitData();
	SetBagHeadFlag(BagFlag,FlagLen);
	Create(BufSize);
}
/*****************************************
 *������    ���������� ~DataBufOcx
 *��������	�����������ڽ���ʱ�Զ�����
 *��ڲ���  ����
 *���ڲ���  ����
 *����ֵ 	����
 *ʱ��  	��2012-5-25 wu
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
 *������    ��PushData
 *��������	���򻺳���д�����ݣ�
 *��ڲ���  ��pBuf--Ҫд�뻻���������ݵĻ������� nBufLen--Ҫд��ĳ��ȣ�
 			  ����д�������ֽ���Ϊ: �������ܳ���-1;
 *���ڲ���  ����
 *����ֵ 	���ɹ�����: �ɹ�д����ֽ���, ʧ�ܷ���: ����ֵ
 			  BUF_FULL -------��������
 			  NOT_SET_BAG_FLAG---û�����ð���ʶ

 *ʱ��  	��2012-5-25 wu
 *****************************************/
BOOL DataBufOcx::PushData( char * pBuf, int nBufLen )
{
	if((pBuf == NULL) || (nBufLen <= 0))
	{
		cout<<"in DataBufOcx over write WriteBinary, args error"<<endl;
		return FALSE;
	}
	cout<<" PushData Start "<<nBufLen<<endl;

	pthread_mutex_lock(&m_Mutex); /*����*/

	if(nBufLen >= GetMaxWriteSize())
	{
		if(GetMaxWriteSize() == 1)
		{
			/* BEGIN: Modified by wuquanwei, 2013/4/24   PN:�޸�PushData�������ڻ���������ʱ�򷵻ػ���������������0.130400001 */
			pthread_mutex_unlock(&m_Mutex);/*����*/
			return BUF_FULL;
			/* END:   Modified by wuquanwei, 2013/4/24   PN:�޸�PushData�������ڻ���������ʱ�򷵻ػ���������������0.130400001 */
		}
		else
		{
			nBufLen = GetMaxWriteSize()-1; //����д�������ֽ���Ϊ: �������ܳ���-1;
		}
	}
	if(BagFlagLen == 0)
	{
		cout<<"in PushData, not set the bag flag!"<<endl;
		pthread_mutex_unlock(&m_Mutex);/*����*/
		return NOT_SET_BAG_FLAG;
	}

	cout<<" WriteBinary Start "<<nBufLen<<endl;
	CCircularBuffer::WriteBinary(pBuf, nBufLen );

	pthread_mutex_unlock(&m_Mutex);/*����*/

	return nBufLen;
}


//��Ҫȡ�����ݴ��ڻ������пɶ�������ʱ�����ػ������е��������ݣ�
//��Ҫȡ������С�ڻ������еĿɶ�����ʱ���������������
/*****************************************
 *������    ��WriteBinary
 *��������	���ӻ�������������
 *��ڲ���  ��pBuf--��ȡ�����ݱ��滺������ nBufLen--Ҫд��ĳ���
 *���ڲ���  ����
 *����ֵ 	���ɹ�����: ��ȡ���ĳ���, ʧ�ܷ���: FALSE
 *ʱ��  	��2012-5-25 wu
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

//�ְ���ȡ��
/*****************************************
 *������    ��PopData
 *��������	���ӻ������������ݣ�Ȼ��ְ������ֺõİ�����
 *��ڲ���  ��buf--��ȡ�����ݱ��滺������ nBufLen--����������ĳ���
 *���ڲ���  ����
 *����ֵ 	���ɹ�����: �ҵ��İ����ݳ���, ʧ�ܷ���: ��Ӧֵ
 			  NOT_SET_BAG_FLAG---û�����ð���ʶ
 			  BUF_EMPUTY---------��������
 			  BUF_NO_BAG---------������û�кϷ���
 			  BUF_DATA_LESS------�������ҵ����ݰ����������ݰ�������
 			  FALSE--------------pBufָ��NULL���߳���Ϊ0

 *ʱ��  	��2012-5-25 wu
 *****************************************/
int DataBufOcx::PopData(char *buf, int BufLen)
{
	int HeadPos = 0;			 	//���ڱ������ʶ��ʼ��ַ
	int BagLen = 0; 			 	//��������İ��ĳ���
	char tmp_buf[24] = {'\0'};  	//���ڱ���ȡ�õİ�ͷ�ͳ���ֵ
	unsigned char FindBagFlag = 0;  //��֤����ʶͨ����־��1--��ͷ�Ϸ��� 2--û�кϷ���ͷ
	int ret = 0, RetVal = -1;

	pthread_mutex_lock(&m_Mutex); 	/*����*/

	if(BagFlagLen == 0)
	{
		cout<<"in PopData, not set the bag flag!"<<endl;
		RetVal = NOT_SET_BAG_FLAG;
		goto EndPop;
	}

	/*�ж���������*/
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
	while(GetMaxReadSize() >= MinBagLen)/*�ְ����ִ���*/
	{
#ifdef DEBUG
		cout<<"in PopData, GetMaxReadSize() = "<<GetMaxReadSize()<<endl;
#endif
 		HeadPos = m_iReadPtr;	/*����һ�¶�ָ���λ��*/
		/*�������в��Ұ���ʶ*/
		ret = FindChar( BagHeadFlag[0],  HeadPos);
		if(ret == FALSE)
		{
			RetVal = BUF_NO_BAG;
			//cout<<"in PopData, GetMaxReadSize() = "<<GetMaxReadSize()<<" readP = "<<m_iReadPtr<<endl;

			/* BEGIN: Added by wuquanwei, 2013/4/27   PN:����������ڵ���������������û�кϷ���ͷ������ջ�����.130427003 */
			if(GetMaxWriteSize() == 1)
				CCircularBuffer::ClearBuf();
			/* END:   Added by wuquanwei, 2013/4/27   PN:����������ڵ���������������û�кϷ���ͷ������ջ�����.130427003 */

			goto EndPop;
		}

		/*�ȶ� ����ʶ+���ȣ������жϰ�ͷ�ͳ���,�������е�����С��Ҫ
			��ȡ�ĳ���ʱ�����ٷְ�*/
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
			if(BagLenEndType == BIG_END)/*���������ֶ�,��λ��ǰ����λ�ں�*/
			{
				BagLen = (tmp_buf[BagLenPos] << 24) | (tmp_buf[BagLenPos+1] << 16) | (tmp_buf[BagLenPos+2] << 8) | (tmp_buf[BagLenPos+3] & 0xFF);
				cout<<"BIG_END BagLen "<<BagLen<<endl;
			}
			else if(BagLenEndType == LITTLE_END)/*���������ֶ�,��λ��ǰ����λ�ں�*/
			{
				BagLen = (tmp_buf[BagLenPos+1] << 8) | (tmp_buf[BagLenPos] & 0xff);
				cout<<"LITTLE_END BagLen "<<BagLen<<endl;
			}
			/*һ���쳣������������ȴ��ڻ������ܳ��ȣ�˵�����ݳ��� m_nBufSize--��CircularBuffer�ж���*/
			if(BagLen > m_nBufSize)
			{
				cout<<"in PopData, this bag len is Invalued!"<<"BagLen = "<<BagLen<<endl;
				RetVal = BAG_LEN_ERROR;
				goto EndPop;
			}
			else if((BagLen < m_nBufSize) && BagLen > GetMaxReadSize()+MinBagLen) //˵�����ݲ���һ��
			{
				cout<<"BagLen "<<BagLen<<" m_nBufSize "<<m_nBufSize<<" MaxReadSize "<<(GetMaxReadSize()+MinBagLen)<<endl;
				/*�ָ���ָ�룬 ������*/
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
			/*���¸�ֵ����ָ�룬�Ӱ�ͷ��ʶ����ʼ������*/
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
			/*����Ѵ���İ���ʶ*/
			SetChar('\0', HeadPos, BagFlagLen);
			//DebugPrintHex(m_pBuf, m_nBufSize);

			RetVal = BagLen;
			cout<<"RetVal "<<RetVal<<"BagLen "<<BagLen<<endl;
			goto EndPop;
		}
		else if(FindBagFlag == 2)      /*��ͷ���Ϸ����������²���*/
		{
			m_iReadPtr = HeadPos + TmpPos; /*�ı��ָ��λ�ã����¼���Ѱ�Ұ�ͷ��ʶ*/
			continue;
		}
	}//end while(GetMaxReadSize() >= BagFlagLen + 2)

	if(GetMaxReadSize() < MinBagLen)
		RetVal = BUF_DATA_LESS;
EndPop:

	pthread_mutex_unlock(&m_Mutex);/*����*/

	return RetVal;
}
/*****************************************
 *������    ��SetBagHeadFlag
 *��������	�����ð�ͷ��ʶ
 *��ڲ���  ��Flag-��ͷ��ʶ BagFlagLen---����ʶ����
 *���ڲ���  ����
 *����ֵ 	���ɹ�����: TRUE, ʧ�ܷ���: FALSE
 *ʱ��  	��2012-5-25 wu
 *****************************************/
BOOL  DataBufOcx::SetBagHeadFlag(char *Flag, int FlagLen)
{
	if((unsigned )FlagLen > sizeof(BagHeadFlag))
		return FALSE;
	for(int i = 0; i < FlagLen; i++)
		BagHeadFlag[i] = Flag[i];

	BagFlagLen = FlagLen;        //����ʶ����
	return TRUE;
}

/*****************************************************************************
 �� �� ��  : DataBufOcx.SetBagLenPos
 ��������  : ���ð������ֶ�λ��
 �������  : int iPos
 �������  : ��
 �� �� ֵ  : BOOL
 �޸���ʷ      :
  1.��    ��   : 2012��11��18��
    ��    ��   : wuquanwei
    �޸�����   : �����ɺ���

*****************************************************************************/
BOOL DataBufOcx::SetBagLenPos(int iPos)
{
	if(iPos >= BagFlagLen)
	{
		BagLenPos = iPos;
		MinBagLen =(unsigned short int)(BagLenPos+2);/*��С�������ڷְ�����ȡ������ʶ�볤��ֵ*/
	}
	else
	{
		cout<<"DataBufOcx: in SetBagLenPos, the iPos is too little!"<<endl;
		return BAG_LEN_POS_TOO_LITTLE;
	}
	return TRUE;
}
/*****************************************************************************
 �� �� ��  : DataBufOcx.SetBagLenEndType
 ��������  : ���ó����ֶεĴ�С�ˣ�Ĭ���Ǵ��
 �������  : int iType  1---���ֽ���ǰ�����ֽ��ں�
 						2---���ֽ��ں󣬵��ֽ���ǰ��
 �������  : ��
 �� �� ֵ  : BAG_LEN_END_TYPE_ERROR---�����itype ����


 �޸���ʷ      :
  1.��    ��   : 2012��11��18��
    ��    ��   : wuquanwei
    �޸�����   : �����ɺ���

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
/*���ڵ��Դ�ӡ*/
/*****************************************
 *������    ��DebugPrintHex
 *��������	�����ڵ��Դ�ӡ
 *��ڲ���  ��pBuf-Ҫ��ӡ�Ļ������׵�ַ Len-���ݳ���
 *���ڲ���  ����
 *����ֵ 	���ɹ�����: TRUE, ʧ�ܷ���: FALSE
 *ʱ��  	��2012-5-25 wu
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
 *������    ��CompareBagHead ˽�з���
 *��������	�����ڱȽϰ���ʶ
 *��ڲ���  ��TmpHeadBuf-�Ӷ����ж�ȡ�İ�ͷ
 			  Pos��ͷ��ʶ����ͬ���ַ�λ�� ��: ���õİ�ͷΪ "####", TmpHeadBuf������Ϊ "##$#"
 			  ��ô��Pos������2������ǵ�һλ��ͬ����: "####"��"$###", ����1
 			  �����ͷ��ʶ����ͬ��Pos�ͷ���0 (����ͬ��ʱ�򲻻᷵��0)

 *���ڲ���  ����
 *����ֵ 	���ɹ�����: FindBagFlag = 1--��ͷ��ʶ��ͬ�� 2--��ͷ��ʶ��ͬ
 *ʱ��  	��2012-6-1 wu
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
				FindBagFlag= 2; 	//��ͷ���Ϸ��������Ƚϣ��ٴβ��Ұ�ͷ
				TmpPos =  i;
				if(i == 0) { TmpPos = 1;} //���iΪ0����ô��ָ���һ����ֹ��ѭ��
				break;
			}
			FindBagFlag = 1;
			TmpPos = 0; 			//��ͷ��ͬ�Ļ����ص�Pos��û����
		}
	return FindBagFlag;
}
/*****************************************
 *������    ��PrintErrMsg
 *��������	�����ڸ��ݷ���ֵ��ӡ������Ϣ
 *��ڲ���  ��userMsg--�û���Ϣ��ʶ�������������Ǹ�ģ���ӡ��
 			  ErrNo-����ʱ�ķ���ֵ
 *���ڲ���  ����
 *����ֵ 	���ɹ�����: TRUE
 *ʱ��  	��2013-5-29 wu
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
 �� �� ��  : DataBufOcx.PrintErrMsg
 ��������  : ��ӡ������Ϣ
 �������  : int iErrNo
 �������  : ��
 �� �� ֵ  : BOOL
 �޸���ʷ      :
  1.��    ��   : 2013��5��29�� ������
    ��    ��   : wuquanwei
    �޸�����   : �����ɺ���

*****************************************************************************/
BOOL DataBufOcx::PrintErrMsg(int iErrNo)
{
	return PrintErrMsg("", iErrNo);
}



