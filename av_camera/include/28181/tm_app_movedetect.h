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

#define VIDEOINPUT_MAX_CHANNEL_NUM      0x0c//8       /* 最大输入通道个数 */

#define MDMAXAREAONECHN   4       /* 每个通道最多设置检测区域个数 */
#define MDDEFAULTINTERV   20       /* 默认帧间隔 */
#define MDDEFMACTHRESHD   20       /* 默认宏阈值 */
#define MDDEFALMTHRESHD   20       /* 默认报警阈值 */

#define MACNUMONEWIDTH    (44)     /* 每帧图像宏块横排数目 */ //((704/16) )
#define MACNUMONELENTH    (36)     /* 每帧图像宏块纵列数目 */ //( (576/16))
#define MACNUMONEFREAME   1584     /* 每帧图像宏块数目 */ //((704/16)*(576/16))

/* 检测区域范围，起始坐标，以及宽高 */
typedef struct {
    unsigned int x;
    unsigned int y;
    unsigned int higth;
    unsigned int width;
}MD_Rec_st;

typedef struct {
    //unsigned char day;      /* BIT1-BIT7 代表星期一到星期天 */
    unsigned char hourbeg;
    unsigned char minbeg;
    unsigned char secbeg;
    unsigned char hourend;
    unsigned char minend;
    unsigned char secend;
}MD_TimeRange_st;

/* 通道参数 */
typedef struct{
    char enableflag;   /* 是否使用检测 */
    unsigned char areaNum;   /* 检测区域数量 */
    /* macro_threshold 可设置MD检测灵敏度等级，数值为0~255值越小灵敏度越高 */
    unsigned char macro_threshold; /* 检测阈值，灵敏度，宏块超过该数值就认为宏块报警 */
    /* alarm_threshold 逻辑分析报警阈值，可根据灵敏度调整*/
    unsigned char alarm_threshold; /* 报警阈值，多少个宏块报警就认为该通道报警或者百分比 */
    unsigned int  interval;  /* 检测间隔，每多少帧检测一次 */
    MD_Rec_st recattr[MDMAXAREAONECHN]; /* 检测区域 */
    MD_TimeRange_st timeattr[7][MDMAXAREAONECHN];/* 检测时间 */
}MD_ChnAttr_st;
#if 0
extern HI_S32 TM_APP_Md_Entry(HI_VOID);
extern HI_S32 TM_APP_InitMdPara(int chn,MD_ChnAttr_st* p_chnMdAttr);
extern HI_S32 TM_APP_UpdateMdPara(int chn,MD_ChnAttr_st* p_chnMdAttr);
extern HI_S32 TM_APP_SetMdParaUpdate();
extern HI_S32 TM_APP_SetMdEnable();
extern HI_S32 TM_APP_SetMdDisable();
/* 获取全部的MD结果 */
extern HI_S32 TM_APP_MD_GetMDResult();
/* 获取单个通道的MD结果 */
extern HI_S32 TM_APP_MD_GetChnMDResult(VENC_CHN VeChn);
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __TM_APP_MOVEDETECT_H__ */
