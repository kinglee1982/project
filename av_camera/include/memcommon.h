#ifndef _MEM_COMMON_H_
#define _MEM_COMMON_H_
//data center handle
typedef void * HDC;
//data package handle
typedef void * HDP;
//data container handle
typedef void * HBC;

enum
{
	CONTAINER_PRI_CYC = 1,	//���滷���������ʱѭ�����Ǿ�����
	CONTAINER_PRI_COM = 2,	//��ͨ���������ʱ����������
	CONTAINER_PRI_SEQ = 3,   //��֧��FIFO������������������
	CONTAINER_PRI_LOW = 4,   //��֧��LIST��������ʱ����ɾ�����ȼ��͵�PKG
};

enum
{
	CONTAINER_FIFO = 1,
	CONTAINER_LIST = 2,
};

enum
{
    PKG_PRIORITY_LOW = 0,   //
    PKG_PRIORITY_NORMAL,//
	PKG_PRIORITY_HIGH,	//���ݰ����ȼ�
};
#endif
