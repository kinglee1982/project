#include "av_public_head.h"
#include "av_common_21.h"

int g_iAvPrintfFlag = 1;

/*****************************************************************************
 �� �� ��  : HI3521_CovTime2Str
 ��������  :
 �������  : HI_CHAR *pTimeBuf, HI_S32 len
             HI_VOID
 �������  :
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��11��26��
    ��    ��   : xzhang
    �޸�����   : �����ɺ���

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