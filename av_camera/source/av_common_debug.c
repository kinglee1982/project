/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ֣�������Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : debug.cc
  �� �� ��   : ����
  ��    ��   : 0x01
  ��������   : 2014��4��11��
  ����޸�   :
  ��������   : ���������Ϣ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2014��4��11��
    ��    ��   : 0x01
    �޸�����   : �����ļ�

******************************************************************************/
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/syscall.h>


#include "av_common_debug.h"
#include "av_print_color.h"

int g_uiPutOutLevel = DEBUG;

char *PutOutLevel[] =
{
    (char *)LIGHT_RED"[FATEL ]",
    (char *)PURPLE"[ALERT ]",
    (char *)RED"[ERROR ]",
    (char *)YELLOW"[ WARM ]",
    (char *)BLUE"[NOTICE]",
    (char *)GREEN"[ INFO ]",
    (char *)CYAN"[DEBUG ]"
};

/*****************************************************************************
 Prototype    : getTimeString
 Description  : ��ȡ��ʽ���պ��ʱ���ַ���
 Input        : char  *pTimeBuf
                int s32Len
 Output       : None
 Return Value : char

  History        :
  1.Date         : 2016/4/11
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
char *getTimeString(char  *pTimeBuf, int s32Len)
{
	char timeBuf[32] = {0};
	time_t nowTime = 0;
	struct tm *pTm = NULL;

	if (s32Len < (int)sizeof(timeBuf))
	{
		return NULL;
	}

	nowTime = time((time_t *)NULL);
	pTm = localtime(&nowTime);
	if (strftime(timeBuf, sizeof(timeBuf), "%D %H:%M:%S", pTm) == 0)
    {
		return NULL;
	}

	if(pTimeBuf != NULL)
	{
		memcpy(pTimeBuf, timeBuf, sizeof(timeBuf));
	}
	else
	{
		return NULL;
	}

    return pTimeBuf;
}

/*****************************************************************************
 Prototype    : TM_SCHEDULE_SetPrintLevel
 Description  : ���ô�ӡ����
 Input        : int level
 Output       : None
 Return Value :

  History        :
  1.Date         : 2016/4/11
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
int  debug_setPrintLevel(int level)
{
    g_uiPutOutLevel = level;
    return 0;
}

/*****************************************************************************
 Prototype    : debug_printf
 Description  : ��װ��ӡ����-�ɵ�����,��schedule_printf�����Ƿ�װ�˺����������������кŵĴ�ӡ
 Input        : UINT level
                const char *proName
                const char *func
                const int line
                const char *format
                ...
 Output       : None
 Return Value :

  History        :
  1.Date         : 2016/3/29
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
void debug_printf(int level, const char *proName,const char *func, const int line,const char *format, ...)
{
    va_list args;

    if ((level > DEBUG) || (level < FATAL))
    {
        printf("tm printf input err level %d\n", level);
        return;
    }


    if (level > g_uiPutOutLevel)
    {
        return;
    }


    char timeBuf[32] = {0};

    va_start(args, format);
    printf("%s:%-8s: %s %s\t[%4d]: ", getTimeString(timeBuf,sizeof(timeBuf)),PutOutLevel[level],\
        proName,func,line);

    vprintf(format, args);
    printf(NONE);

    va_end(args);
}

/*****************************************************************************
 Prototype    : gettid
 Description  : ��ȡ�߳�ID
 Input        : VOID
 Output       : None
 Return Value :

  History        :
  1.Date         : 2016/4/11
    Author       :
    Modification :

*****************************************************************************/
int gettid( )
{
	return syscall(SYS_gettid);
}
