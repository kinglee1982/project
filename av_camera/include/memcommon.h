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
	CONTAINER_PRI_CYC = 1,	//缓存环，缓存溢出时循环覆盖旧数据
	CONTAINER_PRI_COM = 2,	//普通，缓存溢出时丢弃新数据
	CONTAINER_PRI_SEQ = 3,   //仅支持FIFO，缓存满，阻塞操作
	CONTAINER_PRI_LOW = 4,   //仅支持LIST，缓存满时丢弃删除优先级低的PKG
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
	PKG_PRIORITY_HIGH,	//数据包优先级
};
#endif
