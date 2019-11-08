//////////////////////////////////////////////////////////////////////
// CircularBuffer.h: CCircularBuffer 接口
//////////////////////////////////////////////////////////////////////
//
//
// 功能:
//     循环缓冲区，用于缓存数据与分包
//
// 作者:
//     张家银 2012-5-22
//
//////////////////////////////////////////////////////////////////////

#ifndef __CircularBuffer_h
#define __CircularBuffer_h
#include <iostream>

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
using namespace std;
class CCircularBuffer
{
protected:
	///////////////////////////////////////////////////////////////////
	// 保护成员变量
	//
	char * m_pBuf;
	char * m_pTmpBuf; // 临时缓冲区
	int m_nBufSize;   // 循环缓冲区长度
	int m_iReadPtr;   // 读指针
	int m_iWritePtr;  // 写指针

	BOOL Create( int iBufSize );// 功能: 初始化循环缓冲区

public:
	///////////////////////////////////////////////////////////////////
	// 构造函数
	//
	CCircularBuffer();
	///////////////////////////////////////////////////////////////////
	// 析构函数
	//
	virtual ~CCircularBuffer();

	///////////////////////////////////////////////////////////////////
	// 方法: GetMaxReadSize
	// 功能:返回循环缓冲区中可读的数据数量.
	// 参数: (空)
	// 返回: 可读的数据数量（字节数）.
	//
	int GetMaxReadSize();

	///////////////////////////////////////////////////////////////////
	// 方法: GetMaxWriteSize
	// 功能: 循环缓冲区中可以用于写入的空间数量（字节数）.
	// 参数: (空)
	// 返回: 可以用于写入的空间数量（字节数）.
	//
	int GetMaxWriteSize();

	///////////////////////////////////////////////////////////////////
	// 方法: WriteBinary
	// 功能: 写入二进制数据到循环缓冲区.
	// 参数:
	//     [in] pBuf - 将写入的数据区指针.
	//     [in] nBufLen - 将写入的数据长度(字节数).
	// 返回: TRUE 成功, 否则 FALSE.
	//
	virtual BOOL WriteBinary( char * pBuf, int nBufLen );
	///////////////////////////////////////////////////////////////////
	// 方法: ReadBinary
	// 功能: 从循环缓冲区中读取数据.
	// 参数:
	//     [输入/输出] pBuf - 用于存储读取数据的数据区指针
	//     [in] nBufLen - 读取数据长度 (字节数).
	// 返回: TRUE 成功, 否则 FALSE.
	//
	virtual BOOL ReadBinary( char * pBuf, int nBufLen );

	///////////////////////////////////////////////////////////////////
	// 方法: PeekChar
	// 功能: 查看循环缓冲区中指定位置字符, 不提取它.
	// 参数:
	//     [输入] iPos - 要查看的字符位置索引(0开始).
	//     [输出] ch - 查看的字符.
	// 返回: TRUE 成功, 否则 FALSE.
	//
	BOOL PeekChar( int iPos, char & ch );

	///////////////////////////////////////////////////////////////////
	// 方法: FindChar
	// 功能: 在循环缓冲区中查找指定字符，如果找到则返回位置索引
	// 参数:
	//     [输入] chLookFor - 要查找的字符.
	//     [输出] riPos - 字符的位置索引, 如果找到.
	// 返回: TRUE 成功, 否则 FALSE.
	//
	BOOL FindChar( char chLookFor, int & riPos );

	//将某一段数据置为指定的字符
	BOOL SetChar(char ch, int pos, int len);

    //清空循环队列--add 2013-04-27
    int ClearBuf();

private:
		///////////////////////////////////////////////////////////////////
	// 方法: Destroy
	// 功能: 清除循环缓冲区，释放内存
	// 参数: (空)
	// 返回: (空)
	//
	void Destroy();

};

#endif//__CircularBuffer_h

