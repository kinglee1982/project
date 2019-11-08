/******************************************************************************

                  版权所有 (C), 2001-2011, 郑州天迈科技有限公司

 ******************************************************************************
  文 件 名   : debug.cc
  版 本 号   : 初稿
  作    者   : 0x01
  生成日期   : 2014年4月11日
  最近修改   :
  功能描述   : 输出调试信息文件
  函数列表   :
  修改历史   :
  1.日    期   : 2014年4月11日
    作    者   : 0x01
    修改内容   : 创建文件

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
 Description  : 获取格式化收后的时间字符串
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
 Description  : 设置打印级别
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
 Description  : 封装打印函数-可调级别,与schedule_printf区别是封装了函数名，程序名和行号的打印
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
 Description  : 获取线程ID
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
