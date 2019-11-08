#include "av_public_head.h"
#include "av_video_init.h"
#include "av_split.h"
#include "av_sys.h"
#include "av_video_proc.h"
#include "av_vivo.h"
#include "av_store.h"
#include "av_hddfsm.h"
static SPLIT_DEV g_splitdev;

SPLIT_DEV *HI3521_GetSplitDev(HI_VOID)
{
    return &g_splitdev;
}


/*****************************************************************************
 函 数 名  : HI3531_DebugSplitChn
 功能描述  : 调试信息输出
 输入参数  : HI_S32 s32MaxChn, HI_S32 s32StartChn
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年01月14日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3531_DebugSplitChn(HI_S32 s32MaxChn, HI_S32 s32StartChn)
{
    HI_S32 s32Chn = 0;
    SPLIT_DEV *pSplitDev = HI3521_GetSplitDev();
    AV_PRINT("vo maxchn=%d startchn=%d\n", s32MaxChn, s32StartChn);
    AV_PRINT("VICHN\tVOCHN\tVPSSCHN\n");
    for (s32Chn = s32StartChn; s32Chn < s32MaxChn; s32Chn++)
    {
        if (pSplitDev->split_chn[s32Chn].binded == HI_ENABLE)
        {
            AV_PRINT("%d\t%d\t%d\n", pSplitDev->split_chn[s32Chn].vichn, pSplitDev->split_chn[s32Chn].vochn, pSplitDev->split_chn[s32Chn].vpsschn);
        }
    }

    return;
}

/*****************************************************************************
 函 数 名  : HI3521_GetCapVideoSize
 功能描述  :
 输入参数  : HI_S32 s32Chn,HI_S32 s32Mode
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月15日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
PIC_SIZE_E HI3521_GetCapVideoSize(HI_S32 s32Chn,HI_S32 s32Mode)
{
    SPLIT_DEV *pSplitDev = HI3521_GetSplitDev();

    if  (s32Chn < 0 || s32Mode < 0)
    {
        AV_PRINT("HI3521_GetCapVideoSize : invalid  chn(%d) mode(%d)", s32Chn, s32Mode);
        return (PIC_SIZE_E)HI_FAILURE;
    }

    return pSplitDev->split_chn[s32Chn].videosize[s32Mode];
}

/*****************************************************************************
 函 数 名  : HI3521_SplitDev_Init
 功能描述  : HI3521设备的画面分割初始化函数
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月7日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_SplitDev_Init(HI_VOID)
{
    HI_S32 s32ChnCnt = 0;
    SPLIT_DEV *pSplitDev = HI3521_GetSplitDev();
    DISTIBUTE_VPSS *pVpssInfo = HI3521_GetVpssInfo();

    HI_S32 s32Chn = TOTAL_VI_CHN_NUM;

    AV_PRINT("HI3531_SplitDev_Init enter.");

    memset(pSplitDev, 0, sizeof(SPLIT_DEV));

    pSplitDev->enable = HI_TRUE;
    pSplitDev->deflick = 0;
    pSplitDev->vstd = VIDEO_ENCODING_MODE_PAL;

    if (TOTAL_VI_CHN_NUM > MAX_ANALOG_VO_CHN)
    {
        s32Chn  = MAX_ANALOG_VO_CHN;
        AV_PRINT( "TOTAL_VI_CHN_NUM(%d), MAX_ANALOG_VO_CHN(%d)",
                  TOTAL_VI_CHN_NUM, MAX_ANALOG_VO_CHN);
    }

    for (s32ChnCnt = 0; s32ChnCnt < 8; s32ChnCnt++)
    {
        pSplitDev->split_chn[s32ChnCnt].audioen = 0;
        pSplitDev->split_chn[s32ChnCnt].binded = 0;
        pSplitDev->split_chn[s32ChnCnt].zoombinded = 0;
        pSplitDev->split_chn[s32ChnCnt].vichn = s32ChnCnt*2;
        pSplitDev->split_chn[s32ChnCnt].vochn = s32ChnCnt;
        pSplitDev->split_chn[s32ChnCnt].vdechn = 12+s32ChnCnt;
        pSplitDev->split_chn[s32ChnCnt].vpsschn = pVpssInfo->vi_vpss.start + s32ChnCnt;
        pSplitDev->status.index = -1;
        pSplitDev->status.mode = -1;
        if(HI3521_GetTw2968VideoType() ==  1 )
            pSplitDev->split_chn[s32ChnCnt].videosize[0] = PIC_960H;//(enum capture_size_t)HI3521_GetVencSize();
        else
            pSplitDev->split_chn[s32ChnCnt].videosize[0] = PIC_D1;
        pSplitDev->split_chn[s32ChnCnt].videosize[1] = PIC_CIF;
        pSplitDev->split_chn[s32ChnCnt].videv = s32ChnCnt / 2 * 2;
    }
    for (s32ChnCnt = 8; s32ChnCnt < s32Chn; s32ChnCnt++)
    {
        pSplitDev->split_chn[s32ChnCnt].audioen = 0;
        pSplitDev->split_chn[s32ChnCnt].binded = 0;
        pSplitDev->split_chn[s32ChnCnt].zoombinded = 0;
        pSplitDev->split_chn[s32ChnCnt].vichn = s32ChnCnt+4;
        pSplitDev->split_chn[s32ChnCnt].vochn = s32ChnCnt;
        pSplitDev->split_chn[s32ChnCnt].vdechn = s32ChnCnt+4;
        pSplitDev->split_chn[s32ChnCnt].vpsschn = pVpssInfo->vi_vpss.start + s32ChnCnt;
        pSplitDev->status.index = -1;
        pSplitDev->status.mode = -1;
        pSplitDev->split_chn[s32ChnCnt].videosize[0] = PIC_HD720;//(enum capture_size_t)HI3521_GetVencSize();
        pSplitDev->split_chn[s32ChnCnt].videosize[1] = PIC_D1;
        pSplitDev->split_chn[s32ChnCnt].videv = s32ChnCnt / 4 +1;
    }
    AV_PRINT("HI3521_SplitDev_Init suc.");
    return HI_SUCCESS;
}


HI_S32 HI3521_SplitChnDisable(HI_S32 s32Chn)
{
    HI_S32 voLayer = SAMPLE_VO_LAYER_VHD0;
    SPLIT_DEV *pSplitDev = HI3521_GetSplitDev();
    HI_S32 voChn = pSplitDev->split_chn[s32Chn].vochn;
    HI_S32 vpsschn = pSplitDev->split_chn[s32Chn].vpsschn;
    if (pSplitDev->split_chn[s32Chn].binded == HI_DISABLE)
    {
        return HI_SUCCESS;
    }

#ifdef HD_VDEC
    AV_PRINT("unbind voChn(%d) vpsschn(%d)",voChn,vpsschn);
    HI3521_Comm_VO_UnBindVpss(voLayer,voChn,vpsschn,PREVIEW_VPSS_CHN());
    HI_MPI_VO_DisableChn(voLayer,voChn);

#else
    voLayer = SAMPLE_VO_LAYER_VSD0;
    HI3521_Comm_VO_UnBindVi(voLayer,voChn,viChn);
    HI_MPI_VO_DisableChn(voLayer,voChn);
#endif
    pSplitDev->split_chn[s32Chn].binded = HI_DISABLE;
    return HI_SUCCESS;
}


HI_VOID HI3521_Split_ViDisable(HI_VOID)
{
    HI_S32 s32Chn;

    for (s32Chn = 0; s32Chn < TOTAL_VI_CHN_NUM; s32Chn++)
    {
        HI3521_SplitChnDisable(s32Chn);
    }
//	HI3521_Comm_WBC_End();
}

//解码结束一路，释放一路资源


HI_S32 HI3521_VdecChnDisable()
{
    HI_S32 voLayer = SAMPLE_VO_LAYER_VHD0;
    for(int i = 0 ; i < MAX_PLAY_BACK_CHN; i++)
    {
        SPLIT_DEV *pSplitDev = HI3521_GetSplitDev();
        HI_S32 voChn = pSplitDev->split_chn[i].vochn;
        HI_S32 vpsschn = pSplitDev->split_chn[i].vdechn;
        AV_PRINT("HI3521_VdecChnDisable unbind voChn(%d) vpsschn(%d) s32Chn(%d)",voChn,vpsschn,i);
        HI3521_Comm_VO_UnBindVpss(voLayer,voChn,vpsschn,DEC_VPSS_CHN());
        HI3521_Comm_VDEC_UnBindVpss(i,vpsschn);
        HI_MPI_VO_DisableChn(voLayer,voChn);
        HI3521_COMM_VDEC_Stop(i);
    }
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI3531_GetSplitRectCfg
 功能描述  : 获取预览区域配置
 输入参数  : SPLIT_MODE *pstSplitMode, RECT_S *pstRect, VO_CHN_ATTR_S *pstVoChnAttr
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
HI_S32 HI3521_GetSplitRectCfg(SPLIT_MODE *pstSplitMode, RECT_S *pstRect, VO_CHN_ATTR_S *pstVoChnAttr)
{
    HI_S32 i = 0;
    HI_S32 j = 0;
    HI_S32 div = 0;
    HI_S32 u32ScrnDiv = 0;
    HI_S32 u32PicWidth = 0;
    HI_S32 u32PicHeight = 0;
    HI_S32 u32SplitType = 0;
    VO_CHN_ATTR_S stVoChnAttr = {0};

    if ((PREVIEW_NR == (preview_split_mode_e)pstSplitMode->dwMode) || (NULL == pstRect) || (NULL == pstVoChnAttr))
    {
        AV_PRINT("HI3531_GetMultiSplitRect s32SplitMode(%d)", pstSplitMode->dwMode);
        return HI_FAILURE;
    }

    switch (pstSplitMode->dwMode)
    {
    case PREVIEW_4_CHN_SCREEN1:
    case PREVIEW_4_CHN_SCREEN2:
        u32SplitType = pstSplitMode->dwIndex * 4;
        u32ScrnDiv = 4;
        i = 0;
        j = 4;
        break;
    case PREVIEW_4_CHN_SCREEN3:
        u32SplitType = pstSplitMode->dwIndex * 8;
        u32ScrnDiv = 4;
        i = 0;
        j = 4;
        break;

    case PREVIEW_9_CHN_SCREEN:
        u32ScrnDiv = 9;
        u32SplitType = 0;
        i = 0;
        j = 9;
        break;
    default:
        u32SplitType = pstSplitMode->dwIndex;
        u32ScrnDiv = 1;
        i = 0;
        j = 1;
        break;
    }

    /* 对分屏开方，4分屏开方后，每行2屏 */
    div = sqrt(u32ScrnDiv);
    u32PicWidth  = pstRect->u32Width / div;
    u32PicHeight = pstRect->u32Height / div;

    for (; i < j; i++)
    {
        stVoChnAttr.stRect.s32X = (i % div) * u32PicWidth;      /* 每屏开始的X坐标 */
        stVoChnAttr.stRect.s32Y = (i / div) * u32PicHeight;
        stVoChnAttr.stRect.u32Width  = u32PicWidth;     /* 每屏的宽度 */
        stVoChnAttr.stRect.u32Height = u32PicHeight;    /* 每屏的高度 */
        stVoChnAttr.u32Priority = 0;
        stVoChnAttr.bDeflicker  = HI_TRUE;
        stVoChnAttr.stRect.s32X = DATA_ALIGN(stVoChnAttr.stRect.s32X, 1);
        stVoChnAttr.stRect.s32Y = DATA_ALIGN(stVoChnAttr.stRect.s32Y, 1);
        stVoChnAttr.stRect.u32Width = DATA_ALIGN(stVoChnAttr.stRect.u32Width, 1);
        stVoChnAttr.stRect.u32Height = DATA_ALIGN(stVoChnAttr.stRect.u32Height, 1);
        memcpy(pstVoChnAttr + i + u32SplitType, &stVoChnAttr, sizeof(VO_CHN_ATTR_S));
    }

    return HI_SUCCESS;
}

HI_S32 HI3521_Enable_SD0()
{
    HI_S32 s32Ret = HI_SUCCESS;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    VO_CHN_ATTR_S stHdChnAttr;
    s32Ret = HI_MPI_VO_GetVideoLayerAttr(SAMPLE_VO_LAYER_VSD0, &stLayerAttr);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    stHdChnAttr.stRect.s32X       = 0;
    stHdChnAttr.stRect.s32Y       = 0;
    stHdChnAttr.stRect.u32Width   = stLayerAttr.stImageSize.u32Width;;
    stHdChnAttr.stRect.u32Height  = stLayerAttr.stImageSize.u32Height;
    stHdChnAttr.u32Priority       = 0;
    stHdChnAttr.bDeflicker        = HI_FALSE;
    s32Ret = HI_MPI_VO_SetChnAttr(SAMPLE_VO_LAYER_VSD0,0, &stHdChnAttr);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT(" HI_MPI_VO_SetChnAttr failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    s32Ret = HI_MPI_VO_SetChnFrameRate(SAMPLE_VO_LAYER_VSD0, 0, FRAME_RATE_DEFAULT);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT(" HI_MPI_VO_SetChnFrameRate failed with %#x!\n",s32Ret);
        return HI_FAILURE;
    }
    s32Ret = HI_MPI_VO_EnableChn(SAMPLE_VO_LAYER_VSD0, 0);//使用WBC 也需要使能SD
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("HI_MPI_VO_EnableChn failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI3531_InitSplitChn
 功能描述  : 获取预览区域配置
 输入参数  : const SPLIT_MODE* pstSplitMode
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
HI_S32 HI3521_InitSplitChn(SPLIT_MODE* pstSplitMode)
{
    VO_LAYER VoLayer;
    HI_S32 vichn;
    HI_S32 vochn;
    HI_S32 vpsschn;
    HI_S32 s32Chn;
    HI_S32 s32MaxChn = 0;
    HI_S32 s32StartChn = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    VO_CHN_ATTR_S stChnAttr[12];
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    VO_CHN_ATTR_S stHdChnAttr[12];
    VO_VIDEO_LAYER_ATTR_S stHdLayerAttr;
    SPLIT_DEV *pSplitDev = HI3521_GetSplitDev();
    HI_S32 s32VoChn = 0;
    RW_LOCK_T *pVoChnLock = NULL;
    HI3531_GetLock(LOCK_VO_CHN_E, &pVoChnLock);

    memset((HI_VOID *)&stChnAttr, 0, sizeof(VO_CHN_ATTR_S)*12);
    memset((HI_VOID *)&stLayerAttr, 0, sizeof(VO_VIDEO_LAYER_ATTR_S));

    memset((HI_VOID *)&stHdChnAttr, 0, sizeof(VO_CHN_ATTR_S)*12);
    memset((HI_VOID *)&stHdLayerAttr, 0, sizeof(VO_VIDEO_LAYER_ATTR_S));


#ifdef HD_VDEC
    s32Ret =HI3521_Enable_SD0();
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("HI3521_Enable_SD0");
        return HI_FAILURE;
    }
    s32Ret = HI_MPI_VO_GetVideoLayerAttr(SAMPLE_VO_LAYER_VHD0, &stHdLayerAttr);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("HI_MPI_VO_GetVideoLayerAttr(%d) errcode: 0x%x", SAMPLE_VO_DEV_DHD0, s32Ret);
        return HI_FAILURE;
    }
    HI3521_GetSplitRectCfg(pstSplitMode, &stHdLayerAttr.stDispRect, stHdChnAttr);
#else
    s32Ret = HI_MPI_VO_GetVideoLayerAttr(SAMPLE_VO_LAYER_VSD0, &stLayerAttr);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("HI_MPI_VO_GetVideoLayerAttr(%d) errcode: 0x%x", SAMPLE_VO_DEV_DSD0, s32Ret);
        return HI_FAILURE;
    }
    HI3521_GetSplitRectCfg(pstSplitMode, &stLayerAttr.stDispRect, stChnAttr);
#endif

    switch (pstSplitMode->dwMode)
    {
    case PREVIEW_4_CHN_SCREEN1:
    case PREVIEW_4_CHN_SCREEN2:
        s32StartChn = pstSplitMode->dwIndex * 4;
        s32MaxChn = (pstSplitMode->dwIndex) ? 8 : 4;
        break;
    case PREVIEW_4_CHN_SCREEN3:
        s32StartChn = 8;
        s32MaxChn =  12 ;
        break;
    case PREVIEW_9_CHN_SCREEN:
        s32StartChn = 0;
        s32MaxChn = 9;
        break;
    default:
        s32StartChn = pstSplitMode->dwIndex;
        s32MaxChn = pstSplitMode->dwIndex + 1;
        break;
    }

    pSplitDev->status.mode = pstSplitMode->dwMode;
    pSplitDev->status.index = pstSplitMode->dwIndex;

    AV_PRINT("pstSplitMode->dwMode(%d), pstSplitMode->dwIndex(%d)",
             pstSplitMode->dwMode, pstSplitMode->dwIndex);

    for (s32Chn = s32StartChn; s32Chn < s32MaxChn; s32Chn++)
    {
        //	if(PREVIEW_9_CHN_SCREEN == pstSplitMode->dwMode && 8 == s32Chn)
        //	{
        //		break;
        //	}

        if (pSplitDev->split_chn[s32Chn].binded == HI_DISABLE)
        {

            vochn = pSplitDev->split_chn[s32Chn].vochn;
            vpsschn = pSplitDev->split_chn[s32Chn].vpsschn;
            vichn = pSplitDev->split_chn[s32Chn].vichn;

#ifdef HD_VDEC
            //高清的配置VO-vpss绑定
            VoLayer = SAMPLE_VO_LAYER_VHD0;
            s32Ret = HI_MPI_VO_SetChnAttr(VoLayer, vochn, &stHdChnAttr[s32Chn]);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT( "HI_MPI_VO_SetChnAttr(%d,%d) errcode: %#x", vochn, vichn, s32Ret);
            }
            s32Ret = HI_MPI_VO_SetChnFrameRate(VoLayer, vochn, FRAME_RATE_DEFAULT);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT( "HI_MPI_VO_SetChnFrameRate(%d,%d) errcode: %#x", vochn, vichn, s32Ret);
            }
            s32Ret = HI_MPI_VO_EnableChn(VoLayer,vochn);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT( "HI_MPI_VO_EnableChn(%d,%d) errcode: %#x", vochn, vichn, s32Ret);
            }
            s32Ret = HI3521_Comm_VO_BindVpss(VoLayer, vochn, vpsschn, PREVIEW_VPSS_CHN());
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT( "HI3521_Comm_VO_BindVpss(%d,%d) errcode: %#x", vochn, vpsschn, s32Ret);
            }
#else
            //标清的配置VO-VI绑定
            VoLayer = SAMPLE_VO_LAYER_VSD0;
            s32Ret = HI_MPI_VO_SetChnAttr(VoLayer, vochn, &stChnAttr[s32Chn]);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT( "HI_MPI_VO_SetChnAttr(%d,%d) errcode: 0x%x", vochn, vichn, s32Ret);
            }
            s32Ret = HI_MPI_VO_SetChnFrameRate(VoLayer, vochn,  FRAME_RATE_DEFAULT);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT( "HI_MPI_VO_SetChnFrameRate(%d,%d) errcode: 0x%x", vochn, vichn, s32Ret);
            }
            s32Ret = HI_MPI_VO_EnableChn(VoLayer,vochn);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT( "HI_MPI_VO_EnableChn(%d,%d) errcode: 0x%x", vochn, vichn, s32Ret);
            }
            s32Ret =  HI3521_Comm_VO_BindVi(VoLayer, vochn, vichn);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT( "HI3521_Comm_VO_BindVi(%d,%d) errcode: 0x%x", vochn, vichn, s32Ret);
            }
#endif
            pSplitDev->split_chn[s32Chn].binded = HI_ENABLE;


        }
    }


    HI3531_SetLockStat(pVoChnLock, s32VoChn);
    HI3531_DebugSplitChn(s32MaxChn,s32StartChn);
    HI3521_Comm_WBC_Start();

    APP_InitHiFb();
    return HI_SUCCESS;
}



/*****************************************************************************
 函 数 名  : HI3531_InitSplitChn
 功能描述  : 获取预览区域配置
 输入参数  : const SPLIT_MODE* pstSplitMode
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
HI_S32 HI3521_SplitChnSet(SPLIT_MODE* pstSplitMode)
{
    VO_DEV voDev = 0;
    VO_LAYER VoLayer;
//	HI_S32 vichn;
    HI_S32 vochn;
    HI_S32 vpsschn;
    HI_S32 s32Chn;
    HI_S32 s32MaxChn = 0;
    HI_S32 s32StartChn = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    VO_CHN_ATTR_S stChnAttr[12];
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    VO_CHN_ATTR_S stHdChnAttr[12];
    VO_VIDEO_LAYER_ATTR_S stHdLayerAttr;
    SPLIT_DEV *pSplitDev = HI3521_GetSplitDev();
    RW_LOCK_T *pVoChnLock = NULL;
    HI3531_GetLock(LOCK_VO_CHN_E, &pVoChnLock);

    memset((HI_VOID *)&stChnAttr, 0, sizeof(VO_CHN_ATTR_S)*12);
    memset((HI_VOID *)&stLayerAttr, 0, sizeof(VO_VIDEO_LAYER_ATTR_S));

    memset((HI_VOID *)&stHdChnAttr, 0, sizeof(VO_CHN_ATTR_S)*12);
    memset((HI_VOID *)&stHdLayerAttr, 0, sizeof(VO_VIDEO_LAYER_ATTR_S));
    int i = 0;
#ifdef HD_VDEC
    for (i = 0; i < APP_VO_CHNCNT; i++)
    {
        s32Ret = HI_MPI_VO_HideChn(SAMPLE_VO_LAYER_VHD0, i);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( "#[app]HI_MPI_VO_ChnHide(%d,%d) errcode: 0x%x", voDev, i, s32Ret);
        }
    }
    s32Ret = HI_MPI_VO_GetVideoLayerAttr(SAMPLE_VO_LAYER_VHD0, &stHdLayerAttr);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("HI_MPI_VO_GetVideoLayerAttr(%d) errcode: 0x%x", SAMPLE_VO_DEV_DHD0, s32Ret);
        return HI_FAILURE;
    }
    HI3521_GetSplitRectCfg(pstSplitMode, &stHdLayerAttr.stDispRect, stHdChnAttr);
#else
    for (i = 0; i < APP_VO_CHNCNT; i++)
    {
        s32Ret = HI_MPI_VO_HideChn(SAMPLE_VO_LAYER_VSD0, i);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( "#[app]HI_MPI_VO_ChnHide(%d,%d) errcode: 0x%x", voDev, i, s32Ret);
        }
    }
    s32Ret = HI_MPI_VO_GetVideoLayerAttr(SAMPLE_VO_LAYER_VSD0, &stLayerAttr);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("HI_MPI_VO_GetVideoLayerAttr(%d) errcode: 0x%x", SAMPLE_VO_DEV_DSD0, s32Ret);
        return HI_FAILURE;
    }
    HI3521_GetSplitRectCfg(pstSplitMode, &stLayerAttr.stDispRect, stChnAttr);
#endif

    switch (pstSplitMode->dwMode)
    {
    case PREVIEW_4_CHN_SCREEN1:
    case PREVIEW_4_CHN_SCREEN2:
        s32StartChn = pstSplitMode->dwIndex * 4;
        s32MaxChn = (pstSplitMode->dwIndex) ? 8 : 4;
        break;
    case PREVIEW_4_CHN_SCREEN3:
        s32StartChn = 8;
        s32MaxChn =  12 ;
        break;
    case PREVIEW_9_CHN_SCREEN:
        s32StartChn = 0;
        s32MaxChn = 9;
        break;
    default:
        s32StartChn = pstSplitMode->dwIndex;
        s32MaxChn = pstSplitMode->dwIndex + 1;
        break;
    }

    pSplitDev->status.mode = pstSplitMode->dwMode;
    pSplitDev->status.index = pstSplitMode->dwIndex;

    AV_PRINT("pstSplitMode->dwMode(%d), pstSplitMode->dwIndex(%d)",
             pstSplitMode->dwMode, pstSplitMode->dwIndex);

    for (s32Chn = s32StartChn; s32Chn < s32MaxChn; s32Chn++)
    {
        if (pSplitDev->split_chn[s32Chn].binded == HI_DISABLE)
        {

            vochn = pSplitDev->split_chn[s32Chn].vochn;
            vpsschn = pSplitDev->split_chn[s32Chn].vpsschn;
//			vichn = pSplitDev->split_chn[s32Chn].vichn;

#ifdef HD_VDEC
            //高清的配置VO-vpss绑定
            VoLayer = SAMPLE_VO_LAYER_VHD0;
            HI_MPI_VO_SetChnAttr(VoLayer, vochn, &stHdChnAttr[s32Chn]);
            HI_MPI_VO_SetChnFrameRate(VoLayer, vochn, FRAME_RATE_DEFAULT);
            HI_MPI_VO_EnableChn(VoLayer,vochn);
            //		HI3521_Comm_VO_BindVi(VoLayer, vochn, vichn);
            HI3521_Comm_VO_BindVpss(VoLayer, vochn, vpsschn, PREVIEW_VPSS_CHN());


#else
            //标清的配置VO-VI绑定
            VoLayer = SAMPLE_VO_LAYER_VSD0;
            s32Ret = HI_MPI_VO_SetChnAttr(VoLayer, vochn, &stChnAttr[s32Chn]);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT( "HI_MPI_VO_SetChnAttr(%d,%d) errcode: 0x%x", vochn, vichn, s32Ret);
            }
            s32Ret = HI_MPI_VO_SetChnFrameRate(VoLayer, vochn,  FRAME_RATE_DEFAULT);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT( "HI_MPI_VO_SetChnFrameRate(%d,%d) errcode: 0x%x", vochn, vichn, s32Ret);
            }
            s32Ret = HI_MPI_VO_EnableChn(VoLayer,vochn);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT( "HI_MPI_VO_EnableChn(%d,%d) errcode: 0x%x", vochn, vichn, s32Ret);
            }
            s32Ret =  HI3521_Comm_VO_BindVi(VoLayer, vochn, vichn);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT( "HI3521_Comm_VO_BindVi(%d,%d) errcode: 0x%x", vochn, vichn, s32Ret);
            }

#endif
            pSplitDev->split_chn[s32Chn].binded = HI_ENABLE;


        }
    }

//	HI3531_SetLockStat(pVoChnLock, s32VoChn);
    HI3531_DebugSplitChn(s32MaxChn,s32StartChn);
//	HI3521_Comm_WBC_Start();



    return HI_SUCCESS;
}



/*****************************************************************************
 函 数 名  : HI3531_InitSplitChn
 功能描述  : 获取预览区域配置
 输入参数  : const SPLIT_MODE* pstSplitMode
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
HI_S32 HI3521_VdecChnInit(SPLIT_MODE* pstSplitMode)
{
    VO_LAYER VoLayer;
    HI_S32 vdechn;
    HI_S32 vochn;
    HI_S32 s32Chn;
    HI_S32 s32MaxChn = 0;
    HI_S32 s32StartChn = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    VO_CHN_ATTR_S stChnAttr[APP_VO_DECCNT];
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    VO_CHN_ATTR_S stHdChnAttr[APP_VO_DECCNT];
    VO_VIDEO_LAYER_ATTR_S stHdLayerAttr;
    SPLIT_DEV *pSplitDev = HI3521_GetSplitDev();
    RW_LOCK_T *pVoChnLock = NULL;
    HI3531_GetLock(LOCK_VO_CHN_E, &pVoChnLock);

    memset((HI_VOID *)&stChnAttr, 0, sizeof(VO_CHN_ATTR_S)*APP_VO_DECCNT);
    memset((HI_VOID *)&stLayerAttr, 0, sizeof(VO_VIDEO_LAYER_ATTR_S));

    memset((HI_VOID *)&stHdChnAttr, 0, sizeof(VO_CHN_ATTR_S)*APP_VO_DECCNT);
    memset((HI_VOID *)&stHdLayerAttr, 0, sizeof(VO_VIDEO_LAYER_ATTR_S));
#ifdef HD_VDEC
    s32Ret = HI_MPI_VO_GetVideoLayerAttr(SAMPLE_VO_LAYER_VHD0, &stHdLayerAttr);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("HI_MPI_VO_GetVideoLayerAttr(%d) errcode: 0x%x", SAMPLE_VO_DEV_DHD0, s32Ret);
        return HI_FAILURE;
    }
    HI3521_GetSplitRectCfg(pstSplitMode, &stHdLayerAttr.stDispRect, stHdChnAttr);
#else
    for (i = 0; i < APP_VO_DECCNT; i++)
    {
        s32Ret = HI_MPI_VO_HideChn(SAMPLE_VO_LAYER_VSD0, i);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( "#[app]HI_MPI_VO_ChnHide(%d,%d) errcode: 0x%x", voDev, i, s32Ret);
        }
    }
    s32Ret = HI_MPI_VO_GetVideoLayerAttr(SAMPLE_VO_LAYER_VSD0, &stLayerAttr);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("HI_MPI_VO_GetVideoLayerAttr(%d) errcode: 0x%x", SAMPLE_VO_DEV_DSD0, s32Ret);
        return HI_FAILURE;
    }
    HI3521_GetSplitRectCfg(pstSplitMode, &stLayerAttr.stDispRect, stChnAttr);
#endif

    switch (pstSplitMode->dwMode)
    {
    case PREVIEW_4_CHN_SCREEN1:
    case PREVIEW_4_CHN_SCREEN2:
        s32StartChn = pstSplitMode->dwIndex * 4;
        s32MaxChn = (pstSplitMode->dwIndex) ? 8 : 4;
        break;
    case PREVIEW_4_CHN_SCREEN3:
        s32StartChn = 8;
        s32MaxChn =  12 ;
        break;
    case PREVIEW_9_CHN_SCREEN:
        s32StartChn = 0;
        s32MaxChn = 9;
        break;
    default:
        s32StartChn = pstSplitMode->dwIndex;
        s32MaxChn = pstSplitMode->dwIndex + 1;
        break;
    }

    pSplitDev->status.mode = pstSplitMode->dwMode;
    pSplitDev->status.index = pstSplitMode->dwIndex;

    AV_PRINT("pstSplitMode->dwMode(%d), pstSplitMode->dwIndex(%d)",
             pstSplitMode->dwMode, pstSplitMode->dwIndex);

    for (s32Chn = s32StartChn; s32Chn < s32MaxChn; s32Chn++)
    {

        if (pSplitDev->split_chn[s32Chn].binded == HI_DISABLE)
        {

            vochn = pSplitDev->split_chn[s32Chn].vochn;
            vdechn = pSplitDev->split_chn[s32Chn].vdechn;
#ifdef HD_VDEC
            //高清的配置VO-vpss绑定
            VoLayer = SAMPLE_VO_LAYER_VHD0;
            HI_MPI_VO_SetChnAttr(VoLayer, vochn, &stHdChnAttr[s32Chn]);
            HI_MPI_VO_SetChnFrameRate(VoLayer, vochn, FRAME_RATE_DEFAULT);
            HI_MPI_VO_EnableChn(VoLayer,vochn);
            HI3521_Comm_VO_BindVpss(VoLayer, vochn, vdechn,DEC_VPSS_CHN());


#else
            //标清的配置VO-VI绑定
            VoLayer = SAMPLE_VO_LAYER_VSD0;
            s32Ret = HI_MPI_VO_SetChnAttr(VoLayer, vochn, &stChnAttr[s32Chn]);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT( "HI_MPI_VO_SetChnAttr(%d,%d) errcode: 0x%x", vochn, vdechn, s32Ret);
            }
            s32Ret = HI_MPI_VO_SetChnFrameRate(VoLayer, vochn,  FRAME_RATE_DEFAULT);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT( "HI_MPI_VO_SetChnFrameRate(%d,%d) errcode: 0x%x", vochn, vdechn, s32Ret);
            }
            s32Ret = HI_MPI_VO_EnableChn(VoLayer,vochn);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT( "HI_MPI_VO_EnableChn(%d,%d) errcode: 0x%x", vochn, vdechn, s32Ret);
            }
            s32Ret =  HI3521_COMM_VDEC_BindVo(VdChn, VoLayer, vochn);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT( "HI3521_Comm_VO_BindVi(%d,%d) errcode: 0x%x", vochn, vdechn, s32Ret);
            }

#endif
            pSplitDev->split_chn[s32Chn].binded = HI_ENABLE;


        }
    }

//	HI3531_SetLockStat(pVoChnLock, s32VoChn);
    HI3531_DebugSplitChn(s32MaxChn,s32StartChn);
//	HI3521_Comm_WBC_Start();



    return HI_SUCCESS;
}





/*****************************************************************************
 函 数 名  : HI3531_InitSplitChn
 功能描述  : 获取预览区域配置
 输入参数  : const SPLIT_MODE* pstSplitMode
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
HI_S32 HI3521_VdecChnSet(SPLIT_MODE* pstSplitMode)
{
    VO_DEV voDev = 0;
    VO_LAYER VoLayer;
    HI_S32 vdechn;
    HI_S32 vochn;
    HI_S32 s32Chn;
    HI_S32 s32MaxChn = 0;
    HI_S32 s32StartChn = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    VO_CHN_ATTR_S stChnAttr[APP_VO_DECCNT];
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    VO_CHN_ATTR_S stHdChnAttr[APP_VO_DECCNT];
    VO_VIDEO_LAYER_ATTR_S stHdLayerAttr;
    SPLIT_DEV *pSplitDev = HI3521_GetSplitDev();
    RW_LOCK_T *pVoChnLock = NULL;
    HI3531_GetLock(LOCK_VO_CHN_E, &pVoChnLock);

    memset((HI_VOID *)&stChnAttr, 0, sizeof(VO_CHN_ATTR_S)*APP_VO_DECCNT);
    memset((HI_VOID *)&stLayerAttr, 0, sizeof(VO_VIDEO_LAYER_ATTR_S));

    memset((HI_VOID *)&stHdChnAttr, 0, sizeof(VO_CHN_ATTR_S)*APP_VO_DECCNT);
    memset((HI_VOID *)&stHdLayerAttr, 0, sizeof(VO_VIDEO_LAYER_ATTR_S));
    int i = 0;
#ifdef HD_VDEC
    for (i = 0; i < APP_VO_DECCNT; i++)
    {
        s32Ret = HI_MPI_VO_HideChn(SAMPLE_VO_LAYER_VHD0, i);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( "#[app]HI_MPI_VO_ChnHide(%d,%d) errcode: 0x%x", voDev, i, s32Ret);
        }
    }
    s32Ret = HI_MPI_VO_GetVideoLayerAttr(SAMPLE_VO_LAYER_VHD0, &stHdLayerAttr);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("HI_MPI_VO_GetVideoLayerAttr(%d) errcode: 0x%x", SAMPLE_VO_DEV_DHD0, s32Ret);
        return HI_FAILURE;
    }
    HI3521_GetSplitRectCfg(pstSplitMode, &stHdLayerAttr.stDispRect, stHdChnAttr);
#else
    for (i = 0; i < APP_VO_DECCNT; i++)
    {
        s32Ret = HI_MPI_VO_HideChn(SAMPLE_VO_LAYER_VSD0, i);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( "#[app]HI_MPI_VO_ChnHide(%d,%d) errcode: 0x%x", voDev, i, s32Ret);
        }
    }
    s32Ret = HI_MPI_VO_GetVideoLayerAttr(SAMPLE_VO_LAYER_VSD0, &stLayerAttr);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("HI_MPI_VO_GetVideoLayerAttr(%d) errcode: 0x%x", SAMPLE_VO_DEV_DSD0, s32Ret);
        return HI_FAILURE;
    }
    HI3521_GetSplitRectCfg(pstSplitMode, &stLayerAttr.stDispRect, stChnAttr);
#endif

    switch (pstSplitMode->dwMode)
    {
    case PREVIEW_4_CHN_SCREEN1:
    case PREVIEW_4_CHN_SCREEN2:
        s32StartChn = pstSplitMode->dwIndex * 4;
        s32MaxChn = (pstSplitMode->dwIndex) ? 8 : 4;
        break;
    case PREVIEW_4_CHN_SCREEN3:
        s32StartChn = 8;
        s32MaxChn =  12 ;
        break;
    case PREVIEW_9_CHN_SCREEN:
        s32StartChn = 0;
        s32MaxChn = 9;
        break;
    default:
        s32StartChn = pstSplitMode->dwIndex;
        s32MaxChn = pstSplitMode->dwIndex + 1;
        break;
    }

    pSplitDev->status.mode = pstSplitMode->dwMode;
    pSplitDev->status.index = pstSplitMode->dwIndex;

    AV_PRINT("pstSplitMode->dwMode(%d), pstSplitMode->dwIndex(%d)",
             pstSplitMode->dwMode, pstSplitMode->dwIndex);

    for (s32Chn = s32StartChn; s32Chn < s32MaxChn; s32Chn++)
    {
        vochn = pSplitDev->split_chn[s32Chn].vochn;
        vdechn = pSplitDev->split_chn[s32Chn].vdechn;
#ifdef HD_VDEC
        //高清的配置VO-vpss绑定
        VoLayer = SAMPLE_VO_LAYER_VHD0;
        s32Ret = HI_MPI_VO_SetChnAttr(VoLayer, vochn, &stHdChnAttr[s32Chn]);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( "HI_MPI_VO_SetChnAttr(%d,%d) errcode: 0x%x", vochn, vdechn, s32Ret);
        }
        s32Ret = HI_MPI_VO_SetChnFrameRate(VoLayer, vochn, FRAME_RATE_DEFAULT);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( "HI_MPI_VO_SetChnFrameRate errcode: 0x%x" , s32Ret);
        }
        s32Ret = HI_MPI_VO_ShowChn(VoLayer,vochn);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( "HI_MPI_VO_SetChnFrameRateerrcode: 0x%x", s32Ret);
        }
#else
        //标清的配置VO-VI绑定
        VoLayer = SAMPLE_VO_LAYER_VSD0;
        s32Ret = HI_MPI_VO_SetChnAttr(VoLayer, vochn, &stChnAttr[s32Chn]);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( "HI_MPI_VO_SetChnAttr(%d,%d) errcode: 0x%x", vochn, vdechn, s32Ret);
        }
        s32Ret = HI_MPI_VO_SetChnFrameRate(VoLayer, vochn,  FRAME_RATE_DEFAULT);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( "HI_MPI_VO_SetChnFrameRate(%d,%d) errcode: 0x%x", vochn, vdechn, s32Ret);
        }
        s32Ret = HI_MPI_VO_EnableChn(VoLayer,vochn);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( "HI_MPI_VO_EnableChn(%d,%d) errcode: 0x%x", vochn, vdechn, s32Ret);
        }
        s32Ret =  HI3521_COMM_VDEC_BindVo(VdChn, VoLayer, vochn);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( "HI3521_Comm_VO_BindVi(%d,%d) errcode: 0x%x", vochn, vdechn, s32Ret);
        }
#endif
    }
    return HI_SUCCESS;
}




/*****************************************************************************
 函 数 名  : Hi3531_SwitchPreview
 功能描述  : 启动预览
 输入参数  : HI_S32 s32PreviewType
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
HI_VOID Hi3521_SwitchVdecChn(HI_S32 chn)
{
    SPLIT_MODE stSplitMode = {0};
    SPLIT_DEV *pSplitDev = HI3521_GetSplitDev();
    HI_S32 s32PreviewType;
    switch(chn)
    {
    case 0:
        s32PreviewType = PREVIEW_CHN1;
        break;
    case 1:
        s32PreviewType = PREVIEW_CHN2;
        break;
    case 2:
        s32PreviewType = PREVIEW_CHN3;
        break;
    case 3:
        s32PreviewType = PREVIEW_CHN4;
        break;
    case 4:
        s32PreviewType = PREVIEW_4_CHN_SCREEN1;
        break;
    default:
        s32PreviewType = PREVIEW_4_CHN_SCREEN1;
        break;
    }
    stSplitMode.dwMode = (preview_split_mode_e)s32PreviewType;
    if (s32PreviewType <= ((HI_S32)PREVIEW_CHN8))
    {
        stSplitMode.dwIndex = s32PreviewType - 1;
    }
    else if (s32PreviewType == ((HI_S32)PREVIEW_4_CHN_SCREEN1))
    {
        stSplitMode.dwIndex = 0;
    }
    else if (s32PreviewType == ((HI_S32)PREVIEW_4_CHN_SCREEN2)||s32PreviewType == ((HI_S32)PREVIEW_4_CHN_SCREEN3))
    {
        stSplitMode.dwIndex = 1;
    }
    else if (s32PreviewType == ((HI_S32)PREVIEW_9_CHN_SCREEN))
    {
        stSplitMode.dwIndex = 0;
    }
    else
    {
        AV_PRINT("Hi3531_SwitchPreview invaild s32PreviewType");
        return;
    }
    AV_PRINT("mode--index(%d--%d), switchmode --index(%d--%d)",
             pSplitDev->status.mode,pSplitDev->status.index,
             (HI_S32)stSplitMode.dwMode,(HI_S32)stSplitMode.dwIndex);
    if (pSplitDev->status.mode == (HI_S32)stSplitMode.dwMode &&
            pSplitDev->status.index == (HI_S32)stSplitMode.dwIndex)
    {
        AV_PRINT("need not switch");
        return;
    }
    HI3521_VdecChnSet(&stSplitMode);
    return;
}


/*****************************************************************************
 函 数 名  : HI3521_StartDefaultPre
 功能描述  : 启动默认预览
 输入参数  : HI_VOID
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3521_StartDefaultPreview(HI_VOID)
{
    SPLIT_MODE stSplitMode = {0};
    HI3521_Split_ViDisable();
    stSplitMode.dwMode = PREVIEW_4_CHN_SCREEN1;
    stSplitMode.dwIndex = 0;
    AV_PRINT("APP_GetDefaultChnPara : %d %d",stSplitMode.dwMode,stSplitMode.dwIndex);
    HI3521_InitSplitChn(&stSplitMode);
}


/*****************************************************************************
 函 数 名  : Hi3531_SwitchPreview
 功能描述  : 启动预览
 输入参数  : HI_S32 s32PreviewType
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
HI_S32 Hi3521_SwitchPreview(HI_S32 s32PreviewType)
{
    SPLIT_MODE stSplitMode = {0};
    SPLIT_DEV *pSplitDev = HI3521_GetSplitDev();


    stSplitMode.dwMode = (preview_split_mode_e)s32PreviewType;
    if (s32PreviewType <= ((HI_S32)PREVIEW_CHN12))
    {
        stSplitMode.dwIndex = s32PreviewType - 1;
    }
    else if (s32PreviewType == ((HI_S32)PREVIEW_4_CHN_SCREEN1))
    {
        stSplitMode.dwIndex = 0;
    }
    else if (s32PreviewType == ((HI_S32)PREVIEW_4_CHN_SCREEN2)||s32PreviewType == ((HI_S32)PREVIEW_4_CHN_SCREEN3))
    {
        stSplitMode.dwIndex = 1;
    }
    else if (s32PreviewType == ((HI_S32)PREVIEW_9_CHN_SCREEN))
    {
        stSplitMode.dwIndex = 0;
    }
    else
    {
        AV_PRINT("Hi3531_SwitchPreview invaild s32PreviewType");
        return HI_FAILURE;
    }

    AV_PRINT("mode--index(%d--%d), switchmode --index(%d--%d)",
             pSplitDev->status.mode,pSplitDev->status.index,
             (HI_S32)stSplitMode.dwMode,(HI_S32)stSplitMode.dwIndex);

    if (pSplitDev->status.mode == (HI_S32)stSplitMode.dwMode &&
            pSplitDev->status.index == (HI_S32)stSplitMode.dwIndex)
    {
        AV_PRINT("need not switch");
        return HI_SUCCESS;
    }

    HI3521_Split_ViDisable();

    HI3521_SplitChnSet(&stSplitMode);

    return HI_SUCCESS;
}


