/*************************************************************************
	> File Name: source/audio_init.c
	> Author:
	> Mail:
	> Created Time: Tue 24 May 2016 02:31:31 PM UTC
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include "av_public_head.h"
#include "av_common_31.h"
#include "av_common_debug.h"
#include "av_audio_init.h"
#include "av_capture.h"
#include "av_hddfsm.h"
#include "av_mem.h"
#include "av_store.h"
#include "28181_AV_API.h"
#include "memfunc.h"
#include "av_video_init.h"
#include "av_sys.h"
/*********************************************************************/
//音频模块私有数据定义
//static AENC_CHN_INFO_t stAencInfo[HI_AENC_MAX_CHN]; //音频编码通道信息
//统一编号所有音频输入通道号: 0~12，共13路，其中0~7对应tw2968的8路摄像头输入，8~11对应
//nvp6114的四路摄像头的音频0~3通道的输入，12对应一路麦克输入，
//tw2968的音频(0~7)通过AiDev0输入到CPU, nv6114的4路音频(8~11)和麦克的(12)音频通过AiDev1输入到CPU
char AencAichnMap[HI_AENC_MAX_CHN] = {0,1,2,3,4,5,6,7,0,1,2,3,8};
char AencDevMap[HI_AENC_MAX_CHN]   = {0,0,0,0,0,0,0,0,1,1,1,1,1};
//char AencvichnMap[HI_AENC_MAX_CHN] = {4, 5, 6,7,8,9,10,11,0,1,2,3,12};
static AudioFrameInvocation sfSendAencData = NULL;
static HI_BOOL bThRunFlag=HI_FALSE;

FILE *audioRecFp[HI_AENC_MAX_CHN]= {0};
static HI_S32 aenChn=0; //要预览的通道号，通过解码绑定ao，向解码发送数据实现预览

static HI_U64 g_SetAuPts[HI_AENC_MAX_CHN] = {0};
static time_t tGetAencStartTime[HI_AENC_MAX_CHN] = {0};

extern HI_BOOL bFindFirstIFrame[TOTAL_VENC_CHN_NUM];
extern time_t tGetVencStartTime[TOTAL_VENC_CHN_NUM];

HI_BOOL bAuFrameStart[HI_AENC_MAX_CHN] = {HI_FALSE};

//后续考虑通道录音可配置
/*********************************************************************/
HI_S32 HI3521A_InitAencInfo(HI_S32 s32Chn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    Capture_Dev_t *pstCapDev = HI3521_GetCaptureDev();
    pstCapDev->vi_chn[s32Chn].aenc_chn.AiDev = AencDevMap[s32Chn];
    pstCapDev->vi_chn[s32Chn].aenc_chn.AiChn = AencAichnMap[s32Chn];
    pstCapDev->vi_chn[s32Chn].aenc_chn.HiAencChn = s32Chn;
    pstCapDev->vi_chn[s32Chn].aenc_chn.created = HI_ENABLE;
    pstCapDev->vi_chn[s32Chn].aenc_chn.Channel = s32Chn;
    pstCapDev->vi_chn[s32Chn].aenc_chn.info[0].enc_chn = 2 * s32Chn;
    pstCapDev->vi_chn[s32Chn].aenc_chn.info[0].channel = s32Chn;
    pstCapDev->vi_chn[s32Chn].aenc_chn.info[0].dwtype = 0;
    pstCapDev->vi_chn[s32Chn].aenc_chn.info[1].enc_chn = 2 * s32Chn + 1;
    pstCapDev->vi_chn[s32Chn].aenc_chn.info[1].channel = s32Chn;
    pstCapDev->vi_chn[s32Chn].aenc_chn.info[1].dwtype = 1;

    s32Ret = pthread_mutex_init(&(pstCapDev->vi_chn[s32Chn].aenc_chn.lock), NULL);
    if (s32Ret != HI_SUCCESS)
    {
        dPrint(ERROR, "pthread_mutex_init aiChn = %d failed %#X\n", s32Chn, s32Ret);
    }

    return s32Ret;
}
/*********************************************************************/
HI_VOID HI3521A_AiPubAttrInit(AIO_ATTR_S * pstAttr)
{

#if ((AENC_TYPE == AENC_PCM) || (AENC_TYPE ==AENC_ADPCM))
    pstAttr->enBitwidth = AUDIO_BIT_WIDTH_16;         //16位宽
    pstAttr->enSamplerate = AUDIO_SAMPLE_RATE_16000;  //16K采样率
    pstAttr->enSoundmode = AUDIO_SOUND_MODE_MONO;     //单声道
    pstAttr->enWorkmode = AIO_MODE_I2S_SLAVE;         //I2S从模式
    pstAttr->u32ChnCnt = 16;                          //16通道
    pstAttr->u32ClkChnCnt = 16;                       //channel number on FS, valid value:1/2/4/8/16
    pstAttr->u32EXFlag = 0;                           //不使用8bit拓展16bit
    pstAttr->u32FrmNum = 30;                          //缓存帧
    pstAttr->u32PtNumPerFrm = VOIP_DATA_PER_FRAM;     //每帧采样点数
    pstAttr->u32ClkSel = 0;                           //不复用SIO4 的发送时钟和接收时钟

#elif  (AENC_TYPE == AENC_G711)
    pstAttr->enBitwidth = AUDIO_BIT_WIDTH_16;
    pstAttr->enSamplerate = AUDIO_SAMPLE_RATE_16000;
    pstAttr->enSoundmode = AUDIO_SOUND_MODE_MONO;
    pstAttr->enWorkmode = AIO_MODE_I2S_SLAVE;
    pstAttr->u32ChnCnt = 16;
    pstAttr->u32ClkChnCnt = 16;
    pstAttr->u32EXFlag = 0;
    pstAttr->u32FrmNum = 30;
    pstAttr->u32PtNumPerFrm = 480;
    pstAttr->u32ClkSel = 0;
#endif

}

/*****************************************************************************
 Prototype    : HI3521A_AoPubAttrInit
 Description  : AO参数初始化
 Input        : AIO_ATTR_S * pstAttr
 Output       : None
 Return Value :

  History        :
  1.Date         : 2016/5/24
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_VOID HI3521A_AoPubAttrInit(AIO_ATTR_S * pstAttr)
{
#if ((AENC_TYPE == AENC_PCM) || (AENC_TYPE ==AENC_ADPCM))

    pstAttr->enBitwidth = AUDIO_BIT_WIDTH_16;
    pstAttr->enSamplerate = AUDIO_SAMPLE_RATE_16000;
    pstAttr->enSoundmode = AUDIO_SOUND_MODE_MONO;
    pstAttr->enWorkmode = AIO_MODE_I2S_SLAVE;
    pstAttr->u32ChnCnt = 1;
    pstAttr->u32ClkChnCnt = 2;
    pstAttr->u32EXFlag = 0;
    pstAttr->u32FrmNum = 30;
    pstAttr->u32PtNumPerFrm = VOIP_DATA_PER_FRAM;
    pstAttr->u32ClkSel = 0;

#elif (AENC_TYPE == AENC_G711)

    pstAttr->enBitwidth = AUDIO_BIT_WIDTH_16;
    pstAttr->enSamplerate = AUDIO_SAMPLE_RATE_16000;
    pstAttr->enSoundmode = AUDIO_SOUND_MODE_MONO;
    pstAttr->enWorkmode = AIO_MODE_I2S_SLAVE;
    pstAttr->u32ChnCnt = 2;
    pstAttr->u32ClkChnCnt = 2;
    pstAttr->u32EXFlag = 1;
    pstAttr->u32FrmNum = 30;
    pstAttr->u32PtNumPerFrm = 320;
    pstAttr->u32ClkSel = 0;

#endif
    return;
}


/*****************************************************************************
 Prototype    : HI3521A_AiInit
 Description  : ai初始化，启用ai设备和通道
 Input        : HI_VOID
 Output       : None
 Return Value :


  History        :
  1.Date         : 2016/5/24
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_U32 HI3521A_AiInit(HI_VOID)
{
    HI_U32 u32Chn = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    AIO_ATTR_S stAttr;

    AUDIO_DEV AiDev = 0;/*input and output*/


    dPrint(DEBUG, "HI3521a_AiInit enter.\n");
    HI3521A_AiPubAttrInit(&stAttr);

    for (AiDev = 0; AiDev < 2; AiDev++ )
    {
        /* set ai public attr*/
        s32Ret = HI_MPI_AI_SetPubAttr(AiDev, &stAttr);
        if (HI_SUCCESS != s32Ret)
        {
            dPrint(ERROR, "HI_MPI_AI_SetPubAttr AiDev(%d) err(0x%02x)\n", AiDev, s32Ret);
            return HI_FAILURE;
        }

        /* enable ai dev*/
        s32Ret = HI_MPI_AI_Enable(AiDev);
        if (HI_SUCCESS != s32Ret)
        {
            dPrint(ERROR, "HI_MPI_AI_Enable AiDev(%d) err(0x%02x)\n", AiDev, s32Ret);
            return HI_FAILURE;
        }
    }
    for(u32Chn = 0; u32Chn < HI_AI_MAX_CHN; u32Chn++)
    {
        s32Ret = HI_MPI_AI_EnableChn(AencDevMap[u32Chn], AencAichnMap[u32Chn]);
        if (HI_SUCCESS != s32Ret)
        {
            dPrint(ERROR, "HI_MPI_AI_EnableChn (AiDev%d, u32Chn%d) err(0x%02x)\n", AiDev, u32Chn, s32Ret);
            return HI_FAILURE;
        }
    }

    dPrint(DEBUG, "HI3521a_AiInit suc.\n");
    return HI_SUCCESS;
}

/*****************************************************************************
 Prototype    : HI3521A_AiExit
 Description  : AI去初始化
 Input        : HI_VOID
 Output       : None
 Return Value :

  History        :
  1.Date         : 2016/5/24
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_S32 HI3521A_AiExit(HI_VOID)
{
    HI_S32 u32Chn;
    HI_S32 s32Ret = HI_SUCCESS;
    AUDIO_DEV AiDev = 0;
    for (u32Chn = 0; u32Chn < HI_AI_MAX_CHN; u32Chn++)
    {
        s32Ret = HI_MPI_AI_DisableChn(AencDevMap[u32Chn], AencAichnMap[u32Chn]);
        if (HI_SUCCESS != s32Ret)
        {
            dPrint(ERROR, "Disable ai-chn(%d, %d) err(0x%x)\n",AencDevMap[u32Chn], u32Chn, s32Ret);
            return HI_FAILURE;
        }
    }
    for (AiDev = 0; AiDev < 2; AiDev++ )
    {
        s32Ret = HI_MPI_AI_Disable(AiDev);
        if (HI_SUCCESS != s32Ret)
        {
            dPrint(ERROR, "Disable ai-dev(%d) err(0x%x)\n", AiDev, s32Ret);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 Prototype    : HI3521A_AoInit
 Description  : AO初始化
 Input        : HI_U32 u32PtNumPerFrm
 Output       : None
 Return Value :

  History        :
  1.Date         : 2016/5/24
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_U32 HI3521A_AoInit(HI_U32 u32PtNumPerFrm)
{
    HI_S32 s32Ret = HI_SUCCESS;
    AIO_ATTR_S stAttr;
    AUDIO_DEV AoDevId = 0;
    AO_CHN AoChn = 0;

    //AUDIO_RESAMPLE_ATTR_S stAoReSampleAttr;

    HI3521A_AoPubAttrInit(&stAttr);
    dPrint(DEBUG, "HI3521a_AoInit enter.%d %d\n",u32PtNumPerFrm,stAttr.u32ClkChnCnt);
    /* BEGIN: Added by wuquanwei, 2015/3/5   PN:修改mp3播放.201503001 */
    stAttr.u32PtNumPerFrm = u32PtNumPerFrm;
    /* END:   Added by wuquanwei, 2015/3/5   PN:修改mp3播放.201503001 */

    //stAoReSampleAttr.u32InPointNum = stAttr.u32PtNumPerFrm;
    //stAoReSampleAttr.enInSampleRate = stAttr.enSamplerate;
    //stAoReSampleAttr.enReSampleType = AUDIO_RESAMPLE_1X2;

    s32Ret = HI_MPI_AO_SetPubAttr(AoDevId, &stAttr);
    if (HI_SUCCESS != s32Ret)
    {
        dPrint(ERROR, "HI_MPI_AO_SetPubAttr AoDev(%d) err(0x%02x)\n", AoDevId, s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_AO_Enable(AoDevId);
    if (HI_SUCCESS != s32Ret)
    {
        dPrint(ERROR, "HI_MPI_AO_Enable AoDev(%d) err(0x%02x)\n", AoDevId, s32Ret);
        return HI_FAILURE;
    }
    s32Ret = HI_MPI_AO_EnableChn(AoDevId, AoChn);
    if (HI_SUCCESS != s32Ret)
    {
        dPrint(ERROR, "HI_MPI_AO_EnableChn (AoDev%d, AoChn%d) err(0x%02x)\n", AoDevId, AoChn, s32Ret);
        return HI_FAILURE;
    }

    //s32Ret = HI_MPI_AO_DisableReSmp(AoDevId, AoChn);
    //if (HI_SUCCESS != s32Ret)
    //{
    //   AV_PRINT("HI_MPI_AO_DisableReSmp (AoDev%d, AoChn%d) err(0x%02x)", AoDevId, AoChn, s32Ret);
    //    return HI_FAILURE;
    //}
    //s32Ret = HI_MPI_AO_EnableReSmp(AoDevId, AoChn, &stAoReSampleAttr);
    //if (HI_SUCCESS != s32Ret)
    //{
    //    AV_PRINT("HI_MPI_AO_EnableReSmp (AoDev%d, AoChn%d) err(0x%02x)", AoDevId, AoChn, s32Ret);
    //    return HI_FAILURE;
    //}


    dPrint(DEBUG, "HI3521a_AoInit suc.\n");
    return HI_SUCCESS;
}

/*****************************************************************************
 Prototype    : HI3521A_AoExit
 Description  : AO去初始化
 Input        : HI_VOID
 Output       : None
 Return Value :

  History        :
  1.Date         : 2016/5/24
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_S32 HI3521A_AoExit(HI_VOID)
{
    HI_S32 s32Ret = 0;
    AUDIO_DEV AoDev = 0;
    AO_CHN AoChn = 0;

    s32Ret = HI_MPI_AO_DisableChn(AoDev, AoChn);
    if(HI_SUCCESS != s32Ret)
    {
        dPrint(ERROR, "disable ao-chn(%d, %d) err(0x%x)\n", AoDev, AoChn, s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_AO_Disable(AoDev);
    if(HI_SUCCESS != s32Ret)
    {
        dPrint(ERROR, "disable ai-dev(%d) err(0x%x)\n", AoDev, s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static HI_S32 HI3521A_Comm_AoBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn)
{
    MPP_CHN_S stSrcChn,stDestChn;

    stSrcChn.enModId = HI_ID_AI;
    stSrcChn.s32ChnId = AiChn;
    stSrcChn.s32DevId = AiDev;
    stDestChn.enModId = HI_ID_AO;
    stDestChn.s32DevId = AoDev;
    stDestChn.s32ChnId = AoChn;

    return HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}
static HI_S32 HI3521A_Comm_AoUnBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn)
{
    MPP_CHN_S stSrcChn,stDestChn;

    stSrcChn.enModId = HI_ID_AI;
    stSrcChn.s32ChnId = AiChn;
    stSrcChn.s32DevId = AiDev;
    stDestChn.enModId = HI_ID_AO;
    stDestChn.s32DevId = AoDev;
    stDestChn.s32ChnId = AoChn;

    return HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
}

/*****************************************************************************
 Prototype    : HI3521A_StartAudioPreview
 Description  : AI与AO绑定、去绑定设置，
 Input        : AUDIO_DEV AidevId
                AI_CHN AiNum
                HI_BOOL bIsPreview
 Output       : None
 Return Value :

  History        :
  1.Date         : 2016/5/24
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_S32 HI3521A_SetAudioPreview(AI_CHN AiNum,HI_BOOL bIsPreview)
{
    AI_CHN AiChn = AiNum;
    AUDIO_DEV AoDev = 0;
    AO_CHN AoChn = 0;

    if (AUDIO_PREVIEW_START == bIsPreview)
    {
        HI3521A_Comm_AoBindAi(AencDevMap[AiNum], AencAichnMap[AiChn], AoDev, AoChn);
    }
    else
    {
        HI3521A_Comm_AoUnBindAi(AencDevMap[AiNum], AencAichnMap[AiChn], AoDev, AoChn);
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI3521A_Comm_AoBindAdec
 功能描述  : AO 绑定 ADEC 用于录像回放，IP对讲
 输入参数  : AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月17日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521A_Comm_AoBindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn,stDestChn;

    stSrcChn.enModId = HI_ID_ADEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = AdChn;
    stDestChn.enModId = HI_ID_AO;
    stDestChn.s32DevId = AoDev;
    stDestChn.s32ChnId = AoChn;

    s32Ret =  HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        dPrint(DEBUG, "HI_MPI_SYS_Bind failed with (%#x)!\n",s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;

}

/*****************************************************************************
 函 数 名  : HI3521A_Comm_AoUnBindAdec
 功能描述  : AO 解绑定 ADEC
 输入参数  : AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月17日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521A_Comm_AoUnBindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn,stDestChn;

    stSrcChn.enModId = HI_ID_ADEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = AdChn;
    stDestChn.enModId = HI_ID_AO;
    stDestChn.s32DevId = AoDev;
    stDestChn.s32ChnId = AoChn;
    s32Ret =  HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        dPrint(DEBUG, "HI_MPI_SYS_Bind failed with (%#x)!\n",s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI3521A_Comm_AencBindAi
 功能描述  : AENC 绑定 AI
 输入参数  : AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月17日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521A_Comm_AencBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn,stDestChn;

    stSrcChn.enModId = HI_ID_AI;
    stSrcChn.s32DevId = AiDev;
    stSrcChn.s32ChnId = AiChn;
    stDestChn.enModId = HI_ID_AENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = AeChn;
    s32Ret =  HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        dPrint(DEBUG, "HI_MPI_SYS_Bind failed with (%#x)!\n",s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : HI3521A_Comm_AencUnBindAi
 功能描述  : AENC 解绑定 AI
 输入参数  : AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月17日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521A_Comm_AencUnBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn,stDestChn;

    stSrcChn.enModId = HI_ID_AI;
    stSrcChn.s32DevId = AiDev;
    stSrcChn.s32ChnId = AiChn;
    stDestChn.enModId = HI_ID_AENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = AeChn;
    s32Ret =  HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        dPrint(DEBUG, "HI_MPI_SYS_Bind failed with (%#x)!\n",s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 Prototype    : HI3521A_AencInit
 Description  : 音频编码初始化,与ai绑定，操作对象是通道，将指定的通道号绑定到对应的编码通道上
 Input        : HI_S32 s32Chn--AI物理通道
                HI_S32 s32AeChn--编码通道
 Output       : None
 Return Value :

  History        :
  1.Date         : 2016/5/25
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_S32 HI3521A_AencInit(HI_S32 s32Chn, HI_S32 s32AeChn,HI_S32 sAiDev)
{
    HI_S32 s32Ret = 0;
    AENC_CHN_ATTR_S stAencAttr;

    HI_S32 s32AencChn = s32AeChn ;
    HI_S32 s32AiDev = sAiDev;
    HI_S32 s32AiChn = s32Chn;

    if (s32Chn >= HI_AI_MAX_CHN)
    {
        dPrint(ERROR, "ai chn(%d) is out of range\n", s32Chn);
        return HI_FAILURE;
    }

    stAencAttr.u32PtNumPerFrm = 480;
    stAencAttr.u32BufSize = 30;

#if (AENC_TYPE == AENC_G711)

    AENC_ATTR_G711_S stG711;
    stAencAttr.enType = PT_G711A;
    stAencAttr.pValue = &stG711;
    //stG711.resv = 0;

#elif (AENC_TYPE == AENC_PCM)

    AENC_ATTR_LPCM_S stLPCM;
    /* 获取PCM定义的类型 */
    stAencAttr.enType = PT_LPCM;
    stAencAttr.pValue = &stLPCM;
    //stG711.resv = 0;

#elif (AENC_TYPE == AENC_ADPCM)
    AENC_ATTR_ADPCM_S stAdpcmAenc;

    stAencAttr.enType = PT_ADPCMA;
    stAencAttr.pValue = &stAdpcmAenc;
    stAdpcmAenc.enADPCMType = ADPCM_TYPE_DVI4;
#endif

    /* create aenc chn*/
    s32Ret = HI_MPI_AENC_CreateChn(s32AencChn, &stAencAttr);
    if (s32Ret != HI_SUCCESS)
    {
        dPrint(ERROR, "create aenc chn %d err:0x%x\n", s32AencChn, s32Ret);
        return HI_FAILURE;
    }


    /*bind aenc to ai*/
    s32Ret = HI3521A_Comm_AencBindAi(s32AiDev, AencAichnMap[s32AiChn], s32AencChn);
    if (s32Ret != HI_SUCCESS)
    {
        dPrint(ERROR, "Ai(%d,%d) bind to Aenc(%d) err\n", s32AiDev, s32AiChn, s32AencChn);
        return HI_FAILURE;
    }

    s32Ret = HI3521A_InitAencInfo(s32Chn);
    if (s32Ret != HI_SUCCESS)
    {
        dPrint(ERROR, "HI3521A_InitAencInfo err\n");
        return HI_FAILURE;
    }

    return 0;
}

/*****************************************************************************
 Prototype    : HI3521A_AencExit
 Description  : 音频编码去初始化，与ai解绑定,
 Input        : HI_VOID
 Output       : None
 Return Value :

  History        :
  1.Date         : 2016/5/25
    Author       : wuquanwei
    Modification : 操作对象:针对所有启动编码的通道

*****************************************************************************/
HI_S32 HI3521A_AencExit(HI_VOID)
{
    HI_S32 s32Chn = 0;
    HI_S32 s32Ret = 0;
    for (s32Chn = 0; s32Chn < HI_AENC_MAX_CHN; s32Chn++)
    {
        /*音频编码初始化*/
        s32Ret = HI3521A_Comm_AencUnBindAi(AencDevMap[s32Chn],AencAichnMap[s32Chn],s32Chn);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT( "HI3531_AencInit err(0x%x)", s32Ret);
            return s32Ret;
        }
        s32Ret = HI_MPI_AENC_DestroyChn(s32Chn);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT(" HI_MPI_AENC_DestroyChn(%d) failed with %#x!\n", s32Chn, s32Ret);
            return s32Ret;
        }
    }
    return HI_SUCCESS;

}
/*****************************************************************************
 Prototype    : HI3521A_AdecInit
 Description  : 音频解码初始化
 Input        : HI_VOID
 Output       : None
 Return Value :

  History        :
  1.Date         : 2016/5/25
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_S32 HI3521A_AdecInit(HI_VOID)
{
    HI_S32 s32Ret = 0;
    ADEC_CHN AdChn = 0;
    AUDIO_DEV AoDev = 0;
    AO_CHN AoChn = 0;/*输出在1通道*/
    ADEC_CHN_ATTR_S stAdecAttr;

#if (AENC_TYPE ==  AENC_G711)

    ADEC_ATTR_G711_S stAdecG711;
    stAdecAttr.enType = PT_G711A;
    stAdecAttr.u32BufSize = 30;
    stAdecAttr.enMode = ADEC_MODE_STREAM;/* propose use pack mode in your app */
    stAdecAttr.pValue = &stAdecG711;

#elif (AENC_TYPE ==  AENC_PCM)

    ADEC_ATTR_LPCM_S stAdecLPCM;
    stAdecAttr.enType = PT_LPCM;
    stAdecAttr.u32BufSize = 30;
    stAdecAttr.enMode = ADEC_MODE_PACK;/* propose use pack mode in your app */
    stAdecAttr.pValue = &stAdecLPCM;

#elif (AENC_TYPE ==  AENC_ADPCM)
    AENC_ATTR_ADPCM_S stAdpcmAenc;

    stAdecAttr.enType = PT_ADPCMA;
    stAdecAttr.u32BufSize = 30;
    stAdecAttr.enMode = ADEC_MODE_PACK;/* propose use pack mode in your app */
    stAdecAttr.pValue = &stAdpcmAenc;
    stAdpcmAenc.enADPCMType = ADPCM_TYPE_DVI4;
#endif

    dPrint(DEBUG, "use %d type\n",PT_ADPCMA);
    /* create adec chn*/
    s32Ret = HI_MPI_ADEC_CreateChn(AdChn, &stAdecAttr);
    if (s32Ret)
    {
        dPrint(ERROR, "%s: HI_MPI_ADEC_CreateChn(%d) failed with (%#x)!\n", __FUNCTION__, AdChn,s32Ret);
        return s32Ret;
    }

    /*bind ao to adec*/
    s32Ret = HI3521A_Comm_AoBindAdec(AoDev, AoChn, AdChn);
    if (s32Ret != HI_SUCCESS)
    {
        dPrint(ERROR, "Ao(%d,%d) bind to Adec(%d) err\n", AoDev, AoChn, AdChn);
        return HI_FAILURE;
    }
    return 0;
}
/*****************************************************************************
 函 数 名  : HI3521A_AdecExit
 功能描述  :
 输入参数  : HI_VOID
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月11日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521A_AdecExit(HI_VOID)
{
    HI_S32 s32Ret = 0;
    AUDIO_DEV AoDev = 0;
    AO_CHN AoChn = 0;
    ADEC_CHN AdChn = 0;

    s32Ret = HI3521A_Comm_AoUnBindAdec(AoDev, AoChn, AdChn);
    if (s32Ret != HI_SUCCESS)
    {
        dPrint(ERROR, "Ao(%d, %d) unbind adec(%d) err(0x%x)\n", AoDev, AoChn, AdChn, s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_ADEC_DestroyChn(AdChn);
    if (HI_SUCCESS != s32Ret)
    {
        dPrint(ERROR, "destroy adec chn(%d) err(0x%x)\n", AdChn, s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}



/*****************************************************************************
 Prototype    : HI3521A_StartAenc
 Description  : 启动音频编码
 Input        : VOID
 Output       : None
 Return Value :

  History        :
  1.Date         : 2016/5/25
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_S32 HI3521A_StartAencStream( )
{
    HI_S32 s32Ret = HI_SUCCESS;
    pthread_t tid;
    static HI_BOOL bThdStart = HI_FALSE;

    if (HI_FALSE == bThdStart)
    {
        s32Ret = pthread_create(&tid, NULL, HI3521A_GetAencStreamThd, NULL);
        if (s32Ret != HI_SUCCESS)
        {
            dPrint(ERROR, "create thread err");
            return HI_FAILURE;
        }

        s32Ret = pthread_detach(tid);
        if (s32Ret != HI_SUCCESS)
        {
            dPrint(ERROR, "detach thread err");
            return HI_FAILURE;
        }

        bThdStart = HI_FALSE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI3521A_GetAencFd
 功能描述  :
 输入参数  : HI_VOID
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月11日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3521A_GetAencFd(HI_VOID)
{
    HI_S32 s32Chn = 0;
    Capture_Dev_t *pstCapDev = HI3521_GetCaptureDev();
    AENC_CHN_INFO_t *pstAencCtl = NULL;

    for (s32Chn = 0; s32Chn < HI_AENC_MAX_CHN; s32Chn++)
    {
        pstAencCtl = &(pstCapDev->vi_chn[s32Chn].aenc_chn);

        if (HI_ENABLE == pstAencCtl->created)
        {
            pstAencCtl->aenc_fd = HI_MPI_AENC_GetFd(pstAencCtl->HiAencChn);
            dPrint(DEBUG, "chn[%d].fd = %d\n",s32Chn,pstAencCtl->aenc_fd)
        }
    }

    return;
}

/*****************************************************************************
 函 数 名  : HI3531_DebugWRAudioStream
 功能描述  : test for saving G711A stream
 输入参数  : HI_S32 s32AencChn, AUDIO_STREAM_S *pstAudioStream
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月11日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521A_DebugWRAudioStream(HI_S32 s32AencChn, AUDIO_STREAM_S *pstAudioStream)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32Cnt = s32AencChn;

    static FILE *fp[16] = {NULL};
    HI_CHAR name[64] = {0};

//	AV_PRINT("HI3531_DebugWRStream%d ", s32AencChn);
    if (NULL == fp[s32Cnt])
    {

        sprintf(name, "/mnt/nand/audio_chn%d.g711a", s32Cnt);

        fp[s32Cnt] = fopen(name, "wb");
        if (NULL == fp[s32Cnt])
        {
            dPrint(ERROR, "open err(%s)\n", name);
        }
    }
    else
    {
        fwrite(pstAudioStream->pStream + 4, pstAudioStream->u32Len - 4, 1, fp[s32Cnt]);
    }

    return s32Ret;
}


/*****************************************************************************
 Prototype    : HI3521A_IsAencFdValid
 Description  : select检查音频编码数据
 Input        : HI_VOID
 Output       : None
 Return Value :

  History        :
  1.Date         : 2016/5/25
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_BOOL HI3521A_IsAencFdValid(HI_VOID)
{
    HI_S32 s32LogicAiChn = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    struct timeval stTimeOut;
    HI_S32 s32MaxFd = 0;
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();
    AENC_CHN_INFO_t *pstAencCtl = NULL;

    stTimeOut.tv_sec = 1;
    stTimeOut.tv_usec = 100000;
    FD_ZERO(&(pCaptureDev->aenc_rfds));

    for (s32LogicAiChn = 0; s32LogicAiChn < HI_AENC_MAX_CHN; s32LogicAiChn++)
    {
        pstAencCtl = &(pCaptureDev->vi_chn[s32LogicAiChn].aenc_chn);;
        if (((HI_ENABLE == pstAencCtl->started)&& (HI_ENABLE == pstAencCtl->regular_enable))
                ||((HI_ENABLE == pstAencCtl->started)&& (HI_ENABLE == pstAencCtl->extra_enable)))
        {
            FD_SET(pstAencCtl->aenc_fd, &(pCaptureDev->aenc_rfds));
            if (s32MaxFd < pstAencCtl->aenc_fd)
            {
                s32MaxFd = pstAencCtl->aenc_fd;
            }
        }
    }

    s32Ret = select(s32MaxFd + 1,&(pCaptureDev->aenc_rfds), NULL, NULL, &stTimeOut);
    if (s32Ret < 0)
    {
        dPrint(ERROR, "select err(%d)\n", errno);
        return HI_FALSE;
    }
    else if ( 0 == s32Ret )
    {
        dPrint(ERROR, "select timeout, HiAencChn:%d, s32aiChn:%d, AencFd:%d\n", pstAencCtl->HiAencChn, s32LogicAiChn, pstAencCtl->aenc_fd);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_S32 HI3531_WrAencStream2DataCenter_v2(AUDIO_STREAM_S *pstStream, HI_S32 s32Chn, FRAME_HEAD_T *pstFrameHead)
{
    HI_U32 s32FrameLen = 0;

    s32FrameLen = pstStream->u32Len + sizeof(FRAME_HEAD_T);

    HDP hPkg = mem_CreatePackage(s32FrameLen,PKG_PRIORITY_LOW);

    mem_ReferPackage(hPkg);

    HI_U8 *pBuf = (HI_U8*)mem_GetPackageData(hPkg);

    if(s32Chn == 12)
        pstFrameHead->ts = pstStream->u64TimeStamp;
    memcpy(pBuf,pstFrameHead, sizeof(FRAME_HEAD_T));

    memcpy(pBuf + sizeof(FRAME_HEAD_T), pstStream->pStream, pstStream->u32Len);

    //MIC通道
    if(s32Chn!=12)
        mem_PushPackage(hPkg,g_ChnDataCenter[2*s32Chn+1]);
    mem_PushPackage(hPkg,g_ChnDataCenter[2*s32Chn]);
    mem_ReleasePackage(hPkg);
    return HI_SUCCESS;
}

HI_S32 HI3531_WrAencStream2Shm_v2(AUDIO_STREAM_S *pstStream, HI_S32 s32Chn, FRAME_HEAD_T *pstFrameHead)
{
    HI_S32 s32Ret = 0;
    HI_U32 s32FrameLen = 0;
    FRAME_HEAD_T stFrameHead;
    HI_U8 *pBuf;
    HI_CHAR pAudioBuf[AENC_BUF_LEN]= {0};
    memset(&stFrameHead, 0, sizeof(FRAME_HEAD_T));
    memcpy(&stFrameHead, pstFrameHead, sizeof(FRAME_HEAD_T));
    pBuf = (HI_U8 *)pAudioBuf;
    s32FrameLen = pstStream->u32Len + sizeof(FRAME_HEAD_T);
    memcpy(pBuf, &stFrameHead, sizeof(stFrameHead));
    memcpy(pBuf + sizeof(stFrameHead), pstStream->pStream, pstStream->u32Len);
    s32Ret = MEM_WrVencStream2Shm((HI_VOID *)pBuf, s32Chn, s32FrameLen);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("MEM_WrVencStream2Shm err");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


HI_S32 HI3531_SetAencFrameHead(AUDIO_STREAM_S *pstStream, HI_S32 s32Chn, FRAME_HEAD_T *pstFrameHead)
{
    FRAME_HEAD_T stFrameHead;
    time_t nowTime = 0;
    HI_S32 s32VencChn = (s32Chn*2) % TOTAL_VENC_CHN_NUM;
    static HI_S32 setautimes[HI_AENC_MAX_CHN] = {0};
#ifdef USE_SYS_TIMEPTS
    if (bAuFrameStart[s32Chn] == HI_FALSE &&
        (s32Chn == HI_AI_MIC_CHN || bFindFirstIFrame[s32VencChn] == HI_TRUE))
    {
        tGetAencStartTime[s32Chn] = time((time_t *)NULL);
        g_SetAuPts[s32Chn] = (HI_U64)tGetAencStartTime[s32Chn]*1000000;
        bAuFrameStart[s32Chn] = HI_TRUE;
    }

    setautimes[s32Chn]++;
    if ( setautimes[s32Chn]  == 30 )
    {
        setautimes[s32Chn] = 0;
        g_SetAuPts[s32Chn] = (HI_U64)tGetAencStartTime[s32Chn]*1000000;
        if(s32Chn == HI_AI_MIC_CHN)
            tGetAencStartTime[s32Chn]++;
        else
            tGetAencStartTime[s32Chn] = tGetVencStartTime[s32VencChn];
        stFrameHead.ts = g_SetAuPts[s32Chn];
    }
    else
    {
        stFrameHead.ts = g_SetAuPts[s32Chn]+30000;
        g_SetAuPts[s32Chn] = stFrameHead.ts;

    }
#else
    stFrameHead.ts = pstStream->u64TimeStamp;
#endif

    stFrameHead.len = pstStream->u32Len;
    stFrameHead.type = DATA_AUDIO;

#ifdef USE_SYS_TIMEPTS
    nowTime = tGetAencStartTime[s32Chn];
#else
    nowTime = time((time_t *)NULL);
#endif
    stFrameHead.nmk_time1=(HI_U8)(nowTime&0xff);
    stFrameHead.nmk_time2=(HI_U8)((nowTime>>8)&0xff);
    stFrameHead.nmk_time3=(HI_U8)((nowTime>>16)&0xff);
    stFrameHead.nmk_time4=(HI_U8)((nowTime>>24)&0xff);


    strncpy((HI_CHAR *)stFrameHead.flag, "AFRAME", sizeof(stFrameHead.flag));
    memcpy(pstFrameHead, &stFrameHead, sizeof(FRAME_HEAD_T));

    return HI_SUCCESS;

}


/*****************************************************************************
 Prototype    : HI3521A_GetAencStream
 Description  : 获取音频编码数据
 Input        : HI_VOID
 Output       : None
 Return Value :

  History        :
  1.Date         : 2016/5/25
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_S32 HI3521A_GetAencStream( )
{
    HI_S32 s32LogicAiChn = 0;  //音频输入逻辑通道号(0~12)
    HI_S32 s32Ret = HI_SUCCESS;
    Capture_Dev_t *pstCapDev = HI3521_GetCaptureDev();
    AENC_CHN_INFO_t *pstAencCtl = NULL;
    AUDIO_STREAM_S stStream;
    FRAME_HEAD_T stFrameHead;
    //HI_S32 s32aiMaxChn = HI_AENC_MAX_CHN;
    HI_S32 recordchn = 0;
    for (s32LogicAiChn = 0; s32LogicAiChn < HI_AENC_MAX_CHN; s32LogicAiChn++)
    {
        memset(&stStream, 0, sizeof(AUDIO_STREAM_S));

        pstAencCtl = &(pstCapDev->vi_chn[s32LogicAiChn].aenc_chn);;

        if (FD_ISSET(pstAencCtl->aenc_fd, &pstCapDev->aenc_rfds))
        {
            s32Ret = HI_MPI_AENC_GetStream(pstAencCtl->HiAencChn, &stStream, HI_IO_BLOCK);
            if (HI_SUCCESS != s32Ret)
            {
                dPrint(ERROR, "get stream from aenc-chn %d, s32aiChn %d, failed with %#x!\n", pstAencCtl->HiAencChn, s32LogicAiChn, s32Ret);
                return HI_FAILURE;
            }

            //HI3521A_DebugWRAudioStream(s32LogicAiChn, &stStream);

            //TM_APP_AudioStreamCpy(&stStream,s32LogicAiChn);

            recordchn =pstAencCtl->HiAencChn;

            if(recordchn!=12 && mem_DataCenterEmpty(g_ChnDataCenter[2*recordchn+1])!=1)
            {
            HI3531_SetAencFrameHead(&stStream, recordchn, &stFrameHead);

            s32Ret = HI3531_WrAencStream2DataCenter_v2(&stStream, recordchn, &stFrameHead);

            if (HI_SUCCESS != s32Ret)
                {
                    AV_PRINT("HI3531_WrAencStream2Shm err 0x%x", s32Ret);
                }
            }
            else if(mem_DataCenterEmpty(g_ChnDataCenter[2*recordchn]) != 1)
            {
                HI3531_SetAencFrameHead(&stStream, recordchn, &stFrameHead);
                s32Ret = HI3531_WrAencStream2DataCenter_v2(&stStream, recordchn, &stFrameHead);
                if (HI_SUCCESS != s32Ret)
                {
                AV_PRINT("HI3531_WrAencStream2Shm err 0x%x", s32Ret);
                }
            }
#if 0
            /*录像状态下 */
            if (STOR_GetChnRecStat(recordchn) && (recordchn < s32aiMaxChn-1) && (HDD_GetHddEvent(DISK_EVENT_REC)))
            {
                //s32Ret = HI3531_WrAencStream2Shm_v2(&stStream, recordchn, &stFrameHead);
                s32Ret = HI3531_WrAencStream2DataCenter_v2(&stStream, recordchn, &stFrameHead);
                if (HI_SUCCESS != s32Ret)
                {
                    AV_PRINT("HI3531_WrAencStream2Shm err 0x%x", s32Ret);
                    return HI_FAILURE;
                }
            }
#endif
            s32Ret = HI_MPI_AENC_ReleaseStream(pstAencCtl->HiAencChn, &stStream);
            if (HI_SUCCESS != s32Ret)
            {
                dPrint(ERROR, "release stream from aenc-chn %d, s32aiChn %d, failed with %#x!\n", pstAencCtl->HiAencChn, s32LogicAiChn, s32Ret);
                return HI_FAILURE;
            }
        }
    }

    return HI_SUCCESS;

}

/*****************************************************************************
 Prototype    : HI3521A_SetAudioInvocation
 Description  : 设置回调函数
 Input        : AudioFrameInvocation sFun
 Output       : None
 Return Value :

  History        :
  1.Date         : 2016/5/25
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_VOID HI3521A_SetAudioInvocation(AudioFrameInvocation sFun)
{
    sfSendAencData = sFun;
}

/*****************************************************************************
 Prototype    : HI3521A_GetAencStreamThd
 Description  : 音频编码数据获取线程
 Input        : HI_VOID *arg
 Output       : None
 Return Value : HI_VOID

  History        :
  1.Date         : 2016/5/25
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_VOID *HI3521A_GetAencStreamThd(HI_VOID *arg)
{
    bThRunFlag = HI_TRUE; //线程运行标志，结束该线程的时候将标志置为hi_false

#ifdef USE_SYS_TIMEPTS

    HI_S32 s32PTSCHN = 0;
    struct timeval tp;
    gettimeofday(&tp, NULL);
    int   m_second = tp.tv_sec;
    for (s32PTSCHN = 0; s32PTSCHN <HI_AENC_MAX_CHN; s32PTSCHN++)
    {
        g_SetAuPts[s32PTSCHN] = (HI_U64)m_second*1000000+tp.tv_usec;
    }

#endif

    dPrint(ALERT, "aenc tid: %d\n", gettid());


    HI3521A_GetAencFd();
    AV_PRINT("@%s pid is %d,tid%d", __FUNCTION__,getpid(),gettid());
    while ( HI_TRUE == bThRunFlag )
    {
        if (HI3521A_IsAencFdValid())
        {
            HI3521A_GetAencStream();
        }
        //usleep(1000);
        TM_USleep(0,10*1000);
    }

    return (HI_VOID *)HI_SUCCESS;
}

/*****************************************************************************
 Prototype    : HI3521A_VoIPEnable
 Description  : IP对讲时VO的参数重新初始化
 Input        : None
 Output       : None
 Return Value :

  History        :
  1.Date         : 2016/5/26
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_VOID HI3521A_VoIPEnable()
{
    if(HDD_GetHddEvent(DISK_EVENT_REC_LPB))
        return;
    /*解码音频初始化*/
    HI3521A_AdecExit();
    HI3521A_AoExit();

#if (( AENC_TYPE == AENC_PCM) || (AENC_TYPE ==AENC_ADPCM))

    /*LPCM 320 的采样点  */
    HI3521A_AoInit(VOIP_DATA_PER_FRAM);

#elif ( AENC_TYPE == AENC_G711)

    HI3521A_AoInit(REC_AUDIO_SAMP_PER_FRAME);

#endif

    HI3521A_AdecInit();
}

/*****************************************************************************
 Prototype    : HI3521a_VoIPDisable
 Description  : IP对讲结束时，恢复为默认的mp3播放的采样点数
 Input        : None
 Output       : None
 Return Value :

  History        :
  1.Date         : 2016/5/26
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_VOID HI3521A_VoIPDisable()
{
    if(HDD_GetHddEvent(DISK_EVENT_REC_LPB))
        return;
    HI3521A_AdecExit();
    HI3521A_AoExit();
    /*回放结束，重新初始化*/
    HI3521A_AoInit(HI_MP3_PLAY_PTNUMPERFRM);
    dPrint(DEBUG, "VoIP vo Disable");

}

//音频码流处理
void HI3521A_AudioFramProc(AUDIO_STREAM_S *pstStream, HI_S32 s32Chn)
{
    //dPrint(DEBUG, "chn[%d] stream, len=%d\n",s32Chn, pstStream->u32Len);

    int nWrite = 0;

#if 1
    if ( aenChn == s32Chn ) //送往adec解码然后播放，需要adec与ao先绑定
    {
        HI_S32 s32Ret = HI_MPI_ADEC_SendStream(0, pstStream, HI_TRUE);
        if (HI_SUCCESS != s32Ret )
        {
            dPrint(ERROR,"%s: HI_MPI_ADEC_SendStream(%d), failed with %#x!\n",\
                   s32Chn, s32Ret);
        }

    }
#endif

    nWrite = fwrite(&pstStream->u32Len, 1,4, audioRecFp[s32Chn]);

    nWrite = fwrite(pstStream->pStream, 1,pstStream->u32Len, audioRecFp[s32Chn]);
    if ( 0 >= nWrite )
    {
        dPrint(ERROR, "chn[%d] fwrite error\n", s32Chn);
    }

    return;
}

int Hi3521A_OpenRecFile()
{
    int i = 0;
    char fileNam[32] = {0};

    for  ( i = 0 ; i < HI_AENC_MAX_CHN ; i++ )
    {
        snprintf(fileNam, sizeof(fileNam),"./data/audio_chn%d.adpcm", i);
        audioRecFp[i] = fopen(fileNam, "w+");
        if ( NULL == audioRecFp[i] )
        {
            dPrint(ERROR, "fopen %s error\n", fileNam);
            return -1;
        }
    }

    return 0;
}

int Hi3521A_CloseRecFile()
{
    int i = 0;

    for  ( i = 0 ; i < HI_AENC_MAX_CHN ; i++ )
    {
        fclose(audioRecFp[i]);
        audioRecFp[i] = NULL;
    }

    return 0;
}
