#include "av_public_head.h"
#include "av_common_21.h"

int g_iAvPrintfFlag = 1;

/*****************************************************************************
 函 数 名  : HI3521_CovTime2Str
 功能描述  :
 输入参数  : HI_CHAR *pTimeBuf, HI_S32 len
             HI_VOID
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月26日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3521_CovTime2Str(time_t t, HI_CHAR *pTime)
{
    struct tm *tmp;

    if (NULL == pTime)
    {
        AV_PRINT("Input NULL pointer");
        return;
    }
    tmp = localtime(&t);
    sprintf(pTime,"%04d%02d%02d_%02d%02d%02d",
            tmp->tm_year+1900, tmp->tm_mon+1, tmp->tm_mday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
}