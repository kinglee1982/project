#ifndef __AV_DEBUG_H__
#define __AV_DEBUG_H__

extern int g_iAvPrintfFlag;
extern HI_CHAR *AV_GetTime(HI_CHAR *pTimeBuf, HI_S32 s32Len);
extern int gettid();



#define AV_PRINT(format...) \
{ \
	HI_CHAR tmp[64] = {0}; \
	AV_GetTime(tmp, sizeof(tmp)); \
    if (0 != g_iAvPrintfFlag) \
    { \
        printf(tmp), printf("#[av][tid %d][%s][%d]", gettid(), __FILE__, __LINE__),printf(format),printf("\r\n"); \
    } \
}


extern HI_S32 HI3531_DebugPrint(HI_CHAR *buf, HI_S32 len);
extern HI_CHAR *AV_GetTime2(time_t nowTime, HI_CHAR *pTimeBuf, HI_S32 s32Len);
extern int debug_print(const char *cTitle,char *buf, int len);
extern void TM_USleep(int iSec,int iUsec);

#endif


