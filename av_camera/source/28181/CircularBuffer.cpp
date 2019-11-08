//////////////////////////////////////////////////////////////////////
// CircularBuffer.cpp:   CCircularBuffer ʵ��
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#include "CircularBuffer.h"

///////////////////////////////////////////////////////////////////
// ���캯��
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
// ��������
//
CCircularBuffer::~CCircularBuffer()
{
	Destroy();
}

///////////////////////////////////////////////////////////////////
// ����: Create
// ����: ��ʼ��ѭ��������
// ����:
//     [����] iBufSize -- ѭ����������󳤶�
// ����: TRUE �ɹ�, ���� FALSE.
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
			/* BEGIN: Modified by wuquanwei, 2013/4/27   PN:ȥ�����û�������ʵ����û���õ�.130427002 */
			m_pTmpBuf = new char[ iBufSize ];
			if( m_pTmpBuf )
			{
				ZeroMemory( m_pTmpBuf, m_nBufSize );
				bResult = TRUE;
			}
			/* END:   Modified by wuquanwei, 2013/4/27   PN:ȥ�����û�������ʵ����û���õ�.130427002 */
#endif
		}
	}
	return bResult;
}

///////////////////////////////////////////////////////////////////
// ����: Destroy
// ����: ���ѭ�����������ͷ��ڴ�
// ����: (��)
// ����: (��)
//
void CCircularBuffer::Destroy()
{

	if( m_pBuf )
		delete  [] m_pBuf ;

	/* BEGIN: Modified by wuquanwei, 2013/4/27   PN:ȥ�����û�������ʵ����û���õ�.130427002 */
	//if( m_pTmpBuf )
	//	delete  [] m_pTmpBuf ;
	/* END:   Modified by wuquanwei, 2013/4/27   PN:ȥ�����û�������ʵ����û���õ�.130427002 */

	m_pBuf = NULL;
	m_pTmpBuf = NULL;
	m_nBufSize = 0;
	m_iReadPtr = 0;
	m_iWritePtr = 0;
}

///////////////////////////////////////////////////////////////////
// ����: GetMaxReadSize
// ����:����ѭ���������пɶ�����������.
// ����: (��)
// ����: �ɶ��������������ֽ�����.
/*2012-06-08: �޸�m_iReadPtr > m_iWritePtr�������
			  ���m_iReadPtr > m_nBufSize,�������
			  ���ｫ���޸�
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
			//���m_iReadPtr�����ܳ��ȣ���ôȡ��
        /* BEGIN: Modified by wuquanwei, 2012/6/8 */
			m_iReadPtr = m_iReadPtr % m_nBufSize;
        /* END:   Modified by wuquanwei, 2012/6/8   PN: */
			return (m_nBufSize-m_iReadPtr)+m_iWritePtr;
		}
	}
	return 0;
}

///////////////////////////////////////////////////////////////////
// ����: GetMaxWriteSize
// ����: ѭ���������п�������д��Ŀռ��������ֽ�����.
// ����: (��)
// ����: ��������д��Ŀռ��������ֽ�����.
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
// ����: WriteBinary
// ����: д����������ݵ�ѭ��������.
// ����:
//     [in] pBuf - ��д���������ָ��.
//     [in] nBufLen - ��д������ݳ���(�ֽ���).
// ����: TRUE �ɹ�, ���� FALSE.
//
BOOL CCircularBuffer::WriteBinary( char * pBuf, int nBufLen )
{
	BOOL bResult = FALSE;
	{
		if( nBufLen <= GetMaxWriteSize() )
		{

			// ��������������
			if( m_iWritePtr + nBufLen <= m_nBufSize )
			{
				CopyMemory( &m_pBuf[m_iWritePtr], pBuf, nBufLen );
				m_iWritePtr += nBufLen;
			}
			else // �����������Ҫ����
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
// ����: ReadBinary
// ����: ��ѭ���������ж�ȡ����.
// ����:
//     [����/���] pBuf - ���ڴ洢��ȡ���ݵ�������ָ��
//     [in] nBufLen - ��ȡ���ݳ��� (�ֽ���).
// ����: TRUE �ɹ�, ���� FALSE.
//
BOOL CCircularBuffer::ReadBinary( char * pBuf, int nBufLen )
{
	BOOL bResult = FALSE;
	{
		if( nBufLen <= GetMaxReadSize() )
		{
			// ��������������
			if( m_iReadPtr + nBufLen <= m_nBufSize )
			{
				CopyMemory( pBuf, &m_pBuf[m_iReadPtr], nBufLen );
				m_iReadPtr += nBufLen;
			}
			else // �����������Ҫ����
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
// ����: PeekChar
// ����: �鿴ѭ����������ָ��λ���ַ�, ����ȡ��.
// ����:
//     [����] iPos - Ҫ�鿴���ַ�λ������(0��ʼ).
//     [���] ch - �鿴���ַ�.
// ����: TRUE �ɹ�, ���� FALSE.
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
				// ��������������
				ch = m_pBuf[ m_iReadPtr+iPos ];
				bResult = TRUE;
			}
			else if( m_iWritePtr == m_iReadPtr )
			{
				// ��������
			}
			else if( m_iWritePtr < m_iReadPtr )
			{
				// �����������Ҫ����

				if( m_iReadPtr + iPos < m_nBufSize )
				{
					// λ���ڵ�һ��
					ch = m_pBuf[ m_iReadPtr + iPos ];
					bResult = TRUE;
				}
				else
				{
					// λ���ڵڶ���
					ch = m_pBuf[ iPos - (m_nBufSize-m_iReadPtr) ];
					bResult = TRUE;
				}
			}
#else		//�����ָ�볬���˻������ܳ��ȣ���ôȡ��
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
// ����: FindChar
// ����: ��ѭ���������в���ָ���ַ�������ҵ��򷵻�λ������
// ����:
//     [����] chLookFor - Ҫ���ҵ��ַ�.
//     [���] riPos - �ַ���λ������, ����ҵ�.
// ����: TRUE �ɹ�, ���� FALSE.
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

//��ĳһ��������Ϊָ�����ַ� add by wu 2012-05-25
BOOL CCircularBuffer::SetChar(char ch, int pos, int len)
{
	for(int i = 0; i < len; i++)
	{
		m_pBuf[(pos+i)%m_nBufSize] = ch;
		//cout<<"pos = "<<pos<<" pos%m_nBufSize+i="<<(pos+i)%m_nBufSize<<"m_nBufSize = "<<m_nBufSize<<endl;
	}
	return TRUE;
}

/*���ѭ������*/
int CCircularBuffer::ClearBuf()
{
	m_iReadPtr = 0;
	m_iWritePtr = 0;
	return 0;
}

