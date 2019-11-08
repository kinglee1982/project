/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "av_public_head.h"
#include "av_common_31.h"
#include "av_split.h"
#include "av_sys.h"
#include "av_audio_init.h"
#include "av_video_init.h"
#include "av_vivo.h"
#include "av_mem.h"
#include "av_video_proc.h"
#include "av_osd.h"
#include "av_hddfsm.h"
#include "av_store.h"
#include "28181_AV_API.h"
#include "tinyxml2API.h"
#include "av_socket.h"
HI_S32 g_s32Tw2968VideoType = 0; //0:D1 1:960H
HDC g_ChnDataCenter[TOTAL_VENC_CHN_NUM+1] = {NULL};   //12个通道(主次码流)+1个MIC通道
static HI_BOOL vencrun = HI_TRUE;
static HI_S32 g_profile = HP;
static HI_S32 g_MaxVencFd = 0;
extern  char AencAichnMap[HI_AENC_MAX_CHN];
extern  char AencDevMap[HI_AENC_MAX_CHN];
extern HI_U16 Cam_Snap_Chn;
HI_U64 g_SetPts[TOTAL_VENC_CHN_NUM] = {0};
HI_BOOL bFindFirstIFrame[TOTAL_VENC_CHN_NUM] = {HI_FALSE};
time_t tGetVencStartTime[TOTAL_VENC_CHN_NUM] = {0};
HI_U64 g_LastFramePts[TOTAL_VENC_CHN_NUM] = {0};
#ifdef JHY_SDK
HI_U8 g_u8JHY_buffer[4][704*576] = {0};
HI_S32 g_s32ViLogicChn[4] = {5,6,7,8};
HI_S32 g_s32ViJHYChn[4] = {12,13,14,15};
APCCOUNTER g_stCounter;
SCH_KLSTN_DATA_T g_stSchStnInfo;
SCH_KLSTN_DATA_T g_stDoorStnInfo;
HI_BOOL g_bDoortTrigger = HI_FALSE;
#endif
HI_U64 g_SysPtsBase = 0;
static HI_U8 g_u8Company = 0;
static HI_U8 g_u8SubCompany = 0;
void HI3521_SetCompany(HI_U8 u8Company,HI_U8 u8SubCompany)
{
    g_u8Company = u8Company;
    g_u8SubCompany = u8SubCompany;
}
void HI3521_InitSysPtsBase()
{
    HI_U64 pu64CurPts = 0;
    HI_S32 s32Ret = HI_MPI_SYS_GetCurPts(&pu64CurPts);
    if(s32Ret!=HI_SUCCESS)
    {
        AV_PRINT("HI_MPI_SYS_GetCurPts error(%x)\n",s32Ret);
    }
    else
    {
        struct timeval t_now;
        gettimeofday(&t_now,NULL);
        g_SysPtsBase = t_now.tv_sec;
        g_SysPtsBase = g_SysPtsBase*1000*1000+t_now.tv_usec - pu64CurPts;
        AV_PRINT("System PTS:%ld.%06ld MPP System PTS:%lld PTS:%lld"
                 ,t_now.tv_sec,t_now.tv_usec,pu64CurPts,g_SysPtsBase);
    }
}

HI_U64 HI3521_ConvertPts(HI_U64 hiPts)
{
    return (g_SysPtsBase+hiPts);
}
/*****************************************************************************
 函 数 名  : APP_InitHiFb
 功能描述  : 初始化FB
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月9日
    作    者   : zhangjinlei
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_InitHiFb(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32Fd = 0;
    struct fb_var_screeninfo stVarInfo;
    struct fb_fix_screeninfo fix;            /* 叠加层物理信息 */
    HI_CHAR file[12] = "/dev/fb0";
    HI_BOOL bShow;
    HIFB_POINT_S stPoint = {0, 0};
    static struct fb_bitfield g_r32 = {16,8, 0};
    static struct fb_bitfield g_g32 = {8, 8, 0};
    static struct fb_bitfield g_b32 = {0, 8, 0};
    static struct fb_bitfield g_a32 = {24, 8, 0};
    FB_MARK_T *pFbMark = NULL;
    HIFB_ALPHA_S stAlpha;
    HIFB_COLORKEY_S stColorKey;
    APP_GetFbMark(&pFbMark);

    /* 1. open framebuffer device overlay 0 */
    if (pFbMark->s32Fd > 0)
    {
        close(pFbMark->s32Fd);
        pFbMark->s32Fd = 0;
    }

    if (NULL != pFbMark->pcMemAddr)
    {
        munmap(pFbMark->pcMemAddr, pFbMark->u32MemLen);
        pFbMark->pcMemAddr = NULL;
    }
    /* 1. open framebuffer device overlay 0 */
    s32Fd = open(file, O_RDWR, 0);
    if(s32Fd< 0)
    {
        AV_PRINT("open %s failed!\n",file);
        return -1;
    }
    bShow = HI_FALSE;
    if (ioctl(s32Fd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
    {
        AV_PRINT("FBIOPUT_SHOW_HIFB failed!\n");
        return HI_NULL;
    }

    /* 2. set the screen original position */

    stPoint.s32XPos = 0;
    stPoint.s32YPos = 0;


    if (ioctl(s32Fd , FBIOPUT_SCREEN_ORIGIN_HIFB, &stPoint) < 0)
    {
        AV_PRINT("set screen original show position failed!\n");
        close(s32Fd );
        return -1;
    }
    s32Ret = ioctl (s32Fd , FBIOGET_VSCREENINFO, &stVarInfo);
    if (s32Ret < 0)
    {
        AV_PRINT("FBIOGET_VSCREENINFO failed!\n");
        close(s32Fd );
        return -1;
    }
#if 0//分割线显示正常
    stVarInfo.red = stR16;
    stVarInfo.green = stG16;
    stVarInfo.blue = stB16;
    stVarInfo.transp = stA16;
    stVarInfo.bits_per_pixel = 16;
#else
    stVarInfo.red = g_r32;
    stVarInfo.green = g_g32;
    stVarInfo.blue = g_b32;
    stVarInfo.transp = g_a32;
    stVarInfo.bits_per_pixel = 32;
#endif
#ifndef RUIMINGPING
    stVarInfo.xres=800;//720;
    stVarInfo.yres=600;//576;
    stVarInfo.activate=FB_ACTIVATE_NOW;
    stVarInfo.xres_virtual=800;//720;
    stVarInfo.yres_virtual = 600;//576;
    stVarInfo.xoffset=0;
    stVarInfo.yoffset=0;
#else
    stVarInfo.xres=800;//720;
    stVarInfo.yres=480;//576;
    stVarInfo.activate=FB_ACTIVATE_NOW;
    stVarInfo.xres_virtual=800;//720;
    stVarInfo.yres_virtual = 480;//576;
    stVarInfo.xoffset=0;
    stVarInfo.yoffset=0;
#endif

    s32Ret = ioctl (s32Fd , FBIOPUT_VSCREENINFO, &stVarInfo);
    if (s32Ret < 0)
    {
        AV_PRINT("FBIOPUT_VSCREENINFO failed! 0x%x\n",s32Ret);
        close(s32Fd );
        return -1;
    }
    bShow = HI_TRUE;
    ioctl(s32Fd , FBIOPUT_SHOW_HIFB, &bShow);
    /* 7. get the fix screen info */
    if (ioctl(s32Fd, FBIOGET_FSCREENINFO, &fix) < 0)
    {
        AV_PRINT("ioctl Get fix screen info err(%s)", ERR_STR);
        close(s32Fd);
        return HI_FAILURE;
    }
    if (ioctl(s32Fd, FBIOGET_ALPHA_HIFB,  &stAlpha))
    {
        AV_PRINT("Get alpha failed!\n");
        close(s32Fd);
        return -1;
    }

    stColorKey.bKeyEnable = HI_TRUE;
    stColorKey.u32Key = 0x20202;
    if (ioctl(s32Fd, FBIOPUT_COLORKEY_HIFB, &stColorKey) < 0)
    {
        AV_PRINT("FBIOPUT_COLORKEY_HIFB!\n");
        close(s32Fd);
        return HI_NULL;
    }
#if 1
    stAlpha.bAlphaEnable = HI_TRUE;
    stAlpha.bAlphaChannel = HI_TRUE;
    stAlpha.u8Alpha0 = 0x0;
    stAlpha.u8Alpha1 = 0x0;
    stAlpha.u8GlobalAlpha = 0xff;//透明度-->值越小越透明
    if (ioctl(s32Fd, FBIOPUT_ALPHA_HIFB,  &stAlpha) < 0)
    {
        AV_PRINT("Set alpha failed!\n");
        close(s32Fd);
        return -1;
    }
#endif
//	close(s32Fd);
    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : APP_InitHiFb
 功能描述  : 初始化FB
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月9日
    作    者   : zhangjinlei
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_InitHiFb1(HI_VOID)
{
    //HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32Fd = 0;
    HIFB_POINT_S stPoint = {0, 0};           /* 叠加层在屏幕上其实坐标 */
    HIFB_ALPHA_S stAlpha;                    /* 屏幕叠加属性 */
    struct fb_var_screeninfo stVar;          /* 虚拟叠加层信息 */
    struct fb_fix_screeninfo fix;            /* 叠加层物理信息 */
    HI_U32 u32ScreenStride = 0;      		 /* 每行字节数 */
    HI_U8 *pShowScreen;
    //HI_BOOL bCompress = HI_TRUE;
    HI_BOOL bShow;
    struct fb_bitfield stR16 = {10, 5, 0};
    struct fb_bitfield stG16 = {5, 5, 0};
    struct fb_bitfield stB16 = {0, 5, 0};
    struct fb_bitfield stA16 = {15, 1, 0};
    FB_MARK_T *pFbMark = NULL;
    APP_GetFbMark(&pFbMark);

    /* 1. open framebuffer device overlay 0 */
    if (pFbMark->s32Fd > 0)
    {
        close(pFbMark->s32Fd);
        pFbMark->s32Fd = 0;
    }

    if (NULL != pFbMark->pcMemAddr)
    {
        munmap(pFbMark->pcMemAddr, pFbMark->u32MemLen);
        pFbMark->pcMemAddr = NULL;
    }

    s32Fd = open(APP_HIFB_DEVICE, O_RDWR, 0);
    if (s32Fd == -1)
    {
        AV_PRINT("open fb device(%s) err(%s)", APP_HIFB_DEVICE, ERR_STR);
        return HI_FAILURE;
    }
    bShow = HI_FALSE;
    if (ioctl(s32Fd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
    {
        AV_PRINT("FBIOPUT_SHOW_HIFB failed!\n");
        return HI_NULL;
    }
    /* 2. set the screen original position */
    if (ioctl(s32Fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &stPoint) < 0)
    {
        AV_PRINT("ioctl screen original show position err(%s)", ERR_STR);
        close(s32Fd);
        return HI_FAILURE;
    }

    /* 3.set alpha */
    stAlpha.bAlphaEnable = HI_TRUE;
    stAlpha.bAlphaChannel = HI_FALSE;
    stAlpha.u8Alpha0 = 0x0;
    stAlpha.u8Alpha1 = 0x80;
    stAlpha.u8GlobalAlpha = 0x80;
    if (ioctl(s32Fd, FBIOPUT_ALPHA_HIFB,  &stAlpha) < 0)
    {
        AV_PRINT("ioctl Set alpha err(%s)", ERR_STR);
        close(s32Fd);
        return HI_FAILURE;
    }

    /* 4. get the variable screen info */
    if (ioctl(s32Fd, FBIOGET_VSCREENINFO, &stVar) < 0)
    {
        AV_PRINT("ioctl Get variable screen info err(%s)", ERR_STR);
        close(s32Fd);
        return HI_FAILURE;
    }

    /* 5. modify the variable screen info
          the screen size: IMAGE_WIDTH*IMAGE_HEIGHT
          the virtual screen size: VIR_SCREEN_WIDTH*VIR_SCREEN_HEIGHT
          (which equals to VIR_SCREEN_WIDTH*(IMAGE_HEIGHT*2))
          the pixel format: ARGB1555
    */
    stVar.xres_virtual = 1280;
    stVar.yres_virtual = 720;
    stVar.xres = 1280;
    stVar.yres = 720;
    stVar.transp= stA16;
    stVar.red = stR16;
    stVar.green = stG16;
    stVar.blue = stB16;
    stVar.bits_per_pixel = 16;
    stVar.activate = FB_ACTIVATE_NOW;

    //stVar.grayscale = 0;   /* 灰度 */
    /* 6. set the variable screeninfo */
    if (ioctl(s32Fd, FBIOPUT_VSCREENINFO, &stVar) < 0)
    {
        AV_PRINT("ioctl Put variable screen info err(%s)", ERR_STR);
        close(s32Fd);
        return HI_FAILURE;
    }
    bShow = HI_TRUE;
    ioctl(s32Fd, FBIOPUT_SHOW_HIFB, &bShow);
    /* 7. get the fix screen info */
    if (ioctl(s32Fd, FBIOGET_FSCREENINFO, &fix) < 0)
    {
        AV_PRINT("ioctl Get fix screen info err(%s)", ERR_STR);
        close(s32Fd);
        return HI_FAILURE;
    }

    u32ScreenStride = fix.line_length;	/*fix screen stride*/
    pFbMark->u32MemLen = fix.smem_len;
    /* 8. map the physical video memory for user use */
    pShowScreen = (HI_U8 *)mmap(NULL, fix.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, s32Fd, 0);
    if(MAP_FAILED == pShowScreen)
    {
        AV_PRINT("ioctl mmap framebuffer err(%s)", ERR_STR);
        close(s32Fd);
        return HI_FAILURE;
    }

    memset(pShowScreen, 0, u32ScreenStride*stVar.yres);
    pFbMark->pcMemAddr = (HI_CHAR *)pShowScreen;
    pFbMark->s32Fd = s32Fd;

    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : HI3521_ViInit
 功能描述  : HI3521 VI初始化
 输入参数  : HI_VOID

 输出参数  : HI_VOID
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月11日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_ViInit(HI_VOID)
{

    HI3521_Comm_ViInit(SAMPLE_VI_MODE_8_720P,VIDEO_ENCODING_MODE_PAL);
    if(HI3521_GetTw2968VideoType() ==  1 )
        HI3521_Comm_ViInit(SAMPLE_VI_MODE_16_960H,VIDEO_ENCODING_MODE_PAL);
    else
        HI3521_Comm_ViInit(SAMPLE_VI_MODE_16_D1,VIDEO_ENCODING_MODE_PAL);
    HI3521_Open_HdViDev();
    HI3521_Open_SdViDev();
    HI3521_VideoInParaInit();
    return HI_SUCCESS;
}



/*****************************************************************************
 函 数 名  : HI3521_ViInit
 功能描述  : HI3521 VI初始化
 输入参数  : HI_VOID

 输出参数  : HI_VOID
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月11日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_ViExit(HI_VOID)
{
    HI3521_Comm_ViStop(SAMPLE_VI_MODE_8_720P);
    if(HI3521_GetTw2968VideoType() ==  1 )
        HI3521_Comm_ViStop(SAMPLE_VI_MODE_16_960H);
    else
        HI3521_Comm_ViStop(SAMPLE_VI_MODE_16_D1);
    return HI_SUCCESS;
}

HI_S32 HI3531_StopVoChn(VO_LAYER VoLayer)
{
    HI_S32 s32VoChn = 0;
    HI_S32 s32MaxVoChn = 12;
    HI_S32 s32Ret = HI_SUCCESS;

    for (s32VoChn = 0; s32VoChn < s32MaxVoChn; s32VoChn++)
    {
        s32Ret = HI_MPI_VO_DisableChn(VoLayer, s32VoChn);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT("HI_MPI_VO_DisableChn (%d)failed with %#x!\n", VoLayer, s32Ret);
            return HI_FAILURE;
        }
    }



    s32Ret = HI_MPI_VO_DisableVideoLayer(VoLayer);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("HI_MPI_VO_DisableVideoLayer VoDev (%d) failed with %#x!\n", VoLayer, s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}
HI_S32 HI3531_StopDev(VO_DEV VoDev)
{
    HI_S32 s32Ret = HI_SUCCESS;
    s32Ret = HI_MPI_VO_Disable(VoDev);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("HI_MPI_VO_Disable VoDev (%d) failed with %#x!\n", VoDev, s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}


HI_S32 HI3521_StopAllVo(HI_VOID)
{
    HI3521_Comm_WBC_End();
    HI3531_StopVoChn(SAMPLE_VO_LAYER_VSD0);
    HI3531_StopDev(SAMPLE_VO_DEV_DSD0);

    HI3531_StopVoChn(SAMPLE_VO_LAYER_VHD0);
    HI3531_StopDev(SAMPLE_VO_DEV_DHD0);
    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : HI3521_GetSysProfile
 功能描述  : 获取编码Profile
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  : HI_S32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月12日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_GetSysProfile(HI_VOID)
{
    return g_profile;
}

/*****************************************************************************
 函 数 名  : HI3521_H264ParaInit
 功能描述  :
 输入参数  : H264_Param *pstH264Param, HI_S32 s32Chn, HI_S32 s32Vstd, HI_S32 s32StreamType
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
HI_VOID HI3521_H264ParaInit(H264_Param *pstH264Param, HI_S32 s32Chn, HI_S32 s32Vstd, HI_S32 s32StreamType)
{
    pstH264Param->vstd = s32Vstd;
    if (s32StreamType == CAPTURE_VENC_SECOND)
    {
        pstH264Param->size = HI3521_GetCapVideoSize(s32Chn, CAPTURE_VENC_SECOND);
        pstH264Param->bitrate = (pstH264Param->size == PIC_HD720)?512:256;
        pstH264Param->rcmode = VENC_RC_MODE_H264VBR; /*定码率 4M，变码率 12M*/
    }
    else
    {
        pstH264Param->size = HI3521_GetCapVideoSize(s32Chn, CAPTURE_VENC_MAIN);
        pstH264Param->bitrate =  (pstH264Param->size == PIC_HD720)?1024 * 2:1024;/*定码率 4M，变码率 12M*/
        pstH264Param->rcmode = VENC_RC_MODE_H264CBR; /*定码率 4M，变码率 12M*/
    }

    pstH264Param->type = PT_H264;
    pstH264Param->frmrate = (s32Vstd == VIDEO_ENCODING_MODE_PAL ? FRAME_RATE_DEFAULT : 30);
    pstH264Param->fluctuatelevel = 0;
    pstH264Param->mainstream = HI_TRUE;
    pstH264Param->gop = (s32Vstd == VIDEO_ENCODING_MODE_PAL ? 25 : 30);
    pstH264Param->vifield = HI_FALSE;

    return;
}

/*****************************************************************************
 函 数 名  : HI3521_APP_SetVencH264Param
 功能描述  : 设置编码h264的一些参数
 输入参数  : VENC_CHN_ATTR_S *pVencChnAttr
             H264_Param *pH264Param
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月5日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3521_VencSetH264Para(VENC_CHN_ATTR_S *pVencChnAttr, H264_Param *pH264Param)
{
    VENC_ATTR_H264_S stH264Attr;
    SIZE_S stSize;

    stH264Attr.bByFrame = HI_TRUE;
    HI3521_Comm_SYS_GetPicSize((VIDEO_NORM_E)pH264Param->vstd, pH264Param->size, &stSize);
    stH264Attr.u32MaxPicWidth = stSize.u32Width;
    stH264Attr.u32MaxPicHeight = stSize.u32Height;
    stH264Attr.u32PicWidth = stSize.u32Width;
    stH264Attr.u32PicHeight = stSize.u32Height;
    stH264Attr.u32BufSize = stH264Attr.u32PicWidth * stH264Attr.u32PicHeight * 2;
    stH264Attr.u32Profile = HI3521_GetSysProfile();

    pVencChnAttr->stVeAttr.enType = pH264Param->type;
    memcpy(&(pVencChnAttr->stVeAttr.stAttrH264e), &stH264Attr, sizeof(VENC_ATTR_H264_S));

    pVencChnAttr->stRcAttr.enRcMode = pH264Param->rcmode;
    if(pH264Param->rcmode == VENC_RC_MODE_H264CBR)
    {
        VENC_ATTR_H264_CBR_S stH264Cbr;

        stH264Cbr.u32BitRate = pH264Param->bitrate;
        stH264Cbr.u32StatTime = 1;
        stH264Cbr.u32SrcFrmRate = (pH264Param->vstd == VIDEO_ENCODING_MODE_PAL ? 25 : 30);
        stH264Cbr.fr32DstFrmRate = pH264Param->frmrate;
        stH264Cbr.u32Gop = pH264Param->gop;
        stH264Cbr.u32FluctuateLevel = pH264Param->fluctuatelevel;

        memcpy(&(pVencChnAttr->stRcAttr.stAttrH264Cbr), &stH264Cbr, sizeof(VENC_ATTR_H264_CBR_S));

#if 0
        AV_PRINT( "%d x %d, %d Fps profile:%d\n",
                  stH264Attr.u32PicWidth,
                  stH264Attr.u32PicHeight,
                  stH264Cbr.fr32DstFrmRate,
                  stH264Attr.u32Profile);
        AV_PRINT( "enRcMode = %d; MaxBitRate = %d; Gop = %d; u32PicLevel = %d; VencMode = %d\n",
                  pVencChnAttr->stRcAttr.enRcMode,
                  stH264Cbr.u32BitRate,
                  stH264Cbr.u32Gop,
                  stH264Cbr.u32FluctuateLevel,
                  pVencChnAttr->stVeAttr.enType);
        HI3531_DebugVencH264Para(pVencChnAttr);

#endif
    }
    else if(pH264Param->rcmode == VENC_RC_MODE_H264VBR)
    {
        VENC_ATTR_H264_VBR_S stH264Vbr;

        stH264Vbr.u32Gop = pH264Param->gop;
        stH264Vbr.u32StatTime = 1;
        stH264Vbr.u32SrcFrmRate = (pH264Param->vstd == VIDEO_ENCODING_MODE_PAL ? 25 : 30);
        stH264Vbr.fr32DstFrmRate = pH264Param->frmrate;
        //stH264Vbr.u32MinQp = pH264Param->minQp;
        //stH264Vbr.u32MaxQp = pH264Param->maxQp;
        stH264Vbr.u32MinQp = 28;
        stH264Vbr.u32MaxQp = 45;
        stH264Vbr.u32MaxBitRate = pH264Param->bitrate;
        memcpy(&pVencChnAttr->stRcAttr.stAttrH264Vbr, &stH264Vbr, sizeof(VENC_ATTR_H264_VBR_S));
#if 0
        AV_PRINT("%d x %d, %d Fps profile:%d minQp %d maxQp %d\n",
                 stH264Attr.u32PicWidth,
                 stH264Attr.u32PicHeight,
                 stH264Vbr.fr32DstFrmRate,
                 stH264Attr.u32Profile,
                 stH264Vbr.u32MinQp,
                 stH264Vbr.u32MaxQp);

        AV_PRINT("enRcMode = %d; MaxBitRate = %d; Gop = %d; VencMode = %d\n",
                 pVencChnAttr->stRcAttr.enRcMode,
                 stH264Vbr.u32MaxBitRate,
                 stH264Vbr.u32Gop,
                 pVencChnAttr->stVeAttr.enType);
#endif

    }


    return;
}

/*****************************************************************************
 函 数 名  : HI3521_VencStartChn
 功能描述  :
 输入参数  : VENC_CHN veChn, VENC_GRP veGroup, VENC_CHN_ATTR_S *pVencChnAttr, HI_S32 createGroup
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年6月6日
    作    者   : duzhanhui
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_VencStartChn(VENC_CHN veChn,VENC_CHN_ATTR_S *pVencChnAttr, HI_S32 createGroup)
{
    HI_S32 s32Ret = 0;

    s32Ret = HI_MPI_VENC_CreateChn(veChn, pVencChnAttr);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT( "HI_MPI_VENC_CreateChn err 0x%x\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VENC_SetMaxStreamCnt(veChn, 30);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("HI_MPI_VENC_SetMaxStreamCnt(%d) err 0x%x\n", veChn, s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI3521_VencStreamSetMaxFd
 功能描述  :
 输入参数  : HI_S32 s32Fd
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
HI_VOID HI3521_VencStreamSetMaxFd(HI_S32 s32Fd)
{
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();

    FD_SET(s32Fd, &(pCaptureDev->read_fds));
    if (s32Fd > g_MaxVencFd)
    {
        g_MaxVencFd = s32Fd;
    }

    return;
}

/*****************************************************************************
 函 数 名  : HI3521_VencStreamGetMaxFd
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
HI_S32 HI3521_VencStreamGetMaxFd(HI_VOID)
{
    return g_MaxVencFd + 1;
}

/*****************************************************************************
 函 数 名  : HI3521_GetVencStream
 功能描述  :
 输入参数  : HI_S32 s32Chn, HI_S32 s32StreamType
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
HI_S32 HI3521_StreamVencInit(HI_S32 s32Chn, HI_S32 s32StreamType)
{
    HI_S32 s32VpssChn = 0;
    HI_S32 s32Ret = 0;
    HI_S32 s32Vstd = 0;
    HI_S32 s32VencFd = 0;
    VENC_CHN VeChn = s32Chn * 2 + s32StreamType;
    VPSS_GRP VeVpss = CAPTURE_VPSS(s32Chn);
    VENC_CHN_ATTR_S stVencChnAttr;
    H264_Param stH264Param;
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();
    s32Vstd = pCaptureDev->vstd;
    HI3521_H264ParaInit(&stH264Param, s32Chn, s32Vstd, s32StreamType);

    HI3521_VencSetH264Para(&stVencChnAttr, &stH264Param);

    s32Ret = HI3521_VencStartChn(VeChn,&stVencChnAttr, HI_ENABLE);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("HI3521_VencStartChn err 0x%x\n",s32Ret);
        return HI_FAILURE;
    }

    s32VpssChn = s32StreamType ? CAPTURE_SECOND_VPSS_CHN() : CAPTURE_MAIN_VPSS_CHN();
    s32Ret = HI3521_Comm_VENC_BindVpss(VeChn, VeVpss, s32VpssChn);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("HI3531_Comm_VencBindVpss err 0x%x\n",s32Ret);
        return HI_FAILURE;
    }

    s32VencFd = HI_MPI_VENC_GetFd(VeChn);
    if (s32VencFd == HI_ERR_VENC_INVALID_CHNID || s32VencFd == HI_ERR_VENC_SYS_NOTREADY)
    {
        pCaptureDev->vi_chn[s32Chn].venc_chn[s32StreamType].venc_fd = -1;
        AV_PRINT("HI_MPI_VENC_GetFd err 0x%x\n",s32Ret);
        return HI_FAILURE;
    }

    pCaptureDev->vi_chn[s32Chn].venc_chn[s32StreamType].venc_fd = s32VencFd;
    HI3521_VencStreamSetMaxFd(s32VencFd);
    pCaptureDev->vi_chn[s32Chn].venc_chn[s32StreamType].frame_rate = stVencChnAttr.stRcAttr.stAttrH264Cbr.fr32DstFrmRate;
    pCaptureDev->vi_chn[s32Chn].venc_chn[s32StreamType].pic_width = stVencChnAttr.stVeAttr.stAttrH264e.u32PicWidth;
    pCaptureDev->vi_chn[s32Chn].venc_chn[s32StreamType].pic_height = stVencChnAttr.stVeAttr.stAttrH264e.u32PicHeight;
    pCaptureDev->vi_chn[s32Chn].venc_chn[s32StreamType].created = HI_ENABLE;

    pCaptureDev->vi_chn[s32Chn].venc_chn[s32StreamType].info.enc_chn = VeChn;
    //pCaptureDev->vi_chn[s32Chn].venc_chn[s32StreamType].info.channel = s32Chn + (s32Chn / 4) * 4;
    pCaptureDev->vi_chn[s32Chn].venc_chn[s32StreamType].info.channel = s32Chn;
    pCaptureDev->vi_chn[s32Chn].venc_chn[s32StreamType].info.dwtype = s32StreamType;
    pCaptureDev->vi_chn[s32Chn].venc_chn[s32StreamType].info.pic_width = stVencChnAttr.stVeAttr.stAttrH264e.u32PicWidth;
    pCaptureDev->vi_chn[s32Chn].venc_chn[s32StreamType].info.pic_height = stVencChnAttr.stVeAttr.stAttrH264e.u32PicHeight;
    pCaptureDev->vi_chn[s32Chn].venc_chn[s32StreamType].info.frame_rate = (s32Vstd == VIDEO_ENCODING_MODE_PAL ? 25 : 30);
    pCaptureDev->vi_chn[s32Chn].venc_chn[s32StreamType].info.image_size = stH264Param.size;
    pCaptureDev->vi_chn[s32Chn].venc_chn[s32StreamType].image_size = stH264Param.size;
    if(s32StreamType == HI_MAIN_STREAM)
        pCaptureDev->vi_chn[s32Chn].jpeg_chn.image_size = stH264Param.size;
    s32Ret = pthread_mutex_init(&(pCaptureDev->vi_chn[s32Chn].venc_chn[s32StreamType].lock), NULL);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( "pthread_mutex_init viChn = %d failed %#X\n", s32Chn, s32Ret);
        return HI_FAILURE;
    }
	//禁用子码流VPSS
    if(s32StreamType == HI_SUB_STREAM)
    {
        VPSS_CHN s32VpssChn = CAPTURE_SECOND_VPSS_CHN();
        VPSS_GRP VeVpss = CAPTURE_VPSS(s32Chn);
		AV_PRINT("VPSS_DisableChn s32VencChn(%d) VpssChn(%d) VpssGrp(%d)", s32Chn, s32VpssChn,VeVpss);
        s32Ret = HI_MPI_VPSS_DisableChn(VeVpss, s32VpssChn);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT("HI_MPI_VPSS_DisableChn s32VencChn(%d) s32Ret(%#X)", s32Chn, s32Ret);
        }
    }
    //HI3531_DebugVencChn(s32Chn, s32StreamType);
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI3521_VencInit
 功能描述  : HI3521编码初始化
 输入参数  : HI_S32 channel
             HI_S32 vstd
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月5日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_VencInit(HI_S32 s32Chn)
{
    HI_S32 s32Ret = HI_SUCCESS;


    s32Ret = HI3521_StreamVencInit(s32Chn, HI_MAIN_STREAM);
    if (HI_SUCCESS != s32Ret)
    {
        return HI_FAILURE;
    }

    s32Ret = HI3521_StreamVencInit(s32Chn, HI_SUB_STREAM);
    if (HI_SUCCESS != s32Ret)
    {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


HI_S32 HI3521_DestroyVencByChn(VO_LAYER VoLayer,HI_S32 s32Chn, HI_S32 s32StreamType)
{
    HI_S32 s32VpssChn = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    VENC_GRP VpssGrp = s32Chn * 2 + s32StreamType;/*mainstream = HI_MAIN_STREAM, substream = HI_SUB_STREAM*/
    VENC_CHN VencChn = s32Chn * 2 + s32StreamType;
    VPSS_GRP VpssChn = CAPTURE_VPSS(s32Chn);

    s32VpssChn = s32StreamType ? CAPTURE_SECOND_VPSS_CHN() : CAPTURE_MAIN_VPSS_CHN();
    s32Ret = HI3521_Comm_VO_UnBindVpss(VoLayer,VpssGrp, VpssChn, s32VpssChn);
    if (HI_SUCCESS != s32Ret)
    {
        return HI_FAILURE;
    }

    s32Ret = HI3521_Comm_VENC_Stop(VencChn);
    if (HI_SUCCESS != s32Ret)
    {
        return HI_FAILURE;
    }

    return HI_SUCCESS;

}


HI_S32 HI3521_VencExit(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32VencChn = 0;
    vencrun = HI_FALSE;
    for (s32VencChn = 0; s32VencChn < HI_VI_MAX_CHN; s32VencChn++)
    {
        s32Ret = HI3521_DestroyVencByChn(SAMPLE_VO_LAYER_VHD0,s32VencChn, HI_MAIN_STREAM);
        if (HI_SUCCESS != s32Ret)
        {
            return HI_FAILURE;
        }

        s32Ret = HI3521_DestroyVencByChn(SAMPLE_VO_LAYER_VHD0,s32VencChn, HI_SUB_STREAM);
        if (HI_SUCCESS != s32Ret)
        {
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : HI3521_AVencInit
 功能描述  : HI3521 AVenc初始化
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
HI_S32 HI3521_AVencInit(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32Chn = 0;
    AV_PRINT("HI3521_AVencInit enter.");
    HI_S32 s32VencCnt = HI_VI_MAX_CHN;

    for (s32Chn = 0; s32Chn < HI_AENC_MAX_CHN; s32Chn++)
    {
        /*结构体初始化*/
        s32Ret = HI3521_CapDevInit(s32Chn);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT( "HI3521_CapDevInit err(0x%x)", s32Ret);
            return HI_FAILURE;
        }

    }


    for (s32Chn = 0; s32Chn < s32VencCnt; s32Chn++)
    {
        /*视频编码初始化*/
        s32Ret = HI3521_VencInit(s32Chn);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT( "HI3521_VencInit err(0x%x)", s32Ret);
            return HI_FAILURE;
        }

    }

#if 1
    for (s32Chn = 0; s32Chn < HI_AENC_MAX_CHN; s32Chn++)
    {
        /*音频编码初始化*/
        s32Ret = HI3521A_AencInit(s32Chn,s32Chn,AencDevMap[s32Chn]);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT( "HI3531_AencInit err(0x%x)", s32Ret);
            return HI_FAILURE;
        }
    }
#endif

    AV_PRINT("HI3531_AVencInit suc.");
    return HI_SUCCESS;
}



HI_S32 HI3521_StopMicEnc(HI_S32 s32Chn)
{
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();

    if (s32Chn >= HI_AENC_MAX_CHN)
    {
        AV_PRINT( "HI_AENC_MAX_CHN(%d) Invalid viChn(%d)", HI_AENC_MAX_CHN, s32Chn);
        return HI_FAILURE;
    }

    /*MIC停止 */
    if (pCaptureDev->vi_chn[s32Chn].aenc_chn.started == HI_ENABLE)
    {

        pCaptureDev->vi_chn[s32Chn].aenc_chn.started = HI_DISABLE;
        AV_PRINT( "HI_VENC_Stop s32Chn %d success", s32Chn);
    }

    return HI_SUCCESS;
}

HI_S32  HI3521_StartSubStreamVenc(VENC_CHN VeChn)
{
    if (VeChn >= TOTAL_VI_CHN_NUM)
    {
        AV_PRINT( "TOTAL_VI_CHN_NUM(%d) Invalid viChn(%d)", TOTAL_VI_CHN_NUM,VeChn);
        return HI_FAILURE;
    }
    HI_S32	  s32Ret = HI_SUCCESS;
    VENC_CHN  s32VencChn = ((VeChn * HI_STREAM_NUMS_PER_CHN) + CHL_2END_T);
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();
    /*启动通道的视频编码任务*/
    if (pCaptureDev->vi_chn[VeChn].venc_chn[CHL_2END_T].enable == HI_ENABLE &&
            pCaptureDev->vi_chn[VeChn].venc_chn[CHL_2END_T].started == HI_DISABLE)
    {
		VPSS_CHN s32VpssChn = CAPTURE_SECOND_VPSS_CHN();
        VPSS_GRP VeVpss = CAPTURE_VPSS(VeChn);
		AV_PRINT("VPSS_EnaleChn s32VencChn(%d) VpssChn(%d) VpssGrp(%d)", s32VencChn, s32VpssChn,VeVpss);
        s32Ret = HI_MPI_VPSS_EnableChn(VeVpss, s32VpssChn);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT( "HI_MPI_VPSS_EnableChn err(0x%x)", s32Ret);
            return HI_FAILURE;
        }
        s32Ret = HI_MPI_VENC_StartRecvPic(s32VencChn);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT( "HI_MPI_VENC_StartRecvPic err(0x%x)", s32Ret);
            return HI_FAILURE;
        }
        pCaptureDev->vi_chn[VeChn].venc_chn[CHL_2END_T].started = HI_ENABLE;
        AV_PRINT( "HI_MPI_VENC_StartRecvPic CHN(%02d) success",VeChn);
    }
    return HI_SUCCESS;
}
HI_S32  HI3521_StopSubStreamVenc(VENC_CHN VeChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32VencChn = 0;

    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();
    if (VeChn >= TOTAL_VI_CHN_NUM)
    {
        AV_PRINT( "TOTAL_VI_CHN_NUM(%d) Invalid viChn(%d)", TOTAL_VI_CHN_NUM,VeChn);
        return HI_FAILURE;
    }

    s32VencChn = ((VeChn * HI_STREAM_NUMS_PER_CHN) + CHL_2END_T);

    if (pCaptureDev->vi_chn[VeChn].venc_chn[CHL_2END_T].started == HI_ENABLE)
    {
		VPSS_CHN s32VpssChn = CAPTURE_SECOND_VPSS_CHN();
        VPSS_GRP VeVpss = CAPTURE_VPSS(VeChn);
		AV_PRINT("VPSS_DisableChn s32VencChn(%d) VpssChn(%d) VpssGrp(%d)", s32VencChn, s32VpssChn,VeVpss);
        s32Ret = HI_MPI_VPSS_DisableChn(VeVpss, s32VpssChn);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT("HI_MPI_VPSS_DisableChn s32VencChn(%d) s32Ret(%#X)", s32VencChn, s32Ret);
        }
        s32Ret = HI_MPI_VENC_StopRecvPic(s32VencChn);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT("HI_VENC_Stop s32VencChn(%d) s32Ret(%#X)", s32VencChn, s32Ret);
        }
        pCaptureDev->vi_chn[VeChn].venc_chn[CHL_2END_T].started = HI_DISABLE;

        AV_PRINT("HI_VENC_Stop CHN(%02d) success", VeChn);

        HI_MPI_VENC_ResetChn(s32VencChn);
    }

    return HI_SUCCESS;
}
/*****************************************************************************
 函 数 名  : HI3521_StartEnc
 功能描述  : HI3521启动编码
 输入参数  : HI_S32 s32Chn
             HI_U32 u32StreamType
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月8日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_StartEnc(HI_S32 s32Chn, HI_U32 u32StreamType)
{
    HI_S32	  s32Ret = HI_SUCCESS;
    VENC_CHN  s32VencChn = 0;
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();

    if (s32Chn >= TOTAL_VI_CHN_NUM || u32StreamType >= CHL_FUNCTION_NUM)
    {
        AV_PRINT( "TOTAL_VI_CHN_NUM(%d) Invalid viChn(%d); u32StreamType(%d)", TOTAL_VI_CHN_NUM, s32Chn, u32StreamType);
        return HI_FAILURE;
    }

    if (!((CHL_MAIN_T == u32StreamType) || (CHL_2END_T == u32StreamType)))
    {
        AV_PRINT( "Invalid channel(%d); u32StreamType(%d)", s32Chn, u32StreamType);
        return HI_FAILURE;
    }

    s32VencChn = ((s32Chn * HI_STREAM_NUMS_PER_CHN) + u32StreamType);
    /*启动通道的视频编码任务*/
    if (pCaptureDev->vi_chn[s32Chn].venc_chn[u32StreamType].enable == HI_ENABLE &&
            pCaptureDev->vi_chn[s32Chn].venc_chn[u32StreamType].started == HI_DISABLE)
    {
        s32Ret = HI_MPI_VENC_StartRecvPic(s32VencChn);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT( "HI_MPI_VENC_StartRecvPic err(0x%x)", s32Ret);
            return HI_FAILURE;
        }
        pCaptureDev->vi_chn[s32Chn].venc_chn[u32StreamType].started = HI_ENABLE;
    }

    /*启动通道的音频编码任务*/
    if (pCaptureDev->vi_chn[s32Chn].aenc_chn.started == HI_DISABLE)
    {
        pCaptureDev->vi_chn[s32Chn].aenc_chn.started = HI_ENABLE;
    }

    return HI_SUCCESS;
}

HI_S32 HI3521_StartMicEnc(HI_S32 s32Chn)
{
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();

    if (s32Chn >= HI_AENC_MAX_CHN)
    {
        AV_PRINT( "HI_AENC_MAX_CHN(%d) Invalid viChn(%d)", HI_AENC_MAX_CHN, s32Chn);
        return HI_FAILURE;
    }

    /*启动通道的音频编码任务*/
    if (pCaptureDev->vi_chn[s32Chn].aenc_chn.started == HI_DISABLE)
    {
        pCaptureDev->vi_chn[s32Chn].aenc_chn.started = HI_ENABLE;
        AV_PRINT("HI_AENC_Start s32Chn(%d) success", s32Chn);
    }

    return HI_SUCCESS;
}



/*****************************************************************************
 函 数 名  : HI3531_StopEnc
 功能描述  : HI3531停止编码
 输入参数  : HI_S32 s32Chn
             HI_U32 u32StreamType
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月8日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_StopEnc(HI_S32 s32Chn, HI_U32 u32StreamType)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32VencChn = 0;

    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();

    //AV_PRINT( "s32Chn = %d; u32StreamType = %d\n", s32Chn, u32StreamType);

    if (s32Chn >= TOTAL_VI_CHN_NUM || u32StreamType >= CHL_FUNCTION_NUM)
    {
        AV_PRINT("TOTAL_VI_CHN_NUM(%d Invalid s32Chn(%d); u32StreamType(%d)", TOTAL_VI_CHN_NUM, s32Chn, u32StreamType);
        return HI_FAILURE;
    }

    s32VencChn = ((s32Chn * HI_STREAM_NUMS_PER_CHN) + u32StreamType);

    if (pCaptureDev->vi_chn[s32Chn].venc_chn[u32StreamType].started == HI_ENABLE)
    {
        s32Ret = HI_MPI_VENC_StopRecvPic(s32VencChn);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT("HI_VENC_Stop s32VencChn(%d) s32Ret(%#X)", s32VencChn, s32Ret);
        }
        pCaptureDev->vi_chn[s32Chn].venc_chn[u32StreamType].started = HI_DISABLE;

        AV_PRINT("HI_VENC_Stop s32Chn(%d) OK", s32Chn);
    }

    // step 2. 启动通道的音频编码任务
    if (pCaptureDev->vi_chn[s32Chn].aenc_chn.started == HI_ENABLE)
    {

        pCaptureDev->vi_chn[s32Chn].aenc_chn.started = HI_DISABLE;
        AV_PRINT( "HI_VENC_Stop s32Chn %d success", s32Chn);
    }

    return HI_SUCCESS;

}


/*****************************************************************************
 函 数 名  : HI3521_StartAVEnc
 功能描述  : HI3521 Start编码
 输入参数  : HI_VOID

 输出参数  : HI_VOID
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月11日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_StartAVEnc(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32ChnCnt = 0;
    HI_S32 s32ChnTotalCnt = HI_VI_MAX_CHN;

    for (s32ChnCnt = 0; s32ChnCnt < s32ChnTotalCnt; s32ChnCnt++)
    {
        /*启动主码流编码功能*/
        s32Ret = HI3521_StartEnc(s32ChnCnt, CHL_MAIN_T);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT( "HI3521_StartEnc mainStream err 0x%x\n %d %d", s32Ret,s32ChnCnt,s32ChnTotalCnt);
            return HI_FAILURE;
        }
#if 0
        /*启动子码流编码功能*/
        s32Ret = HI3521_StartEnc(s32ChnCnt, CHL_2END_T);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT( "HI3521_StartEnc subStream err 0x%x\n", s32Ret);
            return HI_FAILURE;
        }
#endif
    }

    for(s32ChnCnt = HI_VI_MAX_CHN; s32ChnCnt < HI_AENC_MAX_CHN; s32ChnCnt++)
    {
        HI3521_StartMicEnc(s32ChnCnt);
    }

    return HI_SUCCESS;
}



/*****************************************************************************
 函 数 名  : HI3531_StopAVEnc
 功能描述  : HI3531 Stop编码
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
HI_S32 HI3521_StopAVEnc(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32Chn = 0;

    for (s32Chn = 0; s32Chn < HI_VI_MAX_CHN; s32Chn++)
    {
        /*启动主码流编码功能*/
        s32Ret = HI3521_StopEnc(s32Chn, CHL_MAIN_T);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT( "HI3531_StopEnc mainStream err 0x%x\n", s32Ret);
            return HI_FAILURE;
        }

        /*启动子码流编码功能*/
        s32Ret = HI3521_StopEnc(s32Chn, CHL_2END_T);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT( "HI3531_StopEnc subStream err 0x%x\n", s32Ret);
            return HI_FAILURE;
        }
    }

    for (s32Chn = HI_VI_MAX_CHN; s32Chn < HI_AENC_MAX_CHN; s32Chn++)
    {
        HI3521_StopMicEnc(s32Chn);
    }

    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : HI3521_VencStreamFdChk
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
HI_S32 HI3521_VencStreamFdChk(HI_VOID)
{
    HI_S32 s32Cnt = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    struct timeval stTimeOut;
    HI_S32 s32MaxFd = HI3521_VencStreamGetMaxFd();
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();

    stTimeOut.tv_sec = 2;
    stTimeOut.tv_usec = 100000;
    FD_ZERO(&(pCaptureDev->read_fds));
    HI_S32 s32ChnTotalCnt = TOTAL_VENC_CHN_NUM;

    for (s32Cnt = 0; s32Cnt < s32ChnTotalCnt; s32Cnt++)
    {
        if (pCaptureDev->vi_chn[s32Cnt >> 1].venc_chn[s32Cnt & 0x01].venc_fd != -1)
        {
            FD_SET(pCaptureDev->vi_chn[s32Cnt >> 1].venc_chn[s32Cnt & 0x01].venc_fd,
                   &(pCaptureDev->read_fds));
        }
    }

    s32Ret = select(s32MaxFd, &(pCaptureDev->read_fds), NULL, NULL, &stTimeOut);
    if (s32Ret < 0)
    {
        AV_PRINT("select err[%d, %s]\n", errno, ERR_STR);
        return HI_FAILURE;
    }
    else if (0 == s32Ret)
    {
        AV_PRINT("select fd[%d], time out, err[%d, %s]\n", s32MaxFd, errno, ERR_STR);
        return HI_FAILURE;
    }
    else
    {
        //AV_PRINT("select fd is valid.\n");
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI3521_IsVencStreamEnable
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
HI_BOOL HI3521_IsVencStreamEnable(HI_VOID)
{
    HI_S32 s32VencChn = 0;
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();
    HI_S32 s32ChnTotalCnt = TOTAL_VENC_CHN_NUM;

    for (s32VencChn = 0; s32VencChn < s32ChnTotalCnt ; s32VencChn++)
    {
        if (pCaptureDev->vi_chn[s32VencChn >> 1].venc_chn[s32VencChn & 0x01].started == HI_ENABLE)
        {
            break;
        }
    }

    if (s32VencChn == s32ChnTotalCnt)
    {
        //usleep(CHK_VENC_INTERVAL);
        TM_USleep(0,2*CHK_VENC_INTERVAL);
        return HI_FALSE;
    }

    return HI_TRUE;
}

/*****************************************************************************
 函 数 名  : HI3521_DebugVencSaveH264
 功能描述  : test for saving h264 stream
 输入参数  : FILE* fpH264File, VENC_STREAM_S *pstVencStream
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年6月7日
    作    者   : duzhanhui
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_DebugVencSaveH264(FILE* fpH264File, VENC_STREAM_S *pstVencStream)
{
    HI_U32 i = 0;

    if ((fpH264File == NULL) || (pstVencStream == NULL))
    {
        AV_PRINT("HI3521_VencSaveH264 paras is invalid.\n");
        return HI_FAILURE;
    }

    for (i = 0; i < pstVencStream->u32PackCount; i++)
    {
        fwrite(pstVencStream->pstPack[i].pu8Addr+pstVencStream->pstPack[i].u32Offset,
               pstVencStream->pstPack[i].u32Len-pstVencStream->pstPack[i].u32Offset, 1, fpH264File);

        fflush(fpH264File);
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI3521_DebugWRStream
 功能描述  : test for saving h264 stream
 输入参数  : HI_S32 s32VencChn, VENC_STREAM_S *pstVencStream
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
HI_S32 HI3521_DebugWRStream(HI_S32 s32VencChn, VENC_STREAM_S *pstVencStream)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32Cnt = s32VencChn;
    time_t nowTime;
    char timeStr[32] = {0};

    static FILE *fp[32] = {NULL};
    HI_CHAR name[64] = {0};

    //AV_PRINT("HI3531_DebugWRStream%d ", s32VencChn);
    if (NULL == fp[s32Cnt])
    {

        time(&nowTime);
        HI3521_CovTime2Str(nowTime, timeStr);
        sprintf(name, "./av%d.h264", s32Cnt);

        fp[s32Cnt] = fopen(name, "wb");
        if (NULL == fp[s32Cnt])
        {
            AV_PRINT("fopen %d err[%d:%s].\n", s32Cnt, errno, ERR_STR);
        }
    }
    else
    {
        s32Ret = HI3521_DebugVencSaveH264(fp[s32Cnt], pstVencStream);
    }

    return s32Ret;
}


HI_BOOL HI3521_IsIFrame(HI_U8 *pFrame)
{
    HI_CHAR cFrameType;

    cFrameType = *((HI_U8 *)(pFrame + 4));
    if (APP_VIDEO_VIP_FRAME == cFrameType)
    {
        return HI_TRUE;
    }

    return HI_FALSE;
}



HI_S32 HI3521_SetVencFrameHead(VENC_STREAM_S *pstStream, HI_S32 s32Chn, FRAME_HEAD_T *pstFrameHead)
{
    HI_U32 i = 0;
    HI_U32 s32FrameLen = 0;
    HI_BOOL bFirstPack = HI_TRUE;
    HI_BOOL bIFrame = HI_FALSE;
    FRAME_HEAD_T stFrameHead;
    static time_t tTmpVencTime[24] = {0};

    if (NULL == pstStream)
    {
        AV_PRINT("Input NULL pointer");
        return HI_FAILURE;
    }

    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        s32FrameLen += pstStream->pstPack[i].u32Len;

    }

    memset(&stFrameHead, 0, sizeof(FRAME_HEAD_T));
    HI_U64 u64PtsOffset = 0;
    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        if (bFirstPack)
        {
            bFirstPack = HI_FALSE;
            stFrameHead.len = s32FrameLen;
            /*获取当前时间 */
            bIFrame = HI3521_IsIFrame((HI_U8 *)pstStream->pstPack[i].pu8Addr);
            if(g_LastFramePts[s32Chn] != 0)
                u64PtsOffset = pstStream->pstPack[i].u64PTS - g_LastFramePts[s32Chn];
            if (bIFrame)
            {
                stFrameHead.type = DATA_VIDEO_IFRAME;
                strncpy((HI_CHAR *)stFrameHead.flag, "IFRAME", sizeof(stFrameHead.flag));
                if (bFindFirstIFrame[s32Chn] == HI_FALSE)
                {
                    bFindFirstIFrame[s32Chn] = HI_TRUE;
                    tGetVencStartTime[s32Chn] = time((time_t *)NULL);
                    g_SetPts[s32Chn] = (HI_U64)tGetVencStartTime[s32Chn]*1000000;
                }
                else
                {
                    tGetVencStartTime[s32Chn] = (time_t)((g_SetPts[s32Chn] + u64PtsOffset)/1000000);
                }
                g_SetPts[s32Chn] = g_SetPts[s32Chn] + u64PtsOffset;
                stFrameHead.ts = g_SetPts[s32Chn];
                tTmpVencTime[s32Chn] = (time_t)((g_SetPts[s32Chn])/1000000);
            }
            else
            {
                stFrameHead.type = DATA_VIDEO_PFRAME;
                strncpy((HI_CHAR *)stFrameHead.flag, "PFRAME", sizeof(stFrameHead.flag));
                g_SetPts[s32Chn] = g_SetPts[s32Chn] + u64PtsOffset;
                stFrameHead.ts = g_SetPts[s32Chn];
            }
#if 0
            if(s32Chn == 8)
            {
                printf("------------------>[%s][seq:%d][ts:%lld][offset:%lld][count:%d][PTS[%d]:%lld][LPTS:%lld]\n"
                       ,stFrameHead.flag,pstStream->u32Seq,stFrameHead.ts,u64PtsOffset,pstStream->u32PackCount,i,pstStream->pstPack[i].u64PTS
                       ,g_LastFramePts[s32Chn]);
            }
#endif
            if(bFindFirstIFrame[s32Chn] == HI_TRUE)
                g_LastFramePts[s32Chn] = pstStream->pstPack[i].u64PTS;
            /*转换时间保存 */
#ifdef USE_SYS_TIMEPTS
            stFrameHead.nmk_time1=(HI_U8)tTmpVencTime[s32Chn]&0xff;
            stFrameHead.nmk_time2=(HI_U8)((tTmpVencTime[s32Chn]>>8)&0xff);
            stFrameHead.nmk_time3=(HI_U8)((tTmpVencTime[s32Chn]>>16)&0xff);
            stFrameHead.nmk_time4=(HI_U8)((tTmpVencTime[s32Chn]>>24)&0xff);
#else
            stFrameHead.nmk_time1=(HI_U8)tGetVencStartTime[s32Chn]&0xff;
            stFrameHead.nmk_time2=(HI_U8)((tGetVencStartTime[s32Chn]>>8)&0xff);
            stFrameHead.nmk_time3=(HI_U8)((tGetVencStartTime[s32Chn]>>16)&0xff);
            stFrameHead.nmk_time4=(HI_U8)((tGetVencStartTime[s32Chn]>>24)&0xff);
#endif

            stFrameHead.Chn[0] = (HI_U8)s32Chn;
            stFrameHead.Chn[1] = (HI_U8)s32Chn;

        }
    }

    memcpy(pstFrameHead, &stFrameHead, sizeof(FRAME_HEAD_T));
    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : HI3531_WrMainStream2Shm_v2
 功能描述  : 主码流写入
 输入参数  : VENC_STREAM_S *pstStream
             HI_S32 s32Chn
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年10月18日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_WrMainStream2Shm_v2(VENC_STREAM_S *pstStream, HI_S32 s32Chn, FRAME_HEAD_T *pstFrameHead)
{
    HI_U32 i = 0;
    HI_S32 s32Ret = 0;
    HI_U32 s32FrameLen = 0;
    HI_S32 s32PackLen = 0;
    HI_BOOL bFirstPack = HI_TRUE;
    FRAME_HEAD_T stFrameHead;
    HI_U8 *pVenc = NULL;
    HI_U8 *pVencTmp = NULL;
    static HI_U32 oldFrameTime[APP_VDEC_CHNCNT]= {0};
    static HI_U32 newFrameTime[APP_VDEC_CHNCNT]= {0};
    HI_U32 u32TimeGap = 0; /* 获取视频码流错误日志 */

    if (NULL == pstStream)
    {
        AV_PRINT("Input NULL pointer");
        return HI_FAILURE;
    }

    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        s32FrameLen += pstStream->pstPack[i].u32Len;

    }

    memcpy(&stFrameHead, pstFrameHead, sizeof(FRAME_HEAD_T));

    pVenc = (HI_U8 *)(pstStream->pstPack) + sizeof(VENC_PACK_S) * pstStream->u32PackCount;
    pVencTmp = pVenc;

    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        if (bFirstPack)
        {
            bFirstPack = HI_FALSE;
            memcpy(pVencTmp, &stFrameHead, sizeof(FRAME_HEAD_T));
            pVencTmp += sizeof(stFrameHead);
        }

        s32PackLen = pstStream->pstPack[i].u32Len;
        if (0 != s32PackLen)
        {
            memcpy(pVencTmp, pstStream->pstPack[i].pu8Addr, s32PackLen);
            pVencTmp += s32PackLen;
        }

    }

    s32FrameLen += sizeof(stFrameHead);


    /* 获取视频码流错误日志 */
    if (DATA_VIDEO_IFRAME == stFrameHead.type)
    {
        newFrameTime[s32Chn] = ((stFrameHead.nmk_time4 << 24)|(stFrameHead.nmk_time3 <<16)|
                                (stFrameHead.nmk_time2 <<8)|stFrameHead.nmk_time1);
        u32TimeGap = newFrameTime[s32Chn] - oldFrameTime[s32Chn];
        if (u32TimeGap >= 3)
        {
            s32Ret = HDD_WrRecStat(oldFrameTime[s32Chn], newFrameTime[s32Chn], s32Chn, (HI_CHAR *)APP_VENC_ERROR_LOG);
            if (0 != s32Ret)
            {
                AV_PRINT( " HDD_WrRecStat err 0x%x", s32Ret);
            }
        }

        oldFrameTime[s32Chn] = newFrameTime[s32Chn];

#ifdef REC_SAVE_GPS
        s32Ret = STOR_InsertGpsInfoByIFrame(&stFrameHead, pVencTmp);
        s32FrameLen += (sizeof(FRAME_HEAD_T)+s32Ret);
#endif

    }

    s32Ret = MEM_WrVencStream2Shm(pVenc, s32Chn, s32FrameLen);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("MEM_WrVencStream2Shm error, s32Chn=%d, s32FrameLen=%d", s32Chn, s32FrameLen);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

HI_S32 HI3521_SetAutoBitrate(VENC_CHN VeChn,HI_S32 priority)
{
    HI_S32 iResult = 0;
    VENC_CHN_ATTR_S stAttr;
    VENC_ATTR_H264_VBR_S stH264Vbr;

    if (VeChn%2 == 0)
    {
        return HI_SUCCESS;
    }

    iResult = HI_MPI_VENC_GetChnAttr(VeChn, &stAttr);
    if (iResult != HI_SUCCESS)
    {
        AV_PRINT("HI_MPI_VENC_GetChnAttr error! result=(0x%#x)",iResult);
        return HI_FAILURE;
    }

    memcpy(&stH264Vbr, &stAttr.stRcAttr.stAttrH264Vbr, sizeof(VENC_ATTR_H264_VBR_S));
    AV_PRINT("Vechn (%d), priority (%d)",VeChn, priority);

    switch(priority)
    {
    case 3:
        stH264Vbr.u32Gop = 12;
        stH264Vbr.fr32DstFrmRate = 4;
        stH264Vbr.u32MinQp = 32;
        stH264Vbr.u32MaxQp = 50;
        stH264Vbr.u32MaxBitRate = 192;
        break;
    /* bad */
    case 2:
        stH264Vbr.u32Gop = 24;
        stH264Vbr.fr32DstFrmRate = 8;
        stH264Vbr.u32MinQp = 32;
        stH264Vbr.u32MaxQp = 50;
        stH264Vbr.u32MaxBitRate = 296;
        break;
    /* normal */
    case 0:
        stH264Vbr.u32Gop = 30;
        stH264Vbr.fr32DstFrmRate = 15;
        stH264Vbr.u32MinQp = 32;
        stH264Vbr.u32MaxQp = 50;
        stH264Vbr.u32MaxBitRate = 448;
        break;
    /* better */
    case 1:
        stH264Vbr.u32Gop = 25;
        stH264Vbr.u32SrcFrmRate = 25;
        stH264Vbr.fr32DstFrmRate = stH264Vbr.u32SrcFrmRate;
        stH264Vbr.u32MinQp = 32;
        stH264Vbr.u32MaxQp = 50;
        stH264Vbr.u32MaxBitRate = 512;
        break;
    default:
        break;
    }


    memcpy(&stAttr.stRcAttr.stAttrH264Vbr, &stH264Vbr, sizeof(VENC_ATTR_H264_VBR_S));
    AV_PRINT("u32Gop = %d; u32StatTime = %d; u32ViFrmRate = %d; fr32TargetFrmRate = %d;u32MaxBitRate = %d; u32MaxQp = %d; u32MinQp = %d",\
             stAttr.stRcAttr.stAttrH264Vbr.u32Gop,
             stAttr.stRcAttr.stAttrH264Vbr.u32StatTime,
             stAttr.stRcAttr.stAttrH264Vbr.u32SrcFrmRate,
             stAttr.stRcAttr.stAttrH264Vbr.fr32DstFrmRate,
             stAttr.stRcAttr.stAttrH264Vbr.u32MaxBitRate,
             stAttr.stRcAttr.stAttrH264Vbr.u32MaxQp,
             stAttr.stRcAttr.stAttrH264Vbr.u32MinQp);

    iResult = HI_MPI_VENC_SetChnAttr(VeChn, &stAttr);
    if (iResult != HI_SUCCESS)
    {
        AV_PRINT( "HI_MPI_VENC_SetChnAttr error! result=(0x%#x)",iResult);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
HI_S32 HI3521_InitChnDataCenter()
{
    int i = 0;
    for(; i<(2*HI_VI_MAX_CHN + 1); i++)
    {
        g_ChnDataCenter[i] = mem_CreateDataCenter();
    }
    return HI_SUCCESS;
}

HI_S32 HI3521_WrMainStream2DataCenter(VENC_STREAM_S *pstStream, HI_S32 s32Chn, FRAME_HEAD_T *pstFrameHead)
{
    HI_U32 i = 0;
    HI_S32 s32Ret = 0;
    HI_U32 s32FrameLen = 0;
    HI_S32 s32PackLen = 0;
    FRAME_HEAD_T stFrameHead;
    static HI_U32 oldFrameTime[APP_VDEC_CHNCNT]= {0};
    static HI_U32 newFrameTime[APP_VDEC_CHNCNT]= {0};
    HI_U32 u32TimeGap = 0; /* 获取视频码流错误日志 */

    if (NULL == pstStream)
    {
        AV_PRINT("Input NULL pointer");
        return HI_FAILURE;
    }

    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        s32FrameLen += pstStream->pstPack[i].u32Len;
    }
    s32FrameLen += sizeof(stFrameHead);

    memcpy(&stFrameHead, pstFrameHead, sizeof(FRAME_HEAD_T));

    //主码流
    HDP hPkg = NULL;
    if (DATA_VIDEO_IFRAME == stFrameHead.type && !(s32Chn%2))
    {
        s32FrameLen += (sizeof(FRAME_HEAD_T)+sizeof(STOR_GPS_FRAME_DATA_ST));
        hPkg = mem_CreatePackage(s32FrameLen,PKG_PRIORITY_HIGH);
    }
    else
    {
        hPkg = mem_CreatePackage(s32FrameLen,PKG_PRIORITY_NORMAL);
    }


    if(hPkg==NULL)
    {
        AV_PRINT("mem_CreatePackage error, s32Chn=%d, s32FrameLen=%d", s32Chn, s32FrameLen);
        return HI_FAILURE;
    }
    mem_ReferPackage(hPkg);
    char *pData = mem_GetPackageData(hPkg);
    memcpy(pData, pstFrameHead, sizeof(FRAME_HEAD_T));
    pData+=sizeof(FRAME_HEAD_T);
    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        s32PackLen = pstStream->pstPack[i].u32Len;
        if (0 != s32PackLen)
        {
            memcpy(pData, pstStream->pstPack[i].pu8Addr, s32PackLen);
            pData += s32PackLen;
        }
    }
    /* 获取视频码流错误日志 */
    //主码流
    if (DATA_VIDEO_IFRAME == stFrameHead.type && !(s32Chn%2))
    {
        newFrameTime[s32Chn%2] = ((stFrameHead.nmk_time4 << 24)|(stFrameHead.nmk_time3 <<16)|
                                  (stFrameHead.nmk_time2 <<8)|stFrameHead.nmk_time1);
        u32TimeGap = newFrameTime[s32Chn%2] - oldFrameTime[s32Chn%2];
        if (u32TimeGap >= 3)
        {
            s32Ret = HDD_WrRecStat(oldFrameTime[s32Chn%2], newFrameTime[s32Chn%2], s32Chn%2, (HI_CHAR *)APP_VENC_ERROR_LOG);
            if (0 != s32Ret)
            {
                AV_PRINT("HDD_WrRecStat err 0x%x", s32Ret);
            }
        }
        oldFrameTime[s32Chn%2] = newFrameTime[s32Chn%2];
#ifdef REC_SAVE_GPS
        s32Ret = STOR_InsertGpsInfoByIFrame(&stFrameHead,(HI_U8*)pData);
#endif
    }

    mem_PushPackage(hPkg,g_ChnDataCenter[s32Chn]);

    mem_ReleasePackage(hPkg);

    return HI_SUCCESS;
}
/*****************************************************************************
 函 数 名  : HI3521_WRVencStream2Mem
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
HI_S32 HI3521_WRVencStream2Mem(HI_CHAR *pVencBuf)
{
    static HI_S32 s32VencChn = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    VENC_CHN_STAT_S vencChnStat;
    VENC_STREAM_S vencStream;
    VENC_STREAM_S *pVencStream = &vencStream;
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();
    VENC_CHN_INFO_t *vinfo = NULL;
    FRAME_HEAD_T stFrameHead;
    HI_S32 s32ChnTotalCnt = TOTAL_VENC_CHN_NUM;
    for (s32VencChn %= s32ChnTotalCnt; s32VencChn < s32ChnTotalCnt; s32VencChn++)
    {
        vinfo = &(pCaptureDev->vi_chn[s32VencChn >> 1].venc_chn[s32VencChn & 0x01]);
        if (!(vinfo->started == HI_ENABLE && vinfo->venc_fd != -1))
        {
            continue;
        }

        s32Ret = FD_ISSET(vinfo->venc_fd, &(pCaptureDev->read_fds));
        if (0 == s32Ret)
        {
            continue;
        }
        memset(pVencStream, 0, sizeof(VENC_STREAM_S));
        s32Ret = HI_MPI_VENC_Query(s32VencChn, &vencChnStat);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT( "s32VencChn%d, HI_MPI_VENC_Query error %#x\n", s32VencChn, s32Ret);
            continue;
        }

        if(0 == vencChnStat.u32CurPacks)
        {

            AV_PRINT("NOTE: Current  frame is NULL!");
            continue;
        }

        pVencStream->pstPack = (VENC_PACK_S *)pVencBuf;
        pVencStream->u32PackCount = vencChnStat.u32CurPacks;

        s32Ret = HI_MPI_VENC_GetStream(s32VencChn, pVencStream, HI_IO_BLOCK);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT("s32VencChn%d, HI_MPI_VENC_GetStream error %#x\n", s32VencChn, s32Ret);
            //free(pVencStream->pstPack);
            break;
        }

#ifdef FILE_DEBUG
        if ((s32VencChn % 2))
        {
            HI3521_DebugWRStream(s32VencChn >> 1, pVencStream);
        }
#endif
        HI3521_SetVencFrameHead(pVencStream, s32VencChn, &stFrameHead);
        if(mem_DataCenterEmpty(g_ChnDataCenter[s32VencChn]) != 1)
        {
            HI3521_WrMainStream2DataCenter(pVencStream, s32VencChn, &stFrameHead);
        }

        s32Ret = HI_MPI_VENC_ReleaseStream(s32VencChn, pVencStream);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT("s32VencChn %d %p HI_MPI_VENC_ReleaseStream error %#x\n", s32VencChn,&s32VencChn, s32Ret);
            break;
        }
    }

    return s32Ret;
}

/*****************************************************************************
 函 数 名  : HI3521_GetVencStream
 功能描述  :
 输入参数  : HI_VOID *arg
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
HI_S32 HI3521_GetVencStream(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();
    HI_CHAR *pVencBuf = NULL;

#ifdef USE_SYS_TIMEPTS
    HI_S32 s32PTSCHN = 0;
    struct timeval tp;
    gettimeofday(&tp, NULL);
    int   m_second = tp.tv_sec;
    for(s32PTSCHN = 0; s32PTSCHN < TOTAL_VENC_CHN_NUM; s32PTSCHN++)
    {
        g_SetPts[s32PTSCHN] = (HI_U64)m_second*1000000+tp.tv_usec;
    }
    memset(bFindFirstIFrame, HI_FALSE, TOTAL_VENC_CHN_NUM*sizeof(HI_BOOL));
#endif

    pVencBuf = (HI_CHAR *)malloc(VENC_BUF_LEN);
    if (NULL == pVencBuf)
    {
        AV_PRINT("malloc memory error(%s)", ERR_STR);
        return HI_SUCCESS;
    }
    HI_S32 s32Title = 0;
    HI_S32 s32Chn = 0;

    for (s32Chn = 0; s32Chn < VI_CHN_NUM_HOST_CHIP; s32Chn++)
    {
        for (s32Title = 0; s32Title < MAX_OSD_TITLE; s32Title++)
        {
            HI3521_InitOsdTitle(s32Title, s32Chn, HI_MAIN_STREAM);
            HI3521_InitOsdTitle(s32Title, s32Chn, HI_SUB_STREAM);
        }
    }

    while (vencrun)
    {
        if (!HI3521_IsVencStreamEnable())
        {
            continue;
        }
#ifdef OSD_SHOW
        HI3521_OsdShow();
#endif

#ifndef USE_NEW_OSD_FUNC
        HI3521_SetTimeOsd();
#endif
        HI3521_PthreadMutexLock(&(pCaptureDev->lock));
        s32Ret = HI3521_VencStreamFdChk();
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT("venc stream fd no data");
            HI3521_PthreadMutexUnlock(&(pCaptureDev->lock));
            continue;
        }

        HI3521_WRVencStream2Mem(pVencBuf);

        HI3521_PthreadMutexUnlock(&(pCaptureDev->lock));

        TM_USleep(0,18*1000);
    }

    if (NULL != pVencBuf)
    {
        free(pVencBuf);
        pVencBuf = NULL;
    }
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI3521_CreateVencStreamThd
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
HI_VOID *HI3521_CreateVencStreamThd(HI_VOID *pArg)
{
    AV_PRINT("PID@%s venc pid is %d,tid is %d", __FUNCTION__, getpid(),gettid());
    while(1)
    {
        HI3521_GetVencStream();
    }

    return NULL;
}
/*****************************************************************************
 函 数 名  : HI3521_StartVencStream
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
HI_S32 HI3521_StartVencStream(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    pthread_t tid;
    static HI_BOOL bThdStart = HI_FALSE;

    if (HI_FALSE == bThdStart)
    {
        s32Ret = pthread_create(&tid, NULL, HI3521_CreateVencStreamThd, NULL);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( "create GetVencStream thread err(%s)", ERR_STR);
            return HI_FAILURE;
        }

        s32Ret = pthread_detach(tid);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( "detach GetVencStream thread err(%s)", ERR_STR);
            return HI_FAILURE;
        }

        bThdStart = HI_TRUE;
    }

    return HI_SUCCESS;
}

HI_S32 HI3521_SaveJPEG(FILE *fpJpegFile, VENC_STREAM_S *pstStream)
{
    VENC_PACK_S*  pstData;
    HI_U32 i;

    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        pstData = &pstStream->pstPack[i];
        fwrite(pstData->pu8Addr, pstData->u32Len, 1, fpJpegFile);
    }

    return HI_SUCCESS;
}



/*****************************************************************************
 函 数 名  : HI3531_CovTime2Str
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
HI_VOID HI3531_CovTime2JpgStr(time_t t, HI_CHAR *pTime)
{
    struct tm *tmp;

    if (NULL == pTime)
    {
        AV_PRINT("Input NULL pointer");
        return;
    }
    tmp = localtime(&t);
    sprintf(pTime,"%04d%02d%02d%02d%02d%02d",
            tmp->tm_year+1900, tmp->tm_mon+1, tmp->tm_mday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
}

/*****************************************************************************
 函 数 名  : TEST_HI3531_Mkdir
 功能描述  :判断抓拍目录是否存在，不存在则创建
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月27日
    作    者   : wangdayong
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_Mkdir(void)
{
#define MODE (S_IRWXU | S_IRWXG | S_IRWXO)
    const char *picdir = "/mnt/nand/pic";
    int ret =0;
    DIR * dir;
    if((dir = opendir(picdir))==NULL)//判断目录
    {
        ret = mkdir(picdir, MODE);//创建目录
        if (ret != 0)
        {
            return HI_FAILURE;
        }
        //printf("%s created sucess!/n", dir);
    }
    else
    {
        //printf("%s exist!/n", dir);
        closedir(dir);
        return HI_SUCCESS;
    }
    return HI_SUCCESS;
}


HI_S32 HI3531_GetSnapInfo(void)
{
    int ret = -1;
    int cur_picnum = 0;
    char wbuf[32];
    FILE * fp  = NULL;
    memset(wbuf,0,32);
    ret  = access(SNAPINFO,0);
    if(ret == 0)
    {
        fp = fopen(SNAPINFO,"r+");
    }
    else
    {
        fp = fopen(SNAPINFO,"w+");
    }
    if(fp == NULL)
    {
        AV_PRINT("Open snap file (%s) error!!!!maybe nand error",SNAPINFO);
        return -1;
    }
    fseek(fp,0,SEEK_SET);
    fgets(wbuf,32,fp);
    cur_picnum = atoi(wbuf);
    cur_picnum++;
    if(cur_picnum > MAX_PIC_NUM)
        cur_picnum = 1;
    AV_PRINT("cur_picnum=%d\n",cur_picnum);
    fseek(fp,0,SEEK_SET);
    fprintf(fp,"%d          ",cur_picnum);
    fclose(fp);
    return cur_picnum;
}
HI_S32 HI3531_Find_Oldfile(char * root,char *filename, char *str)
{
    DIR * dir;
    struct dirent * ptr;
    char rmfile[128];
    int ret = HI_FAILURE;
    dir = opendir(root);
    if(dir == NULL)
    {
        perror("fail to open dir\n");
        exit(-1);
    }
    while((ptr = readdir(dir)) != NULL)
    {
        if(strstr(ptr->d_name,str) != NULL)
        {
            strcpy(filename,ptr->d_name);
            sprintf(rmfile,"/mnt/nand/pic/%s",ptr->d_name);
            remove(rmfile);
            ret = HI_SUCCESS;
            break;
        }
    }
    closedir(dir);
    return ret;
}
HI_S32 HI3521_SaveSnap(VENC_STREAM_S *pstStream, HI_S32 s32Chn)
{
    char acFile[64] = {0};
    FILE *pFile;
    HI_S32 s32Ret = HI_SUCCESS;
    int count[16] = {0};
    char filename[32];
    char str[16];
    time_t nowTime;
    HI_CHAR strTime[32] = {0};
    time(&nowTime);
    HI_S32 cur_pic = 0;
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("get czjinfo failed!\n");
        return HI_FAILURE;
    }
    HI3531_CovTime2JpgStr(nowTime, strTime);
    if(count[s32Chn]==0)
    {
        HI3521_Mkdir();
        cur_pic = HI3531_GetSnapInfo();
        if(cur_pic == -1)
        {
            AV_PRINT("HI3531_GetSnapInfo error");
            return HI_FAILURE;
        }
        sprintf(str,"_%d.jpg",cur_pic);
        //查找是否有与当前序号一样的图片，有则删除
        if(HI3531_Find_Oldfile((char*)"/mnt/nand/pic",filename,str)==HI_SUCCESS)
        {
            AV_PRINT("find same file.remove filename =%s\n",filename);
        }
        sprintf(acFile, "/mnt/nand/pic/snap_%d-%d_%s_%d_%d_%d_%d.jpg",g_u8Company,g_u8SubCompany, strTime, s32Chn,cur_pic);

        pFile = fopen(acFile, "wb+");
        if (pFile == NULL)
        {
            AV_PRINT("open file err\n");
            return HI_FAILURE;
        }

        s32Ret = HI3521_SaveJPEG(pFile, pstStream);
        if (HI_SUCCESS != s32Ret)
        {
            fclose(pFile);
            AV_PRINT("save snap picture failed!\n");
            return HI_FAILURE;
        }
        AV_PRINT("save snap file success.%s\n",acFile);
        fclose(pFile);
        count[s32Chn]= 1;
    }
    return HI_SUCCESS;
}


HI_S32 HI3521_SnapUnBindVpss(HI_S32 s32Chn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32VenChn = 0;
    HI_S32 s32VenGrp = 0;
    HI_S32 s32VeVpss = s32Chn;
    JPEG_CHN_INFO_t *pSnapJpegInfo = NULL;
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();

    pSnapJpegInfo = &pCaptureDev->vi_chn[s32Chn].jpeg_chn;

    s32VenChn = pSnapJpegInfo->info.enc_chn;
    s32VenGrp = pSnapJpegInfo->grp;
    s32Ret = HI3521_COMM_VENC_UnBindVpss(s32VenGrp, s32VeVpss, s32VenChn);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("HI3531_Comm_VencUnBindVpss err 0x%x",s32Ret);

    }
    else
    {
        AV_PRINT("HI3531_Comm_VencUnBindVpss ok 0x%x",s32Ret);
    }

    return s32Ret;
}


HI_S32 HI3521_DestroySnapChn(HI_S32 s32Chn)
{
    HI_S32 s32Ret = 0;
    HI_S32 s32VenChn = 0;
    JPEG_CHN_INFO_t *pSnapJpegInfo = NULL;
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();
    pSnapJpegInfo = &pCaptureDev->vi_chn[s32Chn].jpeg_chn;

    AV_PRINT("HI3531_StopSnapShot chan =%d", s32Chn);
    s32VenChn = pSnapJpegInfo->info.enc_chn;
    s32Ret =  HI3521_Comm_VENC_Stop(s32VenChn);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("HI3531_VencStopChn err 0x%x",s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}



/*****************************************************************************
 函 数 名  : APP_GetPicture
 功能描述  : 获取一幅图片
 输入参数  : VENC_CHN SnapChn
             SNAP_MULTI_CHN_S *pic_s
 输出参数  : 无
 返 回 值  :



 修改历史      :
  1.日    期   : 2012年8月27日星期一
    作    者   : zhangjinlei
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_GetSnapChnData(HI_S32 s32Chn)
{
    HI_S32 s32Ret;
    HI_S32 s32VencFd;
    fd_set read_fds;
    struct timeval TimeoutVal;
    VENC_CHN SnapChn = 0;
    VENC_CHN_STAT_S stStat;
    VENC_STREAM_S stStream;
    JPEG_CHN_INFO_t *pSnapJpegInfo = NULL;
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();
    VENC_RECV_PIC_PARAM_S stRecvParam;
    pSnapJpegInfo = (&pCaptureDev->vi_chn[s32Chn].jpeg_chn);
    SnapChn =  pSnapJpegInfo->jpegChn;
    stRecvParam.s32RecvPicNum = 1;
    s32Ret = HI_MPI_VENC_StartRecvPicEx(SnapChn,&stRecvParam);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("HI_MPI_VENC_StartRecvPic err 0x%x", s32Ret);
        return s32Ret;
    }

    s32VencFd = pSnapJpegInfo->venc_fd;

    FD_ZERO(&read_fds);
    FD_SET(s32VencFd, &read_fds);

    TimeoutVal.tv_sec  = 10;
    TimeoutVal.tv_usec = 0;
    s32Ret = select(s32VencFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
    if (s32Ret < 0)
    {
        AV_PRINT("HI3531_GetSnapChnData select err!");
        return HI_FAILURE;
    }
    else if (0 == s32Ret)
    {
        AV_PRINT("HI3531_GetSnapChnData time out!");
        return HI_FAILURE;
    }
    else
    {
        if (FD_ISSET(s32VencFd, &read_fds))
        {
            s32Ret = HI_MPI_VENC_Query(SnapChn, &stStat);
            if (s32Ret != HI_SUCCESS)
            {
                AV_PRINT("HI_MPI_VENC_Query failed! s32Ret = 0x%x", s32Ret);
                fflush(stdout);
                return HI_FAILURE;
            }

            stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
            if (NULL == stStream.pstPack)
            {
                AV_PRINT( "malloc memory err!");
                return HI_FAILURE;
            }

            stStream.u32PackCount = stStat.u32CurPacks;
            s32Ret = HI_MPI_VENC_GetStream(SnapChn, &stStream, HI_TRUE);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT( "HI_MPI_VENC_GetStream failed! s32Ret = 0x%x", s32Ret);
                fflush(stdout);
                free(stStream.pstPack);
                stStream.pstPack = NULL;
                return HI_FAILURE;
            }
            s32Ret = HI3521_SaveSnap(&stStream, s32Chn);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT( "HI3521_SaveSnap failed!");
                fflush(stdout);
            }

            s32Ret = HI_MPI_VENC_ReleaseStream(SnapChn, &stStream);
            if (s32Ret)
            {
                AV_PRINT( "#HI_MPI_VENC_ReleaseStream failed! s32Ret = 0x%x", s32Ret);
                free(stStream.pstPack);
                stStream.pstPack = NULL;
                return HI_FAILURE;
            }
            free(stStream.pstPack);
            stStream.pstPack = NULL;
        }
    }

    s32Ret = HI_MPI_VENC_StopRecvPic(SnapChn);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("HI_MPI_VENC_StopRecvPic err 0x%x", s32Ret);
        return s32Ret;
    }
    return HI_SUCCESS;
}



HI_VOID HI3521_SetSnapTimeOsd(HI_S32 s32Chn)
{
    SYSTEM_TIME 	osdtime;
    static time_t	oldosdtime = 0;
    time_t			curosdtime = time(NULL);
    struct tm		time;

    if (oldosdtime != curosdtime || oldosdtime == 0)
    {
        localtime_r(&curosdtime, &time);

        osdtime.second	= time.tm_sec;			/* seconds (1 - 60) */
        osdtime.minute	= time.tm_min;			/* minutes (1 - 60) */
        osdtime.hour	= time.tm_hour; 		/* hours (1 - 24)	*/
        osdtime.wday	= time.tm_wday; 		/* day of week		*/
        osdtime.day 	= time.tm_mday; 		/* day of the month */
        osdtime.month	= time.tm_mon + 1;		/* month (1-12) 	*/
        osdtime.year	= time.tm_year + 1900;	/* year 			*/
        AV_PRINT("s32Chn(%d)", s32Chn);
        HI3521_CaptureSetTime_Snap(s32Chn, &osdtime);
        oldosdtime = curosdtime;
    }
    return;
}


HI_S32 HI3521_CreateSnapChn(HI_S32 s32Chn)
{
    HI_S32 s32Ret;
    VPSS_GRP VeVpss = CAPTURE_VPSS(s32Chn);
    HI_S32 s32VpssChn = JPEG_VPSS_CHN();
    HI_S32 s32VencFd;
    SIZE_S picsize;
    VENC_CHN_ATTR_S stVencChnAttr;
    VENC_ATTR_JPEG_S stJpegAttr;
    VENC_GRP VeGroup = VENC_CHN_NUM_HOST_CHIP + s32Chn;
    VENC_CHN VeChn = VENC_CHN_NUM_HOST_CHIP + s32Chn;
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();
    JPEG_CHN_INFO_t *pSnapChnInfo = NULL;
    PIC_SIZE_E enPicSize;
    pSnapChnInfo = &pCaptureDev->vi_chn[s32Chn].jpeg_chn;
    if(HI3521_GetTw2968VideoType() ==  1 )
        enPicSize = (s32Chn < 8)?PIC_960H : PIC_HD720;
    else
        enPicSize = (s32Chn < 8)?PIC_D1 : PIC_HD720;
    HI3521_Comm_SYS_GetPicSize((VIDEO_NORM_E)pCaptureDev->vstd, enPicSize,&picsize);
    if (s32Chn < 0 || s32Chn >= VI_CHN_NUM_HOST_CHIP)
    {
        AV_PRINT("JPEGChCreate error, vichn = %d", s32Chn);
        return HI_FAILURE;
    }

    if (pSnapChnInfo->create != 1)
    {
        AV_PRINT("vichn = %d unsupport by app", s32Chn);
        return HI_FAILURE;
    }

    stVencChnAttr.stVeAttr.enType = PT_JPEG;

    stJpegAttr.u32MaxPicWidth  = picsize.u32Width;
    stJpegAttr.u32MaxPicHeight = picsize.u32Height;
    stJpegAttr.u32PicWidth	= picsize.u32Width;
    stJpegAttr.u32PicHeight = picsize.u32Height;
    stJpegAttr.u32BufSize = picsize.u32Width * picsize.u32Height * 2;
    stJpegAttr.bByFrame = HI_TRUE;/*get stream mode is field mode  or frame mode*/
    stJpegAttr.bSupportDCF = HI_FALSE;/*the sign of the VI picture is field or frame?*/
//	stJpegAttr.u32Priority = 0;/*channels precedence level*/
    memcpy(&stVencChnAttr.stVeAttr.stAttrJpeg, &stJpegAttr, sizeof(VENC_ATTR_JPEG_S));
    pSnapChnInfo->grp = VeGroup;
    pSnapChnInfo->viChn = s32Chn;
    pSnapChnInfo->jpegChn = VeChn;
    pSnapChnInfo->width = stJpegAttr.u32PicWidth;
    pSnapChnInfo->height = stJpegAttr.u32PicHeight;
    pSnapChnInfo->Qval = 5;
    pSnapChnInfo->fps = 10;
    pSnapChnInfo->info.enc_chn = VeChn;
    pSnapChnInfo->info.channel = s32Chn;
    pSnapChnInfo->info.dwtype = CHL_JPEG_T;



    s32Ret = HI3521_VencStartChn(VeChn,  &stVencChnAttr,VeGroup);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("HI3531_VencStartChn err 0x%x",s32Ret);
        return HI_FAILURE;
    }
    s32Ret = HI3521_Comm_VENC_BindVpss(VeGroup, VeVpss, s32VpssChn);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("HI3531_Comm_VencBindVpss err 0x%x", s32Ret);
        return HI_FAILURE;
    }

    HI3531_SnapOsdShow(s32Chn);

    HI3521_SetSnapTimeOsd(s32Chn);
    //usleep(20000);
    TM_USleep(0,20000);

    s32Ret = HI_MPI_VENC_SetMaxStreamCnt(VeChn, 1);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("HI_MPI_VENC_SetMaxStreamCnt(%d) err 0x%x\n", VeChn, s32Ret);
        return HI_FAILURE;
    }

    s32VencFd = HI_MPI_VENC_GetFd(VeChn);
    if (s32VencFd == HI_ERR_VENC_INVALID_CHNID || s32VencFd == HI_ERR_VENC_SYS_NOTREADY)
    {
        pSnapChnInfo->venc_fd = -1;
        AV_PRINT("HI_MPI_VENC_GetFd err 0x%x\n",s32Ret);
        return HI_FAILURE;
    }

    pSnapChnInfo->venc_fd = s32VencFd;


    return HI_SUCCESS;
}



HI_S32 HI3521_SnapOnePic(HI_S32 s32Chn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32SnapErr = HI_SUCCESS;

    JPEG_CHN_INFO_t *pSnapChnInfo = NULL;
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();

    pSnapChnInfo = (&pCaptureDev->vi_chn[s32Chn].jpeg_chn);


    pSnapChnInfo->ptr = (HI_U8 *)malloc(1024*400);
    if (NULL == pSnapChnInfo->ptr)
    {
        AV_PRINT("malloc err(%s)!", ERR_STR);
        return HI_FAILURE;
    }

    s32Ret = HI3521_CreateSnapChn(s32Chn);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("HI3531_CreateSnapChn err!");
        s32SnapErr = HI_FAILURE;
        goto SNAP_ERROR;
    }

    s32Ret = HI3521_GetSnapChnData(s32Chn);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT( "HI3531_GetSnapChnData err!");
        s32SnapErr = HI_FAILURE;
        goto SNAP_ERROR;
    }



SNAP_ERROR:
    free(pSnapChnInfo->ptr);
    pSnapChnInfo->ptr = NULL;
    s32Ret = HI3521_DestroySnapChn(s32Chn);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("HI3531_DestroySnapChn err 0x%x",s32Ret);
        s32SnapErr = HI_FAILURE;
    }

    s32Ret = HI3521_SnapUnBindVpss(s32Chn);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("HI3531_SnapUnBindVpss err 0x%x",s32Ret);
        s32SnapErr = HI_FAILURE;
    }

    return s32SnapErr;

}


HI_VOID HI3521_EnableSnapChn(HI_S32 s32Chn)
{
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();
    pCaptureDev->vi_chn[s32Chn].jpeg_chn.create = HI_ENABLE;

    return;
}

HI_S32 HI3521_StartSnapShot(HI_S32 s32SnapChn)
{
    HI_S32 s32Ret = 0;

    HI3521_EnableSnapChn(s32SnapChn);
    s32Ret = HI3521_SnapOnePic(s32SnapChn);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( "HI3531_SnapOnePic failed!");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI3521_StartVencStream
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
HI_S32 HI3521_SnapShotByChn()
{
    HI_S32 s32Chn = 0;
    HI_S32 s32Ret = HI_SUCCESS;
//    HI_S32 s32SnapStat = 0;
//	RW_LOCK_T *pSnapStatLock = NULL;
    if ( Cam_Snap_Chn > 11)
    {
        return HI_FAILURE;
    }

    s32Chn = Cam_Snap_Chn;
//	HI3531_GetLock(LOCK_SNAP_STAT_E, &pSnapStatLock);
//    AV_PRINT("HI3531_GetLock");
//  HI3531_GetLockStat(pSnapStatLock, &s32SnapStat);
//    AV_PRINT("HI3531_GetLockStat");
//    if (APP_JPEGSTATUS_IDLE != s32SnapStat)
//    {
//        return HI_SUCCESS;
//    }

    s32Ret = HI3521_StartSnapShot(s32Chn);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( "HI3531_StartSnapShot err!");
    }

//	HI3531_SetLockStat(pSnapStatLock, APP_JPEGSTATUS_IDLE);
    return s32Ret;
}
HI_VOID HI3521_InitTw2968VideoType()
{
    if(!access("/mnt/flash/video_flag", F_OK))  //文件存在
    {
        FILE *cfg_fp = NULL;
        HI_S32 s32Ret = 0;
        cfg_fp = fopen("/mnt/flash/video_flag", "r+");
        if(cfg_fp == NULL)
        {
            AV_PRINT( "HI3521_InitTw2968VideoType fopen filename=/mnt/flash/video_flag");
            return;
        }
        s32Ret = fseek(cfg_fp,0,SEEK_SET);
        HI_CHAR cVideoType = 0;
        s32Ret = fread(&cVideoType, 1, sizeof(HI_CHAR), cfg_fp);
        if(s32Ret<0)
        {
            AV_PRINT( " read Tw2968 Video Type file err!");
            fclose(cfg_fp);
            g_s32Tw2968VideoType = 0;
            return;
        }
        g_s32Tw2968VideoType = atoi(&cVideoType);

        if(g_s32Tw2968VideoType != 0 && g_s32Tw2968VideoType != 1)
        {
            AV_PRINT( "Tw2968 Video Type file error read value = %d",g_s32Tw2968VideoType);
            fclose(cfg_fp);
            g_s32Tw2968VideoType = 0;
            return;
        }
        fclose(cfg_fp);
    }
    else
    {
        g_s32Tw2968VideoType = 0;
    }
    AV_PRINT("Tw2968 Video Type is %d",g_s32Tw2968VideoType);
}
HI_S32 HI3521_GetTw2968VideoType()
{
    return g_s32Tw2968VideoType;
}
#ifdef JHY_SDK
HI_U16 HI3521_IVE_CalcStride(HI_U16 u16Width, HI_U8 u8Align)
{
    return (u16Width + (u8Align - u16Width%u8Align)%u8Align);
}
HI_S32 HI3521_IVE_CreateImage(IVE_IMAGE_S *pstImg,IVE_IMAGE_TYPE_E enType,HI_U16 u16Width,HI_U16 u16Height)
{
    HI_U32 u32Size = 0;
    HI_S32 s32Ret;
    if (NULL == pstImg)
    {
        AV_PRINT("pstImg is null\n");
        return HI_FAILURE;
    }
    pstImg->enType = enType;
    pstImg->u16Width = u16Width;
    pstImg->u16Height = u16Height;
    pstImg->u16Stride[0] = HI3521_IVE_CalcStride(pstImg->u16Width,16);
    switch(enType)
    {
    case IVE_IMAGE_TYPE_YUV420SP:
    {
        u32Size = pstImg->u16Stride[0] * pstImg->u16Height * 3 / 2;
        s32Ret = HI_MPI_SYS_MmzAlloc_Cached(&pstImg->u32PhyAddr[0], (void**)&pstImg->pu8VirAddr[0], NULL, HI_NULL, u32Size);
        if(s32Ret != HI_SUCCESS)
        {
            AV_PRINT("Mmz Alloc fail,Error(%#x)\n",s32Ret);
            return s32Ret;
        }
        pstImg->u16Stride[1] = pstImg->u16Stride[0];
        pstImg->u32PhyAddr[1] = pstImg->u32PhyAddr[0] + pstImg->u16Stride[0] * pstImg->u16Height;
        pstImg->pu8VirAddr[1] = pstImg->pu8VirAddr[0] + pstImg->u16Stride[0] * pstImg->u16Height;
    }
    break;
    case IVE_IMAGE_TYPE_YUV422SP:
    {
        u32Size = pstImg->u16Stride[0] * pstImg->u16Height * 2;
        s32Ret = HI_MPI_SYS_MmzAlloc_Cached(&pstImg->u32PhyAddr[0], (void**)&pstImg->pu8VirAddr[0], NULL, HI_NULL, u32Size);
        if(s32Ret != HI_SUCCESS)
        {
            AV_PRINT("Mmz Alloc fail,Error(%#x)\n",s32Ret);
            return s32Ret;
        }
        pstImg->u16Stride[1] = pstImg->u16Stride[0];
        pstImg->u32PhyAddr[1] = pstImg->u32PhyAddr[0] + pstImg->u16Stride[0] * pstImg->u16Height;
        pstImg->pu8VirAddr[1] = pstImg->pu8VirAddr[0] + pstImg->u16Stride[0] * pstImg->u16Height;
    }
    break;
    default:
        break;
    }
    return HI_SUCCESS;
}
HI_VOID *HI3521_JHY_WorkThd(HI_VOID *pArg)
{
    AV_PRINT("PID@%s JHY pid is %d,tid is %d", __FUNCTION__, getpid(),gettid());
    IVE_DST_IMAGE_S stDst[4];
    int i = 0;
    HI_S32 s32Ret = HI_FALSE;
    for(i = 0; i<4; i++)
    {
        HI3521_IVE_CreateImage(&(stDst[i]),IVE_IMAGE_TYPE_YUV420SP,704,576);
        if (HI_MPI_VI_SetFrameDepth(g_s32ViJHYChn[i], 4))
        {
            AV_PRINT("HI_MPI_VI_SetFrameDepth err, vi chn %d \n", g_s32ViJHYChn[i]);
            return NULL;
        }
    }
    IVE_SRC_DATA_S stSrcData;
    IVE_DST_DATA_S stDstData;
    IVE_HANDLE IveHandle;
    IVE_DMA_CTRL_S stDmaCtrl;
    HI_BOOL bInstant = HI_FALSE;
    VIDEO_FRAME_INFO_S stFrame[4];
    APCCOUNTER stApCounter;
    memset(&stApCounter,0,sizeof(APCCOUNTER));
    while(1)
    {
        HI_BOOL bFinish = HI_FALSE;
        HI_BOOL bBlock = HI_TRUE;
        HI_BOOL bGetFrame = HI_TRUE;
        GetCounter(&stApCounter);
#if 1
        //关门状态
        if(stApCounter.doorS[FRONT_DOOR] == 0 && stApCounter.doorS[BACK_DOOR] == 0)
        {
            TM_USleep(0,100*1000);
            continue;
        }
#endif
        int j = 0;
        for(i =0; i<4; i++)
        {
            s32Ret = HI_MPI_VI_GetFrame(g_s32ViJHYChn[i], &stFrame[i],10*1000);

            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT("HI_MPI_VI_GetFrame %d frame err %#x\n", g_s32ViJHYChn[i],s32Ret);
                bGetFrame = HI_FALSE;
                break;
            }

            stDmaCtrl.enMode = IVE_DMA_MODE_DIRECT_COPY;

            stSrcData.pu8VirAddr = (HI_U8*)stFrame[i].stVFrame.pVirAddr[0];
            stSrcData.u32PhyAddr = stFrame[i].stVFrame.u32PhyAddr[0];
            stSrcData.u16Width = (HI_U16)stFrame[i].stVFrame.u32Width;
            stSrcData.u16Height = stFrame[i].stVFrame.u32Height;
            stSrcData.u16Stride = (HI_U16)stFrame[i].stVFrame.u32Stride[0];

            stDstData.pu8VirAddr = stDst[i].pu8VirAddr[0];
            stDstData.u32PhyAddr = stDst[i].u32PhyAddr[0];
            stDstData.u16Width = stDst[i].u16Width;
            stDstData.u16Height = stDst[i].u16Height;
            stDstData.u16Stride = stDst[i].u16Stride[0];

            s32Ret = HI_MPI_IVE_DMA(&IveHandle,&stSrcData, &stDstData,
                                    &stDmaCtrl, bInstant);

            s32Ret = HI_MPI_SYS_MmzFlushCache(stDstData.u32PhyAddr,stDstData.pu8VirAddr
                                              ,stSrcData.u16Stride*stSrcData.u16Height*3/2);
            if(s32Ret != HI_SUCCESS)
            {
                AV_PRINT(" HI_MPI_SYS_MmzFlushCache %x\n",s32Ret);
                bGetFrame = HI_FALSE;
                break;
            }

            s32Ret = HI_MPI_IVE_Query(IveHandle,&bFinish,bBlock);
            while(HI_ERR_IVE_QUERY_TIMEOUT == s32Ret && j < 5)
            {
                usleep(100);
                s32Ret = HI_MPI_IVE_Query(IveHandle,&bFinish,bBlock);
            }
            if(s32Ret == HI_SUCCESS)
            {
                memcpy(g_u8JHY_buffer[i],stDstData.pu8VirAddr,704*576);
            }
            else
                bGetFrame = HI_FALSE;
            s32Ret = HI_MPI_VI_ReleaseFrame(g_s32ViJHYChn[i], &stFrame[i]);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT("HI_MPI_VI_ReleaseFrame err %#x\n", s32Ret);
                bGetFrame = HI_FALSE;
                break;
            }
        }
        if(bGetFrame)
        {
            AlgDeal(704,576,&(g_u8JHY_buffer[0][0]));
            APCCOUNTER apcount;
            GetCounter(&apcount);
#if 1
            if((apcount.inNum[0] + apcount.inNum[1]) != (stApCounter.inNum[0] + stApCounter.inNum[1])||
                    (apcount.outNum[0] + apcount.outNum[1]) != (stApCounter.outNum[0] + stApCounter.outNum[1])||
                    (apcount.inNum[2] + apcount.inNum[3]) != (stApCounter.inNum[2] + stApCounter.inNum[3])||
                    (apcount.outNum[2] + apcount.inNum[3]) != (stApCounter.outNum[2] + stApCounter.outNum[3])
              )
            {
                AV_PRINT("Send stn kl to schedule.IN:%d %d %d %d OUT:%d %d %d %d DOOR:%d %d %d %d STA:%d %d"
                         ,apcount.inNum[0],apcount.inNum[1],apcount.inNum[2],apcount.inNum[3]
                         ,apcount.outNum[0],apcount.outNum[1],apcount.outNum[2],apcount.outNum[3]
                         ,apcount.doorS[0],apcount.doorS[1],apcount.doorS[2],apcount.doorS[3]
                         ,apcount.stationInNum,apcount.stationOutNum);
                SCH_KLSTN_REAL_DATA_T stKlRealTimeInfo;
                memset(&stKlRealTimeInfo,0,sizeof(SCH_KLSTN_REAL_DATA_T));
                stKlRealTimeInfo.cmd = SCH_KL_RTINFO_RA;
                stKlRealTimeInfo.ucFrontDoorCome = apcount.inNum[0] + apcount.inNum[1];
                stKlRealTimeInfo.ucFrontDoorGo =  apcount.outNum[0] + apcount.outNum[1];
                stKlRealTimeInfo.ucBackDoorCome = apcount.inNum[2] + apcount.inNum[3];
                stKlRealTimeInfo.ucBackDoorGo = apcount.outNum[2] + apcount.outNum[3];
                DOMAIN_PackageData(ID_SCH, SCH_KL_RTINFO_RA,(HI_U8*)(&stKlRealTimeInfo),sizeof(SCH_KLSTN_DATA_REQ_T));
            }
#endif
        }
        TM_USleep(0,25*1000);
    }
    return NULL;
}
HI_BOOL HI3521_JHY_GetDoorTrigger()
{
    return g_bDoortTrigger;
}
HI_VOID HI3521_JHY_SetDoorTrigger(HI_BOOL bTrigger)
{
    g_bDoortTrigger = bTrigger;
}
HI_S32 HI3521_JHY_SetDoorStnInfo(HI_VOID *pStnInfo)
{
    memcpy(&g_stDoorStnInfo,pStnInfo,sizeof(SCH_KLSTN_DATA_T));
    return HI_SUCCESS;
}
HI_S32 HI3521_JHY_GetDoorStnInfo(HI_VOID *pStnInfo)
{
    memcpy(pStnInfo,&g_stDoorStnInfo,sizeof(SCH_KLSTN_DATA_T));
    return HI_SUCCESS;
}
HI_S32 HI3521_JHY_SetStnInfo(HI_VOID *pStnInfo)
{
    memcpy(&g_stSchStnInfo,pStnInfo,sizeof(SCH_KLSTN_DATA_T));
    return HI_SUCCESS;
}
HI_S32 HI3521_JHY_GetStnInfo(HI_VOID *pStnInfo)
{
    memcpy(pStnInfo,&g_stSchStnInfo,sizeof(SCH_KLSTN_DATA_T));
    return HI_SUCCESS;
}
HI_S32 HI3521_JHY_GetLogicChn(HI_S32 s32Door)
{
    return g_s32ViLogicChn[2*s32Door];
}
HI_S32 HI3521_JHY_Init(HI_VOID)
{
    ApcIniterface();
    memset(&g_stCounter,0,sizeof(APCCOUNTER));
    memset(&g_stSchStnInfo,0,sizeof(SCH_KLSTN_DATA_T));
    memset(&g_stDoorStnInfo,0,sizeof(SCH_KLSTN_DATA_T));

    g_stSchStnInfo.sta_num= 254;
    g_stSchStnInfo.direct= 0;
    g_stSchStnInfo.in_out_flag= 0x02;

    g_stDoorStnInfo.sta_num= 255;
    g_stDoorStnInfo.direct= 0;
    g_stDoorStnInfo.in_out_flag= 0x02;

    //设置门状态
    SetDoorState(BACK_DOOR,DOOR_CLOSE);
    SetDoorState(FRONT_DOOR,DOOR_CLOSE);
    //清空计数
    CounterReset(BACK_DOOR);
    CounterReset(FRONT_DOOR);
    return HI_SUCCESS;
}
HI_S32 HI3521_StartJHY_Work(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    pthread_t tid;
    static HI_BOOL bThdStart = HI_FALSE;

    if (HI_FALSE == bThdStart)
    {
        s32Ret = pthread_create(&tid, NULL, HI3521_JHY_WorkThd, NULL);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( "create HI3521_JHY_WorkThd thread err(%s)", ERR_STR);
            return HI_FAILURE;
        }

        s32Ret = pthread_detach(tid);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( "detach HI3521_JHY_WorkThd thread err(%s)", ERR_STR);
            return HI_FAILURE;
        }

        bThdStart = HI_TRUE;
    }

    return HI_SUCCESS;
}
#endif
