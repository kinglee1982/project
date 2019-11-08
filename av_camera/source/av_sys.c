#include <termio.h>
#include <linux/serio.h>
#include "av_public_head.h"
#include "av_common_21.h"
#include "av_vivo.h"
#include "av_sys.h"
#include "av_split.h"
#include "av_video_init.h"
//#include "av_dec.h"

static DISTIBUTE_VPSS g_vpss_info;

/*****************************************************************************
 函 数 名  : HI3521_GetVpssInfo
 功能描述  : VPSS管理和配置的全局结构
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  : DISTIBUTE_VPSS
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月7日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
DISTIBUTE_VPSS *HI3521_GetVpssInfo(HI_VOID)
{
    return &g_vpss_info;
}

/*****************************************************************************
 函 数 名  : HI3521_GetDevType
 功能描述  : 获取设备的类型
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月10日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
DEV_TYPE_E HI3521_GetDevType(HI_VOID)
{
    return DEV_720P_8CHN;
}

/*****************************************************************************
 函 数 名  : HI3521_Debug_Vpss
 功能描述  :
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  : static
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月17日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3521_Debug_Vpss(HI_VOID)
{
    DISTIBUTE_VPSS *pVpssInfo = HI3521_GetVpssInfo();
    AV_PRINT("HI3521_Debug_Vpss begin");
    AV_PRINT("vistart viend cap_main jpeg cap_second preview pip");
    AV_PRINT("%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
             pVpssInfo->vi_vpss.start,
             pVpssInfo->vi_vpss.end,
             pVpssInfo->capture_main_vpss_chn,
             pVpssInfo->jpeg_vpss_chn,
             pVpssInfo->capture_second_vpss_chn,
             pVpssInfo->preview_vpss_chn,
             pVpssInfo->pip_vpss_chn);
    AV_PRINT("mixstart mixend zoom play");
    AV_PRINT("%d\t%d\t%d\t%d\n",
             pVpssInfo->mix_vpss.start,
             pVpssInfo->mix_vpss.end,
             pVpssInfo->zoom_vpss_chn,
             pVpssInfo->play_vpss_chn);
    AV_PRINT("digitstart digitend vdec");
    AV_PRINT("%d\t%d\t%d\n",
             pVpssInfo->digit_vpss.start,
             pVpssInfo->digit_vpss.end,
             pVpssInfo->vdec_vpss_chn);
    AV_PRINT("HI3521_Debug_Vpss end.");

    return;
}


/*****************************************************************************
 函 数 名  : HI3531_SysVpss_Init
 功能描述  : VPSS的简单初始化复制，各个模式下对应的VPSS物理通道
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  : static
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月8日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
static HI_VOID HI3521_SysVpss_Init(HI_VOID)
{
    DISTIBUTE_VPSS *pVpssInfo = HI3521_GetVpssInfo();
    AV_PRINT("HI3531_SysVpss_Init enter.");
	HI_S32 s32ViCount = TOTAL_VI_CHN_NUM;
    pVpssInfo->vi_vpss.start = 0;
    pVpssInfo->vi_vpss.end = s32ViCount;
    pVpssInfo->capture_main_vpss_chn = VPSS_CHN0;
    pVpssInfo->jpeg_vpss_chn = VPSS_CHN2;//暂时不用
    pVpssInfo->capture_second_vpss_chn = VPSS_CHN1;
    pVpssInfo->preview_vpss_chn = VPSS_CHN3;
    pVpssInfo->pip_vpss_chn = VPSS_CHN3;

    pVpssInfo->mix_vpss.start = pVpssInfo->vi_vpss.end;
    pVpssInfo->mix_vpss.end = (s32ViCount<<1);
    pVpssInfo->zoom_vpss_chn = VPSS_CHN1;
    pVpssInfo->play_vpss_chn = VPSS_CHN3; //暂时不用

    pVpssInfo->digit_vpss.start = pVpssInfo->mix_vpss.end;
    pVpssInfo->digit_vpss.end = ((s32ViCount<<1)+VDEC_CHN_NUM);
    pVpssInfo->vdec_vpss_chn = VPSS_CHN0;
    AV_PRINT("HI3521_SysVpss_Init suc.");
    HI3521_Debug_Vpss();

}

/*****************************************************************************
 函 数 名  : HI_SysConf_Init
 功能描述  : stSysConf
 输入参数  : stSysConf *pstVbConf
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月10日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3521_SysConf_Init(MPP_SYS_CONF_S *pstSysConf)
{
    pstSysConf->u32AlignWidth = 64;

    return;
}

HI_S32 HI3521_Comm_SYS_GetPicSize(VIDEO_NORM_E enNorm, PIC_SIZE_E enPicSize, SIZE_S *pstSize)
{
    switch (enPicSize)
    {
    case PIC_QCIF:
        pstSize->u32Width = D1_WIDTH / 4;
        pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?144:120;
        break;
    case PIC_CIF:
        pstSize->u32Width = D1_WIDTH / 2;
        pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?288:240;
        break;
    case PIC_D1:
        pstSize->u32Width = D1_WIDTH;
        pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?576:480;
        break;
    case PIC_960H:
        pstSize->u32Width = _960H_WIDTH;
        pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?576:480;
        break;
    case PIC_2CIF:
        pstSize->u32Width = D1_WIDTH / 2;
        pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?576:480;
        break;
    case PIC_QVGA:    /* 320 * 240 */
        pstSize->u32Width = 320;
        pstSize->u32Height = 240;
        break;
    case PIC_VGA:     /* 640 * 480 */
        pstSize->u32Width = 640;
        pstSize->u32Height = 480;
        break;
    case PIC_XGA:     /* 1024 * 768 */
        pstSize->u32Width = 1024;
        pstSize->u32Height = 768;
        break;
    case PIC_SXGA:    /* 1400 * 1050 */
        pstSize->u32Width = 1400;
        pstSize->u32Height = 1050;
        break;
    case PIC_UXGA:    /* 1600 * 1200 */
        pstSize->u32Width = 1600;
        pstSize->u32Height = 1200;
        break;
    case PIC_QXGA:    /* 2048 * 1536 */
        pstSize->u32Width = 2048;
        pstSize->u32Height = 1536;
        break;
    case PIC_WVGA:    /* 854 * 480 */
        pstSize->u32Width = 854;
        pstSize->u32Height = 480;
        break;
    case PIC_WSXGA:   /* 1680 * 1050 */
        pstSize->u32Width = 1680;
        pstSize->u32Height = 1050;
        break;
    case PIC_WUXGA:   /* 1920 * 1200 */
        pstSize->u32Width = 1920;
        pstSize->u32Height = 1200;
        break;
    case PIC_WQXGA:   /* 2560 * 1600 */
        pstSize->u32Width = 2560;
        pstSize->u32Height = 1600;
        break;
    case PIC_HD720:   /* 1280 * 720 */
        pstSize->u32Width = 1280;
        pstSize->u32Height = 720;
        break;
    case PIC_HD1080:  /* 1920 * 1080 */
        pstSize->u32Width = 1920;
        pstSize->u32Height = 1080;
        break;
    default:
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}


HI_U32 HI3521_Comm_SYS_CalcPicVbBlkSize(VIDEO_NORM_E enNorm, PIC_SIZE_E enPicSize, PIXEL_FORMAT_E enPixFmt, HI_U32 u32AlignWidth,COMPRESS_MODE_E enCompFmt)
{
    HI_S32 s32Ret      		= HI_FAILURE;
    SIZE_S stSize;
    HI_U32 u32Width 		= 0;
    HI_U32 u32Height 		= 0;
    HI_U32 u32BlkSize 		= 0;
    HI_U32 u32HeaderSize 	= 0;

    s32Ret = HI3521_Comm_SYS_GetPicSize(enNorm, enPicSize, &stSize);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("get picture size[%d] failed!\n", enPicSize);
        return HI_FAILURE;
    }

    if (PIXEL_FORMAT_YUV_SEMIPLANAR_422 != enPixFmt && PIXEL_FORMAT_YUV_SEMIPLANAR_420 != enPixFmt)
    {
        AV_PRINT("pixel format[%d] input failed!\n", enPixFmt);
        return HI_FAILURE;
    }

    if (16!=u32AlignWidth && 32!=u32AlignWidth && 64!=u32AlignWidth)
    {
        AV_PRINT("system align width[%d] input failed!\n",\
                 u32AlignWidth);
        return HI_FAILURE;
    }
    if (D1_WIDTH== stSize.u32Width)
    {
        stSize.u32Width = 720;
    }
    else if (_960H_WIDTH == stSize.u32Width)
    {
        stSize.u32Width = 960;
    }
    //SAMPLE_PRT("w:%d, u32AlignWidth:%d\n", CEILING_2_POWER(stSize.u32Width,u32AlignWidth), u32AlignWidth);

    u32Width  = CEILING_2_POWER(stSize.u32Width, u32AlignWidth);
    u32Height = CEILING_2_POWER(stSize.u32Height,u32AlignWidth);

    if (PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixFmt)
    {
        u32BlkSize = u32Width * u32Height * 2;
    }
    else
    {
        u32BlkSize = u32Width * u32Height * 3 / 2;
    }


    if(COMPRESS_MODE_SEG == enCompFmt)
    {
        VB_PIC_HEADER_SIZE(u32Width,u32Height,enPixFmt,u32HeaderSize);
    }

    u32BlkSize += u32HeaderSize;

    return u32BlkSize;
}

/*****************************************************************************
 函 数 名  : HI3521_VbConf_Init
 功能描述  : VB_CONF_S 配置
 输入参数  : VB_CONF_S *pstVbConf
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年6月6日
    作    者   : duzhanhui
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3521_VbConf_Init(VB_CONF_S *pstVbConf)
{
    VB_CONF_S stVbConf;
    HI_U32 u32BlkSize = 0;
    VIDEO_NORM_E enNorm = VIDEO_ENCODING_MODE_PAL;

    memset(&stVbConf,0,sizeof(VB_CONF_S));

    u32BlkSize = HI3521_Comm_SYS_CalcPicVbBlkSize(enNorm,\
                 PIC_HD720, SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH,COMPRESS_MODE_SEG);
    stVbConf.u32MaxPoolCnt = 128;

    stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt = 64;
    memset(stVbConf.astCommPool[0].acMmzName,0,
           sizeof(stVbConf.astCommPool[0].acMmzName));


    u32BlkSize = HI3521_Comm_SYS_CalcPicVbBlkSize(enNorm,\
                 PIC_960H, SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH,COMPRESS_MODE_SEG);

    stVbConf.astCommPool[1].u32BlkSize = u32BlkSize;
    stVbConf.astCommPool[1].u32BlkCnt = 64;
    memset(stVbConf.astCommPool[1].acMmzName,0,
           sizeof(stVbConf.astCommPool[1].acMmzName));


    u32BlkSize = HI3521_Comm_SYS_CalcPicVbBlkSize(enNorm,\
                 PIC_CIF, SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH,COMPRESS_MODE_SEG);

    stVbConf.astCommPool[2].u32BlkSize = u32BlkSize;
    stVbConf.astCommPool[2].u32BlkCnt = 35;
    memset(stVbConf.astCommPool[2].acMmzName,0,
           sizeof(stVbConf.astCommPool[2].acMmzName));


    memcpy(pstVbConf, &stVbConf, sizeof(VB_CONF_S));

    return;
}

/*****************************************************************************
 函 数 名  : HI3521_SysCtrl_Init
 功能描述  : 系统控制的初始化
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月8日
    作    者   : duzhanhui
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_SysCtrl_Init(HI_VOID)
{

    MPP_SYS_CONF_S stSysConf = {0};
    HI_S32 s32Ret = HI_FAILURE;
    HI_S32 i = 0;
    HI_U32 u32BlkSize = 0;
    HI_U32 u32ViChnCnt = 8;
    VIDEO_NORM_E enNorm = VIDEO_ENCODING_MODE_PAL;

    VB_CONF_S stVbConf;

    AV_PRINT( "HI3521_SysCtrl_Init enter!");

    memset(&stVbConf,0,sizeof(VB_CONF_S));

    HI_MPI_SYS_Exit();

    for(i=0; i<VB_MAX_USER; i++)
    {
        HI_MPI_VB_ExitModCommPool((VB_UID_E)i);
    }
    for(i=0; i<VB_MAX_POOLS; i++)
    {
        HI_MPI_VB_DestroyPool((VB_UID_E)i);
    }
    HI_MPI_VB_Exit();

    HI3521_VbConf_Init(&stVbConf);

    s32Ret = HI_MPI_VB_SetConf(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("HI_MPI_VB_SetConf failed!\n");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VB_Init();
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("HI_MPI_VB_Init failed! s32Ret = 0x%x\n", s32Ret);
        return HI_FAILURE;
    }

    stSysConf.u32AlignWidth = SAMPLE_SYS_ALIGN_WIDTH;
    s32Ret = HI_MPI_SYS_SetConf(&stSysConf);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("HI_MPI_SYS_SetConf failed\n");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_SYS_Init();
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("HI_MPI_SYS_Init failed!\n");
        return HI_FAILURE;
    }

    AV_PRINT("HI_SysCtrl_Init suc");
    return HI_SUCCESS;
}










/*****************************************************************************
 函 数 名  : HI3521_Sys_Init
 功能描述  : app启动前的一个准备工作，这里保留部分代码
 输入参数  : HI_S32
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月7日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_Sys_Init(HI_VOID)
{
    static HI_S32 s32SysInit = HI_DISABLE;
    HI_S32 s32Ret = HI_SUCCESS;
    AV_PRINT("HI3521_Sys_Init enter.");

    /*
    zlog_init("/mnt/flash/app.conf");
    g_pZlog = zlog_get_category("app_cat");
    if (NULL == g_pZlog)
    {
    	AV_PRINT("zlog_init error\n");
    }
    */

    HI3521_SysVpss_Init();

    s32Ret = HI3521_SplitDev_Init();
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( "HI3521_SplitDev_Init failed!");
        return s32Ret;
    }

    if (HI_DISABLE != s32SysInit)
    {
        AV_PRINT( "HI3521 sys has been initted before!");
        return s32Ret;
    }

    s32Ret = HI3521_SysCtrl_Init();
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( "HI_SysCtrl_Init failed!");
        return s32Ret;
    }

    s32Ret = HI3521_Comm_VoPubConfig(SAMPLE_VO_DEV_DHD0, 0);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( "HI3521_Comm_VoPubConfig SAMPLE_VO_DEV_DHD0 failed!");
        return s32Ret;
    }

    s32Ret = HI3521_Comm_VoPubConfig(SAMPLE_VO_DEV_DSD0, 0);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( "HI3521_Comm_VoPubConfig SAMPLE_VO_DEV_DSD0 failed!");
        return s32Ret;
    }
    if(HI3521_GetTw2968VideoType() ==  1 )
        s32Ret = HI3521_Comm_VpssConfig(0,8,VIDEO_ENCODING_MODE_PAL,SAMPLE_VI_MODE_16_960H);
    else
        s32Ret = HI3521_Comm_VpssConfig(0,8,VIDEO_ENCODING_MODE_PAL,SAMPLE_VI_MODE_16_D1);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("HI3521_Comm_VpssConfig failed!");
    }

    s32Ret = HI3521_Comm_VpssConfig(8,4,VIDEO_ENCODING_MODE_PAL,SAMPLE_VI_MODE_8_720P);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("HI3521_Comm_VpssConfig failed!");
    }
    s32Ret = HI3521_Comm_VpssConfig(12,4,VIDEO_ENCODING_MODE_PAL,SAMPLE_VI_MODE_8_720P);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("HI3521_Comm_VpssConfig failed!");
        return HI_FAILURE;
    }
#ifndef HD_VDEC
    HI3521_Comm_VO_StartChn(SAMPLE_VO_LAYER_VSD0,VO_MODE_1MUX);
#endif
    s32SysInit = HI_ENABLE;
    AV_PRINT("HI3521_Sys_Init suc.");

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI3521_PthreadMutexLock
 功能描述  : THREAD MUTEX LOCK
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月11日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_PthreadMutexLock(pthread_mutex_t *pMutex)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = pthread_mutex_lock(pMutex);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( "pthread_mutex_lock err[%d, %s], %#x\n", errno, ERR_STR, s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI3521_PthreadMutexUnlock
 功能描述  : THREAD MUTEX LOCK
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月11日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_PthreadMutexUnlock(pthread_mutex_t *pMutex)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = pthread_mutex_unlock(pMutex);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( "pthread_mutex_lock err[%d, %s], %#x\n", errno, ERR_STR, s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}



