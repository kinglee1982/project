//////////////////////////////////////////////////////////////////////
// CircularBuffer.h: CCircularBuffer �ӿ�
//////////////////////////////////////////////////////////////////////
//
//
// ����:
//     ѭ�������������ڻ���������ְ�
//
// ����:
//     �ż��� 2012-5-22
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
	// ������Ա����
	//
	char * m_pBuf;
	char * m_pTmpBuf; // ��ʱ������
	int m_nBufSize;   // ѭ������������
	int m_iReadPtr;   // ��ָ��
	int m_iWritePtr;  // дָ��

	BOOL Create( int iBufSize );// ����: ��ʼ��ѭ��������

public:
	///////////////////////////////////////////////////////////////////
	// ���캯��
	//
	CCircularBuffer();
	///////////////////////////////////////////////////////////////////
	// ��������
	//
	virtual ~CCircularBuffer();

	///////////////////////////////////////////////////////////////////
	// ����: GetMaxReadSize
	// ����:����ѭ���������пɶ�����������.
	// ����: (��)
	// ����: �ɶ��������������ֽ�����.
	//
	int GetMaxReadSize();

	///////////////////////////////////////////////////////////////////
	// ����: GetMaxWriteSize
	// ����: ѭ���������п�������д��Ŀռ��������ֽ�����.
	// ����: (��)
	// ����: ��������д��Ŀռ��������ֽ�����.
	//
	int GetMaxWriteSize();

	///////////////////////////////////////////////////////////////////
	// ����: WriteBinary
	// ����: д����������ݵ�ѭ��������.
	// ����:
	//     [in] pBuf - ��д���������ָ��.
	//     [in] nBufLen - ��д������ݳ���(�ֽ���).
	// ����: TRUE �ɹ�, ���� FALSE.
	//
	virtual BOOL WriteBinary( char * pBuf, int nBufLen );
	///////////////////////////////////////////////////////////////////
	// ����: ReadBinary
	// ����: ��ѭ���������ж�ȡ����.
	// ����:
	//     [����/���] pBuf - ���ڴ洢��ȡ���ݵ�������ָ��
	//     [in] nBufLen - ��ȡ���ݳ��� (�ֽ���).
	// ����: TRUE �ɹ�, ���� FALSE.
	//
	virtual BOOL ReadBinary( char * pBuf, int nBufLen );

	///////////////////////////////////////////////////////////////////
	// ����: PeekChar
	// ����: �鿴ѭ����������ָ��λ���ַ�, ����ȡ��.
	// ����:
	//     [����] iPos - Ҫ�鿴���ַ�λ������(0��ʼ).
	//     [���] ch - �鿴���ַ�.
	// ����: TRUE �ɹ�, ���� FALSE.
	//
	BOOL PeekChar( int iPos, char & ch );

	///////////////////////////////////////////////////////////////////
	// ����: FindChar
	// ����: ��ѭ���������в���ָ���ַ�������ҵ��򷵻�λ������
	// ����:
	//     [����] chLookFor - Ҫ���ҵ��ַ�.
	//     [���] riPos - �ַ���λ������, ����ҵ�.
	// ����: TRUE �ɹ�, ���� FALSE.
	//
	BOOL FindChar( char chLookFor, int & riPos );

	//��ĳһ��������Ϊָ�����ַ�
	BOOL SetChar(char ch, int pos, int len);

    //���ѭ������--add 2013-04-27
    int ClearBuf();

private:
		///////////////////////////////////////////////////////////////////
	// ����: Destroy
	// ����: ���ѭ�����������ͷ��ڴ�
	// ����: (��)
	// ����: (��)
	//
	void Destroy();

};

#endif//__CircularBuffer_h

