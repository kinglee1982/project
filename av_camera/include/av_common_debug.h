/******************************************************************************

                  版权所有 (C), 2001-2011, 郑州天迈科技有限公司

 ******************************************************************************
  文 件 名   : debug.h
  版 本 号   : 初稿
  作    者   :
  生成日期   : 2013年11月27日
  最近修改   :
  功能描述   : 调试信息开关
  函数列表   :
  修改历史   :
  1.日    期   : 2013年11月27日
    作    者   : xiaomengmeng
    修改内容   : 创建文件

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
