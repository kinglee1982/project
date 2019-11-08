#include "av_public_head.h"
#include "av_sys.h"
#include "av_capture.h"
#include "av_vivo.h"

static Capture_Dev_t g_hidev;

VI_MODE_E HI3521_GetViMode(HI_VOID)
{
#ifdef MODE_720P
	return VI_MODE_8_720P;
#elif defined (MODE_16D1)
    return VI_MODE_16_D1;
#endif
}

/*****************************************************************************
 函 数 名  : HI3521_DebugCapDev
 功能描述  :
 输入参数  : HI_S32 s32Chn
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月17日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3521_DebugCapDev(HI_VOID)
{
	HI_S32 s32ChnCnt = 0;
	Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();

	AV_PRINT("HI3531_DebugCapDev enter.\n");
	AV_PRINT("sysinit aencOpen vencOpen vstd vencformat aencformat vichn vencchn width height\n");

	for (s32ChnCnt = 0; s32ChnCnt < TOTAL_VENC_CHN_NUM; s32ChnCnt++)
	{
		AV_PRINT("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
			pCaptureDev->sys_init,
			pCaptureDev->aenc_opened,
			pCaptureDev->venc_opened,
			pCaptureDev->vstd,
			pCaptureDev->video_codec_format,
			pCaptureDev->audio_codec_format,
			s32ChnCnt >> 1,
			s32ChnCnt & 0x01,
			pCaptureDev->vi_chn[s32ChnCnt >> 1].venc_chn[s32ChnCnt&0x01].pic_width,
			pCaptureDev->vi_chn[s32ChnCnt>>1].venc_chn[s32ChnCnt&0x01].pic_height);
	}

	AV_PRINT("HI3531_DebugCapDev suc.\n");
	return;
}

/*****************************************************************************
 函 数 名  : HI3521_GetCaptureDev
 功能描述  :
 输入参数  : HI_VOID
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月12日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
Capture_Dev_t *HI3521_GetCaptureDev(HI_VOID)
{
	return &g_hidev;
}

/*****************************************************************************
 函 数 名  : HI3521_CapDevInit
 功能描述  : 获取编码采集图片Init
 输入参数  : HI_S32 s32Chn
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月8日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_CapDevInit(HI_S32 s32Chn)
{
	HI_S32 s32ChnCnt = 0;
	HI_S32 s32Ret = 0;
	SIZE_S stSize;
	static HI_S32 s32Opened = HI_DISABLE;
	Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();


	if (s32Opened == HI_ENABLE)
	{
		AV_PRINT("HI3521_CapDevInit chn(%d) suc.", s32Chn);
		HI3521_EnableAVencChn(s32Chn);
		return HI_SUCCESS;
	}

	memset((HI_VOID *)pCaptureDev,0,sizeof(Capture_Dev_t));
	FD_ZERO(&(pCaptureDev->read_fds));

	pCaptureDev->vstd = VIDEO_ENCODING_MODE_PAL;
	pCaptureDev->video_codec_format = CAPTURE_COMP_H264;
	pCaptureDev->audio_codec_format = AUDIO_G711A;


	HI3521_Comm_SYS_GetPicSize((VIDEO_NORM_E)pCaptureDev->vstd, PIC_HD720, &stSize);
	for(s32ChnCnt = 0; s32ChnCnt < 8; s32ChnCnt++)
	{
		pCaptureDev->vi_chn[s32ChnCnt>>1].venc_chn[s32ChnCnt&0x01].pic_width = stSize.u32Width;
		pCaptureDev->vi_chn[s32ChnCnt>>1].venc_chn[s32ChnCnt&0x01].pic_height = stSize.u32Height;
    }

	HI3521_Comm_SYS_GetPicSize((VIDEO_NORM_E)pCaptureDev->vstd,PIC_960H, &stSize);
	for(s32ChnCnt = 8; s32ChnCnt < TOTAL_VENC_CHN_NUM; s32ChnCnt++)
	{
		pCaptureDev->vi_chn[s32ChnCnt>>1].venc_chn[s32ChnCnt&0x01].pic_width = stSize.u32Width;
		pCaptureDev->vi_chn[s32ChnCnt>>1].venc_chn[s32ChnCnt&0x01].pic_height = stSize.u32Height;
    }

	s32Ret = pthread_mutex_init(&(pCaptureDev->lock), NULL);
	if (HI_SUCCESS != s32Ret)
	{
		AV_PRINT("pthread_mutex_init failed (%#X)", s32Ret);
		return HI_FAILURE;
	}

	s32Ret = pthread_mutex_init(&(pCaptureDev->vilock), NULL);
	if (HI_SUCCESS != s32Ret)
	{
		AV_PRINT("pthread_mutex_init failed (%#X)", s32Ret);
		return HI_FAILURE;
	}

    s32Opened = HI_ENABLE;

    for (s32ChnCnt = 0; s32ChnCnt < 4; s32ChnCnt++)
	{
		pCaptureDev->vi_chn[s32Chn].cover.handle[s32ChnCnt] = -1;
		memset((HI_VOID *)&(pCaptureDev->vi_chn[s32Chn].cover.Param[s32ChnCnt]), 0, sizeof(VIDEO_COVER_PARAM));
	}
#ifdef DEBUG_INFO
	HI3521_DebugCapDev();
#endif
	HI3521_EnableAVencChn(s32Chn);

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI3521_EnableAVencChn
 功能描述  :
 输入参数  : HI_S32 s32Chn
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月12日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_EnableAVencChn(HI_S32 s32Chn)
{
	Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();

	pCaptureDev->vi_chn[s32Chn].venc_chn[CHL_MAIN_T].enable = HI_ENABLE;
	pCaptureDev->vi_chn[s32Chn].venc_chn[CHL_2END_T].enable = HI_ENABLE;
	pCaptureDev->vi_chn[s32Chn].venc_chn[CHL_MAIN_T].started = HI_DISABLE;
	pCaptureDev->vi_chn[s32Chn].venc_chn[CHL_2END_T].started = HI_DISABLE;
	pCaptureDev->vi_chn[s32Chn].aenc_chn.regular_enable = HI_ENABLE;/*MAIN*/
	pCaptureDev->vi_chn[s32Chn].aenc_chn.extra_enable = HI_ENABLE;/*MINOR*/

	return 0;
}