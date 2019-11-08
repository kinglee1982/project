/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ֣�������Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : debug.h
  �� �� ��   : ����
  ��    ��   :
  ��������   : 2013��11��27��
  ����޸�   :
  ��������   : ������Ϣ����
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��11��27��
    ��    ��   : xiaomengmeng
    �޸�����   : �����ļ�

******************************************************************************/
#ifndef __DEBUG_H_
#define __DEBUG_H_



typedef enum {
	FATAL = 0,
    ALERT, // 1
	ERROR, // 2
	WARN,  // 3
	NOTICE,// 4
	INFO,  // 5
	DEBUG  // 6
} schedule_debug_level;


#ifndef  PRO_NAME
#define PRO_NAME "#[test]: "
#endif

#define dPrint(DLevel,format, ...) \
{ \
    debug_printf(DLevel, PRO_NAME,__FUNCTION__, __LINE__,format,##__VA_ARGS__);\
}

int  debug_setPrintLevel(int level);
void debug_printf(int level, const char *proName,const char *func, const int line,const char *format, ...);
int gettid( );


#endif
