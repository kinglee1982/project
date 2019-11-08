//////////////////////////////////////////////////////////////////////
// CircularBuffer.cpp:   CCircularBuffer 实现
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#include "CircularBuffer.h"

///////////////////////////////////////////////////////////////////
// 构造函数
//
CCircularBuffer::CCircularBuffer()
{
	m_pBuf = NULL;
	m_pTmpBuf = NULL;
	m_nBufSize = 0;
	m_iReadPtr = 0;
	m_iWritePtr = 0;
}

///////////////////////////////////////////////////////////////////
// 析构函数
//
CCircularBuffer::~CCircularBuffer()
{
	Destroy();
}

///////////////////////////////////////////////////////////////////
// 方法: Create
// 功能: 初始化循环缓冲区
// 参数:
//     [输入] iBufSize -- 循环缓冲区最大长度
// 返回: TRUE 成功, 否则 FALSE.
//
BOOL CCircularBuffer::Create( int iBufSize )
{
	BOOL bResult = FALSE;
	{
		m_pBuf = new char[ iBufSize ];
		if( m_pBuf )
		{
			m_nBufSize = iBufSize;
			ZeroMemory( m_pBuf, m_nBufSize );
#if 0
			/* BEGIN: Modified by wuquanwei, 2013/4/27   PN:去掉备用缓冲区，实际中没有用到.130427002 */
			m_pTmpBuf = new char[ iBufSize ];
			if( m_pTmpBuf )
			{
				ZeroMemory( m_pTmpBuf, m_nBufSize );
				bResult = TRUE;
			}
			/* END:   Modified by wuquanwei, 2013/4/27   PN:去掉备用缓冲区，实际中没有用到.130427002 */
#endif
		}
	}
	return bResult;
}

///////////////////////////////////////////////////////////////////
// 方法: Destroy
// 功能: 清除循环缓冲区，释放内存
// 参数: (空)
// 返回: (空)
//
void CCircularBuffer::Destroy()
{

	if( m_pBuf )
		delete  [] m_pBuf ;

	/* BEGIN: Modified by wuquanwei, 2013/4/27   PN:去掉备用缓冲区，实际中没有用到.130427002 */
	//if( m_pTmpBuf )
	//	delete  [] m_pTmpBuf ;
	/* END:   Modified by wuquanwei, 2013/4/27   PN:去掉备用缓冲区，实际中没有用到.130427002 */

	m_pBuf = NULL;
	m_pTmpBuf = NULL;
	m_nBufSize = 0;
	m_iReadPtr = 0;
	m_iWritePtr = 0;
}

///////////////////////////////////////////////////////////////////
// 方法: GetMaxReadSize
// 功能:返回循环缓冲区中可读的数据数量.
// 参数: (空)
// 返回: 可读的数据数量（字节数）.
/*2012-06-08: 修改m_iReadPtr > m_iWritePtr的情况，
			  如果m_iReadPtr > m_nBufSize,将会出错，
			  这里将其修改
			  wuquanwei
*/
int CCircularBuffer::GetMaxReadSize()
{
	//cout<<"m_iReadPtr = "<<m_iReadPtr<<"m_iWritePtr = "<<m_iWritePtr<<endl;
	//cout<<"m_nBufSize = "<<m_nBufSize<<endl;
	if( m_pBuf )
	{
		if( m_iReadPtr == m_iWritePtr )
			return 0;

		if( m_iReadPtr < m_iWritePtr )
			return m_iWritePtr - m_iReadPtr;

		if( m_iReadPtr > m_iWritePtr )
		{
			//如果m_iReadPtr大于总长度，那么取余
        /* BEGIN: Modified by wuquanwei, 2012/6/8 */
			m_iReadPtr = m_iReadPtr % m_nBufSize;
        /* END:   Modified by wuquanwei, 2012/6/8   PN: */
			return (m_nBufSize-m_iReadPtr)+m_iWritePtr;
		}
	}
	return 0;
}

///////////////////////////////////////////////////////////////////
// 方法: GetMaxWriteSize
// 功能: 循环缓冲区中可以用于写入的空间数量（字节数）.
// 参数: (空)
// 返回: 可以用于写入的空间数量（字节数）.
//
int CCircularBuffer::GetMaxWriteSize()
{
	if( m_pBuf )
	{
		if( m_iReadPtr == m_iWritePtr )
			return m_nBufSize;

		if( m_iWritePtr < m_iReadPtr )
			return m_iReadPtr - m_iWritePtr;

		if( m_iWritePtr > m_iReadPtr )
			return (m_nBufSize-m_iWritePtr)+m_iReadPtr;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////
// 方法: WriteBinary
// 功能: 写入二进制数据到循环缓冲区.
// 参数:
//     [in] pBuf - 将写入的数据区指针.
//     [in] nBufLen - 将写入的数据长度(字节数).
// 返回: TRUE 成功, 否则 FALSE.
//
BOOL CCircularBuffer::WriteBinary( char * pBuf, int nBufLen )
{
	BOOL bResult = FALSE;
	{
		if( nBufLen <= GetMaxWriteSize() )
		{

			// 简单情况，不需回绕
			if( m_iWritePtr + nBufLen <= m_nBufSize )
			{
				CopyMemory( &m_pBuf[m_iWritePtr], pBuf, nBufLen );
				m_iWritePtr += nBufLen;
			}
			else // 复杂情况，需要回绕
			{
				int iFirstChunkSize = m_nBufSize - m_iWritePtr;
				int iSecondChunkSize = nBufLen - iFirstChunkSize;

				CopyMemory( &m_pBuf[m_iWritePtr], pBuf, iFirstChunkSize );
				CopyMemory( &m_pBuf[0], &pBuf[iFirstChunkSize], iSecondChunkSize );
				m_iWritePtr = iSecondChunkSize;
			}
			bResult =TRUE;
		}
	}
	return bResult;
}

///////////////////////////////////////////////////////////////////
// 方法: ReadBinary
// 功能: 从循环缓冲区中读取数据.
// 参数:
//     [输入/输出] pBuf - 用于存储读取数据的数据区指针
//     [in] nBufLen - 读取数据长度 (字节数).
// 返回: TRUE 成功, 否则 FALSE.
//
BOOL CCircularBuffer::ReadBinary( char * pBuf, int nBufLen )
{
	BOOL bResult = FALSE;
	{
		if( nBufLen <= GetMaxReadSize() )
		{
			// 简单情况，不需回绕
			if( m_iReadPtr + nBufLen <= m_nBufSize )
			{
				CopyMemory( pBuf, &m_pBuf[m_iReadPtr], nBufLen );
				m_iReadPtr += nBufLen;
			}
			else // 复杂情况，需要回绕
			{
				int iFirstChunkSize = m_nBufSize - m_iReadPtr;
				int iSecondChunkSize = nBufLen - iFirstChunkSize;

				CopyMemory( pBuf, &m_pBuf[m_iReadPtr], iFirstChunkSize );
				CopyMemory( &pBuf[iFirstChunkSize], &m_pBuf[0], iSecondChunkSize );
				m_iReadPtr = iSecondChunkSize;
			}
			bResult =TRUE;
		}
	}
	return bResult;
}

///////////////////////////////////////////////////////////////////
// 方法: PeekChar
// 功能: 查看循环缓冲区中指定位置字符, 不提取它.
// 参数:
//     [输入] iPos - 要查看的字符位置索引(0开始).
//     [输出] ch - 查看的字符.
// 返回: TRUE 成功, 否则 FALSE.
BOOL CCircularBuffer::PeekChar( int iPos, char & ch )
{
	BOOL bResult = FALSE;
	{
		if( iPos < GetMaxReadSize() )
		{
      /* BEGIN: Modified by wuquanwei, 2012/6/8 */
#if 0
      /* END:   Modified by wuquanwei, 2012/6/8   PN: */
			if( m_iWritePtr > m_iReadPtr )
			{
				// 简单情况，不需回绕
				ch = m_pBuf[ m_iReadPtr+iPos ];
				bResult = TRUE;
			}
			else if( m_iWritePtr == m_iReadPtr )
			{
				// 缓冲区空
			}
			else if( m_iWritePtr < m_iReadPtr )
			{
				// 复杂情况，需要回绕

				if( m_iReadPtr + iPos < m_nBufSize )
				{
					// 位置在第一块
					ch = m_pBuf[ m_iReadPtr + iPos ];
					bResult = TRUE;
				}
				else
				{
					// 位置在第二块
					ch = m_pBuf[ iPos - (m_nBufSize-m_iReadPtr) ];
					bResult = TRUE;
				}
			}
#else		//如果读指针超出了缓冲区总长度，那么取余
			m_iReadPtr = m_iReadPtr%m_nBufSize;
			ch = m_pBuf[(m_iReadPtr+iPos)%m_nBufSize];
			bResult = TRUE;
#endif
		 /* END:   Modified by wuquanwei, 2012/6/8   PN: */
		}
	}
	return bResult;
}

///////////////////////////////////////////////////////////////////
// 方法: FindChar
// 功能: 在循环缓冲区中查找指定字符，如果找到则返回位置索引
// 参数:
//     [输入] chLookFor - 要查找的字符.
//     [输出] riPos - 字符的位置索引, 如果找到.
// 返回: TRUE 成功, 否则 FALSE.
//
BOOL CCircularBuffer::FindChar( char chLookFor, int & riPos )
{
	int i;
	BOOL bResult = FALSE;
	{
		int iSize = GetMaxReadSize();
		//cout<<"in FindChar GetMaxReadSize = "<<GetMaxReadSize()<<endl;
		//cout<<"in FindChar m_iReadPtr = "<<m_iReadPtr<<endl;
		for( i = 0; i < iSize; i++ )
		{
			char ch;
			if( PeekChar( i, ch ) )
			{
				if( ch == chLookFor )
				{
					riPos = i+m_iReadPtr;
					bResult = TRUE;
					break;
				}
			}
		}
	}
	//riPos = i+m_iReadPtr;
	return bResult;
}

//将某一段数据置为指定的字符 add by wu 2012-05-25
BOOL CCircularBuffer::SetChar(char ch, int pos, int len)
{
	for(int i = 0; i < len; i++)
	{
		m_pBuf[(pos+i)%m_nBufSize] = ch;
		//cout<<"pos = "<<pos<<" pos%m_nBufSize+i="<<(pos+i)%m_nBufSize<<"m_nBufSize = "<<m_nBufSize<<endl;
	}
	return TRUE;
}

/*清空循环队列*/
int CCircularBuffer::ClearBuf()
{
	m_iReadPtr = 0;
	m_iWritePtr = 0;
	return 0;
}

