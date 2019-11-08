/******************************************************************************

  Copyright (C), 2001-2011, zhengzhou Tiamaes Technology Co., Ltd.

 ******************************************************************************
  File Name     : tm_app_movedetect.h
  Version       : Initial Draft
  Author        : mabuyun
  Created       : 2014/12/19
  Last Modified :
  Description   : tm_app_movedetect.c header file
  Function List :
  History       :
  1.Date        : 2014/12/19
    Author      : mabuyun
    Modification: Created file

******************************************************************************/


#ifndef __TM_APP_MOVEDETECT_H__
#define __TM_APP_MOVEDETECT_H__
#include "hi_type.h"
#include "hi_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define VIDEOINPUT_MAX_CHANNEL_NUM      0x0c//8       /* �������ͨ������ */

#define MDMAXAREAONECHN   4       /* ÿ��ͨ��������ü��������� */
#define MDDEFAULTINTERV   20       /* Ĭ��֡��� */
#define MDDEFMACTHRESHD   20       /* Ĭ�Ϻ���ֵ */
#define MDDEFALMTHRESHD   20       /* Ĭ�ϱ�����ֵ */

#define MACNUMONEWIDTH    (44)     /* ÿ֡ͼ���������Ŀ */ //((704/16) )
#define MACNUMONELENTH    (36)     /* ÿ֡ͼ����������Ŀ */ //( (576/16))
#define MACNUMONEFREAME   1584     /* ÿ֡ͼ������Ŀ */ //((704/16)*(576/16))

/* �������Χ����ʼ���꣬�Լ���� */
typedef struct {
    unsigned int x;
    unsigned int y;
    unsigned int higth;
    unsigned int width;
}MD_Rec_st;

typedef struct {
    //unsigned char day;      /* BIT1-BIT7 ��������һ�������� */
    unsigned char hourbeg;
    unsigned char minbeg;
    unsigned char secbeg;
    unsigned char hourend;
    unsigned char minend;
    unsigned char secend;
}MD_TimeRange_st;

/* ͨ������ */
typedef struct{
    char enableflag;   /* �Ƿ�ʹ�ü�� */
    unsigned char areaNum;   /* ����������� */
    /* macro_threshold ������MD��������ȵȼ�����ֵΪ0~255ֵԽС������Խ�� */
    unsigned char macro_threshold; /* �����ֵ�������ȣ���鳬������ֵ����Ϊ��鱨�� */
    /* alarm_threshold �߼�����������ֵ���ɸ��������ȵ���*/
    unsigned char alarm_threshold; /* ������ֵ�����ٸ���鱨������Ϊ��ͨ���������߰ٷֱ� */
    unsigned int  interval;  /* �������ÿ����֡���һ�� */
    MD_Rec_st recattr[MDMAXAREAONECHN]; /* ������� */
    MD_TimeRange_st timeattr[7][MDMAXAREAONECHN];/* ���ʱ�� */
}MD_ChnAttr_st;
#if 0
extern HI_S32 TM_APP_Md_Entry(HI_VOID);
extern HI_S32 TM_APP_InitMdPara(int chn,MD_ChnAttr_st* p_chnMdAttr);
extern HI_S32 TM_APP_UpdateMdPara(int chn,MD_ChnAttr_st* p_chnMdAttr);
extern HI_S32 TM_APP_SetMdParaUpdate();
extern HI_S32 TM_APP_SetMdEnable();
extern HI_S32 TM_APP_SetMdDisable();
/* ��ȡȫ����MD��� */
extern HI_S32 TM_APP_MD_GetMDResult();
/* ��ȡ����ͨ����MD��� */
extern HI_S32 TM_APP_MD_GetChnMDResult(VENC_CHN VeChn);
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __TM_APP_MOVEDETECT_H__ */
